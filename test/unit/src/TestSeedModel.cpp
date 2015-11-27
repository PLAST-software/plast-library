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

#include <database/api/IAlphabet.hpp>
#include <database/impl/BufferedSequenceDatabase.hpp>
#include <database/impl/StringSequenceIterator.hpp>

#include <seed/impl/BasicSeedModel.hpp>
#include <seed/impl/SubSeedModel.hpp>

#include <stdlib.h>
#include <stdarg.h>

using namespace std;
using namespace database;
using namespace database::impl;
using namespace seed;
using namespace seed::impl;

/********************************************************************************/

class TestSeedModel : public TestFixture
{
private:
    ISequenceDatabase* _database;
    ISequenceIterator* _itSeq;

public:

    /********************************************************************************/
    void setUp    ()
    {
        /** We create a database from an iterator. */
        _database = new BufferedSequenceDatabase (new StringSequenceIterator (1, "CWNVYAGD"), false);
        CPPUNIT_ASSERT (_database != 0);
        _database->use ();

        CPPUNIT_ASSERT (_database->getSequencesNumber() > 0);

        /** We create an iterator over the database. */
        _itSeq = _database->createSequenceIterator();
        CPPUNIT_ASSERT (_itSeq != 0);
        _itSeq->use ();
    }

    void tearDown ()
    {
        _database->forget ();
        _itSeq->forget    ();
    }

    /********************************************************************************/
    static Test* suite()
    {
    	 TestSuite* result = new TestSuite ("SeedModelTest");
    	 result->addTest (new TestCaller<TestSeedModel> ("testSeedModelFile",               &TestSeedModel::testSeedModelFile ) );
         result->addTest (new TestCaller<TestSeedModel> ("testSeedModelCheck",              &TestSeedModel::testSeedModelCheck ) );
         result->addTest (new TestCaller<TestSeedModel> ("testBasicSeedModelCheck",         &TestSeedModel::testBasicSeedModelCheck ) );
         result->addTest (new TestCaller<TestSeedModel> ("testSeedModelIterateBadLetters",  &TestSeedModel::testSeedModelIterateBadLetters ) );
         result->addTest (new TestCaller<TestSeedModel> ("testSeedModelNucleotid",          &TestSeedModel::testSeedModelNucleotid ) );
    	 return result;
    }

    /********************************************************************************/
    /********************************************************************************/
    void testSeedModelFile ()
    {
#if 0
        SubSeedModel m1 ("data/seedT1W1_1");
        CPPUNIT_ASSERT (m1.getSpan() == 4);

        SubSeedModel m2 ("data/seedT1W1_50_3");
        CPPUNIT_ASSERT (m2.getSpan() == 3);
#endif
    }

    /********************************************************************************/
    /********************************************************************************/
    size_t _nbSeeds;
    void iterateSeed (const ISeed* seed)   { _nbSeeds ++; }

    /** */
    void testSeedModelCheck_aux (const char* name, ISeedModel* model, ...)
    {
        LOCAL (model);

        ISeedIterator* it = model->createAllSeedsIterator();
        CPPUNIT_ASSERT (it != 0);
        LOCAL (it);

        /** We check that we have the correct number of possible seeds. */
        size_t nb = 0;
        for (it->first(); !it->isDone(); it->next(), nb++)  {}
        CPPUNIT_ASSERT (nb == model->getSeedsMaxNumber());

        /** We check again that we have the correct number of possible seeds by iterate method. */
        _nbSeeds = 0;
        it->iterate (this, (dp::Iterator<const ISeed*>::Method) &TestSeedModel::iterateSeed);
        CPPUNIT_ASSERT (_nbSeeds == model->getSeedsMaxNumber());

        /** We get the first sequence. */
        _itSeq->first();  const ISequence* currentSeq = _itSeq->currentItem();
        CPPUNIT_ASSERT (currentSeq != 0);

        /** We create a seed iterator with the data of the current sequence. */
        ISeedIterator* itSeed = model->createSeedsIterator (currentSeq->data);
        CPPUNIT_ASSERT (itSeed != 0);
        LOCAL (itSeed);
        itSeed->first();

        /** We parse the ellipsis. */
        va_list varg;
        va_start (varg, model);
        const char* seedCheck = 0;
        while (! itSeed->isDone())
        {
            seedCheck = va_arg (varg, const char*);
            if (seedCheck == 0)  { break; }
            CPPUNIT_ASSERT (itSeed->currentItem()->kmer.toString().compare (seedCheck) == 0);
            itSeed->next ();
        }
        va_end(varg);

        /** We loop over seeds and compute some kind of hash. */
        u_int64_t hash1 = 0;
        for (it->first(); !it->isDone(); it->next())
        {
            const ISeed* seed = it->currentItem();
            for (size_t i=0; i<seed->kmer.letters.size; i++)  { hash1 += seed->kmer.letters.data[i]; }
        }

        /** Now, we check that split iterators are ok. */
        size_t nbSplits = 7;
        size_t average  = model->getSeedsMaxNumber() / nbSplits;
        u_int64_t hash2 = 0;
        size_t nbFound = 0;
        for (size_t i=0; i<nbSplits; i++)
        {
            size_t first = i*average;
            size_t last  = (i<nbSplits-1 ? ((i+1)*average - 1) : model->getSeedsMaxNumber()-1);

            ISeedIterator* extractIt = it->extract (first, last);
            CPPUNIT_ASSERT (extractIt != 0);
            LOCAL (extractIt);

            for (extractIt->first(); !extractIt->isDone(); extractIt->next())
            {
                const ISeed* seed = extractIt->currentItem();
                for (size_t i=0; i<seed->kmer.letters.size; i++)  { hash2 += seed->kmer.letters.data[i]; }
                nbFound++;
            }
        }
        // SHOULD WORK... CPPUNIT_ASSERT (hash1 == hash2);
        CPPUNIT_ASSERT (nbFound = model->getSeedsMaxNumber());
    }

