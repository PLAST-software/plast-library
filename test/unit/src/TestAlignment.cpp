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
#include <stdio.h>

#include <alignment/core/api/IAlignmentContainer.hpp>
#include <alignment/core/api/IAlignmentContainerVisitor.hpp>
#include <alignment/core/impl/AlignmentContainerFactory.hpp>
#include <alignment/visitors/impl/TabulatedOutputVisitor.hpp>
#include <alignment/visitors/impl/CompareContainerVisitor.hpp>
#include <alignment/visitors/impl/ShrinkContainerVisitor.hpp>
#include <alignment/visitors/impl/FilterContainerVisitor.hpp>
#include <alignment/visitors/impl/ModelBuilderVisitor.hpp>
#include <alignment/tools/impl/AlignmentOverlapCmd.hpp>

#include <database/api/ISequence.hpp>

#include <designpattern/impl/FileLineIterator.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>

#include <map>
#include <list>
#include <string>

using namespace std;
using namespace misc;
using namespace database;
using namespace alignment::core;
using namespace alignment::core::impl;
using namespace alignment::visitors::impl;
using namespace alignment::tools::impl;
using namespace dp::impl;

/** Macro that returns random number in [0..999] */
#define MAX_RAND  500
#define RAN()  (rand() % MAX_RAND)

/** Macro that returns random range. */
#define RANGE_RAN()    Range32 (RAN(), RAN())

/** Macro that returns random alignments. */
#define ALIGN_RAN()    Alignment (RANGE_RAN(), RANGE_RAN())

extern const char* getPath (const char* file);

/********************************************************************************/

class TestAlignment : public TestFixture
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
         //result->addTest (new TestCaller<TestAlignment> ("test_alignment1",                 &TestAlignment::test_alignment1) );
         result->addTest (new TestCaller<TestAlignment> ("test_overlap",                    &TestAlignment::test_overlap) );
         result->addTest (new TestCaller<TestAlignment> ("test_ContainerOverlap",           &TestAlignment::test_ContainerOverlap) );
         result->addTest (new TestCaller<TestAlignment> ("test_ContainerOverlapAll",        &TestAlignment::test_ContainerOverlapAll) );
         result->addTest (new TestCaller<TestAlignment> ("test_ContainerOverlapNone",       &TestAlignment::test_ContainerOverlapNone) );
    	 //result->addTest (new TestCaller<TestAlignment> ("test_test_ContainerOverlapFile",  &TestAlignment::test_ContainerOverlapFile) );
         result->addTest (new TestCaller<TestAlignment> ("test_ContainerCompare",           &TestAlignment::test_ContainerCompare) );
         result->addTest (new TestCaller<TestAlignment> ("test_ContainerCompare2",          &TestAlignment::test_ContainerCompare2) );
         result->addTest (new TestCaller<TestAlignment> ("test_ContainerBigNbAlign",          &TestAlignment::test_ContainerBigNbAlign) );
         result->addTest (new TestCaller<TestAlignment> ("test_Model",          &TestAlignment::test_Model) );
