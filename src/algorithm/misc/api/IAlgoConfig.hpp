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

/** This interface is an Abstract Factory (see [GOF94]) that creates many kind of objects.
 *  The types of the created instances may depend on different criteria:
 *      - user preferences (through command line options, configuration file, ...)
 *      - kind of the algorithm (plastp, tplastn, plastx...)
 *      - the environment (memory, cores, network, ...)
 */
class IConfiguration : public dp::SmartPointer
{
public:

    /** Create default parameters for the given algorithm name. It is the initial entry point for
     *  creating parameters; once created, it is possible to modify some attributes according to
     *  user preferences for instance.
     */
    virtual IParameters* createDefaultParameters (const std::string& algoName) = 0;

    /** Create a command dispatcher instance. Such an instance can be used for parallelization (hits
     * iteration for instance, see IAlgorithm class).
     */
    virtual dp::ICommandDispatcher* createDispatcher () = 0;

    /** Create a database object (with means for retrieving sequence within the database) from an uri (likely
     *  a local file, but it should be a location on a remote computer). A Range can be provided for using only
     *  a part of the database.
     */
    virtual database::ISequenceDatabase*  createDatabase (const std::string& uri, const Range& range, bool filtering) = 0;

    /** Create statistical information for the query database that will be used by the algorithm for getting cutoffs.
     */
    virtual statistics::IQueryInformation* createQueryInformation (
        statistics::IGlobalParameters*  globalStats,
        algo::IParameters*              parameters,
        database::ISequenceDatabase*    queryDb,
        size_t                          subjectSize,
        size_t                          subjectNbSequences
    ) = 0;

    /** Create global statistical information used by the algorithm.
     */
    virtual statistics::IGlobalParameters*  createGlobalParameters (algo::IParameters* params) = 0;

    /** Create a seeds model used by the algorithm.
     */
    virtual seed::ISeedModel* createSeedModel (SeedModelKind_e modelKind, const std::vector<std::string>& subseedStrings) = 0;

    /** Create an indexator instance, ie. something that can index a database given some seeds model (and other params).
     *  Its main feature is to create a source Hit iterator from a seeds model and the subject and query databases. This source
     *  iterator can then be linked to other Hit iterators (ungap, small gap...) for filtering out Hits.
     */
    virtual algo::IIndexator*  createIndexator (seed::ISeedModel* seedsModel, algo::IParameters* params) = 0;

    /** Create a score matrix (BLOSUM50, BLOSUM62...) */
    virtual IScoreMatrix* createScoreMatrix (ScoreMatrixKind_e kind, database::Encoding encoding) = 0;

    /** Create a Hit iterator used by the algorithm. */
    virtual indexation::IHitIterator* createUngapHitIterator (
        indexation::IHitIterator*   source,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         matrix,
        algo::IParameters*          params,
        algo::IAlignmentResult*     ungapResult
    ) = 0;

    /** Create a Hit iterator used by the algorithm. */
    virtual indexation::IHitIterator* createSmallGapHitIterator (
        indexation::IHitIterator*   source,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         matrix,
        algo::IParameters*          params,
        algo::IAlignmentResult*     ungapResult
    ) = 0;

    /** Create a Hit iterator used by the algorithm. */
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

    /** Create a Hit iterator used by the algorithm. */
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

    /** Create a IAlignmentResult instance for holding generated ungap alignments. */
    virtual IAlignmentResult* createUnapAlignmentResult (size_t querySize) = 0;

    /** Create a IAlignmentResult instance for holding generated gap alignments (of interest for the end user). */
    virtual IAlignmentResult* createGapAlignmentResult  (database::ISequenceDatabase* subject, database::ISequenceDatabase* query) = 0;

    /** Create a visitor for the gap alignments (likely a visitor that dump the alignments into a file). */
    virtual AlignmentResultVisitor* createResultVisitor () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_CONFIG_HPP_ */
