
#include <launcher/core/PlastCmd.hpp>
#include <launcher/core/PlastOptionsParser.hpp>
#include <misc/api/version.hpp>

#include <algo/core/impl/DefaultAlgoEnvironment.hpp>
#include <algo/core/impl/AbstractAlgorithm.hpp>
#include <algo/hits/common/AbstractPipeHitIterator.hpp>
#include <algo/hits/gap/FullGapHitIterator.hpp>
#include <designpattern/impl/Property.hpp>

#include <stdio.h>
#include <list>
#include <vector>
#include <map>
#include <math.h>

using namespace std;
using namespace database;
using namespace algo::core;
using namespace algo::core::impl;
using namespace dp;
using namespace dp::impl;
using namespace alignment::filter;
using namespace alignment::core;
using namespace algo::hits;
using namespace algo::hits::gapped;
using namespace statistics;
using namespace os::impl;
using namespace dp;
using namespace dp::impl;
using namespace indexation;

using namespace misc;
using namespace misc::impl;

using namespace launcher::core;

char AcidAscii[] = {'A','C','D','E','F','G','H','I','K','L','M','N','P','Q','R','S','T','V','W','Y','X','B','Z'};

static const char* STR_OPTION_PARTITION    = "-partition";
static const char* STR_OPTION_NEIGHBORLEN  = "-neighborlen";
static const char* STR_OPTION_DISTRIB_FILE = "-distrib-file";
static const char* STR_OPTION_ALIGN_ONLY   = "-align-only";
static const char* STR_OPTION_IDENT_THRES  = "-ident-thres";

static IProperties* parserProperties = 0;

// Define a macro for optimized score retrieval through the vector-matrix.
#define getScore(i,j)  (_matrixAsVector [(i)+((j)<<5)])

extern void dumpBinary (u_int64_t val, size_t n);


/********************************************************************************/
#define COMP(a) ((int) __builtin_popcountll(a))

bool fncomp (u_int64_t lhs, u_int64_t  rhs)
{
    return COMP(lhs) < COMP(rhs);
}

/********************************************************************************/

static int partitions[][32] =
{
    // A  C  D  E  F  G  H  I  K  L  M  N  P  Q  R  S  T  V  W  Y
    {  1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  },  // 0
    {  2, 0, 3, 3, 0, 2, 3, 1, 3, 1, 0, 3, 3, 3, 3, 2, 2, 1, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  },  // 1
    {  2, 2, 6, 6, 0, 5, 0, 1, 7, 1, 1, 3, 4, 7, 7, 3, 3, 1, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  },  // 2
    {  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  },  // 3
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  },  // 4
};

class AnalyzeUngapHitIterator  : public FullGapHitIterator
{
    map<u_int64_t, size_t, bool(*) (u_int64_t, u_int64_t)> _abundance1;
    map<u_int64_t, size_t, bool(*) (u_int64_t, u_int64_t)> _abundance2;

public:

    /** \copydoc common::AbstractPipeHitIterator::AbstractPipeHitIterator */
    AnalyzeUngapHitIterator (
        algo::hits::IHitIterator*               sourceIterator,
        algo::core::IConfiguration*             config,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               scoreMatrix,
        algo::core::IParameters*                parameters,
        alignment::core::IAlignmentContainer*   ungapResult,
        statistics::IQueryInformation*          queryInfo,
        statistics::IGlobalParameters*          globalStats,
        alignment::core::IAlignmentContainer*   alignmentResult
    )
    : FullGapHitIterator (sourceIterator, config, model, scoreMatrix, parameters, ungapResult, queryInfo, globalStats, alignmentResult),
          nbCommonMin(10000),
          _partition(0),
          _neighborlen(_span + 2*_parameters->ungapNeighbourLength),
          _alignOnly(false), _nbItems(0), _identThreshold(0),
          _abundance1(fncomp), _abundance2(fncomp)
    {
        IProperty* prop = 0;

        if ( (prop = parserProperties->getProperty(STR_OPTION_PARTITION))   != 0)  { _partition   = prop->getInt(); }
        if ( (prop = parserProperties->getProperty(STR_OPTION_NEIGHBORLEN)) != 0)  { _neighborlen = prop->getInt(); }
        if ( (prop = parserProperties->getProperty(STR_OPTION_IDENT_THRES)) != 0)  { _identThreshold = ::atof (prop->getString()); }

        _alignOnly = parserProperties->getProperty (STR_OPTION_ALIGN_ONLY);

        _abundance.resize (_neighborlen+1);
    }

