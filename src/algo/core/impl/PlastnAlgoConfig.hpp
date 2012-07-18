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

/** \file PlastnAlgoConfig.hpp
 *  \brief Plastn implementation of the IConfiguration interface
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _PLASTN_ALGO_CONFIG_HPP_
#define _PLASTN_ALGO_CONFIG_HPP_

/********************************************************************************/

#include <algo/core/impl/DefaultAlgoConfig.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

/** \brief Plastn implementation of the IConfiguration interface
 *
 * This abstract factory implementation provides default choices for types of created instances.
 *
 * Note that a IProperties instance is given to the constructor. This instance is used for
 * customizing the types of the instances to be created.
 */
class PlastnConfiguration : public DefaultConfiguration
{
public:

    /** Constructor.
     * \param[in] properties : properties for customizing instances creation.
     */
    PlastnConfiguration (IEnvironment* environment, dp::IProperties* properties);

    /** Destructor. */
    virtual ~PlastnConfiguration ();

    /** \copydoc IConfiguration::createDefaultParameters */
    IParameters* createDefaultParameters (const std::string& algoName);

    /** \copydoc IConfiguration::createIndexationDispatcher */
    dp::ICommandDispatcher* createIndexationDispatcher ();

    /** \copydoc IConfiguration::createDatabase */
    database::ISequenceDatabase*  createDatabase (const std::string& uri, const misc::Range64& range, bool filtering);

    /** \copydoc IConfiguration::createGlobalParameters */
    statistics::IGlobalParameters*  createGlobalParameters (algo::core::IParameters* params);

    /** \copydoc IConfiguration::createSeedModel */
    seed::ISeedModel* createSeedModel (
        misc::SeedModelKind_e modelKind,
        size_t span,
        const std::vector<std::string>& subseedStrings
    );

    /** \copydoc IConfiguration::createScoreMatrix */
    IScoreMatrix* createScoreMatrix (misc::ScoreMatrixKind_e kind, database::Encoding encoding);

    /** \copydoc IConfiguration::createIndexator */
    IIndexator*  createIndexator (
        seed::ISeedModel*        seedsModel,
        algo::core::IParameters* params,
        bool&                    isRunning
    );

    /** \copydoc IConfiguration::createGapAlignmentResult */
    alignment::core::IAlignmentContainer* createGapAlignmentResult  ();
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _PLASTN_ALGO_CONFIG_HPP_ */
