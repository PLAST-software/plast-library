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

/** \file HierarchyAlignmentContainerVisitor.hpp
 *  \brief TO DO
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _HIERARCHY_ALIGNMENT_CONTAINER_VISITOR_HPP_
#define _HIERARCHY_ALIGNMENT_CONTAINER_VISITOR_HPP_

/********************************************************************************/

#include <alignment/core/api/IAlignmentContainerVisitor.hpp>
#include <list>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/** \brief TO DO
 */
class HierarchyAlignmentResultVisitor : public core::IAlignmentContainerVisitor
{
public:

    /** \copydoc IAlignmentResultVisitor::visitAlignmentsList */
    void visitAlignmentsList (
        const database::ISequence* qry,
        const database::ISequence* sbj,
        std::list<core::Alignment>& alignments
    )
    {
        misc::ProgressInfo progress (1, alignments.size ());

        /** The default implementation just visit each alignment of the given list. */
        for (std::list<core::Alignment>::iterator it = alignments.begin(); it != alignments.end(); it++, ++progress)
        {
            this->visitAlignment (& (*it), progress);
        }
    }

    /** \copydoc IAlignmentResultVisitor::finalize */
    void finalize (void)  { }

    /** \copydoc IAlignmentResultVisitor::getPosition */
    u_int64_t getPosition ()  { return 0; }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _HIERARCHY_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
