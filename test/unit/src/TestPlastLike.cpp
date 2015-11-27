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

#include <misc/api/types.hpp>

#include <designpattern/api/IObserver.hpp>
#include <designpattern/impl/CommandDispatcher.hpp>

#include <os/impl/DefaultOsFactory.hpp>
#include <os/impl/TimeTools.hpp>

#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/StringSequenceIterator.hpp>
#include <database/impl/ReadingFrameSequenceIterator.hpp>
#include <database/impl/BufferedSequenceDatabase.hpp>
#include <database/impl/ReadingFrameSequenceDatabase.hpp>

#include <seed/impl/BasicSeedModel.hpp>
#include <seed/impl/SubSeedModel.hpp>

#include <index/impl/DatabaseIndex.hpp>

#include <algo/core/impl/DefaultAlgoConfig.hpp>
#include <algo/core/impl/BasicAlgoIndexator.hpp>
#include <algo/core/impl/ScoreMatrix.hpp>

#include <algo/hits/seed/SeedHitIterator.hpp>
#include <algo/hits/gap/SmallGapHitIterator.hpp>
#include <algo/hits/gap/SmallGapHitIteratorSSE8.hpp>

#include <map>

using namespace std;
using namespace misc;
using namespace os;
using namespace os::impl;
using namespace dp;
using namespace dp::impl;
using namespace database;
using namespace database::impl;
using namespace seed;
using namespace seed::impl;
using namespace indexation;
using namespace indexation::impl;
using namespace algo::core;
using namespace algo::core::impl;
using namespace algo::hits;

/********************************************************************************/

class TestPlastLike : public TestFixture//, public IObserver
{
private:
    IConfiguration*     _config;
    ICommandDispatcher* _dispatcher;

public:

    /********************************************************************************/
    void setUp ()
    {
        _config =  new DefaultConfiguration (0,0);
        _config->use ();

        _dispatcher = new ParallelCommandDispatcher  ();
        //_dispatcher = new SerialCommandDispatcher ();
        _dispatcher->use ();
    }
    void tearDown ()
    {
        _config->forget();
        _dispatcher->forget ();
    }

    /********************************************************************************/
    /********************************************************************************/
    class HitIterationCommand : public ICommand
    {
    public:
        HitIterationCommand (IHitIterator* it, void* client, Iterator<Hit*>::Method method)
            :  _it(it), _client(client), _method(method), _nbHits(0)  {  if (_it)  { _it->use(); }  }
        virtual ~HitIterationCommand ()  {  if (_it)  { _it->forget(); } }
        void execute ()  {  _it->iterate (_client, _method);  }
        u_int64_t getNbHits  () { return _nbHits; }
    private:
        IHitIterator*   _it;
        void*                        _client;
        Iterator<Hit*>::Method _method;
        size_t _nbHits;
    };

    /********************************************************************************/
    /********************************************************************************/
    static Test* suite()
    {
    	 TestSuite* result = new TestSuite ("PlastLikeTest");
         result->addTest (new TestCaller<TestPlastLike> ("test_tplastn_unitary", &TestPlastLike::test_tplastn_unitary ) );
         return result;
    }

    /********************************************************************************/
    void update (EventInfo* evt)
    {
        IterationStatusEvent* e1 = dynamic_cast<IterationStatusEvent*> (evt);
        if (e1 != 0)
        {
            u_int64_t got   = e1->getCurrentIndex();
            u_int64_t total = e1->getTotalNumber();

            if (total > 0)
            {
                static int threshold = 0;

                int percent = (int) (100.0 * (float) got / (float) total);

                if (percent >= threshold)
                {
                    printf ("update  %d percent  (%lld on %lld)\n",
                        percent,
                        e1->getCurrentIndex(),
                        e1->getTotalNumber()
                    );

                    threshold += 2;
                }
            }
        }
    }


    /********************************************************************************/
    int       _frame;
    u_int64_t _nbSeedsAlignments;
    u_int64_t _nbUngapsAlignments;

