/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

#include "cppunit.h"

#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/StringSequenceIterator.hpp>
#include <database/impl/BufferedSequenceDatabase.hpp>
#include <database/impl/BufferedCachedSequenceDatabase.hpp>
#include <database/impl/ReadingFrameSequenceDatabase.hpp>
#include <database/impl/CompositeSequenceDatabase.hpp>
#include <database/impl/FastaDatabaseQuickReader.hpp>
#include <database/impl/FastaSequenceOutput.hpp>

#include <designpattern/impl/IteratorGet.hpp>
#include <designpattern/impl/CommandDispatcher.hpp>
#include <os/impl/DefaultOsFactory.hpp>

using namespace std;

using namespace database;
using namespace database::impl;

using namespace os;
using namespace os::impl;

using namespace dp;
using namespace dp::impl;

using namespace misc;

extern const char* getPath (const char* file);

/** */
struct SeqIterInfo
{
    SeqIterInfo () : nbSequences(0), dataSize(0), checksum(0) {}
    size_t    nbSequences;
    u_int64_t dataSize;
    u_int64_t checksum;
};

/********************************************************************************/

class TestSequenceDatabase : public TestFixture
{
private:
    ISequenceIterator* fastaIterator;
    ISequenceIterator* stringIterator;

public:

    /********************************************************************************/
    void setUp ()
    {
        fastaIterator = new FastaSequenceIterator (getPath("tursiops.fa"), 100);
        fastaIterator->use ();

        stringIterator = new StringSequenceIterator (3, "AAAIAAA", "KLAAA", "FAAA");
        stringIterator->use ();
    }

    void tearDown ()
    {
        fastaIterator->forget ();
        stringIterator->forget ();
    }

    /********************************************************************************/
    static Test* suite()
    {
    	 TestSuite* result = new TestSuite ("SequenceDatabaseTest");

    	 result->addTest (new TestCaller<TestSequenceDatabase> ("testSequenceDatabaseFasta", 	        &TestSequenceDatabase::testSequenceDatabaseFasta ) );
         result->addTest (new TestCaller<TestSequenceDatabase> ("testSequenceDatabaseDirectAccess",     &TestSequenceDatabase::testSequenceDatabaseDirectAccess ) );
         result->addTest (new TestCaller<TestSequenceDatabase> ("testSequenceDatabaseIterativeAccess",  &TestSequenceDatabase::testSequenceDatabaseIterativeAccess ) );
         result->addTest (new TestCaller<TestSequenceDatabase> ("testSequenceDatabaseSplit1",           &TestSequenceDatabase::testSequenceDatabaseSplit1 ) );
         result->addTest (new TestCaller<TestSequenceDatabase> ("testSequenceDatabaseSplit2",           &TestSequenceDatabase::testSequenceDatabaseSplit2 ) );
         result->addTest (new TestCaller<TestSequenceDatabase> ("testSequenceDatabaseSplitBigFile",     &TestSequenceDatabase::testSequenceDatabaseSplitBigFile ) );
         result->addTest (new TestCaller<TestSequenceDatabase> ("testFastaQuickReader",                 &TestSequenceDatabase::testFastaQuickReader ) );
         result->addTest (new TestCaller<TestSequenceDatabase> ("testFastaQuickReader2",                &TestSequenceDatabase::testFastaQuickReader2 ) );
         result->addTest (new TestCaller<TestSequenceDatabase> ("testCompositeDatabase",                &TestSequenceDatabase::testCompositeDatabase ) );
         result->addTest (new TestCaller<TestSequenceDatabase> ("testCompositeDatabase2",                &TestSequenceDatabase::testCompositeDatabase2 ) );
         result->addTest (new TestCaller<TestSequenceDatabase> ("testDatabaseIteratorGet",                &TestSequenceDatabase::testDatabaseIteratorGet ) );

    	 return result;
    }

