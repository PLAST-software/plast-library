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

/** \file DefaultAlgoConfig.hpp
 *  \brief Default implementation of the IConfiguration interface
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _DEFAULT_ALGO_CONFIG_HPP_
#define _DEFAULT_ALGO_CONFIG_HPP_

/********************************************************************************/

#include <designpattern/api/IProperty.hpp>

#include <algo/core/api/IAlgoConfig.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

/** \brief Default implementation of the IConfiguration interface
 *
 * This abstract factory implementation provides default choices for types of created instances.
 *
 * Note that a IProperties instance is given to the constructor. This instance is used for
 * customizing the types of the instances to be created.
 */
class DefaultConfiguration : public IConfiguration
{
public:

    /** Constructor.
     * \param[in] properties : properties for customizing instances creation.
     */
    DefaultConfiguration (dp::IProperties* properties);

    /** Destructor. */
    virtual ~DefaultConfiguration ();

    /** \copydoc IConfiguration::createDefaultParameters */
    IParameters* createDefaultParameters (const std::string& algoName);

    /** \copydoc IConfiguration::createDispatcher */
    dp::ICommandDispatcher* createDispatcher ();

    /** \copydoc IConfiguration::createDatabase */
    database::ISequenceDatabase*  createDatabase (const std::string& uri, const Range& range, bool filtering);

    /** \copydoc IConfiguration::createQueryInformation */
    statistics::IQueryInformation* createQueryInformation (
        statistics::IGlobalParameters*  globalStats,
        algo::core::IParameters*        parameters,
        database::ISequenceDatabase*    queryDb,
        size_t                          subjectSize,
        size_t                          subjectNbSequences
    );

    /** \copydoc IConfiguration::createGlobalParameters */
    statistics::IGlobalParameters*  createGlobalParameters (algo::core::IParameters* params);

    /** \copydoc IConfiguration::createSeedModel */
    seed::ISeedModel* createSeedModel (misc::SeedModelKind_e modelKind, const std::vector<std::string>& subseedStrings);

    /** \copydoc IConfiguration::createIndexator */
    IIndexator*  createIndexator (seed::ISeedModel* seedsModel, algo::core::IParameters* params);

    /** \copydoc IConfiguration::createScoreMatrix */
    IScoreMatrix* createScoreMatrix (misc::ScoreMatrixKind_e kind, database::Encoding encoding);

    /** \copydoc IConfiguration::createUngapHitIterator */
    algo::hits::IHitIterator* createUngapHitIterator (
        algo::hits::IHitIterator*       source,
        seed::ISeedModel*               model,
        algo::core::IScoreMatrix*       matrix,
        algo::core::IParameters*        params,
        algo::align::IAlignmentResult* ungapResult
    );

    /** \copydoc IConfiguration::createSmallGapHitIterator */
    algo::hits::IHitIterator* createSmallGapHitIterator (
        algo::hits::IHitIterator*       source,
        seed::ISeedModel*               model,
        algo::core::IScoreMatrix*       matrix,
        algo::core::IParameters*        params,
        algo::align::IAlignmentResult* ungapResult,
        algo::align::IAlignmentResult* alignmentResult
    );

    /** \copydoc IConfiguration::createFullGapHitIterator */
    algo::hits::IHitIterator* createFullGapHitIterator  (
        algo::hits::IHitIterator*       source,
        seed::ISeedModel*               model,
        algo::core::IScoreMatrix*       matrix,
        algo::core::IParameters*        params,
        statistics::IQueryInformation*  queryInfo,
        statistics::IGlobalParameters*  globalStats,
        algo::align::IAlignmentResult* ungapResult,
        algo::align::IAlignmentResult* alignmentResult
    );

    /** \copydoc IConfiguration::createCompositionHitIterator */
    algo::hits::IHitIterator* createCompositionHitIterator  (
        algo::hits::IHitIterator*       source,
        seed::ISeedModel*               model,
        algo::core::IScoreMatrix*       matrix,
        algo::core::IParameters*        params,
        statistics::IQueryInformation*  queryInfo,
        statistics::IGlobalParameters*  globalStats,
        algo::align::IAlignmentResult* ungapResult,
        algo::align::IAlignmentResult* alignmentResult
    );

    /** \copydoc IConfiguration::createGapAlignmentResult */
    algo::align::IAlignmentResult* createGapAlignmentResult  (database::ISequenceDatabase* subject, database::ISequenceDatabase* query);

    /** \copydoc IConfiguration::createUnapAlignmentResult */
    algo::align::IAlignmentResult* createUnapAlignmentResult (size_t querySize);

    /** \copydoc IConfiguration::createResultVisitor */
    algo::align::IAlignmentResultVisitor* createResultVisitor ();

private:

    dp::IProperties* _properties;
    void setProperties (dp::IProperties* properties)  { SP_SETATTR(properties);  }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _DEFAULT_ALGO_CONFIG_HPP_ */
