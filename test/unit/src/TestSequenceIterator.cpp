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

#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/StringSequenceIterator.hpp>
#include <database/impl/ReadingFrameSequenceIterator.hpp>
#include <database/impl/FastaSequenceOutput.hpp>
#include <database/impl/SequenceTokenizer.hpp>

#include <os/impl/TimeTools.hpp>

using namespace std;
using namespace misc;
using namespace database;
using namespace database::impl;

using namespace os::impl;

extern const char* getPath (const char* file);

/********************************************************************************/

struct Info
{
    Info () : nbSequences(0), checksumComment(0), checksumData(0)  {}

    friend std::ostream& operator<< (std::ostream& s, const Info& i)
    {
        s << "[INFO nbSequences=" << i.nbSequences << "  checksumComment=" << i.checksumComment
          << "  checksumData=" << i.checksumData << "]";
        return s;
    }

    bool operator== (const Info& i) const
    {
        return
            nbSequences     == i.nbSequences     &&
            checksumComment == i.checksumComment &&
            checksumData    == i.checksumData;
    }

    size_t    nbSequences;
    u_int64_t checksumComment;
    u_int64_t checksumData;
};

/********************************************************************************/

class TestSequenceIterator : public TestFixture
{
public:

    /********************************************************************************/
    void setUp    ()  {}
    void tearDown ()  {}

    /********************************************************************************/
    static Test* suite()
    {
    	 TestSuite* result = new TestSuite ("SequenceIteratorTest");

         result->addTest (new TestCaller<TestSequenceIterator> ("testFastDump",             &TestSequenceIterator::testFastaDump) );
    	 result->addTest (new TestCaller<TestSequenceIterator> ("testFastaGoodFile", 	    &TestSequenceIterator::testFastaGoodFile ) );
         result->addTest (new TestCaller<TestSequenceIterator> ("testFastaBadFile", 	    &TestSequenceIterator::testFastaBadFile ) );
         result->addTest (new TestCaller<TestSequenceIterator> ("testFastaBigFile",         &TestSequenceIterator::testFastaBigFile ) );
    	 result->addTest (new TestCaller<TestSequenceIterator> ("testStringIterator", 	    &TestSequenceIterator::testStringIterator ) );
         result->addTest (new TestCaller<TestSequenceIterator> ("testReadingFrameIterator", &TestSequenceIterator::testReadingFrameIterator ) );
         result->addTest (new TestCaller<TestSequenceIterator> ("testFastaOutput",          &TestSequenceIterator::testFastaOutput ) );
         result->addTest (new TestCaller<TestSequenceIterator> ("testFiltering",            &TestSequenceIterator::testFiltering ) );
         result->addTest (new TestCaller<TestSequenceIterator> ("testFastaOutputFrame",     &TestSequenceIterator::testFastaOutputFrame ) );
         result->addTest (new TestCaller<TestSequenceIterator> ("testSequenceTokenizer",    &TestSequenceIterator::testSequenceTokenizer ) );
         result->addTest (new TestCaller<TestSequenceIterator> ("testFilesList",            &TestSequenceIterator::testFilesList ) );
         result->addTest (new TestCaller<TestSequenceIterator> ("testFastaHugeFile",        &TestSequenceIterator::testFastaHugeFile) );

    	 return result;
    }

    /********************************************************************************/
    void testFastaDump ()
    {
        ISequenceIterator* itSeq = new FastaSequenceIterator (getPath("query.fa"), 100);
        LOCAL (itSeq);

        for (itSeq->first(); ! itSeq->isDone(); itSeq->next ())
        {
            const ISequence* seq = itSeq->currentItem();
            CPPUNIT_ASSERT (seq != 0);
            //printf ("seq '%s'\n", seq->toString().c_str());
        }
    }

    /********************************************************************************/
    /* Iterate an existing fasta file and check that some sequences have been found.*/
    /********************************************************************************/
    void testFastaGoodFile_aux (const char* filename)
    {
    	ISequenceIterator* itSeq = new FastaSequenceIterator (filename, 100);
    	LOCAL (itSeq);

    	u_int32_t nbSequences = 0;
    	u_int64_t totalSize   = 0;

        /** We loop over the sequences. */
        for (itSeq->first(); ! itSeq->isDone(); itSeq->next ())
        {
            const ISequence* seq = itSeq->currentItem();
            CPPUNIT_ASSERT (seq != 0);

            totalSize += seq->data.letters.size;

            nbSequences++;
        }

        TRACE ("Found %d sequences for a total size of %ld\n", nbSequences, totalSize);

        CPPUNIT_ASSERT (nbSequences > 0);
    }

    /** */
    void testFastaGoodFile ()
    {
        testFastaGoodFile_aux (getPath ("query.fa"));
        testFastaGoodFile_aux (getPath ("tursiops.fa"));
    }