    /********************************************************************************/
    /*  Create a database from a fasta file and check that we got at least one sequence. */
    /********************************************************************************/
    void testSequenceDatabaseFasta ()
    {
        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (fastaIterator, false);
        LOCAL (database);

        CPPUNIT_ASSERT (database != 0);
        CPPUNIT_ASSERT (database->getSequencesNumber() > 0);
    }

    /********************************************************************************/
    /*  Create a database from a sequence and check that we retrieve correct sequences. */
    /********************************************************************************/
    void testSequenceDatabaseDirectAccess ()
    {
        ISequence seq;

        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (stringIterator, false);
        LOCAL (database);

        CPPUNIT_ASSERT (database != 0);
        CPPUNIT_ASSERT (database->getSequencesNumber() == 3);

        CPPUNIT_ASSERT (database->getSequenceByIndex (0, seq) == true);
        CPPUNIT_ASSERT (seq.data.toString().compare ("AAAIAAA") == 0);
        CPPUNIT_ASSERT (seq.offsetInDb == 0);

        CPPUNIT_ASSERT (database->getSequenceByIndex (1, seq) == true);
        CPPUNIT_ASSERT (seq.data.toString().compare ("KLAAA") == 0);
        CPPUNIT_ASSERT (seq.offsetInDb == 7);

        CPPUNIT_ASSERT (database->getSequenceByIndex (2, seq) == true);
        CPPUNIT_ASSERT (seq.data.toString().compare ("FAAA") == 0);
        CPPUNIT_ASSERT (seq.offsetInDb == 12);

        /** We check that we can't get a sequence with a wrong index. */
        CPPUNIT_ASSERT (database->getSequenceByIndex (3, seq) == false);

        CPPUNIT_ASSERT (database->getSequenceByIndex (-1, seq) == false);
    }

    /********************************************************************************/
    /*  Create a database and test a Sequence iterator created from it. */
    /********************************************************************************/
    void testSequenceDatabaseIterativeAccess ()
    {
        const ISequence* seq = 0;

        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (stringIterator, false);
        LOCAL (database);
        CPPUNIT_ASSERT (database != 0);

        ISequenceIterator* itSeq = database->createSequenceIterator();
        LOCAL (itSeq);
        CPPUNIT_ASSERT (itSeq != 0);

        itSeq->first();
        CPPUNIT_ASSERT (itSeq->isDone() == false);

        seq = itSeq->currentItem();
        CPPUNIT_ASSERT (seq != 0);
        CPPUNIT_ASSERT (seq->data.toString().compare ("AAAIAAA") == 0);

        itSeq->next();
        CPPUNIT_ASSERT (itSeq->isDone() == false);

        seq = itSeq->currentItem();
        CPPUNIT_ASSERT (seq != 0);
        CPPUNIT_ASSERT (seq->data.toString().compare ("KLAAA") == 0);

        itSeq->next();
        CPPUNIT_ASSERT (itSeq->isDone() == false);

        seq = itSeq->currentItem();
        CPPUNIT_ASSERT (seq != 0);
        CPPUNIT_ASSERT (seq->data.toString().compare ("FAAA") == 0);

        itSeq->next();
        CPPUNIT_ASSERT (itSeq->isDone() == true);
    }

    /********************************************************************************/
    /*  Create a database and split it. */
    /********************************************************************************/
    void testSequenceDatabaseSplit1 ()
    {
        ISequence seq;

        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (stringIterator, false);
        LOCAL (database);
        CPPUNIT_ASSERT (database != 0);

        /** We split the database in two. */
        vector<ISequenceDatabase*> splits = database->split (2);
        CPPUNIT_ASSERT (splits.size() == 2);

        /** We check the sequences number in each split database. */
        CPPUNIT_ASSERT (splits[0]->getSequencesNumber() == 2);
        CPPUNIT_ASSERT (splits[1]->getSequencesNumber() == 1);

        CPPUNIT_ASSERT (splits[0]->getSequenceByIndex (0, seq) == true);
        CPPUNIT_ASSERT (seq.data.toString().compare ("AAAIAAA") == 0);

        CPPUNIT_ASSERT (splits[0]->getSequenceByIndex (1, seq) == true);
        CPPUNIT_ASSERT (seq.data.toString().compare ("KLAAA") == 0);

        CPPUNIT_ASSERT (splits[1]->getSequenceByIndex (0, seq) == true);
        CPPUNIT_ASSERT (seq.data.toString().compare ("FAAA") == 0);

        /** We release the split databases. */
        for (size_t i=0; i<splits.size(); i++)   {  ISequenceDatabase* child = splits[i];   LOCAL (child);  }
    }

