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

/** \file Observer.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementation of the ISubject interface.
 */

#ifndef _OBSERVER_H_
#define _OBSERVER_H_

#include <designpattern/api/IObserver.hpp>
#include <list>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Class that notifies potential observers.
 *
 * The main purpose of this class is to manage the set of IObservers attached to the subject.
 *
 * Then, classes that want subject-like behavior can inherit from Subject or have a Subject
 * attribute.
 *
 * \see ISubject
 */
class Subject : public ISubject
{
public:

    /** Constructor. */
    Subject ();

    /** Constructor.
     * \param[in] interface : the identifier of the subject. */
    Subject (const InterfaceId& interface);

    /** Destructor. */
    virtual ~Subject();

    /** Returns the identifier of the subject. */
    InterfaceId getInterface ()  { return _interface; }

    /** Attach an observer to this subject.
     * \param[in] observer : the observer to be attached.
     */
    void addObserver    (IObserver* observer);

    /** Detach an observer from this subject.
     * \param[in] observer : the observer to be detached.
     */
    void removeObserver (IObserver* observer);

    /** Notify observers by sending a EventInfo instance.
     * \param[in]  event : the information to be sent to the observers.
     */
    void notify (EventInfo* event);

private:

    /** Identifier of the subject. */
    InterfaceId             _interface;

    /** Optimization attribute. */
    IObserver*              _singleObserver;

    /** List of observers attached to the subject. */
    std::list<IObserver*>*  _observers;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _OBSERVER_H_ */
