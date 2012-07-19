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

/** \file ReverseStrandVisitor.hpp
 *  \brief A few implementations of IAlignmentResultVisitor interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _REVERSE_STRAND_VISITOR_HPP_
#define _REVERSE_STRAND_VISITOR_HPP_

/********************************************************************************/

#include <alignment/visitors/impl/ProxyVisitor.hpp>

#include <map>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/** \brief Visitor that fills a list holding all Alignment instances
 *
 * This visitor dumps alignments into a list
 */
class ReverseStrandVisitor : public AlignmentsProxyVisitor
{
public:

    /** */
	ReverseStrandVisitor (core::IAlignmentContainerVisitor* ref, core::Alignment::DbKind kind);

    /** \copydoc IAlignmentResultVisitor::visitAlignment */
    void visitAlignment (core::Alignment* align, const misc::ProgressInfo& progress);

protected:
    core::Alignment::DbKind _kind;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _REVERSE_STRAND_VISITOR_HPP_ */