    /********************************************************************************/
    /*  Create a database and split it. */
    /********************************************************************************/
    void testSequenceDatabaseSplit2 ()
    {
        ISequence seq;

        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (
            new StringSequenceIterator (5,
                "ABC", "BCD",
                "CDE", "DEF",
                "EFG"
            ), false
        );
        LOCAL (database);
        CPPUNIT_ASSERT (database != 0);

        /** We split the database in two. */
        vector<ISequenceDatabase*> splits = database->split (3);
        CPPUNIT_ASSERT (splits.size() == 3);

        /** We check the sequences number in each split database. */
        CPPUNIT_ASSERT (splits[0]->getSequencesNumber() == 2);
        CPPUNIT_ASSERT (splits[1]->getSequencesNumber() == 2);
        CPPUNIT_ASSERT (splits[2]->getSequencesNumber() == 1);

        CPPUNIT_ASSERT (splits[0]->getSequenceByIndex (0, seq) == true);    CPPUNIT_ASSERT (seq.data.toString().compare ("ABC") == 0);
        CPPUNIT_ASSERT (splits[0]->getSequenceByIndex (1, seq) == true);    CPPUNIT_ASSERT (seq.data.toString().compare ("BCD") == 0);
        CPPUNIT_ASSERT (splits[0]->getSequenceByIndex (2, seq) == false);

        CPPUNIT_ASSERT (splits[1]->getSequenceByIndex (0, seq) == true);    CPPUNIT_ASSERT (seq.data.toString().compare ("CDE") == 0);
        CPPUNIT_ASSERT (splits[1]->getSequenceByIndex (1, seq) == true);    CPPUNIT_ASSERT (seq.data.toString().compare ("DEF") == 0);
        CPPUNIT_ASSERT (splits[1]->getSequenceByIndex (2, seq) == false);

        CPPUNIT_ASSERT (splits[2]->getSequenceByIndex (0, seq) == true);    CPPUNIT_ASSERT (seq.data.toString().compare ("EFG") == 0);
        CPPUNIT_ASSERT (splits[2]->getSequenceByIndex (1, seq) == false);

        /** We release the split databases. */
        for (size_t i=0; i<splits.size(); i++)   {  ISequenceDatabase* child = splits[i];   LOCAL (child);  }
    }

    /********************************************************************************/
    /*  Create a database and split it. */
    /********************************************************************************/
    void testSequenceDatabaseSplitBigFile ()
    {
        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (fastaIterator, false);
        LOCAL (database);
        CPPUNIT_ASSERT (database != 0);

        /** We split the database in N. */
        size_t nbSplits = 10;
        vector<ISequenceDatabase*> splits = database->split (nbSplits);
        CPPUNIT_ASSERT (splits.size() == nbSplits);

        /** We get the number of sequences of the first split. */
        size_t nbSequences = splits[0]->getSequencesNumber();

        /** We check that the first N-1 split have the same sequences number. */
        for (size_t i=0; i<nbSplits-1; i++)
        {
            CPPUNIT_ASSERT (splits[i]->getSequencesNumber() == nbSequences);
        }

        /** We check that the last split have a sequences number <= as the other splits. */
        CPPUNIT_ASSERT (splits[nbSplits-1]->getSequencesNumber() <= nbSequences);

        /** We release the split databases. */
        for (size_t i=0; i<splits.size(); i++)   {  ISequenceDatabase* child = splits[i];   LOCAL (child);  }
    }