    /** Destructor. */
    virtual ~AnalyzeUngapHitIterator ()
    {
        FILE* file1 = fopen ("/tmp/distrib1.txt", "w");
        if (file1)
        {
            for (map<u_int64_t,size_t>::iterator it=_abundance1.begin(); it != _abundance1.end(); it++)
            {
                fprintf (file1, "%ld  %ld \n", COMP(it->first), it->second);
            }
            fclose (file1);
        }

        FILE* file2 = fopen ("/tmp/distrib2.txt", "w");
        if (file2)
        {
            for (map<u_int64_t,size_t>::iterator it=_abundance2.begin(); it != _abundance2.end(); it++)
            {
                fprintf (file2, "%ld  %ld \n", COMP(it->first), it->second);
            }
            fclose (file2);
        }
    }

    /** \copydoc common::AbstractPipeHitIterator::getName */
    const char* getName ()  { return "AnalyzeUngapHitIterator"; }

    u_int64_t getNbItems()
    {
        if (_splitIterators.empty())  { return _nbItems; }
        u_int64_t result = 0;
        for (size_t i=0; i<_splitIterators.size(); i++)
        {
            AnalyzeUngapHitIterator* current = dynamic_cast<AnalyzeUngapHitIterator*> (_splitIterators[i]);
            if (current)  {  result += current->getNbItems(); }
        }

        return result;
    }

    int getMinCommon ()
    {
        if (_splitIterators.empty())  { return nbCommonMin; }

        int result = 10000;
        for (size_t i=0; i<_splitIterators.size(); i++)
        {
            AnalyzeUngapHitIterator* current = dynamic_cast<AnalyzeUngapHitIterator*> (_splitIterators[i]);
            if (current)  {  if (current->getMinCommon() < result)   { result = current->getMinCommon(); }  }
        }

        return result;
    }

    vector<u_int64_t> getAbundance (u_int64_t& total)
    {
        total = 0;

        cout << "getAbundance -> " << _splitIterators.empty() << "  " << _abundance.size() << endl;
        if (_splitIterators.empty())  { return _abundance; }

        for (size_t i=0; i<_abundance.size(); i++)  { _abundance[i] = 0; }

        for (size_t i=0; i<_splitIterators.size(); i++)
        {
            AnalyzeUngapHitIterator* current = dynamic_cast<AnalyzeUngapHitIterator*> (_splitIterators[i]);
            if (current)
            {
                for (size_t j=0; j<_abundance.size(); j++)
                {
                    _abundance [j] += current->_abundance[j];
                }
            }
        }

        for (size_t i=0; i<_abundance.size(); i++)  { total += _abundance[i]; }

        return _abundance;
    }

protected:

    /** \copydoc common::AbstractPipeHitIterator::clone */
    virtual AbstractPipeHitIterator* clone (algo::hits::IHitIterator* sourceIterator)
    {
        return new AnalyzeUngapHitIterator (sourceIterator, _config, _model, _scoreMatrix, _parameters, _ungapResult, _queryInfo, _globalStats, _alignmentResult);
    }

    /** */
    template<typename Functor>
    void computeScores (LETTER* neighbour1, LETTER* neighbour2, int& score, int& maxScore, int& minScore, const Functor& functor)
    {
        u_int8_t sizeHalfNeighbour = _span + 1*_parameters->ungapNeighbourLength;
        u_int8_t sizeNeighbour     = _span + 2*_parameters->ungapNeighbourLength;

        Functor& fct = (Functor&) functor;

        score=0, maxScore=0, minScore=10000;

        for (u_int8_t i=0; i<sizeHalfNeighbour; i++)
        {
            score += getScore (*(neighbour1++), *(neighbour2++));
            if (maxScore<score)  { maxScore=score; }
            if (minScore>score)  { minScore=score; }
            fct (score, maxScore, minScore);
        }

        score = maxScore;

        for (u_int8_t i=0; i<_parameters->ungapNeighbourLength; i++)
        {
            score += getScore (*(neighbour1++), *(neighbour2++));
            if (maxScore<score)  { maxScore=score; }
            if (minScore>score)  { minScore=score; }
            fct (score, maxScore, minScore);
        }
    }

