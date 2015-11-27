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

#include <database/impl/BufferedSequenceDatabase.hpp>
#include <database/impl/ReadingFrameSequenceDatabase.hpp>
#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/StringSequenceIterator.hpp>
#include <database/impl/ReadingFrameSequenceIterator.hpp>

#include <seed/impl/BasicSeedModel.hpp>
#include <seed/impl/SubSeedModel.hpp>

using namespace std;
using namespace database;
using namespace database::impl;
using namespace seed;
using namespace seed::impl;

extern const char* getPath (const char* file);

/********************************************************************************/

class TestSeedIterator : public TestFixture
{
private:
    ISequenceIterator* fastaIterator;
    ISequenceIterator* stringIterator;
    ISeedModel*        modelSpan3;
    ISeedModel*        modelSpan4;

public:

    /********************************************************************************/
    void setUp ()
    {
        fastaIterator = new FastaSequenceIterator (getPath ("sapiens_1Mo.fa"), 100);
        fastaIterator->use ();

        stringIterator = new StringSequenceIterator (3, "AAAIAAA", "KLAAA", "FAAA");
        stringIterator->use ();

        modelSpan3 = new BasicSeedModel (SUBSEED, 3);

        modelSpan4 = new SubSeedModel (4,
            "A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y",
            "CFYWMLIV,GPATSNHQEDRK",
            "A,C,FYW,G,IV,ML,NH,P,QED,RK,TS",
            "A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y"
        );

        modelSpan3->use();
        modelSpan4->use();
    }

    void tearDown ()
    {
        fastaIterator->forget ();
        stringIterator->forget ();
        modelSpan3->forget ();
        modelSpan4->forget ();
    }

    /********************************************************************************/
    static Test* suite()
    {
    	 TestSuite* result = new TestSuite ("SeedIteratorTest");

    	 result->addTest (new TestCaller<TestSeedIterator> ("testSeedIteratorFasta", 	    &TestSeedIterator::testSeedIteratorFasta ) );
    	 result->addTest (new TestCaller<TestSeedIterator> ("testSeedIteratorContent",      &TestSeedIterator::testSeedIteratorContent ) );
         result->addTest (new TestCaller<TestSeedIterator> ("testSeedIteratorContent2",     &TestSeedIterator::testSeedIteratorContent2 ) );
         result->addTest (new TestCaller<TestSeedIterator> ("testSeedIteratorBadLetters",   &TestSeedIterator::testSeedIteratorBadLetters ) );
         result->addTest (new TestCaller<TestSeedIterator> ("testSeedIteratorNucleotid",   &TestSeedIterator::testSeedIteratorNucleotid) );

    	 return result;
    }

    /********************************************************************************/
    /* Test all seeds of all sequences of a fasta file. */
    /********************************************************************************/
    void testSeedIteratorFasta ()
    {
        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (fastaIterator, false);
        LOCAL (database);

        CPPUNIT_ASSERT (database != 0);
        CPPUNIT_ASSERT (database->getSequencesNumber() > 0);

        /** We create an iterator over the database. */
        ISequenceIterator* itSeq = database->createSequenceIterator();
        LOCAL (itSeq);

        /** We loop over the sequences. */
        for (itSeq->first(); ! itSeq->isDone(); itSeq->next ())
        {
            const ISequence* currentSeq = itSeq->currentItem();
            CPPUNIT_ASSERT (currentSeq != 0);

            /** We create a seed iterator with the data of the current sequence. */
            ISeedIterator* itSeed =  modelSpan3->createSeedsIterator (currentSeq->data);
            CPPUNIT_ASSERT (itSeed != 0);
            LOCAL (itSeed);

            /** We loop over all seeds for the current sequence. */
            for (itSeed->first(); !itSeed->isDone(); itSeed->next())
            {
                const ISeed* seed = itSeed->currentItem();
                CPPUNIT_ASSERT (seed != 0);
                CPPUNIT_ASSERT (seed->code <= modelSpan3->getSeedsMaxNumber());
            }
        }
    }

