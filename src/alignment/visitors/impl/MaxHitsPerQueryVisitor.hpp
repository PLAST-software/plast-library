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

/** \file MaxHitsPerQueryVisitor.hpp
 *  \brief TO DO
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _MAX_HITS_PER_QUERY_VISITOR_HPP_
#define _MAX_HITS_PER_QUERY_VISITOR_HPP_

/********************************************************************************/

#include <alignment/visitors/impl/HierarchyAlignmentVisitor.hpp>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/** \brief Visitor that limits the number of visited alignments.
 *
 * The PLAST algorithm may find a lot of alignments, which may be difficult to use
 * for the end user.
 *
 * An simple idea is to limit the number of alignments seen by the user.
 * This specific visitor takes a IAlignmentResultVisitor instance as reference and
 * limits the number of visited alignments for a given query.
 *
 * This is a Proxy Design Pattern implementation.
 *
 * Code Sample:
 * \code
 * void sample (IAlignmentResult* alignments)
 * {
 *      // we create a visitor for dumping the alignments into a file in a tabulated format
 *      IAlignmentResultVisitor* output = new AlignmentResultOutputTabulatedVisitor ("/tmp/output");
 *
 *      // we create a visitor for limiting to 5 the number of alignments per query
 *      IAlignmentResultVisitor* limit = new MaxHitsPerQueryAlignmentResultVisitor (output, 5);
 *
 *      // we visit the alignments with our proxy visitor
 *      alignments->accept (limit);
 * }
 * \endcode
 */
class MaxHitsPerQueryVisitor : public HierarchyAlignmentResultVisitor
{
public:

    /** Constructor.
     * \param[in] ref : the visitor we want to limit the number of visited alignments.
     * \param[in] maxHitsPerQuery : max number of alignments per query
     */
    MaxHitsPerQueryVisitor (IAlignmentContainerVisitor* ref, u_int32_t maxHitsPerQuery)
        : _ref(0), _maxHitsPerQuery (maxHitsPerQuery), _currentHitsNb(0)  {  setRef (ref);  }

    /** Destructor. */
    virtual ~MaxHitsPerQueryVisitor()   { setRef (0); }

    /** \copydoc IAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence   (const database::ISequence* seq);

    /** \copydoc IAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq);

    /** \copydoc IAlignmentResultVisitor::visitAlignment */
    void visitAlignment (core::Alignment* align);

    /** \copydoc IAlignmentResultVisitor::finish */
    void postVisit (core::IAlignmentContainer* result)  {}

protected:

    /** Proxied visitor. */
    IAlignmentContainerVisitor* _ref;

    /** Smart setter for the _ref attribute. */
    void setRef (IAlignmentContainerVisitor* ref)  { SP_SETATTR(ref); }

    u_int32_t _maxHitsPerQuery;
    u_int32_t _currentHitsNb;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _MAX_HITS_PER_QUERY_VISITOR_HPP_ */