    /********************************************************************************/
    /* Try to iterate an non existing fasta file. */
    /********************************************************************************/
    void testFastaBadFile ()
    {
#if 0
//file: file.fasta is missing.
        ISequenceIterator* itSeq = new FastaSequenceIterator ("/bad/file.fasta", 100);
    	LOCAL (itSeq);

    	size_t nbSequences = 0;
    	size_t totalSize   = 0;

        /** We loop over the sequences. */
        for (itSeq->first(); ! itSeq->isDone(); itSeq->next ())
        {
            const ISequence* seq = itSeq->currentItem();
            CPPUNIT_ASSERT (seq != 0);

            totalSize += seq->data.letters.size;

            nbSequences++;
        }

        CPPUNIT_ASSERT (nbSequences == 0);
        CPPUNIT_ASSERT (totalSize   == 0);

        TRACE ("Found %ld sequences for a total size of %ld\n", nbSequences, totalSize);
#endif
    }

    /********************************************************************************/
    /********************************************************************************/
    void testFastaBigFile ()
    {
        const char* filename = "uniprot_sprot.fa";
        const char* tag      = "fasta";
        TimeInfo info;

        info.addEntry (tag);

        ISequenceIterator* itSeq = new FastaSequenceIterator (getPath (filename));
        LOCAL (itSeq);

        u_int32_t nbSequences = 0;
        u_int64_t totalSize   = 0;

        const LETTER* convert = EncodingManager::singleton().getEncodingConversion(SUBSEED, ASCII);

        /** We loop over the sequences. */
        for (itSeq->first(); ! itSeq->isDone(); itSeq->next ())
        {
            const ISequence* seq = itSeq->currentItem();

            totalSize += seq->getLength();

#if 0
            if (seq->getLength() >= 30000)
            {
                printf (">%s (length=%d) \n", seq->comment, seq->getLength());

                const LETTER* data = seq->getData();

                size_t len = seq->getLength();
                for (size_t i=0; i<len; i++)
                {
                    printf ("%c", convert[(int)data[i]]);
                    if ( (i+1)%100 == 0)  { printf ("\n"); }
                }
                printf ("\n");
            }
#endif
            nbSequences++;
        }

        info.stopEntry (tag);

        CPPUNIT_ASSERT (nbSequences > 0);
        CPPUNIT_ASSERT (totalSize   > 0);

        printf ("File '%s' iterated in %d msec => found %d sequences for a total size of %lld\n",
            getPath (filename),
            info.getEntryByKey(tag),
            nbSequences,
            totalSize
        );
    }

    /********************************************************************************/
    /* Iterate a string sequence iterator, check that the number of sequences is ok
     * and that the data size is ok. */
    /********************************************************************************/
    void testStringIterator ()
    {
    	ISequenceIterator* itSeq = new StringSequenceIterator (3, "AAAIAAA", "KLAAA", "FAAA");
    	LOCAL (itSeq);

    	size_t nbSequences = 0;
    	size_t totalSize   = 0;

        /** We loop over the sequences. */
        for (itSeq->first(); ! itSeq->isDone(); itSeq->next ())
        {
            const ISequence* seq = itSeq->currentItem();
            CPPUNIT_ASSERT (seq != 0);

            /** A little shortcut. */
            const IWord& word = seq->data;

            /** We get the sequence content as a string. */
            string content = word.toString();

            /** We check each sequence content. */
            if (nbSequences == 0)  {  CPPUNIT_ASSERT (content.compare ("AAAIAAA") == 0);    }
            if (nbSequences == 1)  {  CPPUNIT_ASSERT (content.compare ("KLAAA")   == 0);  	}
            if (nbSequences == 2)  {  CPPUNIT_ASSERT (content.compare ("FAAA")    == 0);    }

            totalSize += word.letters.size;
            nbSequences++;
        }

        CPPUNIT_ASSERT (nbSequences == 3);
        CPPUNIT_ASSERT (totalSize   == 16);

        TRACE ("Found %ld sequences for a total size of %ld\n", nbSequences, totalSize);
    }

