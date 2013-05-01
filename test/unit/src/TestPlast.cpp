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
#include <stdio.h>

#include <os/impl/DefaultOsFactory.hpp>

#include <designpattern/api/IObserver.hpp>
#include <designpattern/impl/Property.hpp>
#include <designpattern/impl/EventCatch.hpp>

#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/FastaSequenceOutput.hpp>
#include <database/impl/StringSequenceIterator.hpp>

#include <algo/core/api/IAlgoEvents.hpp>
#include <algo/core/impl/DefaultAlgoEnvironment.hpp>

#include <alignment/core/api/IAlignmentContainer.hpp>
#include <alignment/core/api/IAlignmentContainerVisitor.hpp>

#include <launcher/core/PlastCmd.hpp>

#include <malloc.h>
#include <unistd.h>

using namespace std;
using namespace misc;
using namespace os;
using namespace os::impl;
using namespace dp;
using namespace dp::impl;
using namespace database;
using namespace database::impl;
using namespace algo::core;
using namespace algo::core::impl;
using namespace alignment::core;
using namespace launcher::core;
/********************************************************************************/

class TestPlast : public TestFixture
{
private:

public:

    /********************************************************************************/
    void setUp ()
    {
    }
    void tearDown ()
    {
    }

    /********************************************************************************/
    /********************************************************************************/
    static Test* suite()
    {
    	 TestSuite* result = new TestSuite ("PlastTest");
         //result->addTest (new TestCaller<TestPlast> ("test_plast1", &TestPlast::test_plast1) );
         result->addTest (new TestCaller<TestPlast> ("test_plast2", &TestPlast::test_plast2) );
         return result;
    }

    /********************************************************************************/
    void test_plast1 ()
    {
        const char* ENSTTRP00000007204 =
            "MNSSGPGYPLASLYAGDLHPDVTEAMLYEKFSPAGPILSIRVCRDVATRRSLGYAYINFQ" \
            "QPADAERALDTMNFEVIKGQPIRIMWSQRDPGLRKSGVGNIFIKNLEDSIDNKALYDTFS" \
            "TFGNILSCKVVCDDHGSRGFGFVHFETHEAAQNAISTMNGMLLNDRKVFVGHFKPRRERD" \
            "AELGARAMEFTNIYVKNLHVDVDEQCLQDLFSQFGKILSVKVMDDSHPRFGFVNFETHEA" \
            "QAVTDMNGREVSGRLLYVGRAQKRVERQNELKRRFEQMKQDRLTRYQGVNLYVKNLDDSI" \
            "DDEKLRKEFSPYGVITSAKVMTEGGHSKGFGFVCFSSPEEATKAVTEMNGRIVGTKPLYV" \
            "ALAQRKEERKAILTNQYMQRLSTMRALGSPLLGSFQQPASYFLPATPQPPAQAPYYGSGP" \
            "PVQPASRWTAQLPRPSSASVVQPAAVSRRPSFPIGSARQISTHVPRLVPHAQGVANIGTQ" \
            "TTGPGMAGCSSPRGPLLTHRCSLATHNTHRVQEPAVRVPGQEPLTASMLAAAPLHEQKQM" \
            "IGERLYPLIYNVHTQLAGKITGMLLEIDNSELLLLLESPESLSAK";

        const char* sequence = ENSTTRP00000007204; //"IGERLKEEIDNSELL";

        size_t sequenceLen = strlen (sequence);
        cout << "sequenceLen=" << sequenceLen << endl;

        // we create a subject and a query FASTA files containing the same sequence
        FastaSequenceOutput (new StringSequenceIterator (1, sequence), "/tmp/subject.fa").dump ();
        FastaSequenceOutput (new StringSequenceIterator (1, sequence), "/tmp/query.fa").dump ();

        // we create a IProperties instance holding the subject and query databases URIs
        IProperties* props = new Properties ();
        props->add (0, "-d",     "/tmp/subject.fa");
        props->add (0, "-i",     "/tmp/query.fa");
        props->add (0, "-o",     "/tmp/plast.out");
        props->add (0, "-a",     "1");

        // we create our entry point for the PLAST algorithm
        bool isRunning = true;
        DefaultEnvironment env (props, isRunning);

        /** We want to catch AlgorithmReportEvent instance sent during PLAST execution. */
        EventCatch <AlgorithmReportEvent*> catcher (&env);

        /** IMPORTANT FOR THIS TEST: we don't want to filter out low informative regions. */
        props->add (0, "-F",     "F");

        // we launch the PLAST algorithm
        env.run ();

        // we retrieve (if any) information about gap alignments through a notification information
        if (!catcher.empty())
        {
            /** We retrieve all alignments as a list. */
            list<Alignment> alignments;
//            AlignmentResultListVisitor v (alignments);
//            catcher.front()->getAlignmentResult()->accept (&v);
            cout << "nbAligns=" << alignments.size() << endl;

            //CPPUNIT_ASSERT (alignments.size() == 1);

            for (list<Alignment>::iterator it = alignments.begin(); it != alignments.end(); it++)
            {
                Alignment& align = (*it);

#if 1
                cout << align.toString() << endl;
#else
                CPPUNIT_ASSERT (align._subjectStartInDb == 0);
                CPPUNIT_ASSERT (align._queryStartInDb   == 0);

                CPPUNIT_ASSERT (align._subjectEndInDb == sequenceLen - 1);
                CPPUNIT_ASSERT (align._queryEndInDb   == sequenceLen - 1);
#endif
            }
        }
    }

    /********************************************************************************/
    static void* mainloop (void* args)
    {
        PlastCmd* plast = (PlastCmd*) args;
        plast->execute();
        return 0;
    }

    /********************************************************************************/
    void test_plast2 ()
    {
        // we create a IProperties instance holding the subject and query databases URIs
        IProperties* props = new Properties ();
        props->add (0, "-d",     "/local/users/edrezen/databases/HalobacteriumSalinarum.fa");
        props->add (0, "-i",     "/local/users/edrezen/databases/query_x16.fa");
        props->add (0, "-o",     "/tmp/plast.out");
        props->add (0, "-a",     "1");

        PlastCmd* plast = new PlastCmd (props);
        LOCAL (plast);

        IThread* thread = DefaultFactory::thread().newThread (mainloop, plast);
        sleep (2);

        plast->cancel ();

        thread->join ();
        delete thread;
    }
};

/********************************************************************************/

extern "C" Test* TestPlast_suite()  { return TestPlast::suite (); }

