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

#include <designpattern/api/IObserver.hpp>
#include <designpattern/impl/Property.hpp>
#include <designpattern/impl/EventCatch.hpp>

#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/FastaSequenceOutput.hpp>
#include <database/impl/BufferedSequenceDatabase.hpp>
#include <database/impl/StringSequenceIterator.hpp>

#include <algo/core/api/IAlgoEvents.hpp>
#include <algo/core/impl/DefaultAlgoEnvironment.hpp>

#include <alignment/core/api/IAlignmentContainer.hpp>
#include <alignment/core/api/IAlignmentContainerVisitor.hpp>

using namespace std;
using namespace misc;
using namespace dp;
using namespace dp::impl;
using namespace database;
using namespace database::impl;
using namespace algo::core;
using namespace algo::core::impl;
using namespace alignment::core;

/********************************************************************************/

class TestDatabaseFiltering : public TestFixture
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
         result->addTest (new TestCaller<TestDatabaseFiltering> ("test_plast1", &TestDatabaseFiltering::test_filter1) );
         return result;
    }

    /********************************************************************************/
    /** From  a given sequence data, we build two databases (without and with filtering)
     *  Then returns the sequence from these two built databases.
     */
    int retrieveSequenceData (const char* sequence, string& s1, string& s2)
    {
        int result = 0;

        s1.clear();
        s2.clear();

        /** We create a database from an iterator without filtering. */
        BufferedSequenceDatabase db1 (new StringSequenceIterator (1, sequence), false);

        /** We create a database from an iterator without filtering. */
        BufferedSequenceDatabase db2 (new StringSequenceIterator (1, sequence), true);

        /** We retrieve the sequence from both databases. */
        ISequence seq1, seq2;

        bool found1 = db1.getSequenceByIndex (0, seq1);
        bool found2 = db2.getSequenceByIndex (0, seq2);

        if (found1 && found2)
        {
            s1 = seq1.data.toString().c_str();
            s2 = seq2.data.toString().c_str();

            if (s1.size() == s2.size())
            {
                result = 0;
                for (size_t i=0; i<s1.size(); i++)  {  result += (s1[i] != s2[i] ? 1 : 0);  }
                result = (int) (100*(1.0 - (float) result / (float) s1.size()));
            }
        }

        return result;
    }

    /********************************************************************************/
    void test_filter1 ()
    {
        const char* table[] =  {
            "IGERLKITGNLLEIDNSGLLLLVHTQLAGKITGM",
            "IGERLKITGNLLEIDNSELLLLVHTQLAGKITGM",
        };

        int res;
        string s1, s2;

        for (size_t i=0; i<sizeof(table)/sizeof(table[0]); i++)
        {
            res = retrieveSequenceData (table[i], s1, s2);
            printf ("identity %d\n   %s\n   %s\n",  res,s1.c_str(), s2.c_str() );
        }
    }
};

/********************************************************************************/

extern "C" Test* TestDatabaseFiltering_suite()  { return TestDatabaseFiltering::suite (); }