    /********************************************************************************/
    /* Test all seeds of all sequences. */
    /********************************************************************************/
    void testSeedIteratorContent ()
    {
        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (stringIterator, false);
        LOCAL (database);

        CPPUNIT_ASSERT (database != 0);
        CPPUNIT_ASSERT (database->getSequencesNumber() > 0);

        /** We create an iterator over the database. */
        ISequenceIterator* itSeq = database->createSequenceIterator();
        LOCAL (itSeq);

        size_t nbSequences  = 0;
        size_t nbSeeds      = 0;
        size_t totalNbSeeds = 0;

        /** We loop over the sequences. */
        for (itSeq->first(); ! itSeq->isDone(); itSeq->next ())
        {
            const ISequence* currentSeq = itSeq->currentItem();
            CPPUNIT_ASSERT (currentSeq != 0);
            //printf ("Sequence='%s' \n", currentSeq->data.toString().c_str());

            /** We create a seed iterator with the data of the current sequence. */
            ISeedIterator* itSeed = modelSpan3->createSeedsIterator (currentSeq->data);
            CPPUNIT_ASSERT (itSeed != 0);
            LOCAL (itSeed);

            nbSeeds = 0;

            /** We loop over all seeds for the current sequence. */
            for (itSeed->first(); !itSeed->isDone(); itSeed->next(), totalNbSeeds++)
            {
                const ISeed* seed = itSeed->currentItem();
                CPPUNIT_ASSERT (seed != 0);

                /** A little shortcut. */
                string content = seed->kmer.toString();
                //printf ("seed='%s'  nbSeq=%ld  nbSeeds=%ld\n", content.c_str(), nbSequences, nbSeeds);

                if (nbSequences == 0)
                {
                    if (nbSeeds == 0)  { CPPUNIT_ASSERT (content.compare ("AAA") == 0);  }
                    if (nbSeeds == 1)  { CPPUNIT_ASSERT (content.compare ("AAI") == 0);  }
                    if (nbSeeds == 2)  { CPPUNIT_ASSERT (content.compare ("AIA") == 0);  }
                    if (nbSeeds == 3)  { CPPUNIT_ASSERT (content.compare ("IAA") == 0);  }
                    if (nbSeeds == 4)  { CPPUNIT_ASSERT (content.compare ("AAA") == 0);  }
                }

                if (nbSequences == 1)
                {
                    if (nbSeeds == 0)  { CPPUNIT_ASSERT (content.compare ("KLA") == 0);  }
                    if (nbSeeds == 1)  { CPPUNIT_ASSERT (content.compare ("LAA") == 0);  }
                    if (nbSeeds == 2)  { CPPUNIT_ASSERT (content.compare ("AAA") == 0);  }
                }

                if (nbSequences == 2)
                {
                    if (nbSeeds == 0)  { CPPUNIT_ASSERT (content.compare ("FAA") == 0);  }
                    if (nbSeeds == 1)  { CPPUNIT_ASSERT (content.compare ("AAA") == 0);  }
                }

                nbSeeds++;
            }

            nbSequences++;
        }

        CPPUNIT_ASSERT (totalNbSeeds == 10);
    }

    /********************************************************************************/
    /********************************************************************************/
    void testSeedIteratorContent2 ()
    {
        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (
            new StringSequenceIterator (1,
                "AGMLLPQKXXXXXXXXXXKLMNRP"
            ),
            false
        );
        LOCAL (database);

        CPPUNIT_ASSERT (database != 0);
        CPPUNIT_ASSERT (database->getSequencesNumber() == 1);

        /** We create an iterator over the database. */
        ISequenceIterator* itSeq = database->createSequenceIterator();
        LOCAL (itSeq);

        /** We retrieve the sequence from the database. */
        itSeq->first();
        const ISequence* currentSeq = itSeq->currentItem();
        //printf ("seq='%s'\n", currentSeq->toString().c_str());

        /** We create a seed iterator with the data of the current sequence. */
        ISeedIterator* itSeed = modelSpan4->createSeedsIterator (currentSeq->data);
        CPPUNIT_ASSERT (itSeed != 0);
        LOCAL (itSeed);

#if 0
        /** We loop over all seeds for the current sequence. */
        for (itSeed->first(); !itSeed->isDone(); itSeed->next())
        {
            const ISeed* seed = itSeed->currentItem();
            CPPUNIT_ASSERT (seed != 0);

            /** A little shortcut. */
            string content = seed->kmer.toString();
            printf ("'%s'\n", content.c_str());
        }
#else

        u_int64_t nbRetrieved;
        itSeed->first();
        for (ISeed seed; itSeed->retrieve (seed, nbRetrieved);  )
        {
            /** A little shortcut. */
            string content = seed.kmer.toString();
            printf ("'%s'\n", content.c_str());
        }
#endif
    }

