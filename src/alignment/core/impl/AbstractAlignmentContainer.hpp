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

/** \file AbstractAlignmentContainer.hpp
 *  \brief Abstract implementation of IAlignmentContainer interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ABSTRACT_ALIGNMENT_CONTAINER_HPP_
#define _ABSTRACT_ALIGNMENT_CONTAINER_HPP_

/********************************************************************************/

#include <os/api/IThread.hpp>

#include <alignment/core/api/IAlignmentContainer.hpp>

/********************************************************************************/
namespace alignment {
namespace core      {
namespace impl      {
/********************************************************************************/

/** \brief Abstract implementation of IAlignmentResult interface.
 *
 * This implementation implements a few methods of the IAlignmentResult interface.
 *
 * It provides some synchronization scheme for protecting the instances from
 * concurrent accesses (which can occur since an instance may be shared by different
 * IHitIterator instances, each one iterated in different threads).
 *
 * It is still abstract since methods like insert() and shrink() still have to be
 * implemented by subclasses.
 */
class AbstractAlignmentContainer : public IAlignmentContainer
{
public:

    /** Constructor. */
    AbstractAlignmentContainer ();

    /** Destructor. */
    virtual ~AbstractAlignmentContainer ();

    /** \copydoc IAlignmentContainer::getProperties */
    dp::IProperties* getProperties (const std::string& root);

    /** \copydoc IAlignmentContainer::getAlignmentsNumber */
    u_int32_t getAlignmentsNumber ()  { return _nbAlignments; }

    /** Ugly... */
    void setSize (u_int32_t s)  { _nbAlignments = s; }

    /** \copydoc IAlignmentContainer::insertFirstLevel */
    bool insertFirstLevel (const database::ISequence* sequence)  { return false; }

    /** \copydoc IAlignmentResult::doesFirstLevelExists */
    bool doesFirstLevelExists (const database::ISequence* sequence)  { return false; }

    /** \copydoc IAlignmentContainer::getFirstLevelNumber */
    u_int32_t getFirstLevelNumber () { return 0; }

    /** \copydoc IAlignmentContainer::getSecondLevelNumber */
    u_int32_t getSecondLevelNumber () { return 0; }

    /** \copydoc IAlignmentContainer::insert(const misc::Range64&,const misc::Range64&,u_int32_t) */
    bool insert (const misc::Range64& qry, const misc::Range64& sbj, u_int32_t qryIndex)  { return false; }

    /** \copydoc IAlignmentContainer::merge */
    void merge (const std::vector<IAlignmentContainer*> containers) {  }

    /** \copydoc IAlignmentResult::shrink */
    void shrink () {}

protected:

    /** Synchronizer for preventing for concurrent accesses. */
    os::ISynchronizer* _synchro;

    /** Number of alignments. */
    u_int32_t _nbAlignments;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_ALIGNMENT_CONTAINER_HPP_ */
