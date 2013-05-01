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

#if 0

#include "cppunit.h"

#include <misc/api/RangeTree.hpp>
#include <os/impl/DefaultOsFactory.hpp>
#include <os/impl/TimeTools.hpp>

#include <math.h>
#include <stdio.h>

#include <list>
#include <vector>
#include <algorithm>

using namespace std;
using namespace misc;
using namespace os;
using namespace os::impl;

/********************************************************************************/
class Hsp
{
public:
    size_t x;
    size_t y;
    size_t length;
    Hsp (size_t xx, size_t yy, size_t ll) : x(xx), y(yy), length(ll) {}
};

/********************************************************************************/

class TestMisc : public TestFixture
{
private:

public:

    /********************************************************************************/
    void setUp ()
    {
        srand (time(0));
    }

    void tearDown ()
    {
    }

    /********************************************************************************/
    static Test* suite()
    {
         TestSuite* result = new TestSuite ("MiscTest");

         result->addTest (new TestCaller<TestMisc> ("testRangeTree0",           &TestMisc::testRangeTree0 ) );
         result->addTest (new TestCaller<TestMisc> ("testRangeTree1",           &TestMisc::testRangeTree1 ) );
         result->addTest (new TestCaller<TestMisc> ("testRangeTree2",           &TestMisc::testRangeTree2 ) );
         result->addTest (new TestCaller<TestMisc> ("testRangeTree3",           &TestMisc::testRangeTree3 ) );
         result->addTest (new TestCaller<TestMisc> ("testRangeTree4",           &TestMisc::testRangeTree4 ) );
         result->addTest (new TestCaller<TestMisc> ("testRangeTree5",           &TestMisc::testRangeTree5 ) );
         result->addTest (new TestCaller<TestMisc> ("testRangeTree6",           &TestMisc::testRangeTree6 ) );
         result->addTest (new TestCaller<TestMisc> ("testRangeTree7",           &TestMisc::testRangeTree7 ) );
         result->addTest (new TestCaller<TestMisc> ("testRangeTree8",           &TestMisc::testRangeTree8 ) );
         result->addTest (new TestCaller<TestMisc> ("testRangeTree9",           &TestMisc::testRangeTree9 ) );
         result->addTest (new TestCaller<TestMisc> ("testRangeTree10",          &TestMisc::testRangeTree10 ) );
         result->addTest (new TestCaller<TestMisc> ("testRangeTree11",          &TestMisc::testRangeTree11 ) );

         return result;
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/

    /** */

    void testRangeTree0 ()
    {
    	size_t qryLen = 1000, sbjLen=1000, len=1;

    	RangeTree tree (qryLen, sbjLen);

    	//list<RangeTree::Value> values;
    	//for (size_t i=0; i<N; i++)  {  values.push_back(i); }

    	//RangeTree::Value table[] = {10, 40, 20, 30, 25, 50, 45, 5, 7};
    	//RangeTree::Value table[] = {10, 40, 30, 20};
    	//RangeTree::Value table[] = {20, 10, 30, 5, 25, 40, 35, 45, 34};
    	//RangeTree::Value table[] = {1,2,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
    	RangeTree::Value table[] = {16,15,33,31,32,30};

    	for (size_t i=0; i<sizeof(table)/sizeof(table[0]); i++)
    	{
    		tree.insert (table[i], len);
    		printf ("==========> checkBalance=%d\n", tree.checkBalance());
    	}

    	printf ("depth=%ld  checkBalance=%d  ", tree.getDepth(), tree.checkBalance());   //tree.dump();
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testRangeTree1 ()
    {
    	srand (123456789);

    	TimeInfo ti;

    	size_t nbItems  = 1 << 20;

    	size_t nbSearch = 1 << 20;

    	size_t kmax = nbSearch / nbItems;

        size_t count = 0;

        //size_t qryLen = 10*1000*1000, sbjLen=10*1000*1000, maxLen=1;

    	size_t N = 1 << 20;
    	size_t qryLen = N, sbjLen=N, maxLen=1;

        RangeTree tree (qryLen, sbjLen);

        /** We build a list of HSP. */
        list<Hsp> hsplist;
        for (size_t i=1; i<=nbItems; i++)
        {
            Hsp h = Hsp (rand() % qryLen, rand() % sbjLen, 1 + rand()%maxLen) ;
            hsplist.push_back (h);
        }

        /** We fill the tree. */
        count = 0;
        ti.addEntry ("insert");
        for (list<Hsp>::iterator it = hsplist.begin(); it != hsplist.end(); it++)
        {
            tree.insert (it->x, it->y, it->length);
            count ++;
        }
        ti.stopEntry ("insert");

        printf ("'insert' called %ld times in %d msec  => nbItems=%ld  depth=%ld\n",
			count, ti.getEntryByKey("insert"), tree.getNbItems(), tree.getDepth()
		);

		/** We check the tree. */
        count = 0;
        ti.addEntry ("exist");
        for (size_t k=1; k<=kmax; k++)
        {
			for (list<Hsp>::iterator it = hsplist.begin(); it != hsplist.end(); it++)
			{
				//for (size_t i=0; i< it->length; i++)   {  CPPUNIT_ASSERT (tree.exists (it->x + i, it->y + i) == true);   count++; }
				tree.exists (it->x + 0, it->y + 0);   count++;
			}
        }
        ti.stopEntry ("exist");

        printf ("'exist' called %ld times in %d msec  => %.3f nanosec/search   %ld search/msec\n",
			count, ti.getEntryByKey("exist"),
			(double) (1000 * ti.getEntryByKey("exist")) / (double) count,
			count / ti.getEntryByKey("exist")
		);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testRangeTree2_aux (RangeTree& tree, RangeTree::Coord x, RangeTree::Coord y, size_t length)
    {
        tree.insert (x, y, length);

        /** Important: use a signed integer for the diagonal. */
        int64_t diag = x - y;

        /** We check the whole search space => X.Y cells checked. */
        for (RangeTree::Coord i=0; i<tree.getSizeX(); i++)
        {
            for (RangeTree::Coord j=0; j<tree.getSizeY(); j++)
            {
            	bool check = ((int64_t)(i-j) == diag) && (i>=x)  &&  (i<x+length);

            	CPPUNIT_ASSERT (tree.exists (i,j) == check);
            }
        }
    }

    /** */
    void testRangeTree2 ()
    {
        /** We create some random ranges. */
		for (size_t i=1; i<=20; i++)
		{
			RangeTree tree (4000, 2000);

	        RangeTree::Coord x = rand() % tree.getSizeX();
	        RangeTree::Coord y = rand() % tree.getSizeY();
	        size_t      length = rand() % 100 + 1;

			printf ("[%d,%d %ld] \n", x,y,length);

	        testRangeTree2_aux (tree, x, y, length);
		}
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testRangeTree3 ()
    {
        size_t qryLen = 10*1000*1000, sbjLen=10*1000*1000;

        RangeTree tree (qryLen, sbjLen);

        RangeTree::Coord x = 1000;
        RangeTree::Coord y =  500;
        size_t           n =   20;

        /** We insert a range. */
		tree.insert (x, y, n);
    	CPPUNIT_ASSERT (tree.getNbItems() == 1);

    	/** We try to add new ranges within the first one inserted => should still have only one item. */
    	for (size_t i=1; i<n; i++)
    	{
    		tree.insert (x+i, y+i, n-i);
        	CPPUNIT_ASSERT (tree.getNbItems() == 1);
    	}
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testRangeTree4 ()
    {
        size_t qryLen = 15, sbjLen=15;

        RangeTree tree (qryLen, sbjLen);

        list<Hsp> hsplist;
        hsplist.push_back ( Hsp ( 9, 9, 2) );
        hsplist.push_back ( Hsp ( 4, 4, 3) );
        hsplist.push_back ( Hsp ( 5, 5, 3) );
        hsplist.push_back ( Hsp ( 6, 6, 5) );

        /** We fill the tree. */
        for (list<Hsp>::iterator it = hsplist.begin(); it != hsplist.end(); it++)  {  tree.insert (it->x, it->y, it->length);  }

		/** We check the tree. */
        for (list<Hsp>::iterator it = hsplist.begin(); it != hsplist.end(); it++)
        {
            for (size_t i=0; i< it->length; i++)   {  CPPUNIT_ASSERT (tree.exists (it->x + i, it->y + i) == true);  }
        }
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testRangeTree5 ()
    {
        size_t qryLen = 20000, sbjLen=17000;
        size_t count=0;

        RangeTree::Coord x = 1000;
        RangeTree::Coord y =  500;
        size_t           n =   17;

        RangeTree tree (qryLen, sbjLen);

		size_t nbPieces   = 500;

		/** On cree une sorte de pointillé. */
        for (size_t i=0; i<nbPieces; i++)
        {
        	if (i%2==0)  {  tree.insert (x+i*n, y+i*n, n);  }
        }
        CPPUNIT_ASSERT (tree.getNbItems() == nbPieces/2);

		/** On verifie que les pointillés sont remplis. */
        count = 0;
        for (size_t i=0; i<nbPieces; i++)
        {
        	if (i%2==0)  {  for (size_t j=0; j<n; j++)    { count++;  CPPUNIT_ASSERT (tree.exists (x + n*i + j, y + n*i + j) == true);  }  }
        }
        CPPUNIT_ASSERT (count = n*nbPieces/2);

		/** On verifie que les complement n'est pas rempli. */
        count = 0;
        for (size_t i=0; i<nbPieces; i++)
        {
        	if (i%2==1)  {  for (size_t j=0; j<n; j++)    { count++;  CPPUNIT_ASSERT (tree.exists (x + n*i + j, y + n*i + j) == false);  }  }
        }
        CPPUNIT_ASSERT (count = n*nbPieces/2);

        /** On cree la ligne complete, ie. on complemente les poitilles. */
        tree.insert (x, y, n*nbPieces);

        CPPUNIT_ASSERT (tree.getNbItems() == nbPieces);

		/** On verifie que les pointillés sont remplis. */
        for (size_t i=0; i<nbPieces; i++)
        {
        	if (i%2==0)  {  for (size_t j=0; j<n; j++)    {  CPPUNIT_ASSERT (tree.exists (x + n*i + j, y + n*i + j) == true);  }  }
        }

		/** On verifie que les complement n'est pas rempli. */
        for (size_t i=0; i<nbPieces; i++)
        {
        	if (i%2==1)  {  for (size_t j=0; j<n; j++)    {  CPPUNIT_ASSERT (tree.exists (x + n*i + j, y + n*i + j) == true);  }  }
        }
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testRangeTree6 ()
    {
        //size_t qryLen = 10*1000*1000, sbjLen=10*1000*1000, maxLen=100;
        size_t qryLen = 10*1000, sbjLen=10*1000, maxLen=100;

        size_t nbsearch = 2000000;
        size_t step     = 1000000;
        size_t nmax     = 5;
        size_t tmax     = 16;

nbsearch /= 100;
step     /= 100;
tmax = 3;

        for (size_t t=0; t<tmax; t++)
        {
        	printf ("\n----------------------------------------------------------------\n");
        	printf ("USING A TREE WITH %d SUB TREES\n", (1<<t));

			RangeTree tree (qryLen, sbjLen, t);

			for (size_t n=1; n<=nmax; n++)
			{
				for (size_t i=1; i<=step; i++)  {  tree.insert (rand() % tree.getSizeX(), rand() % tree.getSizeY(), rand()%maxLen);  }

				u_int32_t t0 = DefaultFactory::time().gettime();

				for (size_t k=1; k<=nbsearch; k++)   {  tree.exists (rand() % qryLen, rand() % sbjLen);   }

				u_int32_t t1 = DefaultFactory::time().gettime();

				printf ("[%ld]  nbitems=%ld  nbsearch=%ld  time=%d msec  depth=%ld  log(nbItems)=%ld \n",
					n, tree.getNbItems(), nbsearch, t1-t0, tree.getDepth(), (size_t)log2(tree.getNbItems())
				);
			}
        }
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testRangeTree7 ()
    {
        size_t qryLen = 20*1000*1000, sbjLen=20*1000*1000, maxLen=100;
        //size_t qryLen = 10*1000, sbjLen=10*1000, maxLen=100;

        size_t step = 31;
        size_t nb   = step * 1000*50;
nb /= 1000;

		printf ("1) 2^n trees   2) nbItems   3) nbSearch   4) time (msec)   5) tree.depth   6) log2(nbItems) \n");

        for (size_t t=0; t<=20; t++)
        {
			RangeTree tree (qryLen, sbjLen, t);

#if 0
			for (size_t i=1; i<=nb / step; i++)
			{
				std::vector<RangeTree::Range> ranges(step);
				for (size_t j=0; j<step; j++)  { ranges[j].set (rand() % tree.getSizeX(), rand() % tree.getSizeY(), rand()%maxLen); }

				tree.insert (ranges);
			}
#else
			for (size_t i=1; i<=nb; i++)
			{
				tree.insert (rand() % tree.getSizeX(), rand() % tree.getSizeY(), rand()%maxLen);
			}
#endif

			u_int32_t t0 = DefaultFactory::time().gettime();

			for (size_t k=1; k<=nb; k++)   {  tree.exists (rand() % qryLen, rand() % sbjLen);   }

			u_int32_t t1 = DefaultFactory::time().gettime();

			printf ("%2ld  %ld  %ld  %d  %ld  %ld \n",
				t, tree.getNbItems(), nb, t1-t0, tree.getDepth(), (size_t)log2(tree.getNbItems())
			);
        }
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testRangeTree8 ()
    {
        size_t qryLen = 10, sbjLen=7;

        RangeTree tree (qryLen, sbjLen, 2);

        for (size_t x=0; x<qryLen; x++)
        {
            for (size_t y=0; y<sbjLen; y++)  {  tree.insert (x,y,1); }
        }

        CPPUNIT_ASSERT (tree.getNbItems() == qryLen*sbjLen);

        printf ("depth=%ld\n", tree.getDepth());
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testRangeTree9 ()
    {
    	size_t N = 1*1000*1000;

        size_t qryLen = 10*1000*1000, sbjLen=10*1000*1000, maxLen=100;

    	printf ("N=%ld  qryLen=%ld  sbjLen=%ld  maxLen=%ld \n", N, qryLen, sbjLen, maxLen);

        RangeTree tree (qryLen, sbjLen, 0);

        /** We fill the range tree. */
        u_int32_t t0 = DefaultFactory::time().gettime();
        for (size_t i=0; i<N; i++)  {  tree.insert (rand() % qryLen, rand() % sbjLen, 1 + rand()%maxLen) ;  }
        u_int32_t t1 = DefaultFactory::time().gettime();

        printf ("FILL    in %d msec => nbItems=%ld  log2(nbItems)=%.1f  depth=%ld\n", (t1-t0), tree.getNbItems(), log2(tree.getNbItems()), tree.getDepth());

        printf ("checkbalance=%d\n", tree.checkBalance());
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testRangeTree10 ()
    {
    	srand (123456789);

    	TimeInfo ti;

    	size_t nbItems  = 1 << 22;
    	size_t nbSearch = 1 << 22;

        size_t count = 0;

    	size_t N = 1 << 20;
    	size_t qryLen = N, sbjLen=N;

        RangeTree tree (qryLen, sbjLen);

        /** We fill a vector of random alpha. */
        RangeTree::Value* valuesX = new RangeTree::Value[nbItems];
        RangeTree::Value* valuesY = new RangeTree::Value[nbItems];

        for (size_t i=0; i<nbItems; i++)
        {
        	valuesX [i] = rand() % qryLen;
        	valuesY [i] = rand() % sbjLen;
        }

        ti.addEntry ("insert");
        for (size_t i=0; i<nbItems; i++, count++)
        {
        	tree.insert ( valuesX[i], valuesY[i],1) ;
        }
        ti.stopEntry ("insert");

        printf ("'insert' called %ld times in %d msec  => nbItems=%ld  depth=%ld\n",
			count, ti.getEntryByKey("insert"), tree.getNbItems(), tree.getDepth()
		);

		/** We check the tree. */
        count = 0;
        size_t nbTrue = 0;
        ti.addEntry ("exist");
        for (size_t i=0; i<nbSearch; i++, count++)
        {
			nbTrue += tree.exists (valuesX[i%nbItems], valuesY[i%nbItems]) ? 1 :0;
        }
        ti.stopEntry ("exist");

        CPPUNIT_ASSERT (nbTrue == nbSearch);

        printf ("'exist' called %ld times in %d msec  => %.3f nanosec/search   %d search/msec\n",
			count, ti.getEntryByKey("exist"),
			(double) (1000 * ti.getEntryByKey("exist")) / (double) count,
			count / ti.getEntryByKey("exist")
		);

        delete[] valuesX;
        delete[] valuesY;
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testRangeTree11 ()
    {
        size_t nbSearch = 1 << 22;

        size_t N = 1 << 18;
        size_t qryLen = N, sbjLen=N;

        size_t nbItems = 1;
        size_t kmax    = 28;
        size_t i=0;

        RangeTree tree (qryLen, sbjLen);

        for (size_t k=1; k<=kmax; k++)
        {
            TimeInfo ti;

            nbItems = 1 << k;

            while (tree.getNbItems() < nbItems)
            {
                tree.insert (rand() % qryLen, rand() % sbjLen, 1) ;
            }

            ti.addEntry ("exist");
            for (size_t i=0; i<nbSearch; i++)  {  tree.exists (rand() % qryLen, rand() % sbjLen);  }
            ti.stopEntry ("exist");

            printf ("%ld  %.1f  %ld  %.3f  %d \n",
                tree.getNbItems(),
                log2 (tree.getNbItems()),
                ti.getEntryByKey("exist"),
                (double) (1000 * ti.getEntryByKey("exist")) / (double) nbSearch,
                nbSearch / ti.getEntryByKey("exist")
            );
        }
    }

};

/********************************************************************************/

extern "C" Test* TestMisc_suite()  { return TestMisc::suite (); }

#endif
