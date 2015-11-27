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

#include <os/impl/DefaultOsFactory.hpp>
#include <os/impl/TimeTools.hpp>

#include <designpattern/impl/CommandDispatcher.hpp>

#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/StringSequenceIterator.hpp>
#include <database/impl/ReadingFrameSequenceIterator.hpp>
#include <database/impl/BufferedSequenceDatabase.hpp>
#include <database/impl/ReadingFrameSequenceDatabase.hpp>

#include <index/impl/DatabaseIndex.hpp>

#include <seed/impl/BasicSeedModel.hpp>
#include <seed/impl/SubSeedModel.hpp>

#include <algo/core/impl/ScoreMatrix.hpp>
#include <algo/core/impl/DefaultAlgoConfig.hpp>
#include <algo/core/impl/BasicAlgoIndexator.hpp>

#include <algo/hits/seed/SeedHitIterator.hpp>

#include <map>

using namespace std;
using namespace os;
using namespace os::impl;
using namespace dp;
using namespace dp::impl;
using namespace database;
using namespace database::impl;
using namespace indexation;
using namespace indexation::impl;
using namespace seed;
using namespace seed::impl;
using namespace indexation;
using namespace algo::core;
using namespace algo::core::impl;
using namespace algo::hits;

extern const char* getPath (const char* file);

/********************************************************************************/

static   int8_t**   _matrix;

class TestAlgoInitializer : public TestFixture
{
private:
    IConfiguration*     _config;
    ICommandDispatcher* _dispatcher;
    IScoreMatrix*       _scoreMatrix;
    size_t              _neighbourhoodSize;

public:

    /********************************************************************************/
    void setUp ()
    {
        _config =  new DefaultConfiguration (0, 0);
        _config->use ();

        _dispatcher = new ParallelCommandDispatcher  ();
        //_dispatcher = new SerialCommandDispatcher ();
        _dispatcher->use ();

        _scoreMatrix = ScoreMatrixManager::singleton().getMatrix ("BLOSUM62", SUBSEED, 0, 0);
        _scoreMatrix->use ();
        _matrix = _scoreMatrix->getMatrix();

        _neighbourhoodSize = 0;
    }
    void tearDown ()
    {
        _config->forget();
        _dispatcher->forget ();
        _scoreMatrix->forget ();
    }

