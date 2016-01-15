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

extern "C" Test* TestOs_suite();
extern "C" Test* TestDesignPattern_suite();
extern "C" Test* TestSequenceIterator_suite();
extern "C" Test* TestSequenceDatabase_suite();
extern "C" Test* TestSeedModel_suite();
extern "C" Test* TestSeedIterator_suite();
extern "C" Test* TestDatabaseFiltering_suite();
extern "C" Test* TestDatabaseIndex_suite();
extern "C" Test* TestDatabaseCompare_suite();
extern "C" Test* TestScoreMatrix_suite();
extern "C" Test* TestAlgoInitializer_suite();
extern "C" Test* TestPlastLike_suite();
extern "C" Test* TestPlast_suite();
extern "C" Test* TestAlignment_suite();
extern "C" Test* TestFilter_suite();
extern "C" Test* TestPlastStrings_suite();
extern "C" Test* TestSequenceMask_suite();
extern "C" Test* TestBugs_suite();

/********************************************************************************/

const char* dbDirectory = "../test/db";

const char* getPath (const char* file)
{
    static char buffer[256];
    sprintf (buffer, "%s/%s", dbDirectory, file);
    return buffer;
}

/********************************************************************************/
int main (int argc, char **argv)
{
    if (argc >=2)  { dbDirectory = argv[1]; }

    CppUnit::TextUi::TestRunner runner;

    runner.addTest (TestOs_suite());
    runner.addTest (TestDesignPattern_suite());

    runner.addTest (TestSequenceIterator_suite());
    runner.addTest (TestSequenceDatabase_suite());

    runner.addTest (TestSeedModel_suite());
    runner.addTest (TestSeedIterator_suite());

    runner.addTest (TestDatabaseFiltering_suite());
    runner.addTest (TestDatabaseIndex_suite());
    runner.addTest (TestDatabaseCompare_suite());

    runner.addTest (TestScoreMatrix_suite());

    runner.addTest (TestAlgoInitializer_suite());

    runner.addTest (TestPlastLike_suite());

    runner.addTest (TestPlast_suite());

    runner.addTest (TestAlignment_suite());

    runner.addTest (TestFilter_suite());

    runner.addTest (TestPlastStrings_suite());

    runner.addTest (TestSequenceMask_suite());

    runner.addTest (TestBugs_suite());

    runner.run ();

    return 0;
}