    /** */
    struct DummyScore   { void operator() (int score, int maxScore, int minScore)  {} };

    struct DumpCommon { ~DumpCommon()  { printf("\n"); } };
    struct DumpScore    : DumpCommon { void operator() (int score, int maxScore, int minScore)  {  printf ("%+03d ", score);    } };
    struct DumpScoreMax : DumpCommon { void operator() (int score, int maxScore, int minScore)  {  printf ("%+03d ", maxScore); } };
    struct DumpScoreMin : DumpCommon { void operator() (int score, int maxScore, int minScore)  {  printf ("%+03d ", minScore); } };

    /********************************************************************************/
    bool dynapro (const ISeedOccurrence* occurSubject, const ISeedOccurrence* occurQuery)
    {
        u_int32_t leftOffsetInQuery=0, leftOffsetInSubject=0, rightOffsetInQuery=0, rightOffsetInSubject=0;

        /** Shortcuts. */
        const ISequence& subjectSeq = occurSubject->sequence;
        const ISequence& querySeq   = occurQuery->sequence;

        /** Shortcuts. */
        LETTER* subjectData = subjectSeq.data.letters.data;
        LETTER* queryData   = querySeq.data.letters.data;

        /** We compute the left part of the score. Note that the left extension includes the starting point,
         * the right extension does not. */
        int scoreLeft = _dynpro->compute (
            queryData,
            subjectData,
            occurQuery->offsetInSequence   + 1,
            occurSubject->offsetInSequence + 1,
            & leftOffsetInQuery,
            & leftOffsetInSubject,
            1
        );

        /** We compute the right part of the score. */
        int scoreRight = _dynpro->compute (
            queryData   + occurQuery->offsetInSequence,
            subjectData + occurSubject->offsetInSequence,
            querySeq.data.letters.size   - occurQuery->offsetInSequence   - 1,
            subjectSeq.data.letters.size - occurSubject->offsetInSequence - 1,
            & rightOffsetInQuery,
            & rightOffsetInSubject,
            0
        );

        int score = scoreLeft + scoreRight;

        /** We retrieve statistical information for the current query sequence. */
        IQueryInformation::SequenceInfo& info = _queryInfo->getSeqInfo (querySeq);

        return (score >= info.cut_offs);
    }


    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/
    /********************************************************************************/
    void iterateMethod (algo::hits::Hit* hit)
    {
        int* partition = partitions[_partition];

        int8_t** matrix = _scoreMatrix->getMatrix();

        const Vector<const ISeedOccurrence*>& occur1Vector = hit->occur1;
        const Vector<const ISeedOccurrence*>& occur2Vector = hit->occur2;

        u_int8_t sizeHalfNeighbour = _span + 1*_parameters->ungapNeighbourLength;
        u_int8_t sizeNeighbour     = _span + 2*_parameters->ungapNeighbourLength;

        for (list<IdxCouple>::iterator it = hit->indexes.begin();  it != hit->indexes.end();  it++)
        {
            const ISeedOccurrence* occur1 = occur1Vector.data[it->first];
            const ISeedOccurrence* occur2 = occur2Vector.data[it->second];

            LETTER* neighbour1 = occur1->neighbourhood.letters.data;
            LETTER* neighbour2 = occur2->neighbourhood.letters.data;

//int score, maxScore, minScore;
//computeScores (neighbour1, neighbour2, score, maxScore, minScore, DummyScore ());
//if (maxScore < _parameters->ungapScoreThreshold)
//{
//    printf ("[%ld,%ld]\n", it->first, it->second);
//    printf ("  "); for (size_t i=0; i<sizeNeighbour; i++)  {  printf ("%c   ", AcidAscii[neighbour1[i]]);  }  printf ("\n");
//    printf ("  "); for (size_t i=0; i<sizeNeighbour; i++)  {  printf ("%c   ", AcidAscii[neighbour2[i]]);  }  printf ("\n");
//    printf ("\n");
//
//    for (size_t i=0; i<sizeNeighbour; i++)  {  printf ("%+03d ", getScore(neighbour1[i],neighbour2[i]));  }  printf ("\n");
//    computeScores (neighbour1, neighbour2, score, maxScore, minScore, DumpScore());
//    computeScores (neighbour1, neighbour2, score, maxScore, minScore, DumpScoreMax());
//    computeScores (neighbour1, neighbour2, score, maxScore, minScore, DumpScoreMin());
//    printf ("score=%d  maxScore=%d  minScore=%d\n\n", score, maxScore, minScore);
//}

            // partition 0 ensures (at 98%) that the 4 seed letters match
            size_t offset = 0;

            int nbCommon=0;
            size_t len = _neighborlen;

#if 0
            for (u_int8_t i=offset; i<_neighborlen; i++)  {  if (partition[neighbour1[i]] == partition[neighbour2[i]])  { nbCommon++; }  }
#else
            u_int64_t mask1 = 0;
            u_int64_t mask2 = 0;

#if 0
            for (u_int64_t i=offset; i<_neighborlen; i++)
            {
                mask1 |= (u_int64_t) (partition[neighbour1[i]]) << i;
                mask2 |= (u_int64_t) (partition[neighbour2[i]]) << i;
            }

            u_int64_t maskCommon = (~ (mask1 ^ mask2)) & ( (1LL <<len) - 1);
            nbCommon = __builtin_popcountll (maskCommon) - offset;
#else
            mask1 = occur1->signature;
            mask2 = occur2->signature;

            len = occur1->signatureLength;

            u_int64_t maskCommon =  (mask1 ^ mask2);
            nbCommon = len - __builtin_popcountll (maskCommon);
#endif

            _abundance1[mask1] ++;
            _abundance2[mask2] ++;

//            dumpBinary (mask1, len);  printf ("\n");
//            dumpBinary (mask2, len);  printf ("\n");
//            printf ("---> nbCommon=%d\n\n", nbCommon);

#endif

            float identity = 100.0 * (float)nbCommon / (float)len;

            bool isAlignement = false;

            if (identity >= _identThreshold)
            {
                bool isOk = (_alignOnly &&  (isAlignement = dynapro (occur1Vector.data [it->first], occur2Vector.data [it->second]))) || (!_alignOnly) ;

                if (isOk)
                {
                    _abundance [nbCommon] ++;
                    _nbItems++;
                }
            }

#if 0
            if (false && isAlignement && identity <= _identThreshold)
            {
//                dumpBinary (mask1, _neighborlen);  printf ("\n");
//                dumpBinary (mask2, _neighborlen);  printf ("\n");

                printf ("--------------------------------------------------------------------\n");
                printf (""); for (u_int8_t i=offset; i<_neighborlen; i++)  {  printf ("%c", AcidAscii[neighbour1[i]]);  }  printf ("\n");
                dumpBinary (mask1, _neighborlen);  printf ("\n");
                printf ("\n");
                printf (""); for (u_int8_t i=offset; i<_neighborlen; i++) {  printf ("%c", AcidAscii[neighbour2[i]]);  }  printf ("\n");
                dumpBinary (mask2, _neighborlen);  printf ("\n");
                printf ("\n");

                dumpBinary (maskCommon, _neighborlen);  printf ("\n");
                printf ("identity=%.1f  nbCommon=%d   isAlignment=%d\n", identity, nbCommon, isAlignement);
                printf ("\n");
            }
#endif


//            if (minScore < nbCommonMin)
//            {
//                nbCommonMin = minScore;
//                computeScores (neighbour1, neighbour2, score, maxScore, minScore, DumpScore   ());
//                computeScores (neighbour1, neighbour2, score, maxScore, minScore, DumpScoreMax());
//                computeScores (neighbour1, neighbour2, score, maxScore, minScore, DumpScoreMin());
//                printf ("\n");
//            }


        } /* end of for (list<IdxCouple>::iterator it  */
    }

private:
    int nbCommonMin;

