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

/** \file IAlgoConfig.hpp
 *  \brief Define concepts for configuring PLAST algorithm.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALGO_CONFIG_HPP_
#define _IALGO_CONFIG_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/api/ICommand.hpp>
#include <designpattern/api/IProperty.hpp>

#include <database/api/ISequenceDatabase.hpp>
#include <database/api/IDatabaseQuickReader.hpp>

#include <seed/api/ISeedModel.hpp>

#include <algo/core/api/IScoreMatrix.hpp>
#include <algo/core/api/IAlgoParameters.hpp>
#include <algo/core/api/IAlgoIndexator.hpp>

#include <algo/stats/api/IStatistics.hpp>

#include <algo/hits/api/IHitIterator.hpp>

#include <alignment/core/api/IAlignmentContainer.hpp>
#include <alignment/filter/api/IAlignmentFilter.hpp>

#include <string>
#include <vector>

/********************************************************************************/
/** \brief PLAST algorithm concepts. */
namespace algo {
/** \brief Concept for configuring and running PLAST. */
namespace core {
/********************************************************************************/

/** \brief Factory that creates many kind of objects
 *
 * This interface is an Abstract Factory (see [GOF94]) that creates many kind of objects.
 *
 *  The types of the created instances may depend on different criteria:
 *      - user preferences (through command line options, configuration file, ...)
 *      - kind of the algorithm (plastp, tplastn, plastx...)
 *      - the environment (memory, cores, network, ...)
 *
 *  The IAlgorithm interface is one of the client of this IConfiguration interface; the algorithm
 *  creates through this class the objects it needs for processing its different parts.
 *
 *  The intent of this class is to try to centralize a lot of objects creation for the full PLAST
 */
class IConfiguration : public dp::SmartPointer
{
public:

    /** Create default parameters for the given algorithm name. It is the initial entry point for
     *  creating parameters; once created, it is possible to modify some attributes according to
     *  user preferences for instance.
     *  \param[in] algoName : the name (plastp, plastx, tplastn) of the algorithm to be executed
     *  \return a new IParameters instance
     */
    virtual IParameters* createDefaultParameters (const std::string& algoName) = 0;

    /** Create a command dispatcher instance. Such an instance can be used for parallelization (hits
     * iteration for instance, see IAlgorithm class).
     *  \return a new ICommandDispatcher instance
     */
    virtual dp::ICommandDispatcher* createIndexationDispatcher () = 0;

    /** Create a command dispatcher instance. Such an instance can be used for parallelization (hits
     * iteration for instance, see IAlgorithm class).
     *  \return a new ICommandDispatcher instance
     */
    virtual dp::ICommandDispatcher* createDispatcher () = 0;

    /** Create a database object (with means for retrieving sequence within the database) from an uri (likely
     *  a local file, but it should be a location on a remote computer). A Range can be provided for using only
     *  a part of the database.
     *  \param[in] uri : uri of the database file to be read
     *  \param[in] range : a range to be read in the file (ie. a starting and ending offsets)
     *  \param[in] filtering : tells whether low informative regions have to be filtered out from the database
     *  \return a new ISequenceDatabase instance
     */
    virtual database::ISequenceDatabase*  createDatabase (const std::string& uri, const misc::Range64& range, bool filtering) = 0;

    /** Create statistical information for the query database that will be used by the algorithm for getting cutoffs.
     * \param[in] globalStats : global statistics for the algorithm
     * \param[in] parameters  : for parameterizing the instance creation
     * \param[in] queryDb     : query database for which we want to compute statistical information
     * \param[in] subjectSize : data size of the subject database
     * \param[in] subjectNbSequences : number of sequences in the subject database
     * \return a new IQueryInformation instance
     */
    virtual statistics::IQueryInformation* createQueryInformation (
        statistics::IGlobalParameters*  globalStats,
        algo::core::IParameters*        parameters,
        database::ISequenceDatabase*    queryDb,
        size_t                          subjectSize,
        size_t                          subjectNbSequences
    ) = 0;

    /** Create global statistical information used by the algorithm.
     * \param[in] params : for parameterizing the instance creation
     * \return a new IGlobalParameters instance
     */
    virtual statistics::IGlobalParameters*  createGlobalParameters (algo::core::IParameters* params) = 0;

    /** Create a seeds model used by the algorithm (in particular, used for databases indexation)
     * \param[in] modelKind : kind of the model to be created; normally should be a subseed model
     * \param[in] span : number of letter of each seed (for a given alphabet).
     * \param[in] subseedStrings : strings defining a subseed model
     * \return a new ISeedModel instance
     */
    virtual seed::ISeedModel* createSeedModel (misc::SeedModelKind_e modelKind, size_t span, const std::vector<std::string>& subseedStrings) = 0;

