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

#ifndef _IOBSERVER_H_
#define _IOBSERVER_H_

#include "SmartPointer.hpp"
#include <string>
#include "types.hpp"

/********************************************************************************/
namespace dp {
/********************************************************************************/

/** Forward declarations. */
class ISubject;

/********************************************************************************/

/** We define a type for identifying an interface. */
typedef u_int32_t InterfaceId;

/********************************************************************************/

/** */
class EventInfo : public SmartPointer
{
public:
    EventInfo (const InterfaceId& interface) : _interface(interface) {}
    virtual ~EventInfo() {}

    InterfaceId getInterface ()  const { return _interface; }

protected:
    InterfaceId _interface;
};

/********************************************************************************/

/** Interface of the Observer (see Design Pattern Observer).
 *  Merely defines an 'update' method that can be called by some subjects.
 */
class IObserver
{
public:
	/** Method called by Subject for some notification. */
	virtual void update (EventInfo* evt, ISubject* subject) = 0;
};

/********************************************************************************/

/** Class that notifies potential observers for some interface identifier. */
class ISubject
{
public:
    virtual InterfaceId getInterface () = 0;

    virtual void addObserver    (IObserver* observer) = 0;
    virtual void removeObserver (IObserver* observer) = 0;
    virtual void notify (EventInfo* event) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IOBSERVER_H_ */
