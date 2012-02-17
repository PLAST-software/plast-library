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

/** \file FilterContainerVisitor.hpp
 *  \brief A few implementations of IAlignmentResultVisitor interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _FILTER_ALIGNMENT_CONTAINER_VISITOR_HPP_
#define _FILTER_ALIGNMENT_CONTAINER_VISITOR_HPP_

/********************************************************************************/

#include <alignment/filter/api/IAlignmentFilter.hpp>
#include <alignment/visitors/impl/ModifierContainerVisitor.hpp>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/** \brief Visitor that fills a list holding all Alignment instances
 *
 * This visitor dumps alignments into a list
 */
class FilterContainerVisitor : public ModifierContainerVisitor
{
public:

    /** \copydoc IAlignmentResultVisitor::AbstractAlignmentResultVisitor */
    FilterContainerVisitor (filter::IAlignmentFilter* filter)
        : _filter (filter)  {}

    /** \copydoc IAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence   (const database::ISequence* seq) {}

    /** \copydoc IAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq) {}

    /** \copydoc IAlignmentResultVisitor::visitAlignment */
    void visitAlignment  (core::Alignment* align)  {}

    /** \copydoc IAlignmentResultVisitor::visitAlignmentsList */
    void visitAlignmentsList (
        const database::ISequence* qry,
        const database::ISequence* sbj,
        std::list<core::Alignment>& alignments
    );

private:
    filter::IAlignmentFilter* _filter;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _FILTER_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