    /********************************************************************************/
    /********************************************************************************/
    void testFastaQuickReader ()
    {
        size_t checksum1 = 0;
        size_t checksum2 = 0;

        const char* uri = getPath ("tursiops.fa");

        /** We compute some kind of checksum code on the database. */
        ISequenceIterator* itglobal = new FastaSequenceIterator (uri, 1024);
        LOCAL (itglobal);
        for (itglobal->first(); ! itglobal->isDone(); itglobal->next())
        {
            const ISequence* seq = itglobal->currentItem();
            size_t n = seq->data.letters.size;
            for (size_t i=0; i<n; i++)   {  checksum1 = (checksum1 + seq->data.letters.data[i]) % (1<<16); }
        }

        FastaDatabaseQuickReader reader1 (uri, false);
        reader1.read (0);

        /** We want to split a little the database. */
        u_int64_t maxblocksize = reader1.getTotalSize() / 10;

        FastaDatabaseQuickReader reader2 (uri, false);
        reader2.read (maxblocksize);
        std::vector<u_int64_t>& offsets  = reader2.getOffsets ();

        u_int64_t datasize2    = 0;
        u_int32_t nbsequences2 = 0;
        for (size_t i=0; i<offsets.size()-1; i++)
        {
            u_int64_t a = offsets[i];
            u_int64_t b = offsets[i+1] - 1;

            ISequenceIterator* it = new FastaSequenceIterator (uri, 1024, a, b);
            LOCAL (it);
            for (it->first(); ! it->isDone(); it->next())
            {
                const ISequence* seq = it->currentItem();

                datasize2 += seq->data.letters.size;
                nbsequences2 ++;

                size_t n = seq->data.letters.size;
                for (size_t i=0; i<n; i++)   {  checksum2 = (checksum2 + seq->data.letters.data[i]) % (1<<16);  }
            }
        }

        CPPUNIT_ASSERT (reader1.getDataSize()    == datasize2);
        CPPUNIT_ASSERT (reader1.getNbSequences() == nbsequences2);
        CPPUNIT_ASSERT (checksum1                == checksum2);
    }

    /********************************************************************************/
    /********************************************************************************/
    void testReadingFrameIteratorAux (const char* fname, size_t databaseMaxSize)
    {
        SeqIterInfo info1;
        SeqIterInfo info2;

        const char* filename = getPath (fname);

        /** We use a simple sequence iterator for retrieving some information. */
        ISequenceIterator* itSeq = new FastaSequenceIterator (filename);
        LOCAL (itSeq);
        size_t k=0;
        for (itSeq->first(); !itSeq->isDone(); itSeq->next(), k++)
        {
            const ISequence* seq = itSeq->currentItem();
            CPPUNIT_ASSERT (seq != 0);

            u_int32_t     len  = seq->getLength();
            const LETTER* data = seq->getData();

            info1.nbSequences++;
            info1.dataSize += len;
            for (size_t i=0; i<len; i++)  {  info1.checksum += data [i];  }
        }

        FastaDatabaseQuickReader reader (filename, false);
        reader.read (databaseMaxSize);

        CPPUNIT_ASSERT (info1.nbSequences == reader.getNbSequences());

        // Note on the following commented test: we can't rely on the getDataSize from the reader
        // to be exact in all cases. It provides only a raw estimation
        //CPPUNIT_ASSERT (info1.dataSize    == reader.getDataSize());

        vector<u_int64_t>& offsets = reader.getOffsets ();
        for (size_t i=0; i<offsets.size()-1; i++)
        {
            //printf ("--------------------------- OFFSET [%ld,%ld] ---------------------------\n", offsets[i], offsets[i+1]);

            ISequenceIterator* itSeq = new FastaSequenceIterator (filename, 128*1024, offsets[i], offsets[i+1]);
            LOCAL (itSeq);

            for (itSeq->first(); !itSeq->isDone(); itSeq->next())
            {
                const ISequence* seq = itSeq->currentItem();

                u_int32_t     len  = seq->getLength();
                const LETTER* data = seq->getData();

                info2.nbSequences++;
                info2.dataSize += len;
                for (size_t i=0; i<len; i++)  {  info2.checksum += data [i];  }
            }
        }

        CPPUNIT_ASSERT (info1.nbSequences == info2.nbSequences);
        CPPUNIT_ASSERT (info1.dataSize    == info2.dataSize);
        CPPUNIT_ASSERT (info1.checksum    == info2.checksum);

        printf ("file '%s'  maxsize=%ld => nbSequences=%ld  dataSize=%lld  checksum=%lld\n",
            filename, databaseMaxSize,
            info1.nbSequences, info1.dataSize, info1.checksum
        );
    }