    vector<u_int64_t> _abundance;
    size_t _partition;
    size_t _neighborlen;
    bool   _alignOnly;
    u_int64_t _nbItems;
    float     _identThreshold;
};

/********************************************************************************/
class MyAlgorithm : public AbstractAlgorithm
{
public:

    MyAlgorithm (
        IConfiguration*                               config,
        database::IDatabaseQuickReader*               reader,
        IParameters*                                  params,
        alignment::filter::IAlignmentFilter*          filter,
        alignment::core::IAlignmentContainerVisitor*  resultVisitor,
        algo::core::IDatabasesProvider*               dbProvider,
        statistics::IGlobalParameters*                globalStats,
        os::impl::TimeInfo*                           timeStats,
        bool&                                         isRunning
    )  : AbstractAlgorithm (config, reader, params, filter, resultVisitor, dbProvider, globalStats, timeStats, isRunning), _analyzeIterator(0)  {}

    ~MyAlgorithm ()
    {
        cout << endl << "nbCommon=" << _analyzeIterator->getMinCommon() << "  nbItems=" << _analyzeIterator->getNbItems() << endl;

        u_int64_t sum=0;
        u_int64_t totalAbundance;
        vector<u_int64_t> abundance = _analyzeIterator->getAbundance (totalAbundance);

        IProperty* prop = 0;

        if ( (prop = parserProperties->getProperty (STR_OPTION_DISTRIB_FILE)) != 0)
        {
            FILE* file = fopen (prop->getString(), "w");
            if (file != 0)
            {
                for (size_t i=0; i<abundance.size(); i++)
                {
                    sum += abundance[i];
                    fprintf (file, "%ld %.1f  %lld  %.3f  %.3f \n", i, (100.0 * (double)i) / (double)abundance.size(),
                        abundance[i], 100.0 * (double)abundance[i] / (double)totalAbundance, 100.0*(double)sum/(double)totalAbundance
                    );
                }

                fclose (file);
            }
        }

    }

protected:

