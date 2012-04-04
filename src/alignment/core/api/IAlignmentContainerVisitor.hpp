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

/** \file IAlignmentContainerVisitor.hpp
 *  \brief Interface for some alignments container.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALIGNMENT_CONTAINER_VISITOR_HPP_
#define _IALIGNMENT_CONTAINER_VISITOR_HPP_

/********************************************************************************/

#include <alignment/core/api/Alignment.hpp>

#include <string>
#include <vector>

/********************************************************************************/
namespace alignment {
namespace core      {
/********************************************************************************/

/** Forward declarations. */
class IAlignmentContainer;

/********************************************************************************/

/** \brief Visitor Design Pattern that can visit IAlignmentResult instances.
 *
 * IAlignmentResult instances manage some set of Alignment instances.
 *
 * A mean for iterating these alignments would have to add a factory method in the
 * IAlignmentResult interface creating iterators on the contained Alignment instances.
 *
 * By doing so, we would have lost some hierarchic information. For instance, it is
 * interesting to sort the alignments per query, and then to sort them per subject.
 *
 * Using a visitor approach allows to keep this structural information. In our visitor
 * definition here, we have three methods:
 *      - 1. visitQuerySequence()   : called each time a new query sequence is found in the whole alignments set
 *      - 2. visitSubjectSequence() : called each time a new subject sequence is found in the whole alignments set
 *      - 3. visitAlignment()       : called each time a new alignment is found for the currently visited query/subject sequences.
 *
 * For instance, we could have a series of calls like this: 1,2,3,3,2,3,3,3,1,2,3,3,3,3 which means that we found alignements
 * in two different query sequences, with two subject sequences matches for the first one, and only one for the other.
 *
 * Implementors of this interface therefore can use this hierarchy for structuring themself what they want; this is useful
 * for instance for dumping the alignment in a XML file.
 */
class IAlignmentContainerVisitor : public dp::SmartPointer
{
public:

    /** Called when a new query sequence is visited.
     * \param[in] seq : the visited query sequence.
     */
    virtual void visitQuerySequence   (
        const database::ISequence*  seq,
        const misc::ProgressInfo&   progress
    ) = 0;

    /** Called when a new subject sequence is visited for the currently visited query sequence
     * \param[in] seq : the visited subject sequence.
     */
    virtual void visitSubjectSequence (
        const database::ISequence*  seq,
        const misc::ProgressInfo&   progress
    ) = 0;

    /** Called for a list of alignments for the currently visited [query,subject] pair.
     * \param[in] align : the visited alignments list.
     */
    virtual void visitAlignmentsList (
        const database::ISequence* qry,
        const database::ISequence* sbj,
        std::list<Alignment>& alignments
    ) = 0;

    /** Called when a new alignment is visited for the currenlyt visited query and subject sequences.
     * \param[in] align : the visited alignment.
     */
    virtual void visitAlignment (
        Alignment*                  align,
        const misc::ProgressInfo&   progress
    ) = 0;

    /** Called at the end of the 'accept' method.
     */
    virtual void postVisit (IAlignmentContainer* result) = 0;
};

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _IALIGNMENT_CONTAINER_VISITOR_HPP_ */
