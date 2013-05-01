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

#include <index/impl/DatabaseIndex.hpp>

#include <algo/hits/seed/SeedHitIterator.hpp>
#include <algo/hits/seed/SeedHitIteratorCached.hpp>

#include <set>

using namespace std;
using namespace dp;
using namespace database;
using namespace database::impl;
using namespace seed;
using namespace seed::impl;
using namespace indexation;
using namespace indexation::impl;
using namespace algo::hits;
using namespace algo::hits::seed;

extern const char* getPath (const char* file);

/********************************************************************************/

class TestDatabaseCompare : public TestFixture
{
    ISequenceIterator* fastaIterator;

public:

    /********************************************************************************/
    void setUp ()
    {
        fastaIterator = new FastaSequenceIterator (getPath("tursiops.fa"), 100);
        fastaIterator->use ();
    }

    void tearDown ()
    {
        fastaIterator->forget ();
    }

    /********************************************************************************/
    static Test* suite()
    {
    	 TestSuite* result = new TestSuite ("DatabaseCompareTest");

    	 result->addTest (new TestCaller<TestDatabaseCompare> ("testIndexCompareSequences",       &TestDatabaseCompare::testIndexCompareSequences ) );
    	 result->addTest (new TestCaller<TestDatabaseCompare> ("testIndexCompareSequencesFasta",  &TestDatabaseCompare::testIndexCompareSequencesFasta ) );
    	 result->addTest (new TestCaller<TestDatabaseCompare> ("testHitIterator",                 &TestDatabaseCompare::testHitIterator ) );
         return result;
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testIndexCompareSequences ()
    {
        /** We create two databases. */
        ISequenceDatabase* db1 = new BufferedSequenceDatabase (
            new StringSequenceIterator (2,
                "AADDPP", "CCFFF"
            ), false
        );
        LOCAL (db1);
        CPPUNIT_ASSERT (db1 != 0);

        ISequenceDatabase* db2 = new BufferedSequenceDatabase (
            new StringSequenceIterator (3,
                "XAAADD", "TTT", "PCFF"
            ), false
        );
        LOCAL (db2);
        CPPUNIT_ASSERT (db2 != 0);

        /** We create a model. */
        ISeedModel* model = new BasicSeedModel (SUBSEED, 3);
        LOCAL (model);
        CPPUNIT_ASSERT (model != 0);

        /** We index the two databases. */
        DatabaseIndex index1 (db1, model);
        DatabaseIndex index2 (db2, model);

        /** We build the indexes. */
        index1.build ();
        index2.build ();

        /** We want to count the number of matches between the databases.  We should have 3 matches. */
        size_t nbMatches = 0;

        /** We loop over all possible seeds. */
        ISeedIterator* itSeed = model->createAllSeedsIterator();
        CPPUNIT_ASSERT (itSeed != 0);
        LOCAL (itSeed);

        size_t nbIter = 0;

        /** We loop over all possible seeds for the given model. */
        for (itSeed->first(); !itSeed->isDone(); itSeed->next())
        {
            /** */
            const ISeed* seed = itSeed->currentItem();

            /** We create an occurrence iterator. */
            IOccurrenceIterator* it1 = index1.createOccurrenceIterator (seed);
            if (it1 == 0)  { continue; }
            LOCAL (it1);

            for (it1->first(); ! it1->isDone(); it1->next ())
            {
                const ISeedOccurrence* o1 = it1->currentItem();

                /** We retrieve the sequence given its offset. */
                ISequence seq1;
                u_int32_t offset1;
                u_int64_t offsetDb1;
                db1->getSequenceByOffset (o1->offsetInDatabase, seq1, offset1, offsetDb1);

                /** We create an occurrence iterator. */
                IOccurrenceIterator* it2 = index2.createOccurrenceIterator (seed);
                if (it2 == 0)  { continue; }
                LOCAL (it2);

                for (it2->first(); ! it2->isDone(); it2->next ())
                {
                    const ISeedOccurrence* o2 = it2->currentItem();

                    /** We retrieve the sequence given its offset. */
                    ISequence seq2;
                    u_int32_t offset2;
                    u_int64_t offsetDb2;
                    db2->getSequenceByOffset (o2->offsetInDatabase, seq2, offset2, offsetDb2);

                    nbIter++;

                    if (nbIter==1)  {  CPPUNIT_ASSERT (o1->offsetInDatabase == 0  && o2->offsetInDatabase == 2);    nbMatches++;  }
                    if (nbIter==2)  {  CPPUNIT_ASSERT (o1->offsetInDatabase == 1  && o2->offsetInDatabase == 3);    nbMatches++;  }
                    if (nbIter==3)  {  CPPUNIT_ASSERT (o1->offsetInDatabase == 7  && o2->offsetInDatabase == 10);   nbMatches++;  }
                }
            }
        }

        CPPUNIT_ASSERT (nbMatches == 3);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testIndexCompareSequencesFasta ()
    {
        /** We create two databases. */
        ISequenceDatabase* db1 = new BufferedSequenceDatabase (fastaIterator, false);
        LOCAL (db1);
        CPPUNIT_ASSERT (db1 != 0);

        ISequenceDatabase* db2 = new BufferedSequenceDatabase (new StringSequenceIterator (1, "XAAA"), false);
        LOCAL (db2);
        CPPUNIT_ASSERT (db2 != 0);

        /** We create a model. */
        ISeedModel* model = new BasicSeedModel (SUBSEED, 3);
        LOCAL (model);
        CPPUNIT_ASSERT (model != 0);

        /** We index the two databases. */
        DatabaseIndex index1 (db1, model);
        DatabaseIndex index2 (db2, model);

        /** We build the indexes. */
        index1.build ();
        index2.build ();

        /** We loop over all possible seeds. */
        ISeedIterator* itSeed = model->createAllSeedsIterator();
        CPPUNIT_ASSERT (itSeed != 0);
        LOCAL (itSeed);

        for (itSeed->first(); !itSeed->isDone(); itSeed->next())
        {
            /** */
            const ISeed* seed = itSeed->currentItem();

            /** We create an occurrence iterator. */
            IOccurrenceIterator* it1 = index1.createOccurrenceIterator (seed);
            if (it1 == 0)  { continue; }
            LOCAL (it1);

            for (it1->first(); ! it1->isDone(); it1->next ())
            {
                //const ISeedOccurrence* o1 = it1->currentItem();

                /** We create an occurrence iterator. */
                IOccurrenceIterator* it2 = index2.createOccurrenceIterator (seed);
                if (it2 == 0)  { continue; }
                LOCAL (it2);

                for (it2->first(); ! it2->isDone(); it2->next ())
                {
                    //const ISeedOccurrence* o2 = it2->currentItem();

                }
            }
        }
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testHitIterator ()
    {
        /** We create two databases. */
        ISequenceDatabase* db1 = new BufferedSequenceDatabase (
            new StringSequenceIterator (2,
                "AADDPP", "CCFFF"
            ), false
        );
        LOCAL (db1);
        CPPUNIT_ASSERT (db1 != 0);

        ISequenceDatabase* db2 = new BufferedSequenceDatabase (
            new StringSequenceIterator (3,
                "XAAADD", "TTT", "PCFF"
            ), false
        );
        LOCAL (db2);
        CPPUNIT_ASSERT (db2 != 0);

        /** We create a model. */
        ISeedModel* model = new BasicSeedModel (SUBSEED, 3);
        LOCAL (model);
        CPPUNIT_ASSERT (model != 0);

        /** We index the two databases. */
        IDatabaseIndex* index1 = new DatabaseIndex (db1, model);
        CPPUNIT_ASSERT (index1 != 0);
        LOCAL (index1);

        IDatabaseIndex* index2 = new DatabaseIndex (db2, model);
        CPPUNIT_ASSERT (index2 != 0);
        LOCAL (index2);

        /** We build the indexes. */
        index1->build ();
        index2->build ();


        /** We create a hit iterator for the two indexes. */
        bool isRunning = true;
        IHitIterator* hitIt = new SeedHitIteratorCached (index1, index2, 0, 1.0, isRunning);
        CPPUNIT_ASSERT (hitIt != 0);
        LOCAL (hitIt);

        /** We loop the hits. */
        for (hitIt->first(); ! hitIt->isDone(); hitIt->next())
        {
            const Hit* hit = hitIt->currentItem();
            CPPUNIT_ASSERT (hit != 0);

//            printf ("[db=%p  offdb=%ld  offseq=%d seq[%s] ]\n",
//                hit->occur1->database,
//                hit->occur1->offsetInDatabase,
//                hit->occur1->offsetInSequence,
//                hit->occur1->sequence.data.toString().c_str()
//            );
//
//            printf ("[db=%p  offdb=%ld  offseq=%d seq[%s] ]\n",
//                hit->occur2->database,
//                hit->occur2->offsetInDatabase,
//                hit->occur2->offsetInSequence,
//                hit->occur2->sequence.data.toString().c_str()
//            );
//
//            printf ("\n");
        }
    }

};

/********************************************************************************/

extern "C" Test* TestDatabaseCompare_suite()  { return TestDatabaseCompare::suite (); }
