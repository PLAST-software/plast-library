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

#ifndef _IALGO_CONFIG_HPP_
#define _IALGO_CONFIG_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"

#include "ICommand.hpp"
#include "ISequenceDatabase.hpp"
#include "IStatistics.hpp"

#include "IHitIterator.hpp"

#include "ISeedModel.hpp"

#include "IScoreMatrix.hpp"

#include "IAlgoParameters.hpp"
#include "IAlgoIndexator.hpp"

#include "IStatistics.hpp"

#include "IAlignementResult.hpp"

#include "IProperty.hpp"

#include "IDatabaseQuickReader.hpp"

#include <string>
#include <vector>

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class IConfiguration : public dp::SmartPointer
{
public:

    virtual IParameters* createDefaultParameters (const std::string& algoName) = 0;

//    virtual std::vector<IParameters*>  createParametersList (dp::IProperties* properties, database::IDatabaseQuickReader* reader) = 0;

    virtual dp::ICommandDispatcher* createDispatcher () = 0;

    virtual database::ISequenceDatabase*  createDatabase (const std::string& uri, const Range& range, bool filtering) = 0;

    virtual statistics::IQueryInformation* createQueryInformation (
        statistics::IGlobalParameters*  globalStats,
        algo::IParameters*              parameters,
        database::ISequenceDatabase*    queryDb,
        size_t                          subjectSize,
        size_t                          subjectNbSequences
    ) = 0;

    virtual statistics::IGlobalParameters*  createGlobalParameters (algo::IParameters* params) = 0;

    virtual seed::ISeedModel* createSeedModel (SeedModelKind_e modelKind, const std::vector<std::string>& subseedStrings) = 0;

    virtual algo::IIndexator*  createIndexator (seed::ISeedModel* seedsModel, algo::IParameters* params) = 0;

    virtual IScoreMatrix* createScoreMatrix (ScoreMatrixKind_e kind, database::Encoding encoding) = 0;

    virtual indexation::IHitIterator* createUngapHitIterator (
        indexation::IHitIterator*   source,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         matrix,
        algo::IParameters*          params,
        algo::IAlignmentResult*     ungapResult
    ) = 0;

    virtual indexation::IHitIterator* createSmallGapHitIterator (
        indexation::IHitIterator*   source,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         matrix,
        algo::IParameters*          params,
        algo::IAlignmentResult*     ungapResult
    ) = 0;

    virtual indexation::IHitIterator* createFullGapHitIterator  (
        indexation::IHitIterator*       source,
        seed::ISeedModel*               model,
        algo::IScoreMatrix*             matrix,
        algo::IParameters*              params,
        statistics::IQueryInformation*  queryInfo,
        statistics::IGlobalParameters*  globalStats,
        algo::IAlignmentResult*         ungapResult,
        algo::IAlignmentResult*         alignmentResult
    ) = 0;

    virtual indexation::IHitIterator* createCompositionHitIterator  (
        indexation::IHitIterator*       source,
        seed::ISeedModel*               model,
        algo::IScoreMatrix*             matrix,
        algo::IParameters*              params,
        statistics::IQueryInformation*  queryInfo,
        statistics::IGlobalParameters*  globalStats,
        algo::IAlignmentResult*         ungapResult,
        algo::IAlignmentResult*         alignmentResult
    ) = 0;


    virtual IAlignmentResult* createUnapAlignmentResult (size_t querySize) = 0;

    virtual IAlignmentResult* createGapAlignmentResult  (database::ISequenceDatabase* subject, database::ISequenceDatabase* query) = 0;

    virtual AlignmentResultVisitor* createResultVisitor () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_CONFIG_HPP_ */