//    	 result->addTest (new TestCaller<TestAlignment> ("test_compare",          &TestAlignment::test_compare) );
         return result;
    }

    /********************************************************************************/
    void test_alignment1 ()
    {
        string uri = "/udd/edrezen/tmp/li/result.out";

        IAlignmentContainer* container = AlignmentContainerFactory::singleton().createContainerFromUri (uri);
        LOCAL (container);

        TabulatedOutputVisitor v (uri + "2");
        container->accept (&v);
    }

    /********************************************************************************/
    void test_overlap ()
    {
        /** Note that we don't use == in assert because we use 'double' objects. */
        #define EQ(x,y)  (ABS ((x) - (y)) < 1e-30)

        Alignment a1 (Range32(56,62), Range32(35,40));
        Alignment a2 (Range32(57,64), Range32(36,42));
        CPPUNIT_ASSERT (EQ (Alignment::overlap (a1,a2), 0.625));

        Alignment a3 (Range32(56,62), Range32(40,35));
        Alignment a4 (Range32(57,64), Range32(42,36));
        CPPUNIT_ASSERT (EQ (Alignment::overlap (a3,a4), 0.625));

        Alignment a5 (Range32(56,62), Range32(40,35));
        Alignment a6 (Range32(63,97), Range32(42,36));
        CPPUNIT_ASSERT (EQ (Alignment::overlap (a5,a6), 0.0));
    }

    /********************************************************************************/
    void test_ContainerOverlap ()
    {
        list<Alignment> ref;
        list<Alignment> cmp;

        Alignment a1 (Range32(56,62), Range32(35,40));
        Alignment a2 (Range32(57,64), Range32(36,42));

        double overlap = Alignment::overlap (a1, a2);

        ref.push_back (a1);
        cmp.push_back (a2);

        AlignmentOverlapCmd cmd1 (&ref, &cmp, misc::Range<double>(overlap - 0.1, 1.0) );
        cmd1.execute ();
        CPPUNIT_ASSERT (cmd1.getCommonSize()==1 && cmd1.getSpecificSize()==0);

        AlignmentOverlapCmd cmd2 (&ref, &cmp, misc::Range<double>(overlap + 0.1, 1.0) );
        cmd2.execute ();
        CPPUNIT_ASSERT (cmd2.getCommonSize()==0 && cmd2.getSpecificSize()==1);
    }

    /********************************************************************************/
    void test_ContainerOverlapAll ()
    {
        srand (time(NULL));

        list<Alignment> ref;
        list<Alignment> cmp;

        size_t nb = 1000;
        for (size_t i=0; i<nb; i++)
        {
            Alignment a (ALIGN_RAN());

            ref.push_back (a);
            cmp.push_back (a);
        }

        /** Since ref and comp have same alignments, we should have perfect matches (ie overlap==1.0) */
        AlignmentOverlapCmd cmd (&ref, &cmp, misc::Range<double>(1.0,1.0));
        cmd.execute ();
        CPPUNIT_ASSERT (cmd.getCommonSize()==nb && cmd.getSpecificSize()==0);
    }

    /********************************************************************************/
    void test_ContainerOverlapNone ()
    {
        srand (time(NULL));

        list<Alignment> ref;
        list<Alignment> cmp;

        int32_t shift = MAX_RAND;

        size_t nb = 1000;
        for (size_t i=0; i<nb; i++)
        {
            Range32 s1 (RAN(), RAN());
            Range32 q1 (RAN(), RAN());

            /** The compared alignments will be included in the ref ones. */
            Range32 s2 = s1.shift (shift);
            Range32 q2 = q1.shift (shift);

            ref.push_back (Alignment (s1,q1));
            cmp.push_back (Alignment (s2,q2));
        }

        /** Ref should overlap none comp because comp are shifted far enough far having no overlap at all. */
        AlignmentOverlapCmd cmd (&ref, &cmp, misc::Range<double>(1e-10,1.0));
        cmd.execute ();
        CPPUNIT_ASSERT (cmd.getCommonSize()==0 && cmd.getSpecificSize()==nb);
    }

    /********************************************************************************/
    void test_ContainerOverlapFile ()
    {
        string refUri = "/local/users/edrezen/git/plast/GatTool/Release/out.0";
        string cmpUri = "/local/users/edrezen/git/plast/GatTool/Release/out.3";

        IAlignmentContainer* ref = AlignmentContainerFactory::singleton().createContainerFromUri (refUri);
        LOCAL (ref);

        IAlignmentContainer* cmp = AlignmentContainerFactory::singleton().createContainerFromUri (cmpUri);
        LOCAL (cmp);

        size_t nbPoints = 300;
        for (size_t n=0; n<nbPoints; n++)
        {
            double x = (double)n/(double)nbPoints;
            CompareContainerVisitor vs (cmp, misc::Range<double>(x,1.0));
            ref->accept (&vs);
            printf ("%f %f\n", x, vs.getCommonPercentage());
        }
    }

    /********************************************************************************/
    Range32 getRandomRange ()
    {
        Range32 r;
        r.begin = (rand() % MAX_RAND);
        r.end   = r.begin + (rand() % (MAX_RAND - r.begin));
        return r;
    }

    /********************************************************************************/
    void test_ContainerCompare ()
    {
        srand (time(NULL));

        ISequence fakeSeq;

        size_t nbPositives = 0;

        size_t K = 100;
        for (size_t k=0; k<K; k++)
        {
            IAlignmentContainer* ref = AlignmentContainerFactory::singleton().createContainer();
            CPPUNIT_ASSERT (ref != 0);
            LOCAL (ref);

            IAlignmentContainer* cmp = AlignmentContainerFactory::singleton().createContainer();
            CPPUNIT_ASSERT (cmp != 0);
            LOCAL (cmp);

            Alignment a1 (
                Alignment::AlignSequenceInfo (&fakeSeq, getRandomRange()),
                Alignment::AlignSequenceInfo (&fakeSeq, getRandomRange())
            );

            Alignment a2 (
                Alignment::AlignSequenceInfo (&fakeSeq, getRandomRange()),
                Alignment::AlignSequenceInfo (&fakeSeq, getRandomRange())
            );

            ref->insert (a1, 0);
            cmp->insert (a2, 0);

            double overlap = Alignment::overlap (a1, a2);

            if (overlap > 0)  { nbPositives++; }

            size_t nbPoints = 1000;
            for (size_t n=0; n<nbPoints; n++)
            {
                double x = (double)n/(double)nbPoints;
                CompareContainerVisitor vs (cmp, misc::Range<double>(x,1.0));
                ref->accept (&vs);

                if (x <= overlap)  {  CPPUNIT_ASSERT (vs.getCommonSize()==1 && vs.getSpecificSize()==0); }
                else               {  CPPUNIT_ASSERT (vs.getCommonSize()==0 && vs.getSpecificSize()==1); }
            }
        }

        //printf ("nbPositives [%ld/%ld]\n", nbPositives, K);
    }

    /********************************************************************************/
    void test_ContainerCompare2 ()
    {
        srand (time(NULL));

        ISequence fakeSeq ("stub");

        IAlignmentContainer* ref = AlignmentContainerFactory::singleton().createContainer();
        CPPUNIT_ASSERT (ref != 0);
        LOCAL (ref);

        IAlignmentContainer* cmp = AlignmentContainerFactory::singleton().createContainer();
        CPPUNIT_ASSERT (cmp != 0);
        LOCAL (cmp);

        size_t K = 1000;
        for (size_t k=0; k<K; k++)
        {
            Alignment a1 (
                Alignment::AlignSequenceInfo (&fakeSeq, getRandomRange()),
                Alignment::AlignSequenceInfo (&fakeSeq, getRandomRange())
            );

            Alignment a2 (
                Alignment::AlignSequenceInfo (&fakeSeq, getRandomRange()),
                Alignment::AlignSequenceInfo (&fakeSeq, getRandomRange())
            );

            ref->insert (a1, 0);
            cmp->insert (a2, 0);
        }

        fstream file;
        file.open ("/tmp/compare.txt", fstream::out);
        size_t nbPoints = 300;
        for (size_t n=0; n<nbPoints; n++)
        {
            double x = (double)n/(double)nbPoints;
            CompareContainerVisitor vs (cmp, misc::Range<double>(x,1.0));
            ref->accept (&vs);
            file << x << " " << vs.getCommonPercentage() << endl;
        }
        file.close ();

        TabulatedOutputVisitor v1 ("/tmp/fakeRef");   ref->accept (&v1);
        TabulatedOutputVisitor v2 ("/tmp/fakeCmp");   cmp->accept (&v2);

        IAlignmentContainer* ref2 = AlignmentContainerFactory::singleton().createContainerFromUri ("/tmp/fakeRef");
        CPPUNIT_ASSERT (ref2 != 0);
        LOCAL (ref2);

        IAlignmentContainer* cmp2 = AlignmentContainerFactory::singleton().createContainerFromUri ("/tmp/fakeCmp");
        CPPUNIT_ASSERT (cmp2 != 0);
        LOCAL (cmp2);

        fstream file2;
        file2.open ("/tmp/compare2.txt", fstream::out);
        for (size_t n=0; n<nbPoints; n++)
        {
            double x = (double)n/(double)nbPoints;
            CompareContainerVisitor vs (cmp2, misc::Range<double>(x,1.0));
            ref2->accept (&vs);
            file2 << x << " " << vs.getCommonPercentage() << endl;
        }
        file2.close ();
    }


    /********************************************************************************/
    void test_ContainerBigNbAlign ()
    {
        srand (time(NULL));

        ISequence sbjSeq ("subject");
        ISequence qrySeq ("query");

        IAlignmentContainer* container = AlignmentContainerFactory::singleton().createContainer();
        CPPUNIT_ASSERT (container != 0);
        LOCAL (container);

        for (size_t i=0; i<10000; i++)
        {
            qrySeq.index = i;

            for (size_t j=0; j<10; j++)
            {
                sbjSeq.index = j;

                for (size_t k=0; k<5; k++)
                {
                    Alignment al (
                        Alignment::AlignSequenceInfo (&sbjSeq, getRandomRange()),
                        Alignment::AlignSequenceInfo (&qrySeq, getRandomRange())
                    );

                    container->insert (al, 0);
                }
            }
        }

        printf ("nb=%d\n", container->getAlignmentsNumber());

        FilterContainerVisitor filter (0);
        container->accept (&filter);

        ShrinkContainerVisitor shrink;
        container->accept (&shrink);

        TabulatedOutputVisitor v ("/tmp/output");
        container->accept (&v);
    }

    /********************************************************************************/
    void test_Model ()
    {
        srand (time(NULL));

        ISequence sbjSeq ("subject");
        ISequence qrySeq ("query");

        IAlignmentContainer* container = AlignmentContainerFactory::singleton().createContainer();
        CPPUNIT_ASSERT (container != 0);
        LOCAL (container);

        size_t nbQry   = 7;
        size_t nbSbj   = 5;
        size_t nbAlign = 3;

        for (size_t i=0; i<nbQry; i++)
        {
            qrySeq.index = i;

            for (size_t j=0; j<nbSbj; j++)
            {
                sbjSeq.index = j;

                for (size_t k=0; k<nbAlign; k++)
                {
                    Alignment al (
                        Alignment::AlignSequenceInfo (&sbjSeq, getRandomRange()),
                        Alignment::AlignSequenceInfo (&qrySeq, getRandomRange())
                    );

                    container->insert (al, 0);
                }
            }
        }

        ModelBuilderVisitor v;
        container->accept (&v);
        IRootLevel* model = v.getModel();
        LOCAL (model);

        for (model->first(); !model->isDone(); model->next())
        {
            IQueryLevel* qry = model->currentItem();

            CPPUNIT_ASSERT (qry->size() <= nbSbj);

            for (qry->first(); !qry->isDone(); qry->next())
            {
                ISubjectLevel* sbj = qry->currentItem();

                CPPUNIT_ASSERT (sbj->size() <= nbAlign);

                for (sbj->first(); !sbj->isDone(); sbj->next())
                {
                    //Alignment* align = sbj->currentItem();
                }
            }
        }
    }

    /********************************************************************************/
    struct AlignRange
    {
        int32_t diagonal;
        u_int32_t qryLen;
        u_int32_t sbjLen;
    };

    /********************************************************************************/
    void test_compare ()
    {
        const char* filename = "/media/USB_Disk/users/edrezen/irisa/symbiose/gat/results/main/gpgpu1_blast_20121002_060901.gat";

        u_int32_t qryIdx = 0;
        u_int32_t sbjIdx = 0;

        map <string,u_int32_t> qryIdMap;
        map <string,u_int32_t> sbjIdMap;

        typedef pair<u_int32_t,u_int32_t> Key;
        map <Key, list<AlignRange> > alignsMap;

        /** We read the first file line by line. */
        FileLineIterator itFile (filename);
        for (itFile.first(); !itFile.isDone(); itFile.next())
        {
            const char* line = itFile.currentItem();
            if (line && *line=='#')  { continue; }

            Key currentKey;

            TokenizerIterator tokenIt (line, " \t");

            tokenIt.first();
            map <string,u_int32_t>::iterator qryLook = qryIdMap.find (tokenIt.currentItem());
            if (qryLook == qryIdMap.end())  {  currentKey.first = qryIdMap [tokenIt.currentItem()] = qryIdx ++;  }
            else                            {  currentKey.first = qryLook->second;                               }

            tokenIt.next();
            map <string,u_int32_t>::iterator sbjLook = sbjIdMap.find (tokenIt.currentItem());
            if (sbjLook == sbjIdMap.end())  {  currentKey.second = sbjIdMap [tokenIt.currentItem()] = sbjIdx ++;  }
            else                            {  currentKey.second = sbjLook->second;                               }

            /** We go to the bounds of the alignment. */
            tokenIt.next();     tokenIt.next();

            Range32 qryRange, sbjRange;
            qryRange.begin = ::atol(tokenIt.currentItem());  tokenIt.next();
            qryRange.end   = ::atol(tokenIt.currentItem());  tokenIt.next();
            sbjRange.begin = ::atol(tokenIt.currentItem());  tokenIt.next();
            sbjRange.end   = ::atol(tokenIt.currentItem());  tokenIt.next();

            AlignRange align;
            align.diagonal = qryRange.begin - sbjRange.begin;
            align.qryLen   = qryRange.getLength();
            align.sbjLen   = sbjRange.getLength();

            /** We add the alignment into the corresponding list. */
            alignsMap [currentKey].push_back (align);
        }

        printf ("nbQry=%ld  nbSbj=%ld\n", qryIdMap.size(), sbjIdMap.size());
    }

};

/********************************************************************************/

extern "C" Test* TestAlignment_suite()  { return TestAlignment::suite (); }

