/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.0, released July  2011                                        *
 *   Copyright (c) 2011                                                      *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the CECILL version 2 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   CECILL version 2 License for more details.                              *
 *****************************************************************************/

#include "cppunit.h"

#include <designpattern/impl/CommandDispatcher.hpp>

#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/StringSequenceIterator.hpp>
#include <database/impl/ReadingFrameSequenceIterator.hpp>
#include <database/impl/BufferedSequenceDatabase.hpp>
#include <database/impl/ReadingFrameSequenceDatabase.hpp>

#include <seed/impl/BasicSeedModel.hpp>
#include <seed/impl/SubSeedModel.hpp>

#include <index/impl/DatabaseIndex.hpp>

#include <set>

using namespace std;
using namespace misc;
using namespace dp;
using namespace dp::impl;
using namespace database;
using namespace database::impl;
using namespace seed;
using namespace seed::impl;
using namespace indexation;
using namespace indexation::impl;

extern const char* getPath (const char* file);

#define MIN(a,b) ((a) < (b) ? (a) : (b))

/********************************************************************************/
class IndexBuildCommand : public ICommand
{
public:
    IndexBuildCommand (IDatabaseIndex* index) :  _index(index)  {  if (_index)  { _index->use(); } }
    virtual ~IndexBuildCommand ()  {  if (_index)  { _index->forget(); } }
    void execute ()  {  _index->build ();  }
private:
    IDatabaseIndex*_index;
};

/********************************************************************************/
class IndexMergeCommand : public ICommand
{
public:
    IndexMergeCommand (IDatabaseIndex* index) : _index(index)  {  if (_index)  { _index->use(); } }
    virtual ~IndexMergeCommand ()  {  if (_index)  { _index->forget(); } }
    void execute ()  {  _index->merge ();  }
private:
    IDatabaseIndex* _index;
};

/********************************************************************************/

struct OffsetChecker
{
    const char*  sequence;
    char         span;
    LETTER       seed[4];
    Offset       offsets[32];   // check 32 at most
    size_t       offsetsSize;
};

/********************************************************************************/

class TestDatabaseIndex : public TestFixture
{
private:

    ICommandDispatcher* parallelDispatcher;
    ICommandDispatcher* serialDispatcher;

    ISequenceIterator*  fastaIterator;
    ISequenceIterator*  stringIterator;
    ISeedModel*         modelSpan3;

    /********************************************************************************/
    bool checkIndex (IDatabaseIndex* index)
    {
        ISequenceDatabase* db    = index->getDatabase();
        ISeedModel*        model = index->getModel();

        size_t nbOccurs = 0;
        size_t nbErrors = 0;

        /** We create an iterator that loops all seeds of the model. */
        ISeedIterator* itSeed = model->createAllSeedsIterator();
        CPPUNIT_ASSERT (itSeed != 0);
        LOCAL (itSeed);

        /** We loop over all possible seeds for the given model. */
        for (itSeed->first(); !itSeed->isDone(); itSeed->next())
        {
            /** */
            const ISeed* seed = itSeed->currentItem();

            /** We create an occurrence iterator for the current seed. */
            IOccurrenceIterator* itOccurrences = index->createOccurrenceIterator (seed);

            /** Note that we have to check we didn't get a null iterator. Recall that null iterator
             *  is an optimization that tells that there is nothing to iterate for the given seed. */
            if (itOccurrences == 0)  { continue; }

            LOCAL (itOccurrences);

            for (itOccurrences->first(); ! itOccurrences->isDone(); itOccurrences->next())
            {
                /** A little shortcut. */
                const ISeedOccurrence* occur = itOccurrences->currentItem();

                /** We retrieve the sequence given its offset. */
                ISequence sequence;
                u_int32_t offsetInSequence;
                u_int64_t offsetInDatabase;
                if (db->getSequenceByOffset (occur->offsetInDatabase, sequence, offsetInSequence, offsetInDatabase) == true)
                {
                    /** We retrieve the buffer at the correct offset within the sequence. */
                    //const LETTER* buffer = sequence.data.buffer + offsetInSequence;
                }

                nbOccurs ++;
            }
        }

        return nbOccurs > 0  &&  nbErrors == 0;
    }

public:

    /********************************************************************************/
    void setUp ()
    {
        parallelDispatcher = new ParallelCommandDispatcher  ();
        parallelDispatcher->use();

        serialDispatcher = new SerialCommandDispatcher ();
        serialDispatcher->use();

        fastaIterator = new FastaSequenceIterator (getPath ("sapiens_1Mo.fa"), 100);
        fastaIterator->use ();

        stringIterator = new StringSequenceIterator (3, "AAAIAAA", "KLAAA", "FAAA");
        stringIterator->use ();

        modelSpan3 = new BasicSeedModel (SUBSEED, 3);
        modelSpan3->use();
    }