    /********************************************************************************/
    /********************************************************************************/
    void testFastaQuickReader2 ()
    {
        const char* fileTable[] = { "tursiops.fa", "panda.fa", "Fee5ab_All_non_rRNA.fasta", "uniprot_sprot.fa"};
        size_t      sizeTable[] = { 1000, 10*1000, 100*1000, 1000*1000, 10*1000*1000, 100*1000*1000};

        for (size_t i=0; i<sizeof(fileTable)/sizeof(fileTable[0]); i++)
        {
            for (size_t j=0; j<sizeof(sizeTable)/sizeof(sizeTable[0]); j++)
            {
                testReadingFrameIteratorAux (fileTable[i], sizeTable[j]);
            }
        }
    }

    /********************************************************************************/
    /********************************************************************************/
    void testCompositeDatabase_iterate (const ISequence* seq)
    {
        TRACE ("'%s\n", seq->toString().c_str() );
    }


#define CHECK(offset,found,seq,offsetInSeq,offsetInDb,wantedDb,wantedOffsetInSeq,wantedOffsetInDb,wantedLetters)  \
    found = dbComposite->getSequenceByOffset (offset, seq, offsetInSeq, offsetInDb); \
    CPPUNIT_ASSERT (found);                                 \
    CPPUNIT_ASSERT (seq.database == wantedDb);              \
    CPPUNIT_ASSERT (offsetInSeq == wantedOffsetInSeq);      \
    CPPUNIT_ASSERT (offsetInDb  == wantedOffsetInDb);       \
    CPPUNIT_ASSERT (seq.data.toString().compare(wantedLetters) == 0)

