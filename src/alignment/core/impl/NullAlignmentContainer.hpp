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

/** \file NullAlignmentContainer.hpp
 *  \brief Null implementation of IAlignmentResult interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _NULL_ALIGNMENT_CONTAINER_HPP_
#define _NULL_ALIGNMENT_CONTAINER_HPP_

/********************************************************************************/

#include <alignment/core/api/IAlignmentContainer.hpp>

/********************************************************************************/
namespace alignment {
namespace core      {
namespace impl      {
/********************************************************************************/

/** \brief Null implementation of IAlignmentResult interface.
 *
 * This implementation implements a null methods of the IAlignmentResult interface.
 *
 */
class NullAlignmentResult : public IAlignmentContainer
{
public:

    /** Tells whether or not an alignment is already known. Here, only un ungap alignment
     * is considered, defined by two ISeedOccurrence instances.
     * \param[in] subjectOccur : occurrence in the subject database
     * \param[in] queryOccur   : occurrence in the query database
     * \return true if already existing, false otherwise
     */
    bool doesExist (
        const indexation::ISeedOccurrence* subjectOccur,
        const indexation::ISeedOccurrence* queryOccur,
        u_int32_t bandSize
    ) { return false; }

    /** Tells whether or not an alignment is already known.
     * \param[in] align : the alignment to be checked
     * \return true if already existing, false otherwise
     */
    bool doesExist (const Alignment& align)  { return false; }

    /** \copydoc IAlignmentResult::insertFirstLevel */
    bool insertFirstLevel (const database::ISequence* sequence)  { return false; }

    /** \copydoc IAlignmentResult::insert */
    bool insert (const misc::Range64& qry, const misc::Range64& sbj, u_int32_t qryIndex)  { return false; }

    /** \copydoc IAlignmentResult::getFirstLevelNumber */
    u_int32_t getFirstLevelNumber () { return 0; }

    /** \copydoc IAlignmentResult::insert */
    bool insert (Alignment& align, void* context) { return false; }

    /** \copydoc IAlignmentResult::merge */
    void merge (const std::vector<IAlignmentContainer*> containers)  { }

    /** \copydoc IAlignmentResult::getSize */
    u_int32_t getAlignmentsNumber () { return 0; }

    /** \copydoc IAlignmentResult::getContainer */
    std::list<Alignment>* getContainer (
        const database::ISequence* seqLevel1,
        const database::ISequence* seqLevel2
    ) { return 0; }

    /** \copydoc IAlignmentResult::accept */
    void accept (IAlignmentContainerVisitor* visitor) {}

    /** \copydoc IAlignmentResult::getProperties */
    dp::IProperties* getProperties (const std::string& root) { return 0; }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _NULL_ALIGNMENT_CONTAINER_HPP_ */
