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

/** \file IAlignmentContainer.hpp
 *  \brief Interface for some alignments container.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALIGNMENT_CONTAINER_HPP_
#define _IALIGNMENT_CONTAINER_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/api/IProperty.hpp>

#include <index/api/IOccurrenceIterator.hpp>

#include <alignment/core/api/Alignment.hpp>
#include <alignment/core/api/IAlignmentContainerVisitor.hpp>

#include <string>
#include <vector>

/** Forward declarations. */

/********************************************************************************/
namespace alignment {
namespace core      {
/********************************************************************************/

/** \brief Interface for managing Alignment instances
 *
 * This interface has two main goals:
 *      - providing means for the algorithm for checking existence of an alignment and inserting new ones
 *      - providing means for iterating alignments through the IAlignmentResultVisitor interface
 *
 * Some other auxiliaries methods are available, like shrinking the alignments list (in case of redundant
 * alignments) or reading some alignments from a file.
 *
 * The PLAST algorithm will initially creates IAlignmentResult instances that are filled by the algorithm
 * during its execution when some significant similarities are found between the subject and query
 * databases. For doing so, the PLAST algorithm will mainly use doesExist() and insert() methods.
 * Once the algorithm is finished, it is possible for the end user to iterate the found alignments
 * by giving a IAlignmentResultVisitor instance to the accept() method.
 *
 * Note that the interface makes no difference between ungap and gap alignments. Actually, it is used
 * for both cases (see for instance FullGapHitIterator class).
 */
class IAlignmentContainer : public dp::SmartPointer
{
public:

    /** Tells whether or not an alignment is already known.
     * \param[in] subjectOccur : occurrence in the subject database
     * \param[in] queryOccur   : occurrence in the query database
     * \param[in] bandSize     : if not 0, size to be considered to the right and to the left
     * \return true if already existing, false otherwise
     */
    virtual bool doesExist (
        const indexation::ISeedOccurrence* subjectOccur,
        const indexation::ISeedOccurrence* queryOccur,
        u_int32_t bandSize
    ) = 0;

    /** Tells whether or not an alignment is already known.
     * \param[in] align : the alignment to be checked
     * \return true if already existing, false otherwise
     */
    virtual bool doesExist (const Alignment& align) = 0;

    /** Insert a sequence at first containment level .
     * \param[in] sequence : sequence to be inserted
     * \return true if insertion ok, false otherwise. */
    virtual bool insertFirstLevel (const database::ISequence* sequence) = 0;

    /** Give the number of item at first level.
     * \return the number of items
     */
    virtual u_int32_t getFirstLevelNumber () = 0;

    /** Insert an alignment.
     * \param[in] align   : alignment to be inserted
     * \param[in] context : some context for customizing the insertion
     * \return true if insertion ok, false otherwise. */
    virtual bool insert (Alignment& align, void* context) = 0;

    /** Insert an alignment.
     * \return true if insertion ok, false otherwise. */
    virtual bool insert (const misc::Range64& qry, const misc::Range64& sbj, u_int32_t qryIndex) = 0;

    /** Give the number of known alignments.
     * \return the number of alignments
     */
    virtual u_int32_t getAlignmentsNumber () = 0;

    /** Get the list of alignments for a given [query,subject] pair
     * \return the alignments list
     */
    virtual std::list<Alignment>* getContainer (
        const database::ISequence* seqLevel1,
        const database::ISequence* seqLevel2
    ) = 0;

    /** Accept method of the Visitor Design Pattern.
     * \param[in] visitor : the visitor to be accepted
     */
    virtual void accept (IAlignmentContainerVisitor* visitor) {}

    /** Return properties about the instance.
     * \param[in] root : root string for the properties
     * \return a new IProperties instance
     */
    virtual dp::IProperties* getProperties (const std::string& root) = 0;
};

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _IALIGNMENT_CONTAINER_HPP_ */
