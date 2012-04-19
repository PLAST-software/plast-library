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

/** \file AdapterAlignmentContainerVisitor.hpp
 *  \brief TO DO
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ADAPTER_ALIGNMENT_CONTAINER_VISITOR_HPP_
#define _ADAPTER_ALIGNMENT_CONTAINER_VISITOR_HPP_

/********************************************************************************/

#include <alignment/core/api/IAlignmentContainerVisitor.hpp>
#include <list>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/** \brief Stub all methods.
 */
class AdapterAlignmentResultVisitor : public core::IAlignmentContainerVisitor
{
public:

    void visitQuerySequence   (const database::ISequence* seq,   const misc::ProgressInfo& progress)  {}
    void visitSubjectSequence (const database::ISequence* seq,   const misc::ProgressInfo& progress)  {}
    void visitAlignment       (core::Alignment*           align, const misc::ProgressInfo& progress)  {}

    void postVisit  (core::IAlignmentContainer* result)       {}

    void visitAlignmentsList (
        const database::ISequence* qry,
        const database::ISequence* sbj,
        std::list<core::Alignment>& alignments
    ) {}

    void finalize (void)  {}

    u_int64_t getPosition ()  { return 0; }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ADAPTER_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
