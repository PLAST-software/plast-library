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

/** \file AbstractHitIterator.hpp
 *  \brief Abstract implementation of IHitIterator interface
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ABSTRACT_HITITERATOR_HPP_
#define _ABSTRACT_HITITERATOR_HPP_

/********************************************************************************/

#include <algo/hits/api/IHitIterator.hpp>

#include <stdio.h>

/********************************************************************************/
namespace algo   {
namespace hits   {
/** \brief Abstract implementations for IHitIterator interface. */
namespace common {
/********************************************************************************/

/** \brief Abstract implementation of IHitIterator interface
 *
 * This class implements a few methods of the IHitIterator interface and is used
 * as a base class for complete implementations.
 *
 * In particular, AbstractHitIterator manages:
 *      - a list of IHitIterator instance for splitting process
 *      - informative methods (getInputHitsNumber(), getOutputHitsNumber()) according to the split scheme
 *      - default implementation of the update() method
 *
 * The class is still abstract; complete implementations have to tell how to effectively
 * iterate hits (ie. have to implement the iterate() method).
 */
class AbstractHitIterator : public IHitIterator
{
public:

    /** Constructor.
     */
    AbstractHitIterator ()
        : _inputHitsNumber(0), _outputHitsNumber(0),
          _maxHitsPerIteration (1000*1000),
          _totalTime(0) {}

    /** Destructor. */
    virtual ~AbstractHitIterator ()
    {
        for (size_t i=0; i<_splitIterators.size(); i++)  {  _splitIterators[i]->forget(); }
    }

    /** \copydoc IHitIterator::getInputHitsNumber */
    u_int64_t getInputHitsNumber  ();

    /** \copydoc IHitIterator::getOutputHitsNumber */
    u_int64_t getOutputHitsNumber ();

    /** \copydoc IHitIterator::update
     * This implementation just forwards the notification.
     */
    void update (dp::EventInfo* evt, dp::ISubject* subject)
    {
        /** We forward the event to potential clients. */
        if (this != subject)  {   notify (evt);  }
    }

    /** \copydoc IHitIterator::getProperties */
    dp::IProperties* getProperties ();

protected:

    /** Number of incoming hits. */
    u_int64_t _inputHitsNumber;

    /** Number of outcoming hits. */
    u_int64_t _outputHitsNumber;

    /** Set referenced split instances (ensure use/forget and observer managements).
     * Should be used to link split instances to the instance from which they have
     * been split.
     * \param[in] split : the split instances to be linked to the current instance.
     */
    void setSplitIterators (const std::vector<IHitIterator*>& split);

    /** Set of split IHitIterator instances linked to the current instance. */
    std::vector<IHitIterator*> _splitIterators;

    /** Threshold for controlling memory usage. */
    u_int32_t _maxHitsPerIteration;

    /** Statistics. */
    u_int32_t _totalTime;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_HITITERATOR_HPP_ */
