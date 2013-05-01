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

#include <alignment/core/api/IAlignmentContainer.hpp>
#include <alignment/core/api/IAlignmentContainerVisitor.hpp>
#include <alignment/core/impl/AlignmentContainerFactory.hpp>

#include <alignment/visitors/impl/TabulatedOutputVisitor.hpp>
#include <alignment/visitors/impl/CompareContainerVisitor.hpp>
#include <alignment/visitors/impl/ShrinkContainerVisitor.hpp>
#include <alignment/visitors/impl/FilterContainerVisitor.hpp>

#include <alignment/filter/impl/AlignmentFilterOperator.hpp>
#include <alignment/filter/impl/AlignmentFilterFactory.hpp>

#include <database/api/ISequence.hpp>

#include <memory>

using namespace std;
using namespace misc;
using namespace database;
using namespace alignment::core;
using namespace alignment::core::impl;
using namespace alignment::visitors::impl;

using namespace alignment::filter;
using namespace alignment::filter::impl;

/********************************************************************************/

class TestFilter : public TestFixture
{
private:

    AlignmentFilterFactory _factory;

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
//         result->addTest (new TestCaller<TestFilter> ("test_filter1",  &TestFilter::test_filter1) );
         result->addTest (new TestCaller<TestFilter> ("test_filter2",  &TestFilter::test_filter2) );
//         result->addTest (new TestCaller<TestFilter> ("test_filter3",  &TestFilter::test_filter3) );
         return result;
    }

    /********************************************************************************/
    template<class T> void checkFilterValue (const char* filterName, const T& value, const Alignment& align)
    {
        stringstream ss0;   ss0 << (value+0);   string s   = ss0.str();
        stringstream ss1;   ss1 << (value+1);   string sp  = ss1.str();
        stringstream ss2;   ss2 << (value-1);   string sm  = ss2.str();
        stringstream ss3;   ss3 << (value-2);   string smm = ss3.str();
        stringstream ss4;   ss4 << (value+2);   string spp = ss4.str();

#define TEST(op,s,val) \
        { \
            IAlignmentFilter* f = _factory.createFilter (filterName, op, s.c_str(), 0); \
            CPPUNIT_ASSERT (f != 0); \
            LOCAL (f); \
            CPPUNIT_ASSERT (f->isOk (align) == val); \
        }

#define TEST2(op,s,t,val) \
        { \
            IAlignmentFilter* f = _factory.createFilter (filterName, op, s.c_str(), t.c_str(), 0); \
            CPPUNIT_ASSERT (f != 0); \
            LOCAL (f); \
            CPPUNIT_ASSERT (f->isOk (align) == val); \
        }

        TEST ("==", s,  true);          TEST ("==",  sm, false);        TEST ("==",  sp, false);
        TEST ("!=", s,  false);         TEST ("!=", sm, true);          TEST ("!=", sp, true);
        TEST (">=", sm, true);          TEST (">=", s,  true);          TEST (">=", sp, false);
        TEST (">",  sm, true);          TEST (">",  s,  false);         TEST (">",  sp, false);
        TEST ("<=", sm, false);         TEST ("<=", s,  true);          TEST ("<=", sp, true);

        TEST2 ("[]", sm,  sp,  true);   TEST2 ("][", sm,  sp,  false);
        TEST2 ("[]", sm,  s,   true);   TEST2 ("][", sm,  s,   false);
        TEST2 ("[]", s,   sp,  true);   TEST2 ("][", s,   sp,  false);
        TEST2 ("[]", smm, sm,  false);  TEST2 ("][", smm, sm,  true);
        TEST2 ("[]", sp,  spp, false);  TEST2 ("][", sp,  spp, true);
    }

    /********************************************************************************/
    void test_filter1 ()
    {
        const char* str1 = "foo  bar    47.2   55  37  1    24  81    2627  2685    0.4   31.6";
        Alignment al (str1);

        checkFilterValue <u_int32_t> ("HSP hit from",           2627,   al);
        checkFilterValue <u_int32_t> ("HSP hit to",             2685,   al);
        checkFilterValue <u_int32_t> ("HSP query from",         24,     al);
        checkFilterValue <u_int32_t> ("HSP query to",           81,     al);
        checkFilterValue <double>    ("HSP E­‐Value",            0.4,    al);
        checkFilterValue <double>    ("HSP bit score",          31.6,   al);
        checkFilterValue <u_int32_t> ("HSP alignment length",   55,     al);
        checkFilterValue <u_int32_t> ("HSP # of misses",        37,     al);
        checkFilterValue <u_int32_t> ("HSP # of gaps",          1,      al);
        // can't test because of roundoff errors:  checkFilterValue <double>    ("HSP % of identities",    48.5,   al);
    }

    /********************************************************************************/
    void test_filterDefinition (const char* defname, const char* ope, const char* exp,  Alignment& a, bool match)
    {
        IAlignmentFilter* f = _factory.createFilter (defname, ope, exp, 0);
        CPPUNIT_ASSERT (f != 0);
        LOCAL (f);

        CPPUNIT_ASSERT (f->isOk (a) == match);
    }

    /********************************************************************************/
    void test_filter2 ()
    {
        const char* QryDef = "Query definition";
        const char* SbjDef = "Hit definition";

        const char* QryId = "Query identifier";
        const char* SbjId = "Hit identifier";

        const char* HoldOp   = "::";
        const char* NoHoldOp = "!:";

        ISequence qrySeq ("sp|A5Z2X5|YP010_YEAST hypothetic");
        ISequence sbjSeq ("ENSTTRP00000007202 the quick brown fox");

        const char* str1 = "foo  bar    47.2   55  37  1    24  81    2627  2685    0.4   31.6";
        Alignment al (str1);

        al.setSequence(Alignment::QUERY,   &qrySeq);
        al.setSequence(Alignment::SUBJECT, &sbjSeq);

        test_filterDefinition (QryDef, HoldOp, "A5Z2X5",    al, true);
        test_filterDefinition (QryDef, HoldOp, "ENSTTRP",   al, false);

        test_filterDefinition (SbjDef, HoldOp, "ENSTTRP",   al, true);
        test_filterDefinition (SbjDef, HoldOp, "A5Z2X5",    al, false);

        test_filterDefinition (QryDef, NoHoldOp, "A5Z2X5",  al, false);
        test_filterDefinition (QryDef, NoHoldOp, "ENSTTRP", al, true);

        test_filterDefinition (SbjDef, NoHoldOp, "ENSTTRP", al, false);
        test_filterDefinition (SbjDef, NoHoldOp, "A5Z2X5",  al, true);

        test_filterDefinition (QryDef, HoldOp, "^sp",    al, true);
        test_filterDefinition (SbjDef, HoldOp, "^sp",    al, false);

        test_filterDefinition (QryId,  HoldOp,   "YEAST$", al, true);
        test_filterDefinition (SbjId,  NoHoldOp, "YEAST$", al, true);

        test_filterDefinition (SbjId,  HoldOp,   "0{4}", al, true);

        test_filterDefinition (QryId,  HoldOp,   "[0-9]+$", al, false);
        test_filterDefinition (SbjId,  HoldOp,   "[0-9]+$", al, true);

        test_filterDefinition (QryDef, HoldOp,   "SP",     al, false);   // case sensitive
        test_filterDefinition (QryDef, HoldOp,   "(?i)SP", al, true);    // no case sensitive
        test_filterDefinition (SbjDef, HoldOp,   "(?i)SP", al, false);   // no case sensitive

        test_filterDefinition (SbjId, HoldOp,    "2$",   al, true);
        test_filterDefinition (SbjDef, HoldOp,   "fox$", al, true);
    }

    /********************************************************************************/
    void test_filter3 ()
    {
        ISequence qrySeq ("query annotation");
        ISequence sbjSeq ("subject annotation");

        const char* str1 = "foo  bar    47.2   55  37  1    24  81    2627  2685    0.4   31.6";
        Alignment al (str1);

        list<IAlignmentFilter*> filters;

        IAlignmentFilter* f1 = _factory.createFilter ("HSP hit from", ">=", "2627", 0);
        CPPUNIT_ASSERT (f1 != 0);  LOCAL (f1);   filters.push_back (f1);

        IAlignmentFilter* f2 = _factory.createFilter ("HSP query from", "==", "25", 0);
        CPPUNIT_ASSERT (f2 != 0);  LOCAL (f2);   filters.push_back (f2);

        IAlignmentFilter* f_or = new AlignmentFilterOr (filters);
        CPPUNIT_ASSERT (f_or != 0);  LOCAL (f_or);
        CPPUNIT_ASSERT (f_or->isOk (al) == true);

        IAlignmentFilter* f_and = new AlignmentFilterAnd (filters);
        CPPUNIT_ASSERT (f_and != 0);  LOCAL (f_and);
        CPPUNIT_ASSERT (f_and->isOk (al) == false);
    }
};

/********************************************************************************/

extern "C" Test* TestFilter_suite()  { return TestFilter::suite (); }

