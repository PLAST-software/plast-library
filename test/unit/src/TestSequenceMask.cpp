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

#include <seg/impl/DustMasker.hpp>

#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/BufferedSequenceDatabase.hpp>

#include <misc/api/types.hpp>

#include <string.h>

using namespace std;
using namespace misc;
using namespace database;
using namespace database::impl;

extern const char* getPath (const char* file);

/********************************************************************************/

class TestSequenceMask : public TestFixture
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
    	 TestSuite* result = new TestSuite ("PlastTestSequenceMask");
         //result->addTest (new TestCaller<TestSequenceMask> ("test_dustmasker",  &TestSequenceMask::test_dustmasker) );
         result->addTest (new TestCaller<TestSequenceMask> ("test_maskdb",      &TestSequenceMask::test_maskdb) );
         return result;
    }


    /********************************************************************************/
    void test_dustmasker ()
    {
        const char* seq =
            "CGCTACAGCAGCTAGTTCATCATTGTTTATCAATGATAAAATATAATAAGCTAAAAGGAAACTATAAATA"
            "ACCATGTATAATTATAAGTAGGTACCTATTTTTTTATTTTAAACTGAAATTCAATATTATATAGGCAAAG"
            "ACTTAGATGTAAGATTTCGAAGACTTGGATGTAAACAACAAATAAGATAATAACCATAAAAATAGAAATG"
            "AACGATATTAAAATTAAAAAATACGAAAAAACTAACACGTATTGTGTCCAATAAATTCGATTTGATAATT"
            "AGGTAACAATTTAACGTTAAAACCTATTCTTTTATTATCCGAAAATCCGTCGTGGAATTTGTATTAGCTT"
            "TTTTTCTACATTACCCGTTTGCGAGACAGGTGGGGTCAGACGTAGACGTAGTCTCTGGAGTCAAGACGAA"
            "ATTTTACATTTCACAATTTCCTATAGGCCGAGCAAAATTTATTAAGAACCCACAGGCATCATTACGTTTT"
            "CTTGCACAGAAGACTTCACGCTGAAGTCATTGGGCTATATTTCAACGAGACGTCTGTTGGTTTATAAAGG"
            "GCTATATTTATACAAGAATAGGAGTATGGCAGTATGCTAGGCTGGTATGTAGTACGTATACCTCCTAAGC"
            "CGAAAGGCAGTAAGTGACGATGTAATAGTTTTGAGGAAAATTACTTTTTCTGAATAATATTTTTATTTTT"
            "GTTTGCATTTTGTTAAAATTATTTACTAAATTAATGATTCTCATATGTTTTTTCATAGATTTGATGAACT"
            "ACTGTACCATCTGATTAGCGCATGGTCATAGCTGTTTCCTGTGTGAAATTGTTATCCGCTCACAATTCCA"
            "CACAACATACGAGCCGGAGCATAAAGTGTAAAGCCTGGGGTGCCTAATGAGTGAGCTACTCACATAATTG"
            "CGTGCGCTCACTGCCCGCTTTCCAGTCGGAAACCTGTCGTGCCAGCTGCATTATTGATCCGCCACGCCCC";

        size_t len = strlen (seq);

        vector<RangeU32> regions;

        DustMasker().compute (seq, len, regions);

        for (vector<RangeU32>::iterator it = regions.begin(); it != regions.end(); it++)
        {
            //cout << "region " << *it << endl;
        }
    }

    /********************************************************************************/
    void test_maskdb ()
    {
        const char* file = "aphid_10000.fa" ;

        ISequenceIterator* seqIter = new FastaSequenceIterator (getPath (file), 100);
        CPPUNIT_ASSERT (seqIter != 0);

        DustMasker masker;
        vector<RangeU32> regions;

        for (seqIter->first(); !seqIter->isDone(); seqIter->next ())
        {
            const ISequence* seq = seqIter->currentItem();

            masker.compute ((const char*) seq->data.letters.data, seq->data.letters.size, regions);

            //printf (">%s\n",seq->comment);
            for (vector<RangeU32>::iterator it = regions.begin(); it != regions.end(); it++)
            {
                //cout << it->begin << " - "  << it->end << endl;
            }

        }

    }
};

/********************************************************************************/

extern "C" Test* TestSequenceMask_suite()  { return TestSequenceMask::suite (); }

