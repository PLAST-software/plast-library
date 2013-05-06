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

#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/StringSequenceIterator.hpp>
#include <database/impl/ReverseStrandSequenceIterator.hpp>

#include <iostream>

using namespace std;
using namespace misc;
using namespace database;
using namespace database::impl;

using namespace os::impl;

extern const char* getPath (const char* file);

/********************************************************************************/

class TestBugs : public TestFixture
{
public:

    /********************************************************************************/
    void setUp    ()  {}
    void tearDown ()  {}

    /********************************************************************************/
    static Test* suite()
    {
    	 TestSuite* result = new TestSuite ("BugsTest");

         result->addTest (new TestCaller<TestBugs> ("testCheckComments", &TestBugs::testCheckComments) );
         result->addTest (new TestCaller<TestBugs> ("testCheckComments", &TestBugs::testCheckSpaces) );
         result->addTest (new TestCaller<TestBugs> ("testCheckTforU",    &TestBugs::testCheckTforU) );
         result->addTest (new TestCaller<TestBugs> ("testCheckEmptySeq", &TestBugs::testCheckEmptySeq) );

    	 return result;
    }

    /********************************************************************************/
    /********************************************************************************/
    void testCheckComments ()
    {
    	const char* comments[] =
    	{
			"A93610.1.1301 Archaea;Euryarchaeota;Thermococci;Thermococcales;Thermococcaceae;Thermococcus;unidentified",
			"AAAA02037088.4053.5842 Eukaryota;Opisthokonta;Fungi;Ascomycota;Pezizomycotina;Dothideomycetes;Dothideomycetidae;Oryza sativa Indica Group",
			"AAAA02044470.20.1735 Eukaryota;Archaeplastida;Chloroplastida;Charophyta;Phragmoplastophyta;Streptophyta;Embryophyta;Tracheophyta;Spermatophyta;Magnoliophyta;Liliopsida;Oryza sativa Indica Group",
			"A58083.1.1474 Bacteria;Actinobacteria;Actinobacteria;Corynebacteriales;Corynebacteriaceae;Corynebacterium;unidentified",
			"AAAA02045240.678.2192 Bacteria;Bacteroidetes;Flavobacteria;Flavobacteriales;Flavobacteriaceae;Flavobacterium;Oryza sativa Indica Group",
			"A97104.7.1517 Bacteria;Firmicutes;Bacilli;Lactobacillales;Streptococcaceae;Streptococcus;unidentified",
			"A27627.1.1472 Bacteria;Proteobacteria;Gammaproteobacteria;Pseudomonadales;Moraxellaceae;Moraxella;Moraxella catarrhalis",
			"AAAA02045958.193.1994 Eukaryota;Archaeplastida;Chloroplastida;Charophyta;Phragmoplastophyta;Streptophyta;Embryophyta;Tracheophyta;Spermatophyta;Magnoliophyta;Liliopsida;Oryza sativa Indica Group",
			"AAAA02016423.718.2179 Bacteria;Cyanobacteria;Chloroplast;Oryza sativa Indica Group",
			"AAAB01001758.22.1307 Eukaryota;Opisthokonta;Metazoa;Arthropoda;Hexapoda;Insecta;Anopheles gambiae str. PEST"
    	};

        ISequenceIterator* itSeq = new FastaSequenceIterator (getPath("bug15696.fa"), 1024);
        LOCAL (itSeq);

        size_t nbSeq = 0;

        for (itSeq->first(); ! itSeq->isDone(); itSeq->next (), nbSeq++)
        {
            const ISequence* seq = itSeq->currentItem();
            CPPUNIT_ASSERT (strcmp (seq->comment, comments[nbSeq]) == 0);
        }

        CPPUNIT_ASSERT (nbSeq == 10);
    }

    /********************************************************************************/
    /********************************************************************************/
    void testCheckSpaces ()
    {
    	size_t lengthTable[] =  {  1301, 1790, 1716, 1474, 1515, 1511, 1472, 1802, 1462, 1286 };

        ISequenceIterator* itSeq = new FastaSequenceIterator (getPath("bug15696.fa"), 1024);
        LOCAL (itSeq);

        size_t nbSeq = 0;

        for (itSeq->first(); ! itSeq->isDone(); itSeq->next (), nbSeq++)
        {
            const ISequence* seq = itSeq->currentItem();
            CPPUNIT_ASSERT (seq->getLength() == lengthTable[nbSeq]);
        }

        CPPUNIT_ASSERT (nbSeq == 10);
    }

