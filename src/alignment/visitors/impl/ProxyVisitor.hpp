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

/** \file ProxyVisitor.hpp
 *  \brief A few implementations of IAlignmentResultVisitor interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _PROXY_ALIGNMENT_CONTAINER_VISITOR_HPP_
#define _PROXY_ALIGNMENT_CONTAINER_VISITOR_HPP_

/********************************************************************************/

#include <alignment/core/api/IAlignmentContainerVisitor.hpp>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/** \brief Visitor proxy
 *
 */
class AlignmentsProxyVisitor : public core::IAlignmentContainerVisitor
{
public:

    /** */
    AlignmentsProxyVisitor (core::IAlignmentContainerVisitor* ref) : _ref(0)  {  setRef (ref);  }

    ~AlignmentsProxyVisitor ()   { setRef (0); }

    /** \copydoc IAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence (const database::ISequence* seq, const misc::ProgressInfo& progress)
    { _ref->visitQuerySequence (seq, progress);  }

    /** \copydoc IAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq, const misc::ProgressInfo& progress)
    { _ref->visitSubjectSequence (seq, progress);  }

    /** \copydoc IAlignmentResultVisitor::visitAlignmentsList */
    void visitAlignmentsList (
        const database::ISequence* qry,
        const database::ISequence* sbj,
        std::list<core::Alignment>& alignments
    )
    {
        misc::ProgressInfo progress (1, alignments.size());
        for (std::list<core::Alignment>::iterator it = alignments.begin(); it != alignments.end(); it++, ++progress)
        {
            this->visitAlignment (& (*it), progress);
        }

        _ref->visitAlignmentsList (qry, sbj, alignments);
    }

    /** \copydoc IAlignmentResultVisitor::postVisit */
    void postVisit (core::IAlignmentContainer* result)  { _ref->postVisit(result); }

    /** \copydoc IAlignmentResultVisitor::finalize */
    void finalize (void)  {  _ref->finalize();  }

    /** \copydoc IAlignmentResultVisitor::getPosition */
    u_int64_t getPosition ()  { return _ref->getPosition (); }

protected:
    IAlignmentContainerVisitor* _ref;
    void setRef (IAlignmentContainerVisitor* ref)  { SP_SETATTR(ref); }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _PROXY_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
