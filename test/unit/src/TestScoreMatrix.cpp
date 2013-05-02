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

#include <os/impl/DefaultOsFactory.hpp>

#include <database/api/IAlphabet.hpp>
#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/StringSequenceIterator.hpp>
#include <database/impl/BufferedSequenceDatabase.hpp>

#include <algo/core/impl/ScoreMatrix.hpp>

#include <alignment/core/impl/UngapAlignmentContainer.hpp>

#include <alignment/tools/impl/AlignmentSplitter.hpp>
#include <alignment/tools/impl/AlignmentSplitterBanded.hpp>

#include <alignment/tools/impl/SemiGappedAlign.hpp>

using namespace std;
using namespace os;
using namespace os::impl;
using namespace database;
using namespace database::impl;
using namespace algo::core;
using namespace algo::core::impl;
using namespace alignment::core::impl;
using namespace alignment::tools;
using namespace alignment::tools::impl;

/********************************************************************************/

class TestScoreMatrix : public TestFixture
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
         TestSuite* result = new TestSuite ("ScoreMatrixTest");

         result->addTest (new TestCaller<TestScoreMatrix> ("testEncoding",        &TestScoreMatrix::testEncoding ) );
         result->addTest (new TestCaller<TestScoreMatrix> ("testBlosumRetrieval", &TestScoreMatrix::testBlosumRetrieval ) );
         result->addTest (new TestCaller<TestScoreMatrix> ("testBlosumCheck62",   &TestScoreMatrix::testBlosumCheck62 ) );
         result->addTest (new TestCaller<TestScoreMatrix> ("testBlosumCheck50",   &TestScoreMatrix::testBlosumCheck50 ) );
         result->addTest (new TestCaller<TestScoreMatrix> ("testSplitter",        &TestScoreMatrix::testSplitter) );
