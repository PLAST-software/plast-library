/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
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

	enum StrandId_e  { PLUS=1, MINUS=-1 };

    /** */
	ReverseStrandVisitor (core::IAlignmentContainerVisitor* ref, core::Alignment::DbKind kind, StrandId_e strand);

    /** \copydoc IAlignmentResultVisitor::visitAlignment */
    void visitAlignment (core::Alignment* align, const misc::ProgressInfo& progress);

    /** */
    void reverse ()  {  _strand = _strand == PLUS ? MINUS : PLUS;  }

protected:
    core::Alignment::DbKind _kind;

    StrandId_e _strand;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _REVERSE_STRAND_VISITOR_HPP_ */