    /** */
    void testCompositeDatabase (void)
    {
        ISequenceDatabase* db1 = new BufferedSequenceDatabase (
            new StringSequenceIterator (5,  "A", "B",  "C", "D", "E" ), false
        );
        LOCAL (db1);
        CPPUNIT_ASSERT (db1 != 0);

        ISequenceDatabase* db2 = new BufferedSequenceDatabase (
            new StringSequenceIterator (3,  "AA", "BB",  "CC"), false
        );
        LOCAL (db2);
        CPPUNIT_ASSERT (db2 != 0);

        ISequenceDatabase* db3 = new BufferedSequenceDatabase (
            new StringSequenceIterator (6,  "AAA", "BBB",  "CCC", "DDD", "EEE", "FFF"), false
        );
        LOCAL (db3);
        CPPUNIT_ASSERT (db3 != 0);

        list<ISequenceDatabase*> dbList;
        dbList.push_back (db1);
        dbList.push_back (db2);
        dbList.push_back (db3);

        ISequenceDatabase* dbComposite = new CompositeSequenceDatabase (dbList);
        LOCAL (dbComposite);
        CPPUNIT_ASSERT (dbComposite != 0);

        CPPUNIT_ASSERT (dbComposite->getSequencesNumber() == (5+3+6));
        CPPUNIT_ASSERT (dbComposite->getSize() == (5*1 + 3*2 + 6*3) );

        ISequenceIterator* seqIt = dbComposite->createSequenceIterator();
        LOCAL (seqIt);
        CPPUNIT_ASSERT (seqIt != 0);

#if 1
        for (seqIt->first(); ! seqIt->isDone(); seqIt->next() )
        {
            const ISequence* seq = seqIt->currentItem();
            TRACE ("'%s\n", seq->toString().c_str() );
        }
#else
        seqIt->iterate (this, (dp::Iterator<const ISequence*>::Method) &TestSequenceDatabase::testCompositeDatabase_iterate);
#endif
        size_t nbNotFound = 0;
        ISequence seq;
        for (size_t i=0; i<dbComposite->getSequencesNumber(); i++)
        {
            if (dbComposite->getSequenceByIndex(i, seq) == true)
            {
                     if (0<=i && i<5)   {  CPPUNIT_ASSERT (seq.database == db1); }
                else if (5<=i && i<8)   {  CPPUNIT_ASSERT (seq.database == db2); }
                else if (8<=i && i<14)  {  CPPUNIT_ASSERT (seq.database == db3); }
            }
            else
            {
                nbNotFound ++;
            }
        }

        CPPUNIT_ASSERT (nbNotFound == 0);

        bool found = false;
        u_int32_t offsetInSeq;
        u_int64_t offsetInDb;

        CHECK (0,  found,seq,offsetInSeq,offsetInDb, db1, 0, 0,  "A");
        CHECK (1,  found,seq,offsetInSeq,offsetInDb, db1, 0, 1,  "B");
        CHECK (2,  found,seq,offsetInSeq,offsetInDb, db1, 0, 2,  "C");
        CHECK (3,  found,seq,offsetInSeq,offsetInDb, db1, 0, 3,  "D");
        CHECK (4,  found,seq,offsetInSeq,offsetInDb, db1, 0, 4,  "E");

        CHECK (5,  found,seq,offsetInSeq,offsetInDb, db2, 0, 0,  "AA");
        CHECK (6,  found,seq,offsetInSeq,offsetInDb, db2, 1, 1,  "AA");
        CHECK (7,  found,seq,offsetInSeq,offsetInDb, db2, 0, 2,  "BB");
        CHECK (8,  found,seq,offsetInSeq,offsetInDb, db2, 1, 3,  "BB");
        CHECK (9,  found,seq,offsetInSeq,offsetInDb, db2, 0, 4,  "CC");
        CHECK (10, found,seq,offsetInSeq,offsetInDb, db2, 1, 5,  "CC");

        CHECK (11,  found,seq,offsetInSeq,offsetInDb, db3, 0,  0,  "AAA");
        CHECK (12,  found,seq,offsetInSeq,offsetInDb, db3, 1,  1,  "AAA");
        CHECK (13,  found,seq,offsetInSeq,offsetInDb, db3, 2,  2,  "AAA");
        CHECK (14,  found,seq,offsetInSeq,offsetInDb, db3, 0,  3,  "BBB");
        CHECK (15,  found,seq,offsetInSeq,offsetInDb, db3, 1,  4,  "BBB");
        CHECK (16,  found,seq,offsetInSeq,offsetInDb, db3, 2,  5,  "BBB");
        CHECK (17,  found,seq,offsetInSeq,offsetInDb, db3, 0,  6,  "CCC");
        CHECK (18,  found,seq,offsetInSeq,offsetInDb, db3, 1,  7,  "CCC");
        CHECK (19,  found,seq,offsetInSeq,offsetInDb, db3, 2,  8,  "CCC");
        CHECK (20,  found,seq,offsetInSeq,offsetInDb, db3, 0,  9,  "DDD");
        CHECK (21,  found,seq,offsetInSeq,offsetInDb, db3, 1, 10,  "DDD");
        CHECK (22,  found,seq,offsetInSeq,offsetInDb, db3, 2, 11,  "DDD");
        CHECK (23,  found,seq,offsetInSeq,offsetInDb, db3, 0, 12,  "EEE");
        CHECK (24,  found,seq,offsetInSeq,offsetInDb, db3, 1, 13,  "EEE");
        CHECK (25,  found,seq,offsetInSeq,offsetInDb, db3, 2, 14,  "EEE");
        CHECK (26,  found,seq,offsetInSeq,offsetInDb, db3, 0, 15,  "FFF");
        CHECK (27,  found,seq,offsetInSeq,offsetInDb, db3, 1, 16,  "FFF");
        CHECK (28,  found,seq,offsetInSeq,offsetInDb, db3, 2, 17,  "FFF");
    }

