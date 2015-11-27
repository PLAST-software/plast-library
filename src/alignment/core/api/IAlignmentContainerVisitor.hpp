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

/** \file IAlignmentContainerVisitor.hpp
 *  \brief Interface for some alignments container.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALIGNMENT_CONTAINER_VISITOR_HPP_
#define _IALIGNMENT_CONTAINER_VISITOR_HPP_

/********************************************************************************/

#include <designpattern/impl/Observer.hpp>

#include <alignment/core/api/Alignment.hpp>

#include <string>
#include <vector>

/********************************************************************************/
namespace alignment {
namespace core      {
/********************************************************************************/

/* Forward declarations. */
class IAlignmentContainer;

/********************************************************************************/

/** \brief Visitor Design Pattern that can visit IAlignmentContainer instances.
 *
 * IAlignmentContainer instances manage some set of Alignment instances.
 *
 * A mean for iterating these alignments would have to add a factory method in the
 * IAlignmentContainer interface creating iterators on the contained Alignment instances.
 *
 * By doing so, we would have lost some hierarchical information. For instance, it is
 * interesting to sort the alignments per query, and then to sort them per subject.
 *
 * Using a visitor approach allows to keep this structural information. In our visitor
 * definition here, we have several methods:
 *      - 1. visitQuerySequence()   : called each time a new query sequence is found in the whole alignments set
 *      - 2. visitSubjectSequence() : called each time a new subject sequence is found in the whole alignments set
 *      - 3. visitAlignmentsList()  : called for each found subject sequence
 *      - 4. visitAlignment()       : called each time a new alignment is found for the currently visited query/subject sequences.
 *
 * For instance, we could have a series of calls like this: 1,2,3,3,2,3,3,3,1,2,3,3,3,3 which means that we found alignments
 * in two different query sequences, with two subject sequences matches for the first one, and only one for the other.
 *
 * Implementors of this interface therefore can use this hierarchy for structuring themselves what they want; this is useful
 * for instance for dumping the alignment in a XML file.
 */
class IAlignmentContainerVisitor : public dp::SmartPointer, public dp::impl::Subject
{
public:

    /** Called when a new query sequence is visited.
     * \param[in] seq : the visited query sequence.
     * \param[in] progress : progress information about the visit
     */
    virtual void visitQuerySequence   (
        const database::ISequence*  seq,
        const misc::ProgressInfo&   progress
    ) = 0;

    /** Called when a new subject sequence is visited for the currently visited query sequence
     * \param[in] seq : the visited subject sequence.
     * \param[in] progress : progress information about the visit
     */
    virtual void visitSubjectSequence (
        const database::ISequence*  seq,
        const misc::ProgressInfo&   progress
    ) = 0;

    /** Called for a list of alignments for the currently visited [query,subject] pair.
     * \param[in] qry        : the visited query sequence.
     * \param[in] sbj        : the visited subject sequence.
     * \param[in] alignments : the visited alignments list.
     */
    virtual void visitAlignmentsList (
        const database::ISequence* qry,
        const database::ISequence* sbj,
        std::list<Alignment>& alignments
    ) = 0;

    /** Called when a new alignment is visited for the currently visited query and subject sequences.
     * \param[in] align : the visited alignment.
     * \param[in] progress : progress information about the visit
     */
    virtual void visitAlignment (
        Alignment*                  align,
        const misc::ProgressInfo&   progress
    ) = 0;

    /** Called at the end of the 'accept' method.
     * \param[in] result : the container being visited
     */
    virtual void postVisit (IAlignmentContainer* result) = 0;

    /** Method to be called for finalizing all (after potential 'accept' calls). */
    virtual void finalize (void) = 0;

    /** Returns the position during the visit. Semantics may depend on the implementation
     *  (which is uggly and should be improved).
     */
    virtual u_int64_t getPosition () = 0;
};

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _IALIGNMENT_CONTAINER_VISITOR_HPP_ */