    /** */
    void testSeedsAlignment_aux (const Hit* hit)
    {
        __sync_fetch_and_add (&_nbSeedsAlignments,  1 );

#if 0
         printf ("frame=%d  _nbSeedsAlignments=%d  %ld %ld\n",
             _frame,
             _nbSeedsAlignments,
             hit->occur1.size, hit->occur2.size
         );
         printf ("  => '%s'\n", hit->occur1.data[0]->neighbourhood.toString().c_str());
         printf ("  => '%s'\n", hit->occur2.data[0]->neighbourhood.toString().c_str());
         return;
#endif

#if 1
         if (_frame==FRAME_1  &&  _nbSeedsAlignments==1)   // SCORE 6
         {
             CPPUNIT_ASSERT (hit->occur1.data[0]->neighbourhood.toString().compare ("MYEIMXTG") == 0);
             CPPUNIT_ASSERT (hit->occur2.data[0]->neighbourhood.toString().compare ("MWDITXRI") == 0);

             CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInSequence == 5);
             CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInDatabase == 5);

             CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInSequence == 5);
             CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInDatabase == 5);
         }

         if (_frame==FRAME_1  &&  _nbSeedsAlignments==2)   // SCORE 32
         {
             CPPUNIT_ASSERT (hit->occur1.data[0]->neighbourhood.toString().compare ("QWLGTMXX") == 0);
             CPPUNIT_ASSERT (hit->occur2.data[0]->neighbourhood.toString().compare ("QWMGTMRF") == 0);

             CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInSequence == 0);
             CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInDatabase == 0);

             CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInSequence ==  2);
             CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInDatabase == 12);
         }

         if (_frame==FRAME_1  &&  _nbSeedsAlignments==3)   // SCORE 31
         {
             CPPUNIT_ASSERT (hit->occur1.data[0]->neighbourhood.toString().compare ("WLGTMYQX") == 0);
             CPPUNIT_ASSERT (hit->occur2.data[0]->neighbourhood.toString().compare ("WMGTMRQR") == 0);

             CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInSequence == 1);
             CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInDatabase == 1);

             CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInSequence ==  3);
             CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInDatabase == 13);
         }

         if (_frame==FRAME_5  &&  _nbSeedsAlignments==5)   // SCORE 23
         {
             CPPUNIT_ASSERT (hit->occur1.data[0]->neighbourhood.toString().compare ("VPSHXXIY") == 0);
             CPPUNIT_ASSERT (hit->occur2.data[0]->neighbourhood.toString().compare ("VPSHXXIR") == 0);

             CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInSequence == 5);
             CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInDatabase == 5);

             CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInSequence == 10);
             CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInDatabase == 20);
         }

         if (_frame==FRAME_5  &&  _nbSeedsAlignments==4)   // SCORE 23
         {
             CPPUNIT_ASSERT (hit->occur1.data[0]->neighbourhood.toString().compare ("IVPSHXYS") == 0);
             CPPUNIT_ASSERT (hit->occur2.data[0]->neighbourhood.toString().compare ("IVPSHXRM") == 0);

             CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInSequence == 4);
             CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInDatabase == 4);

             CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInSequence ==  9);
             CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInDatabase == 19);
         }

         if (_frame==FRAME_6  &&  _nbSeedsAlignments==6)   // SCORE 13
         {
             CPPUNIT_ASSERT (hit->occur1.data[0]->neighbourhood.toString().compare ("YLAIXX*T") == 0);
             CPPUNIT_ASSERT (hit->occur2.data[0]->neighbourhood.toString().compare ("YLAIRMXX") == 0);

             CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInSequence == 5);
             CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInDatabase == 5);

             CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInSequence == 0);
             CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInDatabase == 0);
         }