    void tearDown ()
    {
        parallelDispatcher->forget();
        serialDispatcher->forget();
        fastaIterator->forget ();
        stringIterator->forget ();
        modelSpan3->forget ();
    }

    /********************************************************************************/
    static Test* suite()
    {
    	 TestSuite* result = new TestSuite ("DatabaseIndexTest");

    	 result->addTest (new TestCaller<TestDatabaseIndex> ("testIndexCheck",              &TestDatabaseIndex::testIndexCheck ) );
    	 result->addTest (new TestCaller<TestDatabaseIndex> ("testIndexHashModelCheck",     &TestDatabaseIndex::testIndexHashModelCheck ) );
         result->addTest (new TestCaller<TestDatabaseIndex> ("testIndexCheck2",             &TestDatabaseIndex::testIndexCheck2 ) );
         result->addTest (new TestCaller<TestDatabaseIndex> ("testIndexDispatchParallel",   &TestDatabaseIndex::testIndexDispatchParallel ) );
         result->addTest (new TestCaller<TestDatabaseIndex> ("testIndexDispatchSerial",     &TestDatabaseIndex::testIndexDispatchSerial ) );
         result->addTest (new TestCaller<TestDatabaseIndex> ("testIndexMergeCheck",         &TestDatabaseIndex::testIndexMergeCheck ) );
         result->addTest (new TestCaller<TestDatabaseIndex> ("testIndexOccurrenceIterator", &TestDatabaseIndex::testIndexOccurrenceIterator ) );
         result->addTest (new TestCaller<TestDatabaseIndex> ("testDatabaseADN",        &TestDatabaseIndex::testDatabaseADN ) );
         result->addTest (new TestCaller<TestDatabaseIndex> ("testIndexDatabaseADN",        &TestDatabaseIndex::testIndexDatabaseADN ) );
    	 return result;
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testIndexCheck ()
    {
        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (fastaIterator, false);
        CPPUNIT_ASSERT (database != 0);

        /** We create an index for this model. */
        DatabaseIndex index (database, modelSpan3);

        /** We build the index. */
        index.build ();

        CPPUNIT_ASSERT (checkIndex(&index) == true);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testIndexHashModelCheck ()
    {
        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (fastaIterator, false);
        CPPUNIT_ASSERT (database != 0);

        /** We create an index for this model. */
        DatabaseIndex index (database,
            new SubSeedModel (3,
                "H,FY,W,IV,LM,C,RK,Q,E,N,D,A,S,T,G,P",
                "HFYWIVLMC,RKQENDASTGP",
                "H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P"
            )
        );

        /** We build the index. */
        index.build ();

        CPPUNIT_ASSERT (checkIndex(&index) == true);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    bool testIndexCheck2_aux (OffsetChecker& check)
    {
        /** We create a new model. */
        ISeedModel* model = new BasicSeedModel (SUBSEED, check.span);

        /** We create a sequences database. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (
            new StringSequenceIterator (1, check.sequence), false
        );

        /** We want to build neighbourhoods. */
        size_t neighbourhoodSize = 15;
        IWord neighbourhood (model->getSpan() + 2*neighbourhoodSize);

        /** We create an index. */
        DatabaseIndex index (database, model);

        /** We build the index. */
        index.build ();

        set<Offset> aSet (check.offsets, check.offsets + check.offsetsSize);
        size_t nbOk = 0;

        ISeed seed (check.span, check.seed);

        /** We create an offsets iterator for a seed. */
        IOccurrenceIterator* itOccurrences = index.createOccurrenceIterator (&seed, neighbourhoodSize);
        if (itOccurrences != 0)
        {
            LOCAL (itOccurrences);

            for (itOccurrences->first(); ! itOccurrences->isDone(); itOccurrences->next () )
            {
                /** A little shortcut. */
                const ISeedOccurrence* occur = itOccurrences->currentItem();

                /** We check that the current offset belongs to the check set. */
                if (aSet.find(occur->offsetInDatabase) == aSet.end ())  {  break; }

                /** We check the neighbourhood. */
                {
                    const ISequence& seq = occur->sequence;
                    u_int32_t offset     = occur->offsetInSequence;

                    LETTER* buf1 = seq.data.letters.data + offset;

                    size_t nbMismatches = 0;

                    size_t c=0;
                    for (size_t i=0; i<model->getSpan()+neighbourhoodSize; i++, c++)
                    {
                        LETTER l1 = (i < seq.data.letters.size-offset ? buf1[i] : CODE_X);
                        LETTER l2 = occur->neighbourhood.letters.data[c];
                        neighbourhood.letters.data [c] = l1;
                        if (l1 != l2)  {  nbMismatches++;  }
                    }

                    for (size_t i=1; i<=neighbourhoodSize; i++, c++)
                    {
                        LETTER l1 = (i <= offset ? buf1[-i] : CODE_X);
                        LETTER l2 = occur->neighbourhood.letters.data[c];
                        neighbourhood.letters.data [c] = l1;
                        if (l1 != l2)  {  nbMismatches++;   }
                    }

                    if (nbMismatches > 0)
                    {
                        printf ("\nBAD   NEIGHBOURHOOD '%s'\n", occur->neighbourhood.toString().c_str());
                        printf ("BUILT NEIGHBOURHOOD '%s'\n", neighbourhood.toString().c_str());
                    }

                    CPPUNIT_ASSERT (nbMismatches == 0);
                }

                nbOk ++;
            }
        }
        return (nbOk == aSet.size ());
    }

    void testIndexCheck2 ()
    {
        /** */
        OffsetChecker tableOffsetChecker [] =
        {
            { "AAAA",                               1, {CODE_A},                        {0,1,2,3},  4 },
            { "XXAX",                               1, {CODE_A},                        {2},        1 },

            { "AYCDD",                              3, {CODE_Y,CODE_C,CODE_D},          {1},        1 },
            { "AYCXDD",                             3, {CODE_Y,CODE_C,CODE_D},          {},         0 },
            { "AYCDDAPIYCDXE",                      3, {CODE_Y,CODE_C,CODE_D},          {1,8},      2 },
            { "AYCDDAPIYCDXEEZRUOIERUYCDEOIA",      3, {CODE_Y,CODE_C,CODE_D},          {1,8,22},   3 },
            { "AYCDDAPIYXDXEEZRUOIERUYCDEOIA",      3, {CODE_Y,CODE_C,CODE_D},          {1,22},     2 },

            { "fePQRSfzePOIfdfsfiPqRscDEFIsd",      4, {CODE_P,CODE_Q,CODE_R,CODE_S},   {2,18},     2 },
            { "PQRSfzsePfdfsfiPqRscfDEXXPQrs",      4, {CODE_P,CODE_Q,CODE_R,CODE_S},   {0,15,25},  3 },

            { 0,  0, {0}, {0}, 0 }
        };

        for (size_t i=0; ; i++)
        {
            OffsetChecker& check = tableOffsetChecker[i];

            if (check.sequence == 0)  {  break; }
            CPPUNIT_ASSERT (testIndexCheck2_aux (check) == true);
        }
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testIndexCheckDispatch_aux (ICommandDispatcher* dispatcher)
    {
        CPPUNIT_ASSERT (dispatcher != 0);

        /** We create a sequence database for the current frame. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (fastaIterator, false);
        LOCAL (database);
        CPPUNIT_ASSERT (database != 0);

        /** We create 6 commands, one by reading frame. */
        list<ICommand*> commands;
        for (int frame=FRAME_1; frame<=FRAME_6; frame++)
        {
            /** We create a framed database from the nucleotid database. */
            ISequenceDatabase* frameDatabase = new ReadingFrameSequenceDatabase ((ReadingFrame_e)frame, database, false);
            CPPUNIT_ASSERT (frameDatabase != 0);

            /** We create an index for the current frame database. */
            IDatabaseIndex* index = new DatabaseIndex (frameDatabase, modelSpan3);
            CPPUNIT_ASSERT (index != 0);

            /** We create a command for building the index from the sequence iterator. */
            commands.push_back (new IndexBuildCommand (index));
        }

        /** We dispatch the commands. */
        dispatcher->dispatchCommands (commands, 0);
    }

    void testIndexDispatchParallel ()
    {
        testIndexCheckDispatch_aux (parallelDispatcher);
    }

    void testIndexDispatchSerial ()
    {
        testIndexCheckDispatch_aux (serialDispatcher);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testIndexMergeCheck ()
    {
        /** We create a sequences database. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (stringIterator, false);
        LOCAL (database);
        CPPUNIT_ASSERT (database != 0);

        /** We create a global index. */
        IDatabaseIndex* globalIndex = new DatabaseIndex (database, modelSpan3);
        LOCAL(globalIndex);
        CPPUNIT_ASSERT (globalIndex != 0);

        /** We try to split the iterator in several smaller ones. */
        vector<ISequenceDatabase*> splits = database->split (4);

        list<ICommand*> commands;
        for (size_t i=0; i<splits.size(); i++)
        {
            /** We create an index for the current frame. */
            IDatabaseIndex* index = new DatabaseIndex (splits[i], modelSpan3);

            /** We add the index to the global index. */
            globalIndex->addChildIndex (index);

            /** We create a command for building the index from the sequence iterator. */
            commands.push_back (new IndexBuildCommand (index) );
        }

        /** We need a commands dispatcher. */
        CPPUNIT_ASSERT (parallelDispatcher != 0);

        /** We dispatch the commands. */
        serialDispatcher->dispatchCommands (commands, new IndexMergeCommand (globalIndex));

        CPPUNIT_ASSERT (checkIndex(globalIndex) == true);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testIndexOccurrenceIterator ()
    {
        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (
            new StringSequenceIterator (3,
                "RTKLLAAAIAAAPT",
                "PAAALKN",
                "FMWMAAARKLAAAMN"
            ), false
        );
        CPPUNIT_ASSERT (database != 0);

        /** We create an index for this model. */
        IDatabaseIndex* index = new DatabaseIndex (database, modelSpan3);
        CPPUNIT_ASSERT (index != 0);
        LOCAL (index);

        /** We build the index. */
        index->build ();

        /** We check occurrences for some seed hash codes. */
        LETTER tmp[] = { CODE_A, CODE_A, CODE_A };
        ISeed seed (3, tmp);

        size_t neighbourhoodSize = 4;

        /** We create an seed occurrence iterator; note that we want to get neighbourhoods of length 4. */
        IOccurrenceIterator* itOccur = index->createOccurrenceIterator (&seed, neighbourhoodSize);

        if (itOccur != 0)
        {
            LOCAL (itOccur);

            size_t nbFound=0;

            for (itOccur->first(); ! itOccur->isDone(); itOccur->next())
            {
               const ISeedOccurrence* occur = itOccur->currentItem();
               CPPUNIT_ASSERT (occur != 0);

               const ISequence& s1         = occur->sequence;
               u_int32_t offsetInSequence1 = occur->offsetInSequence;

               LETTER* buf1 = s1.data.letters.data + offsetInSequence1;

               size_t nbMismatches = 0;

               size_t c=0;
               for (size_t i=0; i<modelSpan3->getSpan()+neighbourhoodSize; i++, c++)
               {
                   LETTER l1 = (i < s1.data.letters.size-offsetInSequence1 ? buf1[i] : CODE_X);
                   LETTER l2 = occur->neighbourhood.letters.data[c];
                   if (l1 != l2)  { nbMismatches++; }
               }

               for (size_t i=1; i<=neighbourhoodSize; i++, c++)
               {
                   LETTER l1 = (i <= offsetInSequence1 ? buf1[-i] : CODE_X);
                   LETTER l2 = occur->neighbourhood.letters.data[c];
                   if (l1 != l2)  { nbMismatches++; }
               }
               CPPUNIT_ASSERT (nbMismatches == 0);

               string content = occur->neighbourhood.toString();

                    if (nbFound==0)  { CPPUNIT_ASSERT (content.compare ("AAAIAAALLKT") == 0);  nbFound++;   }
               else if (nbFound==1)  { CPPUNIT_ASSERT (content.compare ("AAAPTXXIAAA") == 0);  nbFound++;   }
               else if (nbFound==2)  { CPPUNIT_ASSERT (content.compare ("AAALKNXPXXX") == 0);  nbFound++;   }
               else if (nbFound==3)  { CPPUNIT_ASSERT (content.compare ("AAARKLAMWMF") == 0);  nbFound++;   }
               else if (nbFound==4)  { CPPUNIT_ASSERT (content.compare ("AAAMNXXLKRA") == 0);  nbFound++;   }
            }
            CPPUNIT_ASSERT (nbFound == 5);
        }
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testDatabaseADN ()
    {
        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (
            new StringSequenceIterator (1, "CAATGGCTAGGTACTATGTATGAGATCATGATCTTTACAAATCCGAG"), false
        );
        CPPUNIT_ASSERT (database != 0);
        LOCAL (database);

        for (int frame=FRAME_1; frame<=FRAME_6; frame++)
        {
            size_t nb = 0;

            /** We create a framed database from the nucleotid database. */
            ISequenceDatabase* frameDatabase = new ReadingFrameSequenceDatabase ((ReadingFrame_e)frame, database, false);
            CPPUNIT_ASSERT (frameDatabase != 0);
            LOCAL (frameDatabase);

            ISequenceIterator* seqIt = frameDatabase->createSequenceIterator();
            LOCAL (seqIt);
            nb = 0;
            for (seqIt->first(); !seqIt->isDone(); seqIt->next(), nb++)
            {
                const ISequence* seq = seqIt->currentItem();

                if (frame==0)  {  CPPUNIT_ASSERT (seq->data.toString().compare ("QWLGTMYEIMIFTNPX") == 0); }
                if (frame==1)  {  CPPUNIT_ASSERT (seq->data.toString().compare ("NG*VLCMRS*SLQIRX") == 0); }
                if (frame==2)  {  CPPUNIT_ASSERT (seq->data.toString().compare ("MARYYV*DHDLYKSE") == 0); }
                if (frame==3)  {  CPPUNIT_ASSERT (seq->data.toString().compare ("LGFVKIMISYIVPSHX") == 0); }
                if (frame==4)  {  CPPUNIT_ASSERT (seq->data.toString().compare ("SDL*RS*SHT*YLAIX") == 0); }
                if (frame==5)  {  CPPUNIT_ASSERT (seq->data.toString().compare ("RICKDHDLIHST*PL") == 0); }
            }

            size_t nbSequences = frameDatabase->getSequencesNumber();
            for (size_t i=0; i<nbSequences; i++)
            {
                ISequence seq;
                if (frameDatabase->getSequenceByIndex (i, seq) == true)
                {
                    if (frame==0)  {  CPPUNIT_ASSERT (seq.data.toString().compare ("QWLGTMYEIMIFTNPX") == 0); }
                    if (frame==1)  {  CPPUNIT_ASSERT (seq.data.toString().compare ("NG*VLCMRS*SLQIRX") == 0); }
                    if (frame==2)  {  CPPUNIT_ASSERT (seq.data.toString().compare ("MARYYV*DHDLYKSE") == 0); }
                    if (frame==3)  {  CPPUNIT_ASSERT (seq.data.toString().compare ("LGFVKIMISYIVPSHX") == 0); }
                    if (frame==4)  {  CPPUNIT_ASSERT (seq.data.toString().compare ("SDL*RS*SHT*YLAIX") == 0); }
                    if (frame==5)  {  CPPUNIT_ASSERT (seq.data.toString().compare ("RICKDHDLIHST*PL") == 0); }
                }
            }
        }
    }


    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testIndexDatabaseADN ()
    {
        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (
            new StringSequenceIterator (1, "CAATGGCTAGGTACTATGTATGAGATCATGATCTTTACAAATCCGAG"), false
        );
        CPPUNIT_ASSERT (database != 0);
        LOCAL (database);

        for (int frame=FRAME_1; frame<=FRAME_6; frame++)
        {
            //printf ("-------------------------------------- FRAME %d --------------------------------------\n", frame);

            /** We create a framed database from the nucleotid database. */
            ISequenceDatabase* frameDatabase = new ReadingFrameSequenceDatabase ((ReadingFrame_e)frame, database, false);
            CPPUNIT_ASSERT (frameDatabase != 0);
            LOCAL (frameDatabase);

            /** We create an index for this model. */
            IDatabaseIndex* index = new DatabaseIndex (frameDatabase, modelSpan3);
            CPPUNIT_ASSERT (index != 0);
            LOCAL (index);

            /** We build the index. */
            index->build ();

            /** We loop all possible seeds. */
            ISeedIterator* itSeed = modelSpan3->createAllSeedsIterator();
            if (itSeed != 0)
            {
                LOCAL (itSeed);

                for (itSeed->first(); !itSeed->isDone(); itSeed->next())
                {
                    /** */
                    IOccurrenceIterator* itOccur = index->createOccurrenceIterator (itSeed->currentItem(), 22);
                    if (itOccur != 0)
                    {
                        LOCAL (itOccur);

                        //printf ("SEED '%s'\n", itSeed->currentItem()->kmer.toString().c_str());

                        for (itOccur->first(); !itOccur->isDone(); itOccur->next())
                        {
                            //const ISeedOccurrence* occur = itOccur->currentItem();
                            //printf ("---> '%s' \n", occur->neighbourhood.toString().c_str());
                        }
                    }
                }
            }

            break;
        }
    }

};

/********************************************************************************/

extern "C" Test* TestDatabaseIndex_suite()  { return TestDatabaseIndex::suite (); }