    /********************************************************************************/
    /********************************************************************************/
    void testCheckTforU ()
    {
    	int A=0, C=1, G=2, T=3;

        /** WARNING !  We first switch to nucleotide alphabet. */
        EncodingManager::singleton().setKind (EncodingManager::ALPHABET_NUCLEOTID);

        ISequenceIterator* itSeq = new FastaSequenceIterator (getPath("bug15696.fa"), 1024);
        LOCAL (itSeq);

        size_t nbSeq = 0;

        for (itSeq->first(); ! itSeq->isDone(); itSeq->next (), nbSeq++)
        {
            const ISequence* seq = itSeq->currentItem();
            const LETTER* data = seq->data.letters.data;

            if (nbSeq == 0)  // start with CCACUGCUAU GGGGGUCCGA
            {
            	size_t table[] = {4, 7, 9, 15};
            	for (size_t i=0; i<sizeof(table)/sizeof(table[0]); i++)  {  CPPUNIT_ASSERT(data[table[i]] == T);  }
            }

            if (nbSeq == 2)  // start with CCUGGUUGAU CCUGCCAGUA
            {
            	size_t table[] = {2, 5, 6, 9, 12, 18};
            	for (size_t i=0; i<sizeof(table)/sizeof(table[0]); i++)  {  CPPUNIT_ASSERT(data[table[i]] == T);  }
            }

            if (nbSeq == 5)  // start with AGAGUUCGAU CCUGGCUCAG
            {
            	size_t table[] = {4, 5, 9, 12, 16};
            	for (size_t i=0; i<sizeof(table)/sizeof(table[0]); i++)  {  CPPUNIT_ASSERT(data[table[i]] == T);  }
            }

            if (nbSeq == 9)  // start with GUUGCGGUUA AAACGUUCGA
            {
            	size_t table[] = {1, 2, 7, 8, 15, 16};
            	for (size_t i=0; i<sizeof(table)/sizeof(table[0]); i++)  {  CPPUNIT_ASSERT(data[table[i]] == T);  }
            }
        }

        CPPUNIT_ASSERT (nbSeq == 10);

        /** WARNING !  We switch back to amino acid alphabet. */
        EncodingManager::singleton().setKind (EncodingManager::ALPHABET_AMINO_ACID);
    }

    /********************************************************************************/
    /********************************************************************************/
    void testCheckEmptySeq_aux (ISequenceIterator* itSeq)
    {
        CPPUNIT_ASSERT (itSeq);

        u_int32_t nbSeq      = 0;
        u_int64_t dataLength = 0;

        LOCAL (itSeq);

        for (itSeq->first(); ! itSeq->isDone(); itSeq->next (), nbSeq++)
        {
            const ISequence* seq = itSeq->currentItem();

            if (nbSeq==0)
            {
                CPPUNIT_ASSERT (seq->getLength() == 60);
                CPPUNIT_ASSERT (strcmp(seq->comment, "seq1")==0);
            }

            if (nbSeq==1)
            {
                CPPUNIT_ASSERT (seq->getLength() == 0);
                CPPUNIT_ASSERT (strcmp(seq->comment, "seq2")==0);
            }

            if (nbSeq==2)
            {
                CPPUNIT_ASSERT (seq->getLength() == 60);
                CPPUNIT_ASSERT (strcmp(seq->comment, "seq3")==0);
            }

            dataLength += seq->getLength();
        }

        CPPUNIT_ASSERT (nbSeq      == 3);
        CPPUNIT_ASSERT (dataLength == 120);
    }

    /** */
    void testCheckEmptySeq ()
    {
        testCheckEmptySeq_aux (new FastaSequenceIterator (getPath("bug15696b.fa"), 1024));
        testCheckEmptySeq_aux (new ReverseStrandSequenceIterator (new FastaSequenceIterator (getPath("bug15696b.fa"), 1024)));
    }
};

/********************************************************************************/

extern "C" Test* TestBugs_suite()  { return TestBugs::suite (); }