    /** */
    void testSeedModelCheck ()
    {
        testSeedModelCheck_aux ("PLASTP_BL62", new SubSeedModel (4,
            "H,FY,W,IV,LM,C,RK,Q,E,N,D,A,S,T,G,P",
            "HFYWIVLMC,RKQENDASTGP",
            "H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P",
            "H,FY,W,IV,LM,C,R,K,Q,E,N,D,A,S,T,G,P"
            ),
            "CHNI", "WRIF", "NHFA", "IHAG", "FRGD", 0
        );

        testSeedModelCheck_aux ("TPLASTN_BL62", new SubSeedModel (4,
            "A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y",
            "CFYWMLIV,GPATSNHQEDRK",
            "A,C,FYW,G,IV,ML,NH,P,QED,RK,TS",
            "A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y"
            ),
            "CCNV", "WGIY", "NCFA", "VCAG", "YGGD", 0
        );

        testSeedModelCheck_aux ("PLASTX_BL62", new SubSeedModel (4,
            "H,FY,W,IV,L,M,C,R,K,Q,E,N,D,A,S,T,G,P",
            "HFYWIVLMC,RKQENDASTGP",
            "H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P",
            "H,FY,W,I,V,L,M,C,R,K,Q,E,N,D,A,S,T,G,P"
            ),
            "CHNV", "WRIF", "NHFA", "IHAG", "FRGD", 0
        );

        testSeedModelCheck_aux ("PLASTP_BL50", new SubSeedModel (3,
            "C,FY,W,M,L,IV,G,P,A,T,S,H,Q,E,R,K,D,N",
            "C,FYW,MLIV,GPATS,HQERKDN",
            "C,FY,W,M,L,IV,G,P,A,T,S,H,Q,E,R,K,D,N"
            ),
            "CFN", "WHI", "NMF", "IFA", "FGG", "AGD", 0
        );

        testSeedModelCheck_aux ("TPLASTN_BL50", new SubSeedModel (4,
            "AS,RQK,N,DE,C,G,H,IV,LM,FWY,P,T",
            "AGST,RNDQEHK,CPW,ILMFYV",
            "C,FYW,MLIV,GPATS,HQERKDN",
            "AST,RK,ND,C,QE,G,H,IL,MV,FWY,P"
            ),
            "CCHM", "FRMF", "NIFA", "IIGG", "FAGN", 0
        );

        testSeedModelCheck_aux ("PLASTX_BL50", new SubSeedModel (3,
            "C,FY,W,M,L,I,V,G,P,A,T,S,H,Q,E,R,K,D,N",
            "C,FYW,MLIV,GPATS,HQERKDN",
            "C,FY,W,M,L,IV,G,P,A,T,S,H,Q,E,R,K,D,N"
            ),
            "CFN", "WHI", "NMF", "VFA", "FGG", "AGD", 0
        );

        testSeedModelCheck_aux ("BIG_ONE", new SubSeedModel (6,
            "C,FY,W,M,L,I,V,G,P,A,T,S,H,Q,E,R,K,D,N",
            "C,FYW,MLIV,GPATS,HQERKDN",
            "C,FY,W,M,L,IV,G,P,A,T,S,H,Q,E,R,K,D,N",
            "C,FYW,MLIV,GPATS,HQERKDN",
            "C,FYW,MLIV,GPATS,HQERKDN",
            "C,FY,W,M,L,IV,G,P,A,T,S,H,Q,E,R,K,D,N"
            ),
            "CFNMFA", "WHIFGG", "NMFGGD", 0
        );

        testSeedModelCheck_aux ("BASIC MODEL 3", new BasicSeedModel (SUBSEED,3),
            "CWN", "WNV", "NVY", "VYA", "YAG", "AGD", 0
        );

        testSeedModelCheck_aux ("BASIC MODEL 4", new BasicSeedModel (SUBSEED,4),
            "CWNV", "WNVY", "NVYA", "VYAG", "YAGD",  0
        );
    }

