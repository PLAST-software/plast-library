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

/** \file EventCatch.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Tool for catching specifing notification information.
 */

#ifndef _EVENT_CATCH_HPP_
#define _EVENT_CATCH_HPP_

#include <designpattern/api/IObserver.hpp>
#include <list>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Tool for catching specifing notification information
 */
template <class Item> class EventCatch : public SmartPointer, public IObserver
{
public:

    /** Constructor.
     * \param[in] subject : subject from which we want to have notifications.
     */
    EventCatch (ISubject* subject) : _subject(subject)
    {
        if (_subject)  {  _subject->addObserver (this);  }
    }

    /** Destructor. */
    ~EventCatch ()
    {
        if (_subject)  {  _subject->removeObserver (this);  }

        for (std::list<EventInfo*>::iterator it = _list.begin(); it != _list.end(); it++)
        {
            (*it)->forget();
        }
    }

    /** Returns the list of caught EventInfo instances we received.
     * \return the list.
     */
    const std::list<EventInfo*>& getList () { return _list; }

    /** Tells whether we caught at least one EventInfo instance from the subject.
     * \return true if at least one event was caught, false otherwise
     */
    bool empty () { return getList().empty(); }

    /** Return the first caught event (if any)
     * \return the event.
     */
    Item  front ()  { return (Item) getList().front(); }

private:

    /** */
    void update (EventInfo* evt, ISubject* subject)
    {
        if (subject == _subject)
        {
            Item e = dynamic_cast<Item> (evt);

            if (e != 0)
            {
                /** We use the event and add it into the list. */
                evt->use();
                _list.push_back (evt);
            }
        }
    }

    ISubject*             _subject;
    std::list<EventInfo*> _list;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _EVENT_CATCH_HPP_ */