#endif
    }

    /** */
    void testUngapAlignment_aux (const Hit* hit)
    {
        /** We are interested only in positive scores. */
        if (hit->indexes.empty())  { printf ("rhoo\n");  return; }

        __sync_fetch_and_add (&_nbUngapsAlignments, 1 );

#if 0
         printf ("frame=%d  %ld %ld => score=%d \n", _frame, hit->occur1.size, hit->occur2.size, hit->scores[0][0]);
         printf ("  => '%s'\n", hit->occur1.data[0]->neighbourhood.toString().c_str());
         printf ("  => '%s'\n", hit->occur2.data[0]->neighbourhood.toString().c_str());
         return;
#endif

#if 1
        if (_frame==FRAME_1  &&  _nbUngapsAlignments==1)   // SCORE 32
        {
            CPPUNIT_ASSERT (hit->occur1.data[0]->neighbourhood.toString().compare ("QWLGTMXX") == 0);
            CPPUNIT_ASSERT (hit->occur2.data[0]->neighbourhood.toString().compare ("QWMGTMRF") == 0);

            CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInSequence == 0);
            CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInDatabase == 0);

            CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInSequence ==  2);
            CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInDatabase == 12);
        }

        if (_frame==FRAME_1  &&  _nbUngapsAlignments==2)   // SCORE 31
        {
            CPPUNIT_ASSERT (hit->occur1.data[0]->neighbourhood.toString().compare ("WLGTMYQX") == 0);
            CPPUNIT_ASSERT (hit->occur2.data[0]->neighbourhood.toString().compare ("WMGTMRQR") == 0);

            CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInSequence == 1);
            CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInDatabase == 1);

            CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInSequence ==  3);
            CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInDatabase == 13);
        }

        if (_frame==FRAME_5  &&  _nbUngapsAlignments==3)   // SCORE 23
        {
            CPPUNIT_ASSERT (hit->occur1.data[0]->neighbourhood.toString().compare ("IVPSHXYS") == 0);
            CPPUNIT_ASSERT (hit->occur2.data[0]->neighbourhood.toString().compare ("IVPSHXRM") == 0);

            CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInSequence == 4);
            CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInDatabase == 4);

            CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInSequence ==  9);
            CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInDatabase == 19);
        }

        if (_frame==FRAME_5  &&  _nbUngapsAlignments==4)   // SCORE 23
        {
            CPPUNIT_ASSERT (hit->occur1.data[0]->neighbourhood.toString().compare ("VPSHXXIY") == 0);
            CPPUNIT_ASSERT (hit->occur2.data[0]->neighbourhood.toString().compare ("VPSHXXIR") == 0);

            CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInSequence == 5);
            CPPUNIT_ASSERT (hit->occur1.data[0]->offsetInDatabase == 5);

            CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInSequence == 10);
            CPPUNIT_ASSERT (hit->occur2.data[0]->offsetInDatabase == 20);
        }