    /********************************************************************************/
    /* Test 6 reading frames iteration over a nucleotids sequence. */
    /********************************************************************************/
    void testReadingFrameIterator ()
    {
        const ISequence* seq = 0;

        /** We need a nucleotids sequence. */
        ISequenceIterator* itNucleotids = new StringSequenceIterator (1, "CAATGGCTAGGTACTATGTATGAGATCATGATCTTTACAAATCCGAG");
        LOCAL (itNucleotids);

        /** We test reading frame 1. */
        ReadingFrameSequenceIterator itSeq1 (FRAME_1, itNucleotids);     itSeq1.first();    seq = itSeq1.currentItem();
        CPPUNIT_ASSERT (seq != 0);
        CPPUNIT_ASSERT (seq->data.toString().compare ("QWLGTMYEIMIFTNPX") == 0);

        /** We test reading frame 2. */
        ReadingFrameSequenceIterator itSeq2 (FRAME_2, itNucleotids);     itSeq2.first();    seq = itSeq2.currentItem();
        CPPUNIT_ASSERT (seq != 0);
        CPPUNIT_ASSERT (seq->data.toString().compare ("NG*VLCMRS*SLQIRX") == 0);

        /** We test reading frame 3. */
        ReadingFrameSequenceIterator itSeq3 (FRAME_3, itNucleotids);     itSeq3.first();    seq = itSeq3.currentItem();
        CPPUNIT_ASSERT (seq != 0);
        CPPUNIT_ASSERT (seq->data.toString().compare ("MARYYV*DHDLYKSE") == 0);

        /** We test reading frame 4. */
        ReadingFrameSequenceIterator itSeq4 (FRAME_4, itNucleotids);     itSeq4.first();    seq = itSeq4.currentItem();
        CPPUNIT_ASSERT (seq != 0);
        CPPUNIT_ASSERT (seq->data.toString().compare ("LGFVKIMISYIVPSHX") == 0);

        /** We test reading frame 5. */
        ReadingFrameSequenceIterator itSeq5 (FRAME_5, itNucleotids);     itSeq5.first();    seq = itSeq5.currentItem();
        CPPUNIT_ASSERT (seq != 0);
        CPPUNIT_ASSERT (seq->data.toString().compare ("SDL*RS*SHT*YLAIX") == 0);

        /** We test reading frame 6. */
        ReadingFrameSequenceIterator itSeq6 (FRAME_6, itNucleotids);     itSeq6.first();    seq = itSeq6.currentItem();
        CPPUNIT_ASSERT (seq != 0);
        CPPUNIT_ASSERT (seq->data.toString().compare ("RICKDHDLIHST*PL") == 0);
    }

    /********************************************************************************/
    /********************************************************************************/
    void testFastaOutput ()
    {
        const char* filename = "/tmp/output.fa";

        ISequenceIterator* itSeq = new StringSequenceIterator (3, "AAAIAAA", "KLAAA", "FAAA");
        LOCAL (itSeq);

        list<string> comments;
        comments.push_back ("foo");
        comments.push_back ("bar");
        comments.push_back ("dub");

        /** We output the sequences into a file. */
        FastaSequenceOutput output (itSeq, filename, comments);
        output.dump ();

        /** We open the created file. */
        ISequenceIterator* itSeq2 = new FastaSequenceIterator (filename, 100);
        LOCAL (itSeq2);

        int i = 0;
        for (itSeq2->first(); ! itSeq2->isDone(); itSeq2->next (), i++)
        {
            const ISequence* seq = itSeq2->currentItem();
            CPPUNIT_ASSERT (seq != 0);

            if (i==0)  { CPPUNIT_ASSERT (strcmp(seq->comment, "foo")==0); }
            if (i==1)  { CPPUNIT_ASSERT (strcmp(seq->comment, "bar")==0); }
            if (i==2)  { CPPUNIT_ASSERT (strcmp(seq->comment, "dub")==0); }

            if (i==0)  { CPPUNIT_ASSERT (seq->data.toString().compare ("AAAIAAA") == 0); }
            if (i==1)  { CPPUNIT_ASSERT (seq->data.toString().compare ("KLAAA")   == 0); }
            if (i==2)  { CPPUNIT_ASSERT (seq->data.toString().compare ("FAAA")    == 0); }
        }
        CPPUNIT_ASSERT (i==3);
    }

    /********************************************************************************/
    /********************************************************************************/
    void testFiltering ()
    {
        ISequenceIterator* itSeq = new StringSequenceIterator (1, "IGERLKITGNLLEIDNSELLLLVHTQLAGKITGM");
        LOCAL (itSeq);

        for (itSeq->first(); ! itSeq->isDone(); itSeq->next ())
        {
            const ISequence* seq = itSeq->currentItem();
            CPPUNIT_ASSERT (seq != 0);
            printf ("%s\n", seq->toString().c_str());
        }
    }

    /********************************************************************************/
    /********************************************************************************/
    void testFastaOutputFrame ()
    {
        ISequenceIterator* itSeq = new FastaSequenceIterator (getPath("sapiens_1Mo.fa"), 100);
        LOCAL (itSeq);

        ISequenceIterator* itSeq1 = new ReadingFrameSequenceIterator (FRAME_1, itSeq);
        LOCAL (itSeq1);

        /** We output the sequences into a file. */
        list<string> comments;
        FastaSequenceOutput output (itSeq1, "sapiens_1Mo_FRAME_1.fa", comments);
        output.dump ();
    }