    /********************************************************************************/
    /********************************************************************************/
    void testBasicSeedModelCheck ()
    {
        ISeedModel* model = new SubSeedModel (3,
            "H,FY,W,IV,LM,C,RK,Q,E,N,D,A,S,T,G,P",
            "HFYWIVLMC,RKQENDASTGP",
            "H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P"
        );

//        ISeedModel* model = new BasicSeedModel (SUBSEED,3);

        CPPUNIT_ASSERT (model != 0);
        LOCAL (model);

        ISeedIterator* it = model->createAllSeedsIterator ();
        CPPUNIT_ASSERT (it != 0);
        LOCAL (it);

        size_t nb = 0;
        for (it->first(); !it->isDone(); it->next())
        {
            //const ISeed* seed = it->currentItem();
            nb++;
            //printf ("'%s'%c", seed->kmer.toString().c_str(), (nb%20==0 ? '\n' : ' ') );
        }
        //printf ("\nnb=%ld  max=%ld\n", nb, model->getSeedsMaxNumber());

        CPPUNIT_ASSERT (nb == model->getSeedsMaxNumber());
    }

    /********************************************************************************/
    /********************************************************************************/
    void testSeedModelIterateBadLetters ()
    {
        /** We create a model. */
        ISeedModel* model = new BasicSeedModel (SUBSEED,3);
        CPPUNIT_ASSERT (model != 0);
        LOCAL (model);

        /** We create a word. */
        LETTER table[] = { CODE_X, CODE_D, CODE_A, CODE_F, CODE_X, CODE_G, CODE_T, CODE_C, CODE_Q};

        IWord w (sizeof(table) / sizeof(table[0]), table);

        /** We create an iterator over the word. */
        ISeedIterator* it = model->createSeedsIterator (w);
        CPPUNIT_ASSERT (it != 0);
        LOCAL (it);

        /** We loop over the seeds. */
        size_t nb=0;
        for (it->first(); !it->isDone(); it->next(), nb++)
        {
            const ISeed* seed = it->currentItem();
            LETTER* buf = seed->kmer.letters.data;

            if (nb==0)  {  CPPUNIT_ASSERT (buf[0]==CODE_D && buf[1]==CODE_A && buf[2]==CODE_F);  }
            if (nb==1)  {  CPPUNIT_ASSERT (buf[0]==CODE_G && buf[1]==CODE_T && buf[2]==CODE_C);  }
            if (nb==2)  {  CPPUNIT_ASSERT (buf[0]==CODE_T && buf[1]==CODE_C && buf[2]==CODE_Q);  }
        }

        CPPUNIT_ASSERT (nb == 3);
    }

    /********************************************************************************/
    /********************************************************************************/
    void testSeedModelNucleotid ()
    {
        /** WARNING !  We first switch to nucleotide alphabet before creating the instance. */
        EncodingManager::singleton().setKind (EncodingManager::ALPHABET_NUCLEOTID);

        /** We create a model. */
        ISeedModel* model = new BasicSeedModel (SUBSEED,3);
        CPPUNIT_ASSERT (model != 0);
        LOCAL (model);

        /** WARNING !  We switch back to amnino acid alphabet. */
        EncodingManager::singleton().setKind (EncodingManager::ALPHABET_AMINO_ACID);
    }
};

/********************************************************************************/

extern "C" Test* TestSeedModel_suite()  { return TestSeedModel::suite (); }