//         result->addTest (new TestCaller<TestScoreMatrix> ("testSplitter2",        &TestScoreMatrix::testSplitter2) );
         result->addTest (new TestCaller<TestScoreMatrix> ("testSemiGappedAlign", &TestScoreMatrix::testSemiGappedAlign) );
         result->addTest (new TestCaller<TestScoreMatrix> ("testDumpMatrix",      &TestScoreMatrix::testDumpMatrix) );
         result->addTest (new TestCaller<TestScoreMatrix> ("testDoubleScore",     &TestScoreMatrix::testDoubleScore) );

         return result;
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testEncoding ()
    {
        const LETTER* convert1 = EncodingManager::singleton().getEncodingConversion (SUBSEED, NCBI);
        const LETTER* convert2 = EncodingManager::singleton().getEncodingConversion (NCBI,    SUBSEED);

        for (LETTER l0=CODE_A; l0<CODE_BAD; l0++)
        {
            LETTER l1 = convert1 [(int)l0];
            LETTER l2 = convert2 [(int)l1];

            CPPUNIT_ASSERT (l0 == l2);
        }
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testBlosumRetrieval ()
    {
        /** We check that we can retrieve the wanted matrix. */
        IScoreMatrix* m1 = ScoreMatrixManager::singleton().getMatrix ("BLOSUM62", NCBI, 0, 0);
        CPPUNIT_ASSERT (m1 != 0);
        LOCAL (m1);

        IScoreMatrix* m2 = ScoreMatrixManager::singleton().getMatrix ("BLOSUM50", NCBI, 0, 0);
        CPPUNIT_ASSERT (m2 != 0);
        LOCAL (m2);

        /** We check that we can't retrieve the wanted matrix. */
        CPPUNIT_ASSERT_THROW (ScoreMatrixManager::singleton().getMatrix ("foo", NCBI, 0, 0), ScoreMatrixFailure);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testBlosumCheck62_aux (Encoding encoding)
    {
        try  {
            /** We retrieve a score matrix. */
            IScoreMatrix* scoreMatrix = ScoreMatrixManager::singleton().getMatrix ("BLOSUM62", encoding, 0, 0);

            /** We check that we got the matrix. */
            CPPUNIT_ASSERT (scoreMatrix != 0);
            LOCAL (scoreMatrix);

            /** We use a little shortcut. */
            int8_t** m = scoreMatrix->getMatrix();
            CPPUNIT_ASSERT (m != 0);

            /** We retrieve a converter from ASCII to NCBI alphabet. */
            const LETTER* convert = EncodingManager::singleton().getEncodingConversion (ASCII, encoding);
            CPPUNIT_ASSERT (convert != 0);

            /** NOTE !  found m[E,W]=-2 in book "Bioinformatique, Génomique et post-génomique" from F.Dardel & F.Képès */

            /** A little help. */
            #define CHECK(a,b,score)  CPPUNIT_ASSERT ((m) [(int)convert[a]] [(int)convert[b]] == score);

#ifdef BIGTEST
            /** We test some scores. */
            CHECK('A','A',  4);     CHECK('A','R', -1);     CHECK('A','N', -2);     CHECK('A','D', -2);
            CHECK('A','C',  0);     CHECK('A','Q', -1);     CHECK('A','E', -1);     CHECK('A','G',  0);
            CHECK('A','H', -2);     CHECK('A','I', -1);     CHECK('A','L', -1);     CHECK('A','K', -1);
            CHECK('A','M', -1);     CHECK('A','F', -2);     CHECK('A','P', -1);     CHECK('A','S',  1);
            CHECK('A','T',  0);     CHECK('A','W', -3);     CHECK('A','Y', -2);     CHECK('A','V',  0);
            CHECK('A','B', -2);     CHECK('A','J', -1);     CHECK('A','Z', -1);     CHECK('A','X', -1);
            CHECK('A','*', -4);

            CHECK('R','A', -1);     CHECK('R','R',  5);     CHECK('R','N',  0);     CHECK('R','D', -2);
            CHECK('R','C', -3);     CHECK('R','Q',  1);     CHECK('R','E',  0);     CHECK('R','G', -2);
            CHECK('R','H',  0);     CHECK('R','I', -3);     CHECK('R','L', -2);     CHECK('R','K',  2);
            CHECK('R','M', -1);     CHECK('R','F', -3);     CHECK('R','P', -2);     CHECK('R','S', -1);
            CHECK('R','T', -1);     CHECK('R','W', -3);     CHECK('R','Y', -2);     CHECK('R','V', -3);
            CHECK('R','B', -1);     CHECK('R','J', -2);     CHECK('R','Z',  0);     CHECK('R','X', -1);
            CHECK('R','*', -4);

            CHECK('N','A', -2);     CHECK('N','R',  0);     CHECK('N','N',  6);     CHECK('N','D',  1);
            CHECK('N','C', -3);     CHECK('N','Q',  0);     CHECK('N','E',  0);     CHECK('N','G',  0);
            CHECK('N','H',  1);     CHECK('N','I', -3);     CHECK('N','L', -3);     CHECK('N','K',  0);
            CHECK('N','M', -2);     CHECK('N','F', -3);     CHECK('N','P', -2);     CHECK('N','S',  1);
            CHECK('N','T',  0);     CHECK('N','W', -4);     CHECK('N','Y', -2);     CHECK('N','V', -3);
            CHECK('N','B',  4);     CHECK('N','J', -3);     CHECK('N','Z',  0);     CHECK('N','X', -1);
            CHECK('N','*', -4);

            CHECK('D','A', -2);     CHECK('D','R', -2);     CHECK('D','N',  1);     CHECK('D','D',  6);
            CHECK('D','C', -3);     CHECK('D','Q',  0);     CHECK('D','E',  2);     CHECK('D','G', -1);
            CHECK('D','H', -1);     CHECK('D','I', -3);     CHECK('D','L', -4);     CHECK('D','K', -1);
            CHECK('D','M', -3);     CHECK('D','F', -3);     CHECK('D','P', -1);     CHECK('D','S',  0);
            CHECK('D','T', -1);     CHECK('D','W', -4);     CHECK('D','Y', -3);     CHECK('D','V', -3);
            CHECK('D','B',  4);     CHECK('D','J', -3);     CHECK('D','Z',  1);     CHECK('D','X', -1);
            CHECK('D','*', -4);

            CHECK('C','A',  0);     CHECK('C','R', -3);     CHECK('C','N', -3);     CHECK('C','D', -3);
            CHECK('C','C',  9);     CHECK('C','Q', -3);     CHECK('C','E', -4);     CHECK('C','G', -3);
            CHECK('C','H', -3);     CHECK('C','I', -1);     CHECK('C','L', -1);     CHECK('C','K', -3);
            CHECK('C','M', -1);     CHECK('C','F', -2);     CHECK('C','P', -3);     CHECK('C','S', -1);
            CHECK('C','T', -1);     CHECK('C','W', -2);     CHECK('C','Y', -2);     CHECK('C','V', -1);
            CHECK('C','B', -3);     CHECK('C','J', -1);     CHECK('C','Z', -3);     CHECK('C','X', -1);
            CHECK('C','*', -4);

            CHECK('Q','A', -1);     CHECK('Q','R',  1);     CHECK('Q','N',  0);     CHECK('Q','D',  0);
            CHECK('Q','C', -3);     CHECK('Q','Q',  5);     CHECK('Q','E',  2);     CHECK('Q','G', -2);
            CHECK('Q','H',  0);     CHECK('Q','I', -3);     CHECK('Q','L', -2);     CHECK('Q','K',  1);
            CHECK('Q','M',  0);     CHECK('Q','F', -3);     CHECK('Q','P', -1);     CHECK('Q','S',  0);
            CHECK('Q','T', -1);     CHECK('Q','W', -2);     CHECK('Q','Y', -1);     CHECK('Q','V', -2);
            CHECK('Q','B',  0);     CHECK('Q','J', -2);     CHECK('Q','Z',  4);     CHECK('Q','X', -1);
            CHECK('Q','*', -4);

            CHECK('E','A', -1);     CHECK('E','R',  0);     CHECK('E','N',  0);     CHECK('E','D',  2);
            CHECK('E','C', -4);     CHECK('E','Q',  2);     CHECK('E','E',  5);     CHECK('E','G', -2);
            CHECK('E','H',  0);     CHECK('E','I', -3);     CHECK('E','L', -3);     CHECK('E','K',  1);
            CHECK('E','M', -2);     CHECK('E','F', -3);     CHECK('E','P', -1);     CHECK('E','S',  0);
            CHECK('E','T', -1);     CHECK('E','W', -3);     CHECK('E','Y', -2);     CHECK('E','V', -2);
            CHECK('E','B',  1);     CHECK('E','J', -3);     CHECK('E','Z',  4);     CHECK('E','X', -1);
            CHECK('E','*', -4);

            CHECK('G','A',  0);     CHECK('G','R', -2);     CHECK('G','N',  0);     CHECK('G','D', -1);
            CHECK('G','C', -3);     CHECK('G','Q', -2);     CHECK('G','E', -2);     CHECK('G','G',  6);
            CHECK('G','H', -2);     CHECK('G','I', -4);     CHECK('G','L', -4);     CHECK('G','K', -2);
            CHECK('G','M', -3);     CHECK('G','F', -3);     CHECK('G','P', -2);     CHECK('G','S',  0);
            CHECK('G','T', -2);     CHECK('G','W', -2);     CHECK('G','Y', -3);     CHECK('G','V', -3);
            CHECK('G','B', -1);     CHECK('G','J', -4);     CHECK('G','Z', -2);     CHECK('G','X', -1);
            CHECK('G','*', -4);

            CHECK('H','A', -2);     CHECK('H','R',  0);     CHECK('H','N',  1);     CHECK('H','D', -1);
            CHECK('H','C', -3);     CHECK('H','Q',  0);     CHECK('H','E',  0);     CHECK('H','G', -2);
            CHECK('H','H',  8);     CHECK('H','I', -3);     CHECK('H','L', -3);     CHECK('H','K', -1);
            CHECK('H','M', -2);     CHECK('H','F', -1);     CHECK('H','P', -2);     CHECK('H','S', -1);
            CHECK('H','T', -2);     CHECK('H','W', -2);     CHECK('H','Y',  2);     CHECK('H','V', -3);
            CHECK('H','B',  0);     CHECK('H','J', -3);     CHECK('H','Z',  0);     CHECK('H','X', -1);
            CHECK('H','*', -4);

            CHECK('I','A', -1);     CHECK('I','R', -3);     CHECK('I','N', -3);     CHECK('I','D', -3);
            CHECK('I','C', -1);     CHECK('I','Q', -3);     CHECK('I','E', -3);     CHECK('I','G', -4);
            CHECK('I','H', -3);     CHECK('I','I',  4);     CHECK('I','L',  2);     CHECK('I','K', -3);
            CHECK('I','M',  1);     CHECK('I','F',  0);     CHECK('I','P', -3);     CHECK('I','S', -2);
            CHECK('I','T', -1);     CHECK('I','W', -3);     CHECK('I','Y', -1);     CHECK('I','V',  3);
            CHECK('I','B', -3);     CHECK('I','J',  3);     CHECK('I','Z', -3);     CHECK('I','X', -1);
            CHECK('I','*', -4);

            CHECK('L','A', -1);     CHECK('L','R', -2);     CHECK('L','N', -3);     CHECK('L','D', -4);
            CHECK('L','C', -1);     CHECK('L','Q', -2);     CHECK('L','E', -3);     CHECK('L','G', -4);
            CHECK('L','H', -3);     CHECK('L','I',  2);     CHECK('L','L',  4);     CHECK('L','K', -2);
            CHECK('L','M',  2);     CHECK('L','F',  0);     CHECK('L','P', -3);     CHECK('L','S', -2);
            CHECK('L','T', -1);     CHECK('L','W', -2);     CHECK('L','Y', -1);     CHECK('L','V',  1);
            CHECK('L','B', -4);     CHECK('L','J',  3);     CHECK('L','Z', -3);     CHECK('L','X', -1);
            CHECK('L','*', -4);

            CHECK('K','A', -1);     CHECK('K','R',  2);     CHECK('K','N',  0);     CHECK('K','D', -1);
            CHECK('K','C', -3);     CHECK('K','Q',  1);     CHECK('K','E',  1);     CHECK('K','G', -2);
            CHECK('K','H', -1);     CHECK('K','I', -3);     CHECK('K','L', -2);     CHECK('K','K',  5);
            CHECK('K','M', -1);     CHECK('K','F', -3);     CHECK('K','P', -1);     CHECK('K','S',  0);
            CHECK('K','T', -1);     CHECK('K','W', -3);     CHECK('K','Y', -2);     CHECK('K','V', -2);
            CHECK('K','B',  0);     CHECK('K','J', -3);     CHECK('K','Z',  1);     CHECK('K','X', -1);
            CHECK('K','*', -4);

            CHECK('M','A', -1);     CHECK('M','R', -1);     CHECK('M','N', -2);     CHECK('M','D', -3);
            CHECK('M','C', -1);     CHECK('M','Q',  0);     CHECK('M','E', -2);     CHECK('M','G', -3);
            CHECK('M','H', -2);     CHECK('M','I',  1);     CHECK('M','L',  2);     CHECK('M','K', -1);
            CHECK('M','M',  5);     CHECK('M','F',  0);     CHECK('M','P', -2);     CHECK('M','S', -1);
            CHECK('M','T', -1);     CHECK('M','W', -1);     CHECK('M','Y', -1);     CHECK('M','V',  1);
            CHECK('M','B', -3);     CHECK('M','J',  2);     CHECK('M','Z', -1);     CHECK('M','X', -1);
            CHECK('M','*', -4);

            CHECK('F','A', -2);     CHECK('F','R', -3);     CHECK('F','N', -3);     CHECK('F','D', -3);
            CHECK('F','C', -2);     CHECK('F','Q', -3);     CHECK('F','E', -3);     CHECK('F','G', -3);
            CHECK('F','H', -1);     CHECK('F','I',  0);     CHECK('F','L',  0);     CHECK('F','K', -3);
            CHECK('F','M',  0);     CHECK('F','F',  6);     CHECK('F','P', -4);     CHECK('F','S', -2);
            CHECK('F','T', -2);     CHECK('F','W',  1);     CHECK('F','Y',  3);     CHECK('F','V', -1);
            CHECK('F','B', -3);     CHECK('F','J',  0);     CHECK('F','Z', -3);     CHECK('F','X', -1);
            CHECK('F','*', -4);

            CHECK('P','A', -1);     CHECK('P','R', -2);     CHECK('P','N', -2);     CHECK('P','D', -1);
            CHECK('P','C', -3);     CHECK('P','Q', -1);     CHECK('P','E', -1);     CHECK('P','G', -2);
            CHECK('P','H', -2);     CHECK('P','I', -3);     CHECK('P','L', -3);     CHECK('P','K', -1);
            CHECK('P','M', -2);     CHECK('P','F', -4);     CHECK('P','P',  7);     CHECK('P','S', -1);
            CHECK('P','T', -1);     CHECK('P','W', -4);     CHECK('P','Y', -3);     CHECK('P','V', -2);
            CHECK('P','B', -2);     CHECK('P','J', -3);     CHECK('P','Z', -1);     CHECK('P','X', -1);
            CHECK('P','*', -4);

            CHECK('S','A',  1);     CHECK('S','R', -1);     CHECK('S','N',  1);     CHECK('S','D',  0);
            CHECK('S','C', -1);     CHECK('S','Q',  0);     CHECK('S','E',  0);     CHECK('S','G',  0);
            CHECK('S','H', -1);     CHECK('S','I', -2);     CHECK('S','L', -2);     CHECK('S','K',  0);
            CHECK('S','M', -1);     CHECK('S','F', -2);     CHECK('S','P', -1);     CHECK('S','S',  4);
            CHECK('S','T',  1);     CHECK('S','W', -3);     CHECK('S','Y', -2);     CHECK('S','V', -2);
            CHECK('S','B',  0);     CHECK('S','J', -2);     CHECK('S','Z',  0);     CHECK('S','X', -1);
            CHECK('S','*', -4);

            CHECK('T','A',  0);     CHECK('T','R', -1);     CHECK('T','N',  0);     CHECK('T','D', -1);
            CHECK('T','C', -1);     CHECK('T','Q', -1);     CHECK('T','E', -1);     CHECK('T','G', -2);
            CHECK('T','H', -2);     CHECK('T','I', -1);     CHECK('T','L', -1);     CHECK('T','K', -1);
            CHECK('T','M', -1);     CHECK('T','F', -2);     CHECK('T','P', -1);     CHECK('T','S',  1);
            CHECK('T','T',  5);     CHECK('T','W', -2);     CHECK('T','Y', -2);     CHECK('T','V',  0);
            CHECK('T','B', -1);     CHECK('T','J', -1);     CHECK('T','Z', -1);     CHECK('T','X', -1);
            CHECK('T','*', -4);

            CHECK('W','A', -3);     CHECK('W','R', -3);     CHECK('W','N', -4);     CHECK('W','D', -4);
            CHECK('W','C', -2);     CHECK('W','Q', -2);     CHECK('W','E', -3);     CHECK('W','G', -2);
            CHECK('W','H', -2);     CHECK('W','I', -3);     CHECK('W','L', -2);     CHECK('W','K', -3);
            CHECK('W','M', -1);     CHECK('W','F',  1);     CHECK('W','P', -4);     CHECK('W','S', -3);
            CHECK('W','T', -2);     CHECK('W','W', 11);     CHECK('W','Y',  2);     CHECK('W','V', -3);
            CHECK('W','B', -4);     CHECK('W','J', -2);     CHECK('W','Z', -2);     CHECK('W','X', -1);
            CHECK('W','*', -4);

            CHECK('Y','A', -2);     CHECK('Y','R', -2);     CHECK('Y','N', -2);     CHECK('Y','D', -3);
            CHECK('Y','C', -2);     CHECK('Y','Q', -1);     CHECK('Y','E', -2);     CHECK('Y','G', -3);
            CHECK('Y','H',  2);     CHECK('Y','I', -1);     CHECK('Y','L', -1);     CHECK('Y','K', -2);
            CHECK('Y','M', -1);     CHECK('Y','F',  3);     CHECK('Y','P', -3);     CHECK('Y','S', -2);
            CHECK('Y','T', -2);     CHECK('Y','W',  2);     CHECK('Y','Y',  7);     CHECK('Y','V', -1);
            CHECK('Y','B', -3);     CHECK('Y','J', -1);     CHECK('Y','Z', -2);     CHECK('Y','X', -1);
            CHECK('Y','*', -4);

            CHECK('V','A',  0);     CHECK('V','R', -3);     CHECK('V','N', -3);     CHECK('V','D', -3);
            CHECK('V','C', -1);     CHECK('V','Q', -2);     CHECK('V','E', -2);     CHECK('V','G', -3);
            CHECK('V','H', -3);     CHECK('V','I',  3);     CHECK('V','L',  1);     CHECK('V','K', -2);
            CHECK('V','M',  1);     CHECK('V','F', -1);     CHECK('V','P', -2);     CHECK('V','S', -2);
            CHECK('V','T',  0);     CHECK('V','W', -3);     CHECK('V','Y', -1);     CHECK('V','V',  4);
            CHECK('V','B', -3);     CHECK('V','J',  2);     CHECK('V','Z', -2);     CHECK('V','X', -1);
            CHECK('V','*', -4);

            CHECK('B','A', -2);     CHECK('B','R', -1);     CHECK('B','N',  4);     CHECK('B','D',  4);
            CHECK('B','C', -3);     CHECK('B','Q',  0);     CHECK('B','E',  1);     CHECK('B','G', -1);
            CHECK('B','H',  0);     CHECK('B','I', -3);     CHECK('B','L', -4);     CHECK('B','K',  0);
            CHECK('B','M', -3);     CHECK('B','F', -3);     CHECK('B','P', -2);     CHECK('B','S',  0);
            CHECK('B','T', -1);     CHECK('B','W', -4);     CHECK('B','Y', -3);     CHECK('B','V', -3);
            CHECK('B','B',  4);     CHECK('B','J', -3);     CHECK('B','Z',  0);     CHECK('B','X', -1);
            CHECK('B','*', -4);

            CHECK('J','A', -1);     CHECK('J','R', -2);     CHECK('J','N', -3);     CHECK('J','D', -3);
            CHECK('J','C', -1);     CHECK('J','Q', -2);     CHECK('J','E', -3);     CHECK('J','G', -4);
            CHECK('J','H', -3);     CHECK('J','I',  3);     CHECK('J','L',  3);     CHECK('J','K', -3);
            CHECK('J','M',  2);     CHECK('J','F',  0);     CHECK('J','P', -3);     CHECK('J','S', -2);
            CHECK('J','T', -1);     CHECK('J','W', -2);     CHECK('J','Y', -1);     CHECK('J','V',  2);
            CHECK('J','B', -3);     CHECK('J','J',  3);     CHECK('J','Z', -3);     CHECK('J','X', -1);
            CHECK('J','*', -4);

            CHECK('Z','A', -1);     CHECK('Z','R',  0);     CHECK('Z','N',  0);     CHECK('Z','D',  1);
            CHECK('Z','C', -3);     CHECK('Z','Q',  4);     CHECK('Z','E',  4);     CHECK('Z','G', -2);
            CHECK('Z','H',  0);     CHECK('Z','I', -3);     CHECK('Z','L', -3);     CHECK('Z','K',  1);
            CHECK('Z','M', -1);     CHECK('Z','F', -3);     CHECK('Z','P', -1);     CHECK('Z','S',  0);
            CHECK('Z','T', -1);     CHECK('Z','W', -2);     CHECK('Z','Y', -2);     CHECK('Z','V', -2);
            CHECK('Z','B',  0);     CHECK('Z','J', -3);     CHECK('Z','Z',  4);     CHECK('Z','X', -1);
            CHECK('Z','*', -4);

            CHECK('X','A', -1);     CHECK('X','R', -1);     CHECK('X','N', -1);     CHECK('X','D', -1);
            CHECK('X','C', -1);     CHECK('X','Q', -1);     CHECK('X','E', -1);     CHECK('X','G', -1);
            CHECK('X','H', -1);     CHECK('X','I', -1);     CHECK('X','L', -1);     CHECK('X','K', -1);
            CHECK('X','M', -1);     CHECK('X','F', -1);     CHECK('X','P', -1);     CHECK('X','S', -1);
            CHECK('X','T', -1);     CHECK('X','W', -1);     CHECK('X','Y', -1);     CHECK('X','V', -1);
            CHECK('X','B', -1);     CHECK('X','J', -1);     CHECK('X','Z', -1);     CHECK('X','X', -1);
            CHECK('X','*', -4);

            CHECK('*','A', -4);     CHECK('*','R', -4);     CHECK('*','N', -4);     CHECK('*','D', -4);
            CHECK('*','C', -4);     CHECK('*','Q', -4);     CHECK('*','E', -4);     CHECK('*','G', -4);
            CHECK('*','H', -4);     CHECK('*','I', -4);     CHECK('*','L', -4);     CHECK('*','K', -4);
            CHECK('*','M', -4);     CHECK('*','F', -4);     CHECK('*','P', -4);     CHECK('*','S', -4);
            CHECK('*','T', -4);     CHECK('*','W', -4);     CHECK('*','Y', -4);     CHECK('*','V', -4);
            CHECK('*','B', -4);     CHECK('*','J', -4);     CHECK('*','Z', -4);     CHECK('*','X', -4);
#endif
        }
        catch (ScoreMatrixFailure & e)  {  CPPUNIT_ASSERT (false);  }
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testBlosumCheck62 ()
    {
        testBlosumCheck62_aux (SUBSEED);
        testBlosumCheck62_aux (NCBI);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testBlosumCheck50_aux (Encoding encoding)
    {
        try  {
            /** We retrieve a score matrix. */
            IScoreMatrix* scoreMatrix = ScoreMatrixManager::singleton().getMatrix ("BLOSUM50", encoding, 0, 0);

            /** We check that we got the matrix. */
            CPPUNIT_ASSERT (scoreMatrix != 0);
            LOCAL (scoreMatrix);

            /** We use a little shortcut. */
            int8_t** m = scoreMatrix->getMatrix();
            CPPUNIT_ASSERT (m != 0);

            /** We retrieve a converter from ASCII to NCBI alphabet. */
            const LETTER* convert = EncodingManager::singleton().getEncodingConversion (ASCII, encoding);
            CPPUNIT_ASSERT (convert != 0);

            /** A little help. */
            #define CHECK(a,b,score)  CPPUNIT_ASSERT ((m) [(int)convert[a]] [(int)convert[b]] == score);

#ifdef BIGTEST
            /** We test some scores. */
            CHECK('A','A',  5);     CHECK('A','R', -2);     CHECK('A','N', -1);     CHECK('A','D', -2);
            CHECK('A','C', -1);     CHECK('A','Q', -1);     CHECK('A','E', -1);     CHECK('A','G',  0);
            CHECK('A','H', -2);     CHECK('A','I', -1);     CHECK('A','L', -2);     CHECK('A','K', -1);
            CHECK('A','M', -1);     CHECK('A','F', -3);     CHECK('A','P', -1);     CHECK('A','S',  1);
            CHECK('A','T',  0);     CHECK('A','W', -3);     CHECK('A','Y', -2);     CHECK('A','V',  0);
            CHECK('A','B', -2);     CHECK('A','J', -2);     CHECK('A','Z', -1);     CHECK('A','X', -1);
            CHECK('A','*', -5);

            CHECK('R','A', -2);     CHECK('R','R',  7);     CHECK('R','N', -1);     CHECK('R','D', -2);
            CHECK('R','C', -4);     CHECK('R','Q',  1);     CHECK('R','E',  0);     CHECK('R','G', -3);
            CHECK('R','H',  0);     CHECK('R','I', -4);     CHECK('R','L', -3);     CHECK('R','K',  3);
            CHECK('R','M', -2);     CHECK('R','F', -3);     CHECK('R','P', -3);     CHECK('R','S', -1);
            CHECK('R','T', -1);     CHECK('R','W', -3);     CHECK('R','Y', -1);     CHECK('R','V', -3);
            CHECK('R','B', -1);     CHECK('R','J', -3);     CHECK('R','Z',  0);     CHECK('R','X', -1);
            CHECK('R','*', -5);

            CHECK('N','A', -1);     CHECK('N','R', -1);     CHECK('N','N',  7);     CHECK('N','D',  2);
            CHECK('N','C', -2);     CHECK('N','Q',  0);     CHECK('N','E',  0);     CHECK('N','G',  0);
            CHECK('N','H',  1);     CHECK('N','I', -3);     CHECK('N','L', -4);     CHECK('N','K',  0);
            CHECK('N','M', -2);     CHECK('N','F', -4);     CHECK('N','P', -2);     CHECK('N','S',  1);
            CHECK('N','T',  0);     CHECK('N','W', -4);     CHECK('N','Y', -2);     CHECK('N','V', -3);
            CHECK('N','B',  5);     CHECK('N','J', -4);     CHECK('N','Z',  0);     CHECK('N','X', -1);
            CHECK('N','*', -5);

            CHECK('D','A', -2);     CHECK('D','R', -2);     CHECK('D','N',  2);     CHECK('D','D',  8);
            CHECK('D','C', -4);     CHECK('D','Q',  0);     CHECK('D','E',  2);     CHECK('D','G', -1);
            CHECK('D','H', -1);     CHECK('D','I', -4);     CHECK('D','L', -4);     CHECK('D','K', -1);
            CHECK('D','M', -4);     CHECK('D','F', -5);     CHECK('D','P', -1);     CHECK('D','S',  0);
            CHECK('D','T', -1);     CHECK('D','W', -5);     CHECK('D','Y', -3);     CHECK('D','V', -4);
            CHECK('D','B',  6);     CHECK('D','J', -4);     CHECK('D','Z',  1);     CHECK('D','X', -1);
            CHECK('D','*', -5);

            CHECK('C','A', -1);     CHECK('C','R', -4);     CHECK('C','N', -2);     CHECK('C','D', -4);
            CHECK('C','C', 13);     CHECK('C','Q', -3);     CHECK('C','E', -3);     CHECK('C','G', -3);
            CHECK('C','H', -3);     CHECK('C','I', -2);     CHECK('C','L', -2);     CHECK('C','K', -3);
            CHECK('C','M', -2);     CHECK('C','F', -2);     CHECK('C','P', -4);     CHECK('C','S', -1);
            CHECK('C','T', -1);     CHECK('C','W', -5);     CHECK('C','Y', -3);     CHECK('C','V', -1);
            CHECK('C','B', -3);     CHECK('C','J', -2);     CHECK('C','Z', -3);     CHECK('C','X', -1);
            CHECK('C','*', -5);

            CHECK('Q','A', -1);     CHECK('Q','R',  1);     CHECK('Q','N',  0);     CHECK('Q','D',  0);
            CHECK('Q','C', -3);     CHECK('Q','Q',  7);     CHECK('Q','E',  2);     CHECK('Q','G', -2);
            CHECK('Q','H',  1);     CHECK('Q','I', -3);     CHECK('Q','L', -2);     CHECK('Q','K',  2);
            CHECK('Q','M',  0);     CHECK('Q','F', -4);     CHECK('Q','P', -1);     CHECK('Q','S',  0);
            CHECK('Q','T', -1);     CHECK('Q','W', -1);     CHECK('Q','Y', -1);     CHECK('Q','V', -3);
            CHECK('Q','B',  0);     CHECK('Q','J', -3);     CHECK('Q','Z',  4);     CHECK('Q','X', -1);
            CHECK('Q','*', -5);

            CHECK('E','A', -1);     CHECK('E','R',  0);     CHECK('E','N',  0);     CHECK('E','D',  2);
            CHECK('E','C', -3);     CHECK('E','Q',  2);     CHECK('E','E',  6);     CHECK('E','G', -3);
            CHECK('E','H',  0);     CHECK('E','I', -4);     CHECK('E','L', -3);     CHECK('E','K',  1);
            CHECK('E','M', -2);     CHECK('E','F', -3);     CHECK('E','P', -1);     CHECK('E','S', -1);
            CHECK('E','T', -1);     CHECK('E','W', -3);     CHECK('E','Y', -2);     CHECK('E','V', -3);
            CHECK('E','B',  1);     CHECK('E','J', -3);     CHECK('E','Z',  5);     CHECK('E','X', -1);
            CHECK('E','*', -5);

            CHECK('G','A',  0);     CHECK('G','R', -3);     CHECK('G','N',  0);     CHECK('G','D', -1);
            CHECK('G','C', -3);     CHECK('G','Q', -2);     CHECK('G','E', -3);     CHECK('G','G',  8);
            CHECK('G','H', -2);     CHECK('G','I', -4);     CHECK('G','L', -4);     CHECK('G','K', -2);
            CHECK('G','M', -3);     CHECK('G','F', -4);     CHECK('G','P', -2);     CHECK('G','S',  0);
            CHECK('G','T', -2);     CHECK('G','W', -3);     CHECK('G','Y', -3);     CHECK('G','V', -4);
            CHECK('G','B', -1);     CHECK('G','J', -4);     CHECK('G','Z', -2);     CHECK('G','X', -1);
            CHECK('G','*', -5);

            CHECK('H','A', -2);     CHECK('H','R',  0);     CHECK('H','N',  1);     CHECK('H','D', -1);
            CHECK('H','C', -3);     CHECK('H','Q',  1);     CHECK('H','E',  0);     CHECK('H','G', -2);
            CHECK('H','H', 10);     CHECK('H','I', -4);     CHECK('H','L', -3);     CHECK('H','K',  0);
            CHECK('H','M', -1);     CHECK('H','F', -1);     CHECK('H','P', -2);     CHECK('H','S', -1);
            CHECK('H','T', -2);     CHECK('H','W', -3);     CHECK('H','Y',  2);     CHECK('H','V', -4);
            CHECK('H','B',  0);     CHECK('H','J', -3);     CHECK('H','Z',  0);     CHECK('H','X', -1);
            CHECK('H','*', -5);

            CHECK('I','A', -1);     CHECK('I','R', -4);     CHECK('I','N', -3);     CHECK('I','D', -4);
            CHECK('I','C', -2);     CHECK('I','Q', -3);     CHECK('I','E', -4);     CHECK('I','G', -4);
            CHECK('I','H', -4);     CHECK('I','I',  5);     CHECK('I','L',  2);     CHECK('I','K', -3);
            CHECK('I','M',  2);     CHECK('I','F',  0);     CHECK('I','P', -3);     CHECK('I','S', -3);
            CHECK('I','T', -1);     CHECK('I','W', -3);     CHECK('I','Y', -1);     CHECK('I','V',  4);
            CHECK('I','B', -4);     CHECK('I','J',  4);     CHECK('I','Z', -3);     CHECK('I','X', -1);
            CHECK('I','*', -5);

            CHECK('L','A', -2);     CHECK('L','R', -3);     CHECK('L','N', -4);     CHECK('L','D', -4);
            CHECK('L','C', -2);     CHECK('L','Q', -2);     CHECK('L','E', -3);     CHECK('L','G', -4);
            CHECK('L','H', -3);     CHECK('L','I',  2);     CHECK('L','L',  5);     CHECK('L','K', -3);
            CHECK('L','M',  3);     CHECK('L','F',  1);     CHECK('L','P', -4);     CHECK('L','S', -3);
            CHECK('L','T', -1);     CHECK('L','W', -2);     CHECK('L','Y', -1);     CHECK('L','V',  1);
            CHECK('L','B', -4);     CHECK('L','J',  4);     CHECK('L','Z', -3);     CHECK('L','X', -1);
            CHECK('L','*', -5);

            CHECK('K','A', -1);     CHECK('K','R',  3);     CHECK('K','N',  0);     CHECK('K','D', -1);
            CHECK('K','C', -3);     CHECK('K','Q',  2);     CHECK('K','E',  1);     CHECK('K','G', -2);
            CHECK('K','H',  0);     CHECK('K','I', -3);     CHECK('K','L', -3);     CHECK('K','K',  6);
            CHECK('K','M', -2);     CHECK('K','F', -4);     CHECK('K','P', -1);     CHECK('K','S',  0);
            CHECK('K','T', -1);     CHECK('K','W', -3);     CHECK('K','Y', -2);     CHECK('K','V', -3);
            CHECK('K','B',  0);     CHECK('K','J', -3);     CHECK('K','Z',  1);     CHECK('K','X', -1);
            CHECK('K','*', -5);

            CHECK('M','A', -1);     CHECK('M','R', -2);     CHECK('M','N', -2);     CHECK('M','D', -4);
            CHECK('M','C', -2);     CHECK('M','Q',  0);     CHECK('M','E', -2);     CHECK('M','G', -3);
            CHECK('M','H', -1);     CHECK('M','I',  2);     CHECK('M','L',  3);     CHECK('M','K', -2);
            CHECK('M','M',  7);     CHECK('M','F',  0);     CHECK('M','P', -3);     CHECK('M','S', -2);
            CHECK('M','T', -1);     CHECK('M','W', -1);     CHECK('M','Y',  0);     CHECK('M','V',  1);
            CHECK('M','B', -3);     CHECK('M','J',  2);     CHECK('M','Z', -1);     CHECK('M','X', -1);
            CHECK('M','*', -5);

            CHECK('F','A', -3);     CHECK('F','R', -3);     CHECK('F','N', -4);     CHECK('F','D', -5);
            CHECK('F','C', -2);     CHECK('F','Q', -4);     CHECK('F','E', -3);     CHECK('F','G', -4);
            CHECK('F','H', -1);     CHECK('F','I',  0);     CHECK('F','L',  1);     CHECK('F','K', -4);
            CHECK('F','M',  0);     CHECK('F','F',  8);     CHECK('F','P', -4);     CHECK('F','S', -3);
            CHECK('F','T', -2);     CHECK('F','W',  1);     CHECK('F','Y',  4);     CHECK('F','V', -1);
            CHECK('F','B', -4);     CHECK('F','J',  1);     CHECK('F','Z', -4);     CHECK('F','X', -1);
            CHECK('F','*', -5);

            CHECK('P','A', -1);     CHECK('P','R', -3);     CHECK('P','N', -2);     CHECK('P','D', -1);
            CHECK('P','C', -4);     CHECK('P','Q', -1);     CHECK('P','E', -1);     CHECK('P','G', -2);
            CHECK('P','H', -2);     CHECK('P','I', -3);     CHECK('P','L', -4);     CHECK('P','K', -1);
            CHECK('P','M', -3);     CHECK('P','F', -4);     CHECK('P','P', 10);     CHECK('P','S', -1);
            CHECK('P','T', -1);     CHECK('P','W', -4);     CHECK('P','Y', -3);     CHECK('P','V', -3);
            CHECK('P','B', -2);     CHECK('P','J', -3);     CHECK('P','Z', -1);     CHECK('P','X', -1);
            CHECK('P','*', -5);

            CHECK('S','A',  1);     CHECK('S','R', -1);     CHECK('S','N',  1);     CHECK('S','D',  0);
            CHECK('S','C', -1);     CHECK('S','Q',  0);     CHECK('S','E', -1);     CHECK('S','G',  0);
            CHECK('S','H', -1);     CHECK('S','I', -3);     CHECK('S','L', -3);     CHECK('S','K',  0);
            CHECK('S','M', -2);     CHECK('S','F', -3);     CHECK('S','P', -1);     CHECK('S','S',  5);
            CHECK('S','T',  2);     CHECK('S','W', -4);     CHECK('S','Y', -2);     CHECK('S','V', -2);
            CHECK('S','B',  0);     CHECK('S','J', -3);     CHECK('S','Z',  0);     CHECK('S','X', -1);
            CHECK('S','*', -5);

            CHECK('T','A',  0);     CHECK('T','R', -1);     CHECK('T','N',  0);     CHECK('T','D', -1);
            CHECK('T','C', -1);     CHECK('T','Q', -1);     CHECK('T','E', -1);     CHECK('T','G', -2);
            CHECK('T','H', -2);     CHECK('T','I', -1);     CHECK('T','L', -1);     CHECK('T','K', -1);
            CHECK('T','M', -1);     CHECK('T','F', -2);     CHECK('T','P', -1);     CHECK('T','S',  2);
            CHECK('T','T',  5);     CHECK('T','W', -3);     CHECK('T','Y', -2);     CHECK('T','V',  0);
            CHECK('T','B',  0);     CHECK('T','J', -1);     CHECK('T','Z', -1);     CHECK('T','X', -1);
            CHECK('T','*', -5);

            CHECK('W','A', -3);     CHECK('W','R', -3);     CHECK('W','N', -4);     CHECK('W','D', -5);
            CHECK('W','C', -5);     CHECK('W','Q', -1);     CHECK('W','E', -3);     CHECK('W','G', -3);
            CHECK('W','H', -3);     CHECK('W','I', -3);     CHECK('W','L', -2);     CHECK('W','K', -3);
            CHECK('W','M', -1);     CHECK('W','F',  1);     CHECK('W','P', -4);     CHECK('W','S', -4);
            CHECK('W','T', -3);     CHECK('W','W', 15);     CHECK('W','Y',  2);     CHECK('W','V', -3);
            CHECK('W','B', -5);     CHECK('W','J', -2);     CHECK('W','Z', -2);     CHECK('W','X', -1);
            CHECK('W','*', -5);

            CHECK('Y','A', -2);     CHECK('Y','R', -1);     CHECK('Y','N', -2);     CHECK('Y','D', -3);
            CHECK('Y','C', -3);     CHECK('Y','Q', -1);     CHECK('Y','E', -2);     CHECK('Y','G', -3);
            CHECK('Y','H',  2);     CHECK('Y','I', -1);     CHECK('Y','L', -1);     CHECK('Y','K', -2);
            CHECK('Y','M',  0);     CHECK('Y','F',  4);     CHECK('Y','P', -3);     CHECK('Y','S', -2);
            CHECK('Y','T', -2);     CHECK('Y','W',  2);     CHECK('Y','Y',  8);     CHECK('Y','V', -1);
            CHECK('Y','B', -3);     CHECK('Y','J', -1);     CHECK('Y','Z', -2);     CHECK('Y','X', -1);
            CHECK('Y','*', -5);

            CHECK('V','A',  0);     CHECK('V','R', -3);     CHECK('V','N', -3);     CHECK('V','D', -4);
            CHECK('V','C', -1);     CHECK('V','Q', -3);     CHECK('V','E', -3);     CHECK('V','G', -4);
            CHECK('V','H', -4);     CHECK('V','I',  4);     CHECK('V','L',  1);     CHECK('V','K', -3);
            CHECK('V','M',  1);     CHECK('V','F', -1);     CHECK('V','P', -3);     CHECK('V','S', -2);
            CHECK('V','T',  0);     CHECK('V','W', -3);     CHECK('V','Y', -1);     CHECK('V','V',  5);
            CHECK('V','B', -3);     CHECK('V','J',  2);     CHECK('V','Z', -3);     CHECK('V','X', -1);
            CHECK('V','*', -5);

            CHECK('B','A', -2);     CHECK('B','R', -1);     CHECK('B','N',  5);     CHECK('B','D',  6);
            CHECK('B','C', -3);     CHECK('B','Q',  0);     CHECK('B','E',  1);     CHECK('B','G', -1);
            CHECK('B','H',  0);     CHECK('B','I', -4);     CHECK('B','L', -4);     CHECK('B','K',  0);
            CHECK('B','M', -3);     CHECK('B','F', -4);     CHECK('B','P', -2);     CHECK('B','S',  0);
            CHECK('B','T',  0);     CHECK('B','W', -5);     CHECK('B','Y', -3);     CHECK('B','V', -3);
            CHECK('B','B',  6);     CHECK('B','J', -4);     CHECK('B','Z',  1);     CHECK('B','X', -1);
            CHECK('B','*', -5);

            CHECK('J','A', -2);     CHECK('J','R', -3);     CHECK('J','N', -4);     CHECK('J','D', -4);
            CHECK('J','C', -2);     CHECK('J','Q', -3);     CHECK('J','E', -3);     CHECK('J','G', -4);
            CHECK('J','H', -3);     CHECK('J','I',  4);     CHECK('J','L',  4);     CHECK('J','K', -3);
            CHECK('J','M',  2);     CHECK('J','F',  1);     CHECK('J','P', -3);     CHECK('J','S', -3);
            CHECK('J','T', -1);     CHECK('J','W', -2);     CHECK('J','Y', -1);     CHECK('J','V',  2);
            CHECK('J','B', -4);     CHECK('J','J',  4);     CHECK('J','Z', -3);     CHECK('J','X', -1);
            CHECK('J','*', -5);

            CHECK('Z','A', -1);     CHECK('Z','R',  0);     CHECK('Z','N',  0);     CHECK('Z','D',  1);
            CHECK('Z','C', -3);     CHECK('Z','Q',  4);     CHECK('Z','E',  5);     CHECK('Z','G', -2);
            CHECK('Z','H',  0);     CHECK('Z','I', -3);     CHECK('Z','L', -3);     CHECK('Z','K',  1);
            CHECK('Z','M', -1);     CHECK('Z','F', -4);     CHECK('Z','P', -1);     CHECK('Z','S',  0);
            CHECK('Z','T', -1);     CHECK('Z','W', -2);     CHECK('Z','Y', -2);     CHECK('Z','V', -3);
            CHECK('Z','B',  1);     CHECK('Z','J', -3);     CHECK('Z','Z',  5);     CHECK('Z','X', -1);
            CHECK('Z','*', -5);

            CHECK('X','A', -1);     CHECK('X','R', -1);     CHECK('X','N', -1);     CHECK('X','D', -1);
            CHECK('X','C', -1);     CHECK('X','Q', -1);     CHECK('X','E', -1);     CHECK('X','G', -1);
            CHECK('X','H', -1);     CHECK('X','I', -1);     CHECK('X','L', -1);     CHECK('X','K', -1);
            CHECK('X','M', -1);     CHECK('X','F', -1);     CHECK('X','P', -1);     CHECK('X','S', -1);
            CHECK('X','T', -1);     CHECK('X','W', -1);     CHECK('X','Y', -1);     CHECK('X','V', -1);
            CHECK('X','B', -1);     CHECK('X','J', -1);     CHECK('X','Z', -1);     CHECK('X','X', -1);
            CHECK('X','*', -5);

            CHECK('*','A', -5);     CHECK('*','R', -5);     CHECK('*','N', -5);     CHECK('*','D', -5);
            CHECK('*','C', -5);     CHECK('*','Q', -5);     CHECK('*','E', -5);     CHECK('*','G', -5);
            CHECK('*','H', -5);     CHECK('*','I', -5);     CHECK('*','L', -5);     CHECK('*','K', -5);
            CHECK('*','M', -5);     CHECK('*','F', -5);     CHECK('*','P', -5);     CHECK('*','S', -5);
            CHECK('*','T', -5);     CHECK('*','W', -5);     CHECK('*','Y', -5);     CHECK('*','V', -5);
            CHECK('*','B', -5);     CHECK('*','J', -5);     CHECK('*','Z', -5);     CHECK('*','X', -5);
#endif
        }
        catch (ScoreMatrixFailure & e)  {  CPPUNIT_ASSERT (false);  }
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testBlosumCheck50 ()
    {
        testBlosumCheck50_aux (SUBSEED);
        testBlosumCheck50_aux (NCBI);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testSplitter ()
    {
        /** We retrieve a score matrix. */
        IScoreMatrix* scoreMatrix = ScoreMatrixManager::singleton().getMatrix ("BLOSUM62", SUBSEED, 0, 0);
        CPPUNIT_ASSERT (scoreMatrix != 0);
        LOCAL (scoreMatrix);

        ISequenceDatabase* subject = new BufferedSequenceDatabase (
            new StringSequenceIterator (1, "FRRTIQKNLHPSYSC"),
            false
        );
        LOCAL (subject);

        ISequenceDatabase* query = new BufferedSequenceDatabase (
            new StringSequenceIterator (1, "FKRAAEGKQKYLC"),
            false
        );
        LOCAL (query);

        ISequence subjectSeq;   subject->getSequenceByIndex (0, subjectSeq);
        ISequence querySeq;     query->getSequenceByIndex   (0, querySeq);

        AlignmentSplitterBanded split (scoreMatrix, 11, 1);

        database::LETTER   subjectAlign [10000];
        database::LETTER   queryAlign   [10000];

        IAlignmentSplitter::SplitOutput out (1000, subjectAlign, queryAlign);

        /** Shortcut. */
        u_int32_t* splittab = out.splittab.data;

        split.splitAlign (
            subjectSeq.data.letters.data,
            querySeq.data.letters.data,
            misc::Range32 (0, subjectSeq.data.letters.size-1),
            misc::Range32 (0, querySeq.data.letters.size-1),
            out
        );

#if 1
        printf ("\n");
        printf ("nbSplits=%d  identity=%d  nbGapQry=%d  nbGapSbj=%d  nbMis=%d  alignSize=%d\n",
            out.alignSize/4, out.identity, out.nbGapQry, out.nbGapSbj, out.nbMis, out.alignSize
        );

        const LETTER* convert = EncodingManager::singleton().getEncodingConversion(SUBSEED, ASCII);

        printf ("\n");
        printf ("seq   Q: %s\n", querySeq.data.toString().c_str());
        printf ("seq   S: %s\n", subjectSeq.data.toString().c_str());

        printf ("\n");
        printf ("align Q: "); for (u_int32_t i=1; i<=out.alignSize; i++)  { printf ("%c", convert[(int)queryAlign[out.alignSize-i]]);    }  printf("\n");
        printf ("align S: "); for (u_int32_t i=1; i<=out.alignSize; i++)  { printf ("%c", convert[(int)subjectAlign[out.alignSize-i]]);  }  printf("\n");

        printf ("\n");
        printf ("splits: \n");

        for (size_t i=0; i<out.alignSize; i+=4)
        {
            printf ("    [%2d,%2d]  ", splittab[i+2], splittab[i+0]);
            for (u_int32_t k=splittab[i+2]; k<=splittab[i+0]; k++)
            {
                printf ("%c", convert[(int)querySeq.data.letters.data[k]]);
            }
            printf ("\n");

            printf ("    [%2d,%2d]  ", splittab[i+3], splittab[i+1]);
            for (u_int32_t k=splittab[i+3]; k<=splittab[i+1]; k++)
            {
                printf ("%c", convert[(int)subjectSeq.data.letters.data[k]]);
            }
            printf ("\n");

            printf ("\n");
        }
#endif
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testSplitter2 ()
    {
        /** We retrieve a score matrix. */
        IScoreMatrix* scoreMatrix = ScoreMatrixManager::singleton().getMatrix ("BLOSUM62", SUBSEED, 0, 0);
        CPPUNIT_ASSERT (scoreMatrix != 0);
        LOCAL (scoreMatrix);

        ISequenceDatabase* subject = new BufferedSequenceDatabase (
            new StringSequenceIterator (1,
                "MTMDKSELVQKAKLAEQAERYDDMAAAMKAVTEQGHELSNEERNLLSVAYKNVVGARRSS"
                "WRVISSIEQKTERNEKKQQMGKEYREKIEAELQDICNDVLELLDKYLIPNATQPESKVFY"
                "LKMKGDYFRYLSEVASGDNKQTTVSNSQQAYQEAFEISKKEMQPTHPIRLGLALNFSVFY"
                "YEILNSPEKACSLAKTAFDEAIAELDTLNEESYKDSTLIMQLLRDNLTLWTSENQGDEGD"
                "AGEGEN"
            ),
            false
        );
        LOCAL (subject);

        ISequenceDatabase* query = new BufferedSequenceDatabase (
            new StringSequenceIterator (1,
                "MGDREQLLQRARLAEQEERYDDMASVMNAVTELNEPLSNEDRNLLSVAYKNVVGARRSSW"
                "RVISSIEQKTTADGNEKKLEKVKAYREKIEKELETVCNDVLALLDKFLIKNCNDFQYESK"
                "VFYLKMKGDYYRYLAEVASGEKKNSVVEASEAAYKEAFEISKEHMQPTHPIRLGLALNFS"
                "VFYYEIQNAPEQACLLAKQAFDDAIAELDTLNEDSYKDSTLIMQLLRDNLTLWTSDQQDE"
                "EAGEGN"
            ),
            false
        );
        LOCAL (query);

        ISequence subjectSeq;   subject->getSequenceByIndex (0, subjectSeq);
        ISequence querySeq;     query->getSequenceByIndex   (0, querySeq);

        AlignmentSplitterBanded split (scoreMatrix, 11, 1);
//        AlignmentSplitter split (scoreMatrix, 11, 1);

        database::LETTER   subjectAlign [10000];
        database::LETTER   queryAlign   [10000];

        IAlignmentSplitter::SplitOutput out (1000, subjectAlign, queryAlign);

        /** Shortcut. */
        u_int32_t* splittab = out.splittab.data;

        split.splitAlign (
            subjectSeq.data.letters.data,
            querySeq.data.letters.data,
            misc::Range32 (0, subjectSeq.data.letters.size-1),
            misc::Range32 (0, querySeq.data.letters.size-1),
            out
        );

#if 1
        printf ("\n");
        printf ("nbSplits=%d  identity=%d  nbGapQry=%d  nbGapSbj=%d  nbMis=%d  alignSize=%d\n",
            out.alignSize/4, out.identity, out.nbGapQry, out.nbGapSbj, out.nbMis, out.alignSize
        );

        const LETTER* convert = EncodingManager::singleton().getEncodingConversion(SUBSEED, ASCII);

        printf ("\n");
        printf ("seq   Q: %s\n", querySeq.data.toString().c_str());
        printf ("seq   S: %s\n", subjectSeq.data.toString().c_str());

        printf ("\n");
        printf ("align Q: "); for (u_int32_t i=1; i<=out.alignSize; i++)  { printf ("%c", convert[(int)queryAlign[out.alignSize-i]]);    }  printf("\n");
        printf ("align S: "); for (u_int32_t i=1; i<=out.alignSize; i++)  { printf ("%c", convert[(int)subjectAlign[out.alignSize-i]]);  }  printf("\n");

        printf ("\n");
        printf ("splits: \n");

        for (size_t i=0; i<out.alignSize; i+=4)
        {
            printf ("    [%2d,%2d]  ", splittab[i+2], splittab[i+0]);
            for (u_int32_t k=splittab[i+2]; k<=splittab[i+0]; k++)
            {
                printf ("%c", convert[(int)querySeq.data.letters.data[k]]);
            }
            printf ("\n");

            printf ("    [%2d,%2d]  ", splittab[i+3], splittab[i+1]);
            for (u_int32_t k=splittab[i+3]; k<=splittab[i+1]; k++)
            {
                printf ("%c", convert[(int)subjectSeq.data.letters.data[k]]);
            }
            printf ("\n");

            printf ("\n");
        }
#endif
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testSemiGappedAlign ()
    {
        struct Entry
        {
            const char* seq1;
            const char* seq2;
            int         gapOpen;
            int         gapExtend;
            int         xdropoff;
        };

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

        const char* ENSTTRP00000007202 = "CDEFGHIKLMNP";

        //Entry entry = { "FKRAAEGKQKYLC", "FRRTIQKNLHPSYSC", 6, 0, 38};
        //Entry entry = { "VPVESDLQEIIKEISIMQQCDSPHVVKYYGSYFKNTDLWIVMEYCGAGSVSDIIRLRNKT", "ALAQRKEERKAILTNQYMQRLSTMRALGSPLLGSFQQPASYFLPATPQPPAQAPYYGSGP", 11, 1, 38};
        //Entry entry = { "IGERLKEEIDNSELL", "IGERLKEEIDNSELL", 11, 1, 38};
        //Entry entry = { "IGERLKEEIDNSELL", "IGERLKFFIDNSELL", 11, 1, 38};
        //Entry entry = { ENSTTRP00000007204, ENSTTRP00000007204, 11, 1, 38};
        Entry entry = { ENSTTRP00000007202, ENSTTRP00000007202, 11, 1, 200};

        //const char* seq = "IGERLKEEIDNSELL";
        //const char* seq = "IGER";

        /** We retrieve a score matrix. */
        IScoreMatrix* scoreMatrix = ScoreMatrixManager::singleton().getMatrix ("BLOSUM62", SUBSEED, 0, 0);
        CPPUNIT_ASSERT (scoreMatrix != 0);
        LOCAL (scoreMatrix);

        ISequenceDatabase* subject = new BufferedSequenceDatabase (
            new StringSequenceIterator (1, entry.seq1),
            false
        );
        LOCAL (subject);

        ISequenceDatabase* query = new BufferedSequenceDatabase (
            new StringSequenceIterator (1, entry.seq2),
            false
        );
        LOCAL (query);

        ISequence subjectSeq;   subject->getSequenceByIndex (0, subjectSeq);
        ISequence querySeq;     query->getSequenceByIndex   (0, querySeq);

        SemiGapAlign dynpro (
            scoreMatrix,
            entry.gapOpen,
            entry.gapExtend,
            entry.xdropoff
        );

        int leftOffsetInQuery=0, leftOffsetInSubject=0, rightOffsetInQuery=0, rightOffsetInSubject=0;

        LETTER* queryData   = querySeq.data.letters.data;
        LETTER* subjectData = subjectSeq.data.letters.data;

        size_t queryLen   = querySeq.data.letters.size;
        size_t subjectLen = subjectSeq.data.letters.size;

        printf ("\nqueryLen=%ld  subjectLen=%ld\n", queryLen, subjectLen);

        for (size_t i=0; i<queryLen; i++)    { printf ("%2d ", queryData[i]);   }  printf ("\n");
        for (size_t i=0; i<subjectLen; i++)  { printf ("%2d ", subjectData[i]); }  printf ("\n");

        int scoreLeft=0, scoreRight=0;

        printf ("\n");
        for (size_t offsetInQry=0; offsetInQry<queryLen; offsetInQry++)
        {
            for (size_t offsetInSbj=0; offsetInSbj<subjectLen; offsetInSbj++)
            {
                printf ("\n************************************  OFFSET QRY=%2ld   SBJ=%2ld  ************************************\n",
                    offsetInQry, offsetInSbj
                );

                //printf ("[%2ld]  a_off=%2d  b_off=%2d  \n", offset, a_offset, b_offset);

#if 0
                /** We compute the left part of the score. */
                printf ("   ------------------------------------------------\n");
                printf ("   LEFT SCORE\n");
                scoreLeft = dynpro.compute (
                        queryData,
                        subjectData,
                        offsetInQry + 1,
                        offsetInSbj + 1,
                        & leftOffsetInQuery,
                        & leftOffsetInSubject,
                        1
                );
                printf ("\n");

                //            leftOffsetInQuery--;
                //            leftOffsetInSubject--;
#endif

#if 0
                printf ("   ------------------------------------------------\n");
                printf ("   RIGHT SCORE\n");
                /** We compute the right part of the score. */
                scoreRight = dynpro.compute (
                        queryData   + offsetInQry - 0 ,
                        subjectData + offsetInSbj - 0,
                        querySeq.data.letters.size   - offsetInQry - 1,
                        subjectSeq.data.letters.size - offsetInSbj - 1,
                        & rightOffsetInQuery,
                        & rightOffsetInSubject,
                        0
                );
                printf ("\n");
#endif
                /** We create a new alignment. */
                int32_t queryStartInSeq   = offsetInQry - leftOffsetInQuery;
                int32_t subjectStartInSeq = offsetInSbj - leftOffsetInSubject;

                int32_t  queryEndInSeq    = offsetInQry + rightOffsetInQuery;
                int32_t  subjectEndInSeq  = offsetInSbj + rightOffsetInSubject;

                printf ("[%ld]  left [%d,%d]   right [%d,%d]  =>  Q [%d,%d]  S [%d,%d]\n",
                        offsetInQry,
                        leftOffsetInQuery, leftOffsetInSubject,
                        rightOffsetInQuery, rightOffsetInSubject,
                        queryStartInSeq,   queryEndInSeq,
                        subjectStartInSeq, subjectEndInSeq
                );
                printf("\n");
            }
        }
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testDumpMatrix ()
    {
        bool dumpAll     = false;
        bool dumpShifted = true;
        bool dumpFreq    = true;

        /** We retrieve a score matrix. */
        IScoreMatrix* scoreMatrix = ScoreMatrixManager::singleton().getMatrix ("BLOSUM62", SUBSEED, 0, 0);

        int8_t   biais  = scoreMatrix->getDefaultScore();
        int8_t** matrix = scoreMatrix->getMatrix();

        const LETTER* convert = EncodingManager::singleton().getEncodingConversion(SUBSEED, ASCII);

#define TABLE1

#if defined (TABLE1)
        LETTER table[] =
        {
            CODE_A, CODE_R, CODE_N, CODE_D, CODE_C, CODE_Q, CODE_E, CODE_G, CODE_H, CODE_I,
            CODE_L, CODE_K, CODE_M, CODE_F, CODE_P, CODE_S, CODE_T, CODE_W, CODE_Y, CODE_V,
            CODE_B, CODE_Z, CODE_X, CODE_STAR
        };
#elif defined (TABLE2)
        LETTER table[] =
        {
            CODE_C, CODE_S, CODE_T, CODE_P, CODE_A, CODE_G, CODE_N, CODE_D, CODE_E, CODE_Q,
            CODE_H, CODE_R, CODE_K, CODE_M, CODE_I, CODE_L, CODE_V, CODE_F, CODE_Y, CODE_W
        };
#elif defined (TABLE3)
        LETTER table[] =
        {
            CODE_A, CODE_R, CODE_N, CODE_D
        };
#endif
        size_t frequencies[16];  memset (frequencies, 0, sizeof(frequencies));
        size_t total = 0;

        printf ("\n");

        size_t N = ARRAYSIZE(table);

#if 0
        printf ("\\begin{tabular}{");  for (size_t i=0; i<N+1; i++)  { printf ("l"); }  printf ("}\n");

        printf ("  &  ");
        for (size_t i=0; i<N; i++) { printf ("%c  %c ", convert[(int)table[i]], (i+1<N ? '&': ' '));  }
        printf ("\\\\ \n");

        for (size_t i=0; i<N; i++)
        {
            printf ("%c & ", convert[(int)table[i]]);

            for (size_t j=0; j<N; j++)
            {
                int8_t val = matrix [(int)table[i]] [(int)table[j]];

                total ++;
                frequencies [(int)(val-biais)] ++;

                int8_t valDump = (dumpShifted ? val-biais : val);

                if (dumpAll)
                {
                    printf ("%2d  ", valDump);
                }
                else
                {
                    if (i<=j)  {  printf ("%2d  %c", valDump, (j+1<N ? '&': ' ') );  }
                    else       {  printf ("    %c", (j+1<N ? '&': ' ') );  }
                }
            }
            printf ("\\\\ \n");
        }
        printf ("\\end{tabular}\n");
#else
        printf ("    ");  for (size_t i=0; i<N; i++) { printf ("%c   ", convert[(int)table[i]]); }  printf ("\n");

        for (size_t i=0; i<N; i++)
        {
            printf ("%c  ", convert[(int)table[i]]);

            for (size_t j=0; j<N; j++)
            {
                int8_t val = matrix [(int)table[i]] [(int)table[j]];

                total ++;
                frequencies [(int)(val-biais)] ++;

                int8_t valDump = (dumpShifted ? val-biais : val);

                if (dumpAll)
                {
                    printf ("%2d  ", valDump);
                }
                else
                {
                    if (i<=j)  {  printf ("%2d  ", valDump);  }
                    else       {  printf ("    ");  }
                }
            }
            printf ("\n");
        }

#endif
        printf ("\n");

        if (dumpFreq)
        {
            printf ("total=%ld\n\n", total);

            for (size_t i=0; i<ARRAYSIZE(frequencies); i++)
            {
                printf ("%2ld    %3ld  %8.2f \n", i+biais, frequencies[i], 100*(float)frequencies[i] / (float)total);
            }
        }
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/

    static inline void score1 (LETTER* buf1, LETTER* buf2, int8_t* scores, size_t n, int8_t* matrixAsVector)
    {
        for (size_t i=0; i<n; i++)
        {
            scores[i] = matrixAsVector [ buf1[i] + (buf2[2]<<5) ];
        }
    }

    static inline void score2 (LETTER* buf1, LETTER* buf2, int8_t* scores, size_t n, u_int16_t* matrixAsVector)
    {
        u_int16_t* pt1 = (u_int16_t*) buf1;
        u_int16_t* pt2 = (u_int16_t*) buf2;

        u_int16_t* scores2 = (u_int16_t*)scores;

        for (size_t i=0; i<4; i++)
        {
            u_int16_t i1 = *pt1;
            u_int16_t i2 = *pt2;

//            i1 = (i1 >> 3) + (i1 & 0x1F);
//            i2 = (i2 >> 3) + (i2 & 0x1F);

//            i1 = 0;
//            i2 = 0;

            scores2[i] = matrixAsVector [ (i1 >> 3) + (i1 & 0x1F) + (((i2 >> 3) + (i2 & 0x1F))<<10)];


            pt1++; pt2++;
        }
    }


    void testDoubleScore (void)
    {
        /** We retrieve a score matrix. */
        IScoreMatrix* scoreMatrix = ScoreMatrixManager::singleton().getMatrix ("BLOSUM62", SUBSEED, 0, 0);
        LOCAL (scoreMatrix);

        int8_t   biais           = scoreMatrix->getDefaultScore();
        int8_t** matrix          = scoreMatrix->getMatrix();
        int8_t*  matrixAsVector  = scoreMatrix->getMatrixAsVector();

        LETTER buf1[] = {CODE_R, CODE_P, CODE_C, CODE_D, CODE_Q, CODE_W, CODE_T, CODE_K };
        LETTER buf2[] = {CODE_R, CODE_P, CODE_C, CODE_D, CODE_Q, CODE_W, CODE_T, CODE_K };
        int8_t scores[8];

        size_t N = 100*1000*1000;
        size_t M = 1;

#if 0
        u_int32_t t0 = DefaultFactory::time().gettime();
        for (size_t i=0; i<N; i++)
        {
            for (size_t j=0; j<M; j++)
            {
                score1 (buf1, buf2, scores, 8, matrixAsVector);
            }
        }
        u_int32_t t1 = DefaultFactory::time().gettime();

#else
        u_int16_t* matrix2 = (u_int16_t*) DefaultFactory::memory().calloc (1024*1024, sizeof(u_int16_t));
        memset (matrix2, 0, 1024*1024*sizeof(u_int8_t));

#define IDX(a,b)       ((((b) & 0x1F) << 5) + ((a) & 0x1F))
#define IDX2(a,b,c,d)  ((IDX(c,d) << 10) + IDX(a,b))

#define SET_SCORE(a,b)  (((b)<<8) + ((a)<<0))

        u_int8_t m1 = ~0;
        u_int8_t m2 = 0;
        int m  = 0;

        for (size_t i1=0; i1<CODE_BAD; i1++)
        {
            for (size_t j1=0; j1<CODE_BAD; j1++)
            {
                for (size_t i2=0; i2<CODE_BAD; i2++)
                {
                    for (size_t j2=0; j2<CODE_BAD; j2++)
                    {
                        int idx = IDX2 (i1,i2, j1,j2);

                        if (m<idx)        { m  = idx;      }

                        matrix2 [idx] = SET_SCORE (matrix[i1][j1]-biais, matrix[i2][j2]-biais);

                        m1 = MIN (m1, matrix2[idx]);
                        m2 = MAX (m2, matrix2[idx]);
                    }
                }
            }
        }

        u_int16_t* pt1 = (u_int16_t*) buf1;
        u_int16_t* pt2 = (u_int16_t*) buf2;

        u_int16_t i1 = (*pt1 >> 3) + (*pt1 & 0x1F);
        u_int16_t i2 = (*pt2 >> 3) + (*pt2 & 0x1F);

        u_int16_t val = matrix2 [ i1 + (i2<<10)];

        u_int32_t t0 = DefaultFactory::time().gettime();
        for (size_t i=0; i<N; i++)
        {
            for (size_t j=0; j<M; j++)
            {
                score2 (buf1, buf2, scores, 8, matrix2);
            }
        }
        u_int32_t t1 = DefaultFactory::time().gettime();

        DefaultFactory::memory().free (matrix2);
#endif

        printf ("TIME: %d msec\n", t1-t0);
    }
};

/********************************************************************************/

extern "C" Test* TestScoreMatrix_suite()  { return TestScoreMatrix::suite (); }


