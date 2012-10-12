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

/** \file AlgorithmPlastn.hpp
 *  \brief Implementation of the IAlgorithm interface for plastn
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALGORITHM_PLASTN_HPP_
#define _ALGORITHM_PLASTN_HPP_

/********************************************************************************/

#include <algo/core/impl/AbstractAlgorithm.hpp>
#include <alignment/core/api/IHspContainer.hpp>
#include <designpattern/impl/RangeIterator.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

/** \brief Implementation of the plastn algorithm (ADN/ADN)
 *
 * The plastn algorithm inherits from the AbstractAlgorithm class and specifies
 * what are the reading frames to be used for the subject database.
 */
class AlgorithmPlastn : public AbstractAlgorithm
{
public:

    /** \copydoc AbstractAlgorithm */
    AlgorithmPlastn (
        IConfiguration*                                 config,
        database::IDatabaseQuickReader*                 reader,
        IParameters*                                    params,
        alignment::filter::IAlignmentFilter*            filter,
        alignment::core::IAlignmentContainerVisitor*    resultVisitor,
        algo::core::IDatabasesProvider*                 dbProvider,
        statistics::IGlobalParameters*                  statistics,
        os::impl::TimeInfo*                             timeStats,
        bool&                                           isRunning,
        int                                             actualStrand
    );

    /** */
    ~AlgorithmPlastn ();

protected:

    /** */
    void computeAlignments (
        alignment::core::IAlignmentContainer*   alignmentResult,
        alignment::core::IAlignmentContainer*   ungapAlignmentResult,
        database::ISequenceDatabase*            subjectDb,
        database::ISequenceDatabase*            queryDb,
        dp::ICommandDispatcher*                 dispatcher,
        os::impl::TimeInfo*                     timeStats
    );

    /** */
    void finalizeAlignments (alignment::core::IAlignmentContainer* alignmentResult, os::impl::TimeInfo* timeStats);

    /** */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

    /** */
    alignment::core::IHspContainer* pass0 (
		database::ISequenceDatabase*        subjectDb,
		database::ISequenceDatabase*        queryDb,
		dp::ICommandDispatcher*             dispatcher,
		dp::impl::RangeIterator<u_int32_t>& rangeIterator
    );

    /** */
    alignment::core::IHspContainer* pass1 (
		database::ISequenceDatabase*        subjectDb,
		database::ISequenceDatabase*        queryDb,
		dp::ICommandDispatcher*             dispatcher,
		alignment::core::IHspContainer*		sourceHsp,
		int									xdrop
    );

    /** */
    void pass2 (
		database::ISequenceDatabase*        	subjectDb,
		database::ISequenceDatabase*        	queryDb,
		dp::ICommandDispatcher*             	dispatcher,
		alignment::core::IHspContainer*			sourceHsp,
		alignment::core::IAlignmentContainer*   alignContainer
    );

    alignment::core::IHspContainer* _hspContainer;
    void setHspContainer (alignment::core::IHspContainer* hspContainer)  { SP_SETATTR (hspContainer); }

    std::vector<u_int32_t> timesVec;

    size_t _nbPasses;
    size_t _currentPass;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALGORITHM_PLASTN_HPP_ */