    /********************************************************************************/
    /********************************************************************************/
    void testSeedIteratorBadLetters ()
    {
        ISeedModel* model = modelSpan4;

        /** We create a database from an iterator. */
        ISequenceDatabase* database = new BufferedSequenceDatabase (
            new StringSequenceIterator (1,
                //"AGXMLLPQKXXXAPQRS*LMNRP"
                //"AXGMLLPQKXXXAPQRS*LMNRP"
                "APFFXGMLLPQS*LMNRP"
            ),
            false
        );
        LOCAL (database);

        CPPUNIT_ASSERT (database != 0);
        CPPUNIT_ASSERT (database->getSequencesNumber() == 1);

        /** We retrieve the sequence. */
        ISequence seq;
        CPPUNIT_ASSERT (database->getSequenceByIndex (0, seq) == true);

        /** We create a seed iterator with the data of the current sequence. */
        ISeedIterator* itSeed = model->createSeedsIterator (seq.data);
        CPPUNIT_ASSERT (itSeed != 0);
        LOCAL (itSeed);

        Offset previousOffset = 0;
        vector<Offset> badIntervals;

        /** Shortcut. */
        size_t span = model->getSpan();

        for (itSeed->first(); !itSeed->isDone(); itSeed->next())
        {
            Offset currentOffset = itSeed->currentItem()->offset;

            if ( (currentOffset > span) && (currentOffset != previousOffset + 1) )
            {
                printf ("BAD [%lld,%lld]\n", previousOffset + span,  currentOffset  - 1);

                badIntervals.push_back (previousOffset + span);
                badIntervals.push_back (currentOffset  - 1);
            }

            previousOffset = currentOffset;
        }

        /** We copy the sequence data. */
        IWord copySeq (seq.data.letters.size, seq.data.letters.data);

        LETTER* data = copySeq.letters.data;

        size_t nbInarowLettersRequired = 6;

        for (size_t i=2; i<badIntervals.size(); i+=2)
        {
            Offset off0 = badIntervals[i-1];
            Offset off1 = badIntervals[i];

            if ( (data[off1] == CODE_STAR)  &&  (off1 - off0 - 1 <= nbInarowLettersRequired) )
            {
                //for (size_t j=off0+1; j<=off1-1; j++)  {  data[j] = CODE_X;  }

                memset (data+off0+1, CODE_X, off1-off0-1);

//                printf ("BAD interval [%ld,%ld]  (previous [%ld,%ld]) => invalidate [%ld,%ld] \n",
//                    badIntervals[i+0], badIntervals[i+1],
//                    badIntervals[i-2], badIntervals[i-1],
//                    badIntervals[i-1]+1, badIntervals[i+0]-1
//                );
            }
        }

        printf ("\n");
        printf ("data  ='%s'\n", seq.data.toString().c_str());
        printf ("modif ='%s'\n", copySeq.toString().c_str());

        /** We create a seed iterator with the data of the current sequence. */
        ISeedIterator* itSeed2 = model->createSeedsIterator (copySeq);
        CPPUNIT_ASSERT (itSeed2 != 0);
        LOCAL (itSeed2);

        for (itSeed2->first(); !itSeed2->isDone(); itSeed2->next())
        {
            const ISeed* current = itSeed2->currentItem();
            printf ("'%s'\n", current->kmer.toString().c_str());
        }
    }

    void testSeedIteratorNucleotid ()
    {
        /** WARNING !  We first switch to nucleotide alphabet before creating the instance. */
        EncodingManager::singleton().setKind (EncodingManager::ALPHABET_NUCLEOTID);

        /** We create a model. */
        ISeedModel* model = new BasicSeedModel (SUBSEED, 11);
        CPPUNIT_ASSERT (model != 0);
        LOCAL (model);

        size_t nbSeeds = 0;

        /** We create a seed iterator with the data of the current sequence. */
        ISeedIterator* itSeed = model->createAllSeedsIterator();
        CPPUNIT_ASSERT (itSeed != 0);
        LOCAL (itSeed);

        for (itSeed->first(); !itSeed->isDone(); itSeed->next())
        {
            const ISeed* current = itSeed->currentItem();
            //printf ("'%s'\n", current->kmer.toString().c_str());
            nbSeeds++;
        }

        CPPUNIT_ASSERT (nbSeeds == 4194304);  // 4^11 = 4194304

        /** WARNING !  We switch back to amnino acid alphabet. */
        EncodingManager::singleton().setKind (EncodingManager::ALPHABET_AMINO_ACID);
    }
};

/********************************************************************************/

extern "C" Test* TestSeedIterator_suite()  { return TestSeedIterator::suite (); }
