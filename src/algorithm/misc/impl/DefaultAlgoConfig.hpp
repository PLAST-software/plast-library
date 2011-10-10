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

#ifndef _DEFAULT_ALGO_CONFIG_HPP_
#define _DEFAULT_ALGO_CONFIG_HPP_

/********************************************************************************/

#include "IAlgoConfig.hpp"
#include "IProperty.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class DefaultConfiguration : public IConfiguration
{
public:

    DefaultConfiguration (dp::IProperties* properties);
    virtual ~DefaultConfiguration ();

    IParameters* createDefaultParameters (const std::string& algoName);

    dp::ICommandDispatcher* createDispatcher ();

    database::ISequenceDatabase*  createDatabase (const std::string& uri, const Range& range, bool filtering);

    statistics::IQueryInformation* createQueryInformation (
        statistics::IGlobalParameters*  globalStats,
        algo::IParameters*              parameters,
        database::ISequenceDatabase*    queryDb,
        size_t                          subjectSize,
        size_t                          subjectNbSequences
    );

    statistics::IGlobalParameters*  createGlobalParameters (algo::IParameters* params);

    seed::ISeedModel* createSeedModel (SeedModelKind_e modelKind, const std::vector<std::string>& subseedStrings);

    IIndexator*  createIndexator (seed::ISeedModel* seedsModel, algo::IParameters* params);

    IScoreMatrix* createScoreMatrix (ScoreMatrixKind_e kind, database::Encoding encoding);

    indexation::IHitIterator* createUngapHitIterator (
        indexation::IHitIterator*   source,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         matrix,
        algo::IParameters*          params,
        algo::IAlignmentResult*     ungapResult
    );

    indexation::IHitIterator* createSmallGapHitIterator (
        indexation::IHitIterator*   source,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         matrix,
        algo::IParameters*          params,
        algo::IAlignmentResult*     ungapResult
    );

    indexation::IHitIterator* createFullGapHitIterator  (
        indexation::IHitIterator*       source,
        seed::ISeedModel*               model,
        algo::IScoreMatrix*             matrix,
        algo::IParameters*              params,
        statistics::IQueryInformation*  queryInfo,
        statistics::IGlobalParameters*  globalStats,
        algo::IAlignmentResult*         ungapResult,
        algo::IAlignmentResult*         alignmentResult
    );

    indexation::IHitIterator* createCompositionHitIterator  (
        indexation::IHitIterator*       source,
        seed::ISeedModel*               model,
        algo::IScoreMatrix*             matrix,
        algo::IParameters*              params,
        statistics::IQueryInformation*  queryInfo,
        statistics::IGlobalParameters*  globalStats,
        algo::IAlignmentResult*         ungapResult,
        algo::IAlignmentResult*         alignmentResult
    );

    IAlignmentResult* createGapAlignmentResult  (database::ISequenceDatabase* subject, database::ISequenceDatabase* query);

    IAlignmentResult* createUnapAlignmentResult (size_t querySize);

    AlignmentResultVisitor* createResultVisitor ();

private:

    dp::IProperties* _properties;
    void setProperties (dp::IProperties* properties)  { SP_SETATTR(properties);  }

    void findDabasesUri (std::list <std::pair <std::string,std::string> >& uri, database::IDatabaseQuickReader* reader);

    u_int64_t getFileSize (const std::string& uri);

    std::string createUri (const std::string& uri, u_int64_t i0, u_int64_t i1);
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _DEFAULT_ALGO_CONFIG_HPP_ */