    /********************************************************************************/
    /********************************************************************************/
    void testSequenceTokenizer ()
    {
        const char* str = "AGCDEFXXXXXFXXXXXKLMNR";

        ISequenceIterator* itSeq = new StringSequenceIterator (1, str);
        LOCAL (itSeq);

        /** We retrieve the sequence from the database. */
        itSeq->first();
        const ISequence* currentSeq = itSeq->currentItem();

        size_t i=0;
        SequenceTokenizer tokenizer (currentSeq->data);
        for (tokenizer.first(); !tokenizer.isDone(); tokenizer.next(), i++)
        {
            pair<size_t,size_t>& current = tokenizer.currentItem();

            // printf ("[%ld,%ld]\n", current.first, current.second);

            if (i==0)  { CPPUNIT_ASSERT (current.first==0  && current.second==5 ); }
            if (i==1)  { CPPUNIT_ASSERT (current.first==11 && current.second==11); }
            if (i==2)  { CPPUNIT_ASSERT (current.first==17 && current.second==21); }
        }
    }

    /********************************************************************************/
    /********************************************************************************/
    void testFilesList_aux (std::vector<string>& files)
    {
        Info info1, info2;

        /** We iterate each file and get some information. */
        for (size_t i=0; i<files.size(); i++)
        {
            ISequenceIterator* itSeq = new FastaSequenceIterator (getPath (files[i].c_str()));
            CPPUNIT_ASSERT (itSeq);
            LOCAL (itSeq);

            for (itSeq->first(); ! itSeq->isDone(); itSeq->next ())
            {
                const ISequence* seq = itSeq->currentItem();

                info1.nbSequences ++;

                size_t lenComment = strlen(seq->comment);
                for (size_t k=0; k<lenComment; k++)  {  info1.checksumComment += seq->comment[k];  }

                size_t lenData = seq->getLength();
                for (size_t k=0; k<lenData; k++)     {  info1.checksumData    += seq->getData()[k];  }
            }
        }

        /** Now we iterated the concatenation file. */
        string concatFile;
        for (size_t i=0; i<files.size(); i++)
        {
            if (i>0) { concatFile += ','; }
            concatFile += getPath (files[i].c_str());
        }

        ISequenceIterator* itSeq = new FastaSequenceIterator (concatFile.c_str());
        CPPUNIT_ASSERT (itSeq);
        LOCAL (itSeq);

        for (itSeq->first(); ! itSeq->isDone(); itSeq->next ())
        {
            const ISequence* seq = itSeq->currentItem();

            info2.nbSequences ++;

            size_t lenComment = strlen(seq->comment);
            for (size_t k=0; k<lenComment; k++)  {  info2.checksumComment += seq->comment[k];  }

            size_t lenData = seq->getLength();
            for (size_t k=0; k<lenData; k++)     {  info2.checksumData    += seq->getData()[k];  }
        }

        cout << "info1 " << info1 << endl;
        cout << "info2 " << info2 << endl;

        CPPUNIT_ASSERT (info1 == info2);
    }

    /********************************************************************************/
    /********************************************************************************/
    void testFilesList (void)
    {
        vector<string> files;

        files.push_back ("tursiops.fa");        testFilesList_aux (files);
        files.push_back ("query.fa");           testFilesList_aux (files);
        files.push_back ("panda.fa");           testFilesList_aux (files);
        files.push_back ("sapiens_1Mo.fa");     testFilesList_aux (files);
        files.push_back ("yeast.fa");           testFilesList_aux (files);
        files.push_back ("uniprot_sprot.fa");   testFilesList_aux (files);
        files.push_back ("chr10.fa");           testFilesList_aux (files);
    }

    /********************************************************************************/
    /********************************************************************************/
    void testFastaHugeFile ()
    {
        const char* filename = "bugs/15202/hugecomment.fa";

        size_t maxLineSize = 1000;
        ISequenceIterator* itSeq = new FastaSequenceIterator (getPath (filename), maxLineSize);
        LOCAL (itSeq);

        u_int32_t nbSequences = 0;
        u_int64_t totalSize   = 0;

        size_t maxlen = 0;

        /** We loop over the sequences. */
        for (itSeq->first(); ! itSeq->isDone(); itSeq->next ())
        {
            const ISequence* seq = itSeq->currentItem();

            size_t len = strlen (seq->comment);  if (len > maxlen)  { maxlen = len;  }

            char bufId[128*1024]; size_t lenId = sizeof(bufId);
            seq->retrieveId(bufId, lenId);

            totalSize += seq->getLength();

            nbSequences++;
        }

        CPPUNIT_ASSERT (nbSequences == 1);
    }
};

/********************************************************************************/

extern "C" Test* TestSequenceIterator_suite()  { return TestSequenceIterator::suite (); }
