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
#include <misc/api/Vector.hpp>
#include <designpattern/api/ICommand.hpp>
#include <designpattern/impl/CommandDispatcher.hpp>
#include <misc/api/types.hpp>

#include <math.h>
#include <stdlib.h>

#include <iostream>

using namespace std;
using namespace misc;
using namespace os;
using namespace os::impl;
using namespace dp;
using namespace dp::impl;

/********************************************************************************/

class TestOs : public TestFixture
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
    static Test* suite()
    {
         TestSuite* result = new TestSuite ("OsTest");

         result->addTest (new TestCaller<TestOs> ("testMemory", &TestOs::testMemory ) );
         result->addTest (new TestCaller<TestOs> ("testVector", &TestOs::testVector ) );
         result->addTest (new TestCaller<TestOs> ("testThread1", &TestOs::testThread1) );

         return result;
    }

    /********************************************************************************/
    /********************************************************************************/
    void testMemory ()
    {
        CPPUNIT_ASSERT (true);
    }

    /********************************************************************************/
    /********************************************************************************/
    void testVector ()
    {
        Vector<int> v1;
        CPPUNIT_ASSERT (v1.size == 0);

        Vector<float> v2 (100);
        CPPUNIT_ASSERT (v2.size == 100);

        /** Test on vector fill. */
        for (size_t i=0; i<v2.size; i++)  { v2.data[i] = i; }
        for (size_t i=0; i<v2.size; i++)  { CPPUNIT_ASSERT (v2.data[i] == i);  }

        /** Test on vector resize. */
        v2.resize (v2.size*2);
        CPPUNIT_ASSERT (v2.size == 200);

        /** We check that the initial items are still ok. */
        for (size_t i=0; i<100; i++)  { CPPUNIT_ASSERT (v2.data[i] == i);  }

        /** We create a vector with a non null size (which force some memory allocation). */
        Vector<int> v3 (10);
        CPPUNIT_ASSERT (v3.size == 10);

        /** We use an external table as referenced data. */
        int table[] = {1,5,13,34};
        v3.setReference (sizeof(table)/sizeof(table[0]), table);
        CPPUNIT_ASSERT (v3.size == 4);
        CPPUNIT_ASSERT (v3.data[0] == 1);
        CPPUNIT_ASSERT (v3.data[1] == 5);
        CPPUNIT_ASSERT (v3.data[2] == 13);
        CPPUNIT_ASSERT (v3.data[3] == 34);

        /** We check that the size hasn't change. */
        CPPUNIT_ASSERT (v3.size == 4);
    }

    /********************************************************************************/
    /********************************************************************************/

    class CmdCalculus : public ICommand
    {
    public:

        CmdCalculus (u_int64_t k0, u_int64_t k1) : _k0(k0), _k1(k1) {}

        void execute ()
        {
            for (size_t k=_k0; k<_k1; k++)   {  compute (k);  }
        }

        double compute (double x)
        {
            return exp (sin (x)*sin(x) + cos(x)*cos(x));
        }

    private:
        u_int64_t _k0;
        u_int64_t _k1;
    };

    /********************************************************************************/
    /********************************************************************************/
    class CmdAllocation : public ICommand
    {
    public:

        CmdAllocation (u_int64_t k0, u_int64_t k1) : _k0(k0), _k1(k1) {}

        void execute ()
        {
            for (size_t k=_k0; k<_k1; k++)   {  allocate (k);  }
        }

        void allocate (size_t k)
        {
            double* f = (double*) malloc ((0*220+6*1024)*sizeof(double));

            if (f != NULL)  {  free (f); }
        }

    private:
        u_int64_t _k0;
        u_int64_t _k1;
    };



    void testThread1 ()
    {
        printf ("start\n");

        u_int64_t nbCompute = 600*1000*100;

        //u_int8_t nbCpuTable[] = {1,2,4,8};
        u_int8_t nbCpuTable[] = {1,2,3,4,5,6,7,8};
        //u_int8_t nbCpuTable[] = {8,4,2,1};
        size_t   nbCpu = sizeof(nbCpuTable)/sizeof(nbCpuTable[0]);

        vector<u_int32_t> timeVector;

        for (u_int8_t k=0; k<nbCpu; k++)
        {
            u_int8_t nbCommands = nbCpuTable[k];
            u_int64_t step = nbCompute / nbCommands;

            list<ICommand*> cmdList;
            for (size_t i=0; i<nbCommands; i++)
            {
                cmdList.push_back (new CmdAllocation (i*step, (i+1)*step) );
                //cmdList.push_back (new CmdCalculus (i*step, (i+1)*step) );
            }

            ParallelCommandDispatcher dispatcher (cmdList.size());

            u_int32_t t0 = DefaultFactory::singleton().time().gettime();
            dispatcher.dispatchCommands (cmdList, 0);
            u_int32_t t1 = DefaultFactory::singleton().time().gettime();

            timeVector.push_back (t1-t0);
        }

        printf ("VALUES...\n");
        for (size_t i=0; i<timeVector.size(); i++)
        {
            printf ("%d  %d  %f\n", nbCpuTable[i], timeVector[i], (float)timeVector[0]/ (float)timeVector[i]);
            //printf ("%d  %d\n", nbCpuTable[i], timeVector[i]);
        }

        printf ("\n");
        printf ("RATIO...\n");
        for (size_t i=0; i<timeVector.size()-1; i++)
        {
            printf ("%ld  %f\n", i, (float) timeVector[i] / (float) timeVector[i+1]);
        }
    }
};

/********************************************************************************/

extern "C" Test* TestOs_suite()  { return TestOs::suite (); }