    /** Create an indexator instance, ie. something that can index a database given some seeds model (and other params).
     *  Its main feature is to create a source Hit iterator from a seeds model and the subject and query databases.
     *  This source iterator can then be linked to other Hit iterators (ungap, small gap...) for filtering out Hits.
     *  \param[in] seedsModel : the seed model used for indexation
     *  \param[in] params : for parameterizing the instance creation
     *  \return a new IIndexator instance
     */
    virtual algo::core::IIndexator*  createIndexator (
        seed::ISeedModel*        seedsModel,
        algo::core::IParameters* params,
        bool&                    isRunning
    ) = 0;

    /** Create a score matrix (BLOSUM50, BLOSUM62...)
     * \param[in] kind : kind of the matrix; likely to be BLOSUM62
     * \param[in] encoding : gives the encoding scheme for the matrix
     * \return a new IScoreMatrix instance
     */
    virtual IScoreMatrix* createScoreMatrix (misc::ScoreMatrixKind_e kind, database::Encoding encoding) = 0;

    /** Create a Hit iterator used during the ungap part of the PLAST algorithm.
     * \return a new IHitIterator instance
     */
    virtual algo::hits::IHitIterator* createUngapHitIterator (
        algo::hits::IHitIterator*               source,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               matrix,
        algo::core::IParameters*                params,
        alignment::core::IAlignmentContainer*   ungapResult
    ) = 0;

    /** Create a Hit iterator used during the ungap part of the PLAST algorithm.
     * \return a new IHitIterator instance
     */
    virtual algo::hits::IHitIterator* createUngapExtendHitIterator (
        algo::hits::IHitIterator*               source,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               matrix,
        algo::core::IParameters*                params,
        alignment::core::IAlignmentContainer*   ungapResult,
        statistics::IGlobalParameters*          globalStats,
        statistics::IQueryInformation*          queryInfo
    ) = 0;

    /** Create a Hit iterator used during the small gap part of the PLAST algorithm.
     * \return a new IHitIterator instance
     */
    virtual algo::hits::IHitIterator* createSmallGapHitIterator (
        algo::hits::IHitIterator*               source,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               matrix,
        algo::core::IParameters*                params,
        alignment::core::IAlignmentContainer*   ungapResult,
        alignment::core::IAlignmentContainer*   alignmentResult
    ) = 0;

    /** Create a Hit iterator used during the full gap part of the PLAST algorithm.
     * \return a new IHitIterator instance
     */
    virtual algo::hits::IHitIterator* createFullGapHitIterator  (
        algo::hits::IHitIterator*               source,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               matrix,
        algo::core::IParameters*                params,
        statistics::IQueryInformation*          queryInfo,
        statistics::IGlobalParameters*          globalStats,
        alignment::core::IAlignmentContainer*   ungapResult,
        alignment::core::IAlignmentContainer*   alignmentResult
    ) = 0;

    /** Create a Hit iterator used during the composition part of the PLAST algorithm.
     * \return a new IHitIterator instance
     */
    virtual algo::hits::IHitIterator* createCompositionHitIterator  (
        algo::hits::IHitIterator*               source,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               matrix,
        algo::core::IParameters*                params,
        statistics::IQueryInformation*          queryInfo,
        statistics::IGlobalParameters*          globalStats,
        alignment::core::IAlignmentContainer*   ungapResult,
        alignment::core::IAlignmentContainer*   alignmentResult
    ) = 0;

    /** Create a IAlignmentResult instance for holding generated ungap alignments.
     * \param[in] querySize
     * \return a new IAlignmentResult instance
     */
    virtual alignment::core::IAlignmentContainer* createUnapAlignmentResult (size_t querySize) = 0;

    /** Create a IAlignmentResult instance for holding generated gap alignments (which are of interest for the end user).
     * \param[in] subject : the subject database
     * \param[in] query   : the query database
     * \return a new IAlignmentResult instance
     */
    virtual alignment::core::IAlignmentContainer* createGapAlignmentResult  () = 0;

    /** Create a visitor for the gap alignments (likely a visitor that dump the alignments into a file).
     * \return a new AlignmentResultVisitor instance
     */
    virtual alignment::core::IAlignmentContainerVisitor* createResultVisitor () = 0;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_CONFIG_HPP_ */
