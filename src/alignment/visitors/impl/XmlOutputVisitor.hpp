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

/** \file XmlOutputVisitor.hpp
 *  \brief A few implementations of IAlignmentResultVisitor interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _XML_OUTPUT_ALIGNMENT_CONTAINER_VISITOR_HPP_
#define _XML_OUTPUT_ALIGNMENT_CONTAINER_VISITOR_HPP_

/********************************************************************************/

#include <alignment/visitors/impl/OstreamVisitor.hpp>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/** \brief Alignments file dump in XML format
 *
 * This visitor dumps alignments into a file in XML format.
 */
class XmlOutputVisitor : public OstreamVisitor
{
public:

    /** \copydoc AbstractAlignmentResultVisitor::AbstractAlignmentResultVisitor */
    XmlOutputVisitor (std::ostream* ostream);

    /** */
    XmlOutputVisitor (const std::string& uri);

    /** Destructor. */
    virtual ~XmlOutputVisitor ();

    /** \copydoc AbstractAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence   (const database::ISequence* seq, const misc::ProgressInfo& progress);

    /** \copydoc AbstractAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq, const misc::ProgressInfo& progress);

    /** \copydoc AbstractAlignmentResultVisitor::visitAlignment */
    void visitAlignment (core::Alignment* align, const misc::ProgressInfo& progress);

private:
    void printline (size_t depth, const char* format, ...);

    const database::ISequence* _currentQuery;
    const database::ISequence* _currentSubject;

    u_int32_t _nbQuery;
    u_int32_t _nbSubject;
    u_int32_t _nbAlign;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _XML_OUTPUT_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
