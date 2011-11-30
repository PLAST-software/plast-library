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

/** \file AbstractAlignmentResult.hpp
 *  \brief Abstract implementation of IAlignmentResult interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ABSTRACT_ALIGNMENT_RESULT_HPP_
#define _ABSTRACT_ALIGNMENT_RESULT_HPP_

/********************************************************************************/

#include <os/api/IThread.hpp>

#include <algo/align/api/IAlignmentResult.hpp>

/********************************************************************************/
namespace algo   {
namespace align {
/** \brief Implementation of concepts about alignments with gaps. */
namespace impl   {
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
class AbstractAlignmentResult : public IAlignmentResult
{
public:

    /** Default constructor. */
    AbstractAlignmentResult ();

    /** Destructor. */
    virtual ~AbstractAlignmentResult ();

    /** \copydoc IAlignmentResult::getProperties */
    dp::IProperties* getProperties (const std::string& root);

    /** \copydoc IAlignmentResult::readFromFile */
    void readFromFile (
        const char* fileuri,
        std::map<std::string,int>& subjectComments,
        std::map<std::string,int>& queryComments
    );

    /** Tells whether or not an alignment is already known.
     * \param[in] align : the alignment to be checked
     * \return always false
     */
    bool doesExist (const Alignment& align) { return false; }

    /** \copydoc IAlignmentResult::getAlignments
     * Default implementation returns an emtpy list.
     */
    std::list<Alignment> getAlignments (const database::ISequence* querySeq, const database::ISequence* subjectSeq)
    {
        std::list<Alignment> result;
        return result;
    }

protected:

    /** Synchronizer for preventing for concurrent accesses. */
    os::ISynchronizer* _synchro;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_ALIGNMENT_RESULT_HPP_ */
