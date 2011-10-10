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

#ifndef _OBSERVER_H_
#define _OBSERVER_H_

#include "IObserver.hpp"
#include <list>

/********************************************************************************/
namespace dp {
/********************************************************************************/

/** Class that notifies potential observers. */
class Subject : public ISubject
{
public:
    Subject ();
    Subject (const InterfaceId& interface);
    virtual ~Subject();

    InterfaceId getInterface ()  { return _interface; }

    /** */
    void addObserver    (IObserver* observer);
    void removeObserver (IObserver* observer);

    /** */
    void notify (EventInfo* event);

private:
    InterfaceId             _interface;
    IObserver*              _singleObserver;
    std::list<IObserver*>*  _observers;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _OBSERVER_H_ */