    /********************************************************************************/
    static Test* suite()
    {
    	 TestSuite* result = new TestSuite ("AlgoInitializerTest");
    	 result->addTest (new TestCaller<TestAlgoInitializer> ("testInit1",    &TestAlgoInitializer::testInit1 ) );
         result->addTest (new TestCaller<TestAlgoInitializer> ("testInitBigFile",        &TestAlgoInitializer::testInitBigFile ) );
         result->addTest (new TestCaller<TestAlgoInitializer> ("testInitBigFileIterate", &TestAlgoInitializer::testInitBigFileIterate ) );
         result->addTest (new TestCaller<TestAlgoInitializer> ("testInitBigFileSplit",   &TestAlgoInitializer::testInitBigFileSplit ) );
         return result;
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testInit1 ()
    {
        bool cancel = false;

        IIndexator* indexator = new BasicSortedIndexator (
            new BasicSeedModel (SUBSEED, 3),
            _config->createDefaultParameters ("plastp"),
            new DatabaseIndexFactory (),
            1.0,
            cancel
        );
        CPPUNIT_ASSERT (indexator != 0);
        LOCAL (indexator);

        /** We build the indexes. */
        indexator->setSubjectDatabase (new BufferedSequenceDatabase (new StringSequenceIterator (2, "AADDPP", "CCFFF"), false));
        indexator->setQueryDatabase   (new BufferedSequenceDatabase (new StringSequenceIterator (3, "XAAADD", "TTT", "PCFF"), false));
        indexator->build (new SerialCommandDispatcher ());

        /** We create a Hit iterator. */
        IHitIterator* it = indexator->createHitIterator ();
        CPPUNIT_ASSERT (it != 0);
        LOCAL (it);

        size_t nb = 0;

        for (it->first(); ! it->isDone(); it->next())
        {
            /** We get the current hit of the iterator. */
            const Hit* hit = it->currentItem();
            CPPUNIT_ASSERT (hit != 0);

            /** Shortcuts. */
            const ISeedOccurrence* o1 = hit->occur1.data[0];
            const ISeedOccurrence* o2 = hit->occur2.data[0];
            CPPUNIT_ASSERT (o1!=0 && o2!=0);

            nb++;

            if (nb == 1)  {  CPPUNIT_ASSERT (o1->offsetInDatabase == 0  && o2->offsetInDatabase == 2);  }
            if (nb == 2)  {  CPPUNIT_ASSERT (o1->offsetInDatabase == 1  && o2->offsetInDatabase == 3);  }
            if (nb == 3)  {  CPPUNIT_ASSERT (o1->offsetInDatabase == 7  && o2->offsetInDatabase == 10); }

            if (false)  {  TRACE ("seed [%ld]   [%ld] \n",  o1->offsetInDatabase,  o2->offsetInDatabase  );  }

        }

        /** We should have 3 hits. */
        CPPUNIT_ASSERT (nb == 3);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testInitBigFile ()
    {
        const char* dbname    = getPath ("tursiops.fa");
        const char* queryname = getPath ("query.fa");

        bool cancel = false;

        IIndexator* indexator = new BasicSortedIndexator (
            new SubSeedModel (4,
                "H,FY,W,IV,LM,C,RK,Q,E,N,D,A,S,T,G,P",
                "HFYWIVLMC,RKQENDASTGP",
                "H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P",
                "H,FY,W,IV,LM,C,R,K,Q,E,N,D,A,S,T,G,P"
            ),
            _config->createDefaultParameters ("plastp"),
            new DatabaseIndexFactory (),
            1.0,
            cancel
        );
        CPPUNIT_ASSERT (indexator != 0);
        LOCAL (indexator);

        /** We build the indexes. */
        indexator->setSubjectDatabase (new BufferedSequenceDatabase (new FastaSequenceIterator (dbname,    100), false));
        indexator->setQueryDatabase   (new BufferedSequenceDatabase (new FastaSequenceIterator (queryname, 100), false));
        indexator->build (new SerialCommandDispatcher ());

        size_t nbHits  = 0;

#if 0
        /** We create a Hit iterator. */
        IHitIterator* it = initializer->createHitIterator ();
        CPPUNIT_ASSERT (it != 0);
        LOCAL (it);
        TIME_TICK (t, "iterator generic creation");

        /** We loop all the hits. */
        for (it->first(); ! it->isDone(); it->next())
        {
            /** We get the current hit of the iterator. */
            const Hit* hit = it->currentItem();
            nbHits++;
        }
#else
        IHitIterator* itHit = indexator->createHitIterator();
        CPPUNIT_ASSERT (itHit != 0);
        LOCAL (itHit);

        for (itHit->first(); ! itHit->isDone(); itHit->next())
        {
            /** We get the current hit of the iterator. */
            //const Hit* hit = itHit->currentItem();

            if (false)
            {
                TRACE ("seed [%ld]  [%ld]\n",
                    hit->occur1.data[0]->offsetInDatabase,
                    hit->occur2.data[0]->offsetInDatabase
                );
            }

            nbHits++;
        }
#endif

        TRACE ("nbHits=%ld\n", nbHits);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    class TestIterate
    {
    public:
        TestIterate ()  : number(0) {}
        void foo (Hit* hit)
        {
//            size_t offset1, offset2;

//            (hit->occur1->database)->getSequenceByOffset (hit->occur1->offsetInDatabase, offset1);
//            (hit->occur2->database)->getSequenceByOffset (hit->occur2->offsetInDatabase, offset2);

            number++;
        }
        u_int64_t number;
    };

    void testInitBigFileIterate ()
    {
        const char* dbname    = getPath ("tursiops.fa");
        const char* queryname = getPath ("query.fa");

        bool cancel = false;

        IIndexator* indexator = new BasicSortedIndexator (
            new SubSeedModel (4,
                "H,FY,W,IV,LM,C,RK,Q,E,N,D,A,S,T,G,P",
                "HFYWIVLMC,RKQENDASTGP",
                "H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P",
                "H,FY,W,IV,LM,C,R,K,Q,E,N,D,A,S,T,G,P"
            ),
            _config->createDefaultParameters ("plastp"),
            new DatabaseIndexFactory (),
            1.0,
            cancel
        );
        CPPUNIT_ASSERT (indexator != 0);
        LOCAL (indexator);

        /** We build the indexes. */
        indexator->setSubjectDatabase (new BufferedSequenceDatabase (new FastaSequenceIterator (dbname,    100), false));
        indexator->setQueryDatabase   (new BufferedSequenceDatabase (new FastaSequenceIterator (queryname, 100), false));
        indexator->build (new SerialCommandDispatcher ());

        /** We create a Hit iterator. */
        IHitIterator* it = indexator->createHitIterator ();
        CPPUNIT_ASSERT (it != 0);
        LOCAL (it);

        TestIterate test;
        it->iterate (&test, (Iterator<Hit*>::Method) &TestIterate::foo);

        TRACE ("nbHits=%ld\n", test.number);
    }


    /********************************************************************************/
    /* */
    /********************************************************************************/
    class HitIterationCommand : public ICommand
    {
    public:
        HitIterationCommand (IHitIterator* it, void* client, Iterator<Hit*>::Method method)
            :  _it(it), _client(client), _method(method), _nbHits(0)
        {
            if (_it)  { _it->use(); }
        }

        virtual ~HitIterationCommand ()  {  if (_it)  { _it->forget(); } }

        void execute ()
        {
            _nbHits=0;
            _it->iterate (_client, _method);
        }

        u_int64_t getNbHits  () { return _nbHits; }

    private:
        IHitIterator*   _it;

        void*                        _client;
        Iterator<Hit*>::Method _method;

        size_t _nbHits;
    };

    /********************************************************************************/
    void dispatchHitIterate (IHitIterator* itHit, ICommandDispatcher* dispatcher, void* client, Iterator<Hit*>::Method method)
    {
        CPPUNIT_ASSERT (itHit != 0);
        CPPUNIT_ASSERT (dispatcher != 0);
        CPPUNIT_ASSERT (client != 0);

        /** We split the iterator in several iterators. */
        std::vector<IHitIterator*> its = itHit->split (_dispatcher->getExecutionUnitsNumber());

        /** We create a list of commands for iterating the hits. */
        list<ICommand*> commands;
        for (size_t i=0; i<its.size(); i++)
        {
            commands.push_back (new HitIterationCommand (its[i], client, method));
        }

        /** We run the commands through a dispatcher. */
       _dispatcher->dispatchCommands (commands, 0);
    }

    /********************************************************************************/
    void testInitBigFileSplit ()
    {
        const char* dbname    = getPath ("tursiops.fa");
        const char* queryname = getPath ("query.fa");

        bool cancel = false;

        IIndexator* indexator = new BasicSortedIndexator (
            new SubSeedModel (4,
                "H,FY,W,IV,LM,C,RK,Q,E,N,D,A,S,T,G,P",
                "HFYWIVLMC,RKQENDASTGP",
                "H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P",
                "H,FY,W,IV,LM,C,R,K,Q,E,N,D,A,S,T,G,P"
            ),
            _config->createDefaultParameters ("plastp"),
            new DatabaseIndexFactory (),
            1.0,
            cancel
        );
        CPPUNIT_ASSERT (indexator != 0);
        LOCAL (indexator);

        /** We build the indexes. */
        indexator->setSubjectDatabase (new BufferedSequenceDatabase (new FastaSequenceIterator (dbname,    100), false));
        indexator->setQueryDatabase   (new BufferedSequenceDatabase (new FastaSequenceIterator (queryname, 100), false));
        indexator->build (_dispatcher);

        /** We create a hit iterator. */
        IHitIterator* itHit = indexator->createHitIterator();
        CPPUNIT_ASSERT (itHit != 0);
        LOCAL (itHit);

        /** We split the iterator in several iterators. */
        std::vector<IHitIterator*> its = itHit->split (_dispatcher->getExecutionUnitsNumber());

        /** We create a list of commands for iterating the hits. */
        list<ICommand*> commands;
        for (size_t i=0; i<its.size(); i++)
        {
#if 0
            ICommand* cmd = new HitIterationCommand (its[i], 1);
            cmd->use ();
            commands.push_back (cmd);
#endif
        }

        /** We run the commands through a dispatcher. */
       _dispatcher->dispatchCommands (commands, 0);

        u_int64_t nbHits   = 0;
        for (list<ICommand*>::iterator itCmd=commands.begin(); itCmd!=commands.end(); itCmd++)
        {
            HitIterationCommand* cmd = dynamic_cast<HitIterationCommand*> (*itCmd);
            if (cmd != 0)
            {
                nbHits   += cmd->getNbHits();
                cmd->forget ();
            }
        }

        TRACE ("\nnbHits=%ld \n", nbHits);
    }
};

/********************************************************************************/

extern "C" Test* TestAlgoInitializer_suite()  { return TestAlgoInitializer::suite (); }