#endif
    }

    /********************************************************************************/
    void dispatchHitIterate (IHitIterator* itHit, ICommandDispatcher* dispatcher, void* client, Iterator<Hit*>::Method method)
    {
        CPPUNIT_ASSERT (itHit != 0);
        CPPUNIT_ASSERT (dispatcher != 0);
        CPPUNIT_ASSERT (client != 0);

        LOCAL (dispatcher);

        /** We split the iterator in several iterators. */
        std::vector<IHitIterator*> its = itHit->split (dispatcher->getExecutionUnitsNumber());

        /** We create a list of commands for iterating the hits. */
        list<ICommand*> commands;
        for (size_t i=0; i<its.size(); i++)
        {
            /** We create a new command. */
            commands.push_back (new HitIterationCommand (its[i], client, method));
        }

        /** We run the commands through a dispatcher. */
        dispatcher->dispatchCommands (commands, 0);
    }

    /********************************************************************************/
    void test_tplastn_unitary ()
    {
        /** We create specifically a serial command dispatcher (don't use parallel for this test). */
        ICommandDispatcher* dispatcher = new SerialCommandDispatcher ();
        CPPUNIT_ASSERT (dispatcher != 0);
        LOCAL (dispatcher);

        /** We create a seed model. */
        ISeedModel* seedModel = new SubSeedModel (4,
            "A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y",
            "CFYWMLIV,GPATSNHQEDRK",
            "A,C,FYW,G,IV,ML,NH,P,QED,RK,TS",
            "A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y"
        );
        CPPUNIT_ASSERT (seedModel != 0);
        LOCAL (seedModel);

        /** We create a score matrix. */
        IScoreMatrix* scoreMatrix = ScoreMatrixManager::singleton().getMatrix ("BLOSUM62", SUBSEED, 0, 0);
        CPPUNIT_ASSERT (scoreMatrix != 0);
        LOCAL (scoreMatrix);

        /** We get the nucleotid database and the query database. */
        ISequenceDatabase* nucleotidDatabase = new BufferedSequenceDatabase (
            new StringSequenceIterator (1, "CAATGGCTAGGTACTATGTATGAGATCATG"),
            false
        );
        CPPUNIT_ASSERT (nucleotidDatabase != 0);
        LOCAL (nucleotidDatabase);

        ISequenceDatabase* queryDatabase = new BufferedSequenceDatabase (
            new StringSequenceIterator (2, "YLAIRMWDIT", "FRQWMGTMRIVPSH"),
            false
        );
        CPPUNIT_ASSERT (queryDatabase != 0);
        LOCAL (queryDatabase);

        /** We build 6 frames databases. */
        list<ISequenceDatabase*> sixFrameDatabases;
        for (_frame=FRAME_1; _frame<=FRAME_6; _frame++)
        {
            /** We create a framed database from the nucleotid database. */
            ISequenceDatabase* frameDatabase = new ReadingFrameSequenceDatabase ((ReadingFrame_e)_frame, nucleotidDatabase, false);
            CPPUNIT_ASSERT (frameDatabase != 0);

            /** We add it into the frame databases list. */
            sixFrameDatabases.push_back (frameDatabase);
        }

        _nbSeedsAlignments  = 0;
        _nbUngapsAlignments = 0;

        IParameters* params = _config->createDefaultParameters("tplastn");
        LOCAL (params);
        params->ungapNeighbourLength = 2;
        params->ungapScoreThreshold  = 20;

        /** We loop over each frame database. */
        _frame = FRAME_1;
        for (list<ISequenceDatabase*>::iterator itDb = sixFrameDatabases.begin(); itDb != sixFrameDatabases.end(); itDb++, _frame++)
        {
            bool isRunning = true;
            IIndexator* indexator = new BasicSortedIndexator (seedModel, params, new DatabaseIndexFactory(), 1.0, isRunning);
            CPPUNIT_ASSERT (indexator != 0);
            LOCAL (indexator);

            /** We build the indexes. */
            indexator->setSubjectDatabase (*itDb);
            indexator->setQueryDatabase   (queryDatabase);
            indexator->build (dispatcher);

            /** We create a hit iterator. */
            IHitIterator* itHit = indexator->createHitIterator();
            CPPUNIT_ASSERT (itHit != 0);
            LOCAL (itHit);

            /** We dispatch the iteration through a command dispatcher. */
            dispatchHitIterate (
                itHit,
                dispatcher,
                this,
                (Iterator<Hit*>::Method) &TestPlastLike::testSeedsAlignment_aux
            );

            IHitIterator* ungapIterator = _config->createUngapHitIterator (
                itHit,
                seedModel,
                scoreMatrix,
                params,
                0,
                isRunning
            );
            CPPUNIT_ASSERT (ungapIterator != 0);
            LOCAL (ungapIterator);

            /** We dispatch the iteration through a command dispatcher. */
            dispatchHitIterate (
                ungapIterator,
                dispatcher,
                this,
                (Iterator<Hit*>::Method) &TestPlastLike::testUngapAlignment_aux
            );
        }

        //printf ("FOUND: %d SEEDS ALIGNEMENTS,  %d UNGAP ALIGNMENTS \n", _nbSeedsAlignments, _nbUngapsAlignments);
    }
};

/********************************************************************************/

extern "C" Test* TestPlastLike_suite()  { return TestPlastLike::suite (); }