    /** */
    void testCompositeDatabase2 (void)
    {
        const char* filename = "sapiens_1Mo.fa";

        ISequenceDatabase* database = new BufferedSequenceDatabase (
            new FastaSequenceIterator (getPath(filename), 100),
            false
        );
        LOCAL (database);

        /** We build 6 frames databases. */
        list<ISequenceDatabase*> sixFrameDatabases;
        for (int frame=FRAME_1; frame<=FRAME_6; frame++)
        {
            /** We create a framed database from the nucleotid database. */
            ISequenceDatabase* frameDatabase = new ReadingFrameSequenceDatabase ((ReadingFrame_e)frame, database, false);
            CPPUNIT_ASSERT (frameDatabase != 0);

            /** We add it into the frame databases list. */
            sixFrameDatabases.push_back (frameDatabase);
        }
        CPPUNIT_ASSERT (sixFrameDatabases.size() == 6);

        /** We create a composite database. */
        ISequenceDatabase* compositeDb = new CompositeSequenceDatabase (sixFrameDatabases);
        CPPUNIT_ASSERT (compositeDb != 0);
        LOCAL (compositeDb);

        /** We iterate 6 frames sequences in two ways: from the 6 distinct databases and from the composite database. */
        ISequenceIterator* compositeIterator = compositeDb->createSequenceIterator();
        LOCAL (compositeIterator);

        vector<ISequenceIterator*> its;
        for (list<ISequenceDatabase*>::iterator it = sixFrameDatabases.begin(); it != sixFrameDatabases.end(); it++)
        {
            its.push_back ((*it)->createSequenceIterator());
        }

        size_t nb = 0;
        compositeIterator->first ();
        for (size_t i=0; i<its.size(); i++)
        {
            ISequenceIterator* current = its[i];

            for (current->first(); !current->isDone() && !compositeIterator->isDone(); current->next(), compositeIterator->next())
            {
                const ISequence* seq1 = current->currentItem();
                const ISequence* seq2 = compositeIterator->currentItem();

                CPPUNIT_ASSERT (seq1->data == seq2->data);

                nb++;
            }
        }

        CPPUNIT_ASSERT (nb == compositeDb->getSequencesNumber());
    }

    /** */
    void testDatabaseIteratorGet ()
    {
        const char* filename = "query.fa";

        /** */
        class MyCmd : public IteratorCommand<const ISequence*>
        {
        public:

            MyCmd (IteratorGet<const ISequence*>* it) : IteratorCommand<const ISequence*> (it)  { }

            void execute (const ISequence*& current, size_t& nbGot)
            {
                printf ("=> %ld  %s\n", nbGot, current->toString().c_str() );
            }
        };

        ISequenceDatabase* database = new BufferedCachedSequenceDatabase (
            new FastaSequenceIterator (getPath(filename), 100),
            false
        );
        LOCAL (database);

        /** We create a ISequence iterator from the database. */
        ISequenceIterator* it1 = database->createSequenceIterator();

        /** We create a "get" iterator from the ISequence iterator. */
        IteratorGet<const ISequence*>* it2 = new IteratorGet<const ISequence*> (it1);

#if 1
        /** We build a list of commands that will iterate our list, through the created iterator. */
        std::list<ICommand*> commands;
        for (size_t i=1; i<=1; i++)  {  commands.push_back (new MyCmd (it2)); }

        /** We dispatch the commands to a dispatcher. */
        SerialCommandDispatcher().dispatchCommands (commands, 0);
#else
      IteratorGet<const ISequence*> itGet (it);

      const ISequence* current = 0;
      size_t nbGot = 0;

      while (itGet.get (current,nbGot))
      {
          printf ("---> (got=%ld)  %s  %s\n", nbGot, current->toString().c_str(), current->comment );
      }
#endif
    }
};

/********************************************************************************/

extern "C" Test* TestSequenceDatabase_suite()  { return TestSequenceDatabase::suite (); }