    /** \copydoc IAlgorithm::createHitIterator */
    algo::hits::IHitIterator* createHitIterator (
        IConfiguration*                         config,
        algo::hits::IHitIterator*               sourceHits,
        alignment::core::IAlignmentContainer*   ungapAlignResult,
        alignment::core::IAlignmentContainer*   alignResult
    )
    {
        IHitIterator* ungapHitIterator = getConfig()->createUngapHitIterator (
            sourceHits, getSeedsModel(), getScoreMatrix(), getParams(), ungapAlignResult, _isRunning
        );

        AnalyzeUngapHitIterator* analyzeUngapIterator = new AnalyzeUngapHitIterator (
            ungapHitIterator, getConfig(), getSeedsModel(), getScoreMatrix(), getParams(), ungapAlignResult, getQueryInfo(), getGlobalStatistics(), alignResult
        );

        _analyzeIterator = analyzeUngapIterator;

        return analyzeUngapIterator;
    }

    AnalyzeUngapHitIterator* _analyzeIterator;
};

/********************************************************************************/

class MyEnv : public DefaultEnvironment
{
public:

    MyEnv (dp::IProperties* properties, bool& isRunning) : DefaultEnvironment (properties, isRunning)  {  }

protected:

    list<IAlgorithm*> createAlgorithm (
        IConfiguration*             config,
        IDatabaseQuickReader*       reader,
        IParameters*                params,
        IAlignmentFilter*           filter,
        IAlignmentContainerVisitor* resultVisitor,
        IDatabasesProvider*         dbProvider,
        IGlobalParameters*          globalStats,
        TimeInfo*                   timeInfo,
        bool&                       isRunning
    )
    {
        list<IAlgorithm*> result;
        result.push_back (new MyAlgorithm (config, reader, params, filter, resultVisitor, dbProvider, globalStats, timeInfo, isRunning));
        return result;
    }
};

/********************************************************************************/

int main (int argc, char **argv)
{
    PlastOptionsParser parser;
    parser.add (new OptionOneParam (STR_OPTION_PARTITION,    ""));
    parser.add (new OptionOneParam (STR_OPTION_NEIGHBORLEN,  ""));
    parser.add (new OptionOneParam (STR_OPTION_DISTRIB_FILE, ""));
    parser.add (new OptionNoParam  (STR_OPTION_ALIGN_ONLY,   ""));
    parser.add (new OptionOneParam (STR_OPTION_IDENT_THRES,  ""));

    bool isRunning = true;

    try
    {
        /** We parse the provided options. */
        parser.parse (argc, argv);

        /** We retrieve the parser properties. */
        parserProperties = parser.getProperties();

        /** We create a command. */
        //PlastCmd cmd (props, new MyEnv(props,isRunning));
        PlastCmd cmd (parserProperties, new MyEnv(parserProperties,isRunning));

        /** We launch plast with the aggregated properties. */
        cmd.execute ();
    }
    catch (...)  {  fprintf (stderr, MSG_MAIN_MSG4); }

    return EXIT_SUCCESS;
}
