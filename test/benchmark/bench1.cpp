
#include <launcher/core/PlastCmd.hpp>
#include <launcher/core/PlastOptionsParser.hpp>
#include <misc/api/version.hpp>

#include <algo/core/impl/DefaultAlgoEnvironment.hpp>
#include <algo/core/impl/AbstractAlgorithm.hpp>
#include <algo/hits/common/AbstractPipeHitIterator.hpp>
#include <designpattern/impl/Property.hpp>

#include <stdio.h>
#include <list>
#include <vector>

using namespace std;
using namespace database;
using namespace algo::core;
using namespace algo::core::impl;
using namespace dp;
using namespace dp::impl;
using namespace alignment::filter;
using namespace alignment::core;
using namespace algo::hits;
using namespace statistics;
using namespace os::impl;
using namespace dp;
using namespace dp::impl;
using namespace indexation;

using namespace misc;
using namespace misc::impl;

using namespace launcher::core;

char AcidAscii[] = {'A','C','D','E','F','G','H','I','K','L','M','N','P','Q','R','S','T','V','W','Y','X','B','Z'};

/********************************************************************************/

vector<LETTER> convert (const char* letters)
{
    vector<LETTER> result;

    for ( ; *letters; letters++)
    {
        size_t i=0;
        for (i=0;i<ARRAYSIZE(AcidAscii); i++)  {  if (*letters == AcidAscii[i])  { break; }  }
        result.push_back(i);
    }

    return result;
}

/********************************************************************************/

static int partition[] =
{
// A  C  D  E  F  G  H  I  K  L  M  N  P  Q  R  S  T  V  W  Y
   1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0
};

//static int partition[] =
//{
//// A  C  D  E  F  G  H  I  K  L  M  N  P  Q  R  S  T  V  W  Y
//   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
//};


//static int partition[] =
//{
//// A  C  D  E  F  G  H  I  K  L  M  N  P  Q  R  S  T  V  W  Y
//   0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3
//};

//static int partition[] =
//{
//// A  C  D  E  F  G  H  I  K  L  M  N  P  Q  R  S  T  V  W  Y
//   0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1
//};


class AnalyzeUngapHitIterator  : public algo::hits::common::AbstractPipeHitIterator
{
public:

    /** \copydoc common::AbstractPipeHitIterator::AbstractPipeHitIterator */
    AnalyzeUngapHitIterator (
        algo::hits::IHitIterator*               sourceIterator,
        ::seed::ISeedModel*                     model,
        algo::core::IScoreMatrix*               scoreMatrix,
        algo::core::IParameters*                parameters,
        alignment::core::IAlignmentContainer*   ungapResult,
        alignment::core::IAlignmentContainer*   alignmentResult
    )
    : AbstractPipeHitIterator (sourceIterator, model, scoreMatrix, parameters, ungapResult), nbCommonMin(10000)
    {
        _abundance.resize (48);
    }

    /** Destructor. */
    virtual ~AnalyzeUngapHitIterator () {}

    /** \copydoc common::AbstractPipeHitIterator::getName */
    const char* getName ()  { return "AnalyzeUngapHitIterator"; }

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
        return new AnalyzeUngapHitIterator (sourceIterator, _model, _scoreMatrix, _parameters, _ungapResult, 0);
    }

    /** \copydoc common::AbstractPipeHitIterator::iterateMethod */
    void iterateMethod (algo::hits::Hit* hit)
    {
        int8_t** matrix = _scoreMatrix->getMatrix();

        const Vector<const ISeedOccurrence*>& occur1Vector = hit->occur1;
        const Vector<const ISeedOccurrence*>& occur2Vector = hit->occur2;

        u_int8_t sizeHalfNeighbour = _span + 1*_parameters->ungapNeighbourLength;
        u_int8_t sizeNeighbour     = _span + 2*_parameters->ungapNeighbourLength;

        for (list<IdxCouple>::iterator it = hit->indexes.begin();  it != hit->indexes.end();  it++)
        {
            LETTER* neighbour1 = occur1Vector.data[it->first]->neighbourhood.letters.data;
            LETTER* neighbour2 = occur2Vector.data[it->second]->neighbourhood.letters.data;

            size_t nb1 = occur1Vector.data[it->first]->neighbourhood.letters.size;
            size_t nb2 = occur2Vector.data[it->second]->neighbourhood.letters.size;
#if 1
//            printf ("[%ld,%ld]\n", it->first, it->second);
//            for (size_t i=0; i<nb1; i++)  {  printf ("%c", AcidAscii[neighbour1[i]]);  }  printf ("\n");
//            for (size_t i=0; i<nb2; i++)  {  printf ("%c", AcidAscii[neighbour2[i]]);  }  printf ("\n");

            int score=0, maxScore=0;

            for (u_int8_t i=0; i<sizeHalfNeighbour; i++)
            {
                int val = matrix [neighbour1[i]] [neighbour2[i]];
                score += val;
                if (maxScore<score)  { maxScore=score; }
            }

            score = maxScore;

            for (u_int8_t i=sizeHalfNeighbour; i<sizeNeighbour; i++)
            {
                int val = matrix [neighbour1[i]] [neighbour2[i]];
                score += val;
                if (maxScore<score)  { maxScore=score; }
            }

            int nbCommon=0;
            for (u_int8_t i=0; i<_abundance.size(); i++)
            {
                if (partition[neighbour1[i]] == partition[neighbour2[i]])  { nbCommon++; }
            }
            _abundance [nbCommon] ++;

            if (nbCommon < nbCommonMin)  { nbCommonMin = nbCommon;  }
#endif
        }
    }

private:
    int nbCommonMin;

    vector<u_int64_t> _abundance;
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
        cout << "nbCommon=" << _analyzeIterator->getMinCommon() << endl;

        u_int64_t sum=0;
        u_int64_t totalAbundance;
        vector<u_int64_t> abundance = _analyzeIterator->getAbundance (totalAbundance);
        for (size_t i=0; i<abundance.size(); i++)
        {
            sum += abundance[i];
            printf ("%ld %.1f  %lld  %.3f  %.3f \n", i, (100.0 * (double)i) / 48.0,
                abundance[i], 100.0 * (double)abundance[i] / (double)totalAbundance, 100.0*(double)sum/(double)totalAbundance
            );
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
            ungapHitIterator, getSeedsModel(), getScoreMatrix(), getParams(), ungapAlignResult, alignResult
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
    bool isRunning = true;

    try
    {
        /** We parse the provided options. */
        parser.parse (argc, argv);

        /** We retrieve the parser properties. */
        IProperties* props = parser.getProperties();

        /** We create a command. */
        //PlastCmd cmd (props, new MyEnv(props,isRunning));
        PlastCmd cmd (props, new MyEnv(props,isRunning));

        /** We launch plast with the aggregated properties. */
        cmd.execute ();
    }
    catch (...)  {  fprintf (stderr, MSG_MAIN_MSG4); }

    return EXIT_SUCCESS;
}
