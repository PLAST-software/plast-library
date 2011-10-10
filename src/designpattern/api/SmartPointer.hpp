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

/*****************************************************************************
 *   This file holds interfaces related to the Design Pattern Proxy.
 *   It provides a smart pointer mechanism for easing instances life cycle
 *   management.
 *
 *   It mainly provides for such objects a reference counter. When this
 *   counter becomes zero, the instance is deleted.
 *
 *   Clients that want a token on an instance should call the 'use' method
 *   on the instance and call the 'forget' to get rid of it.
 *****************************************************************************/

#ifndef SMART_POINTER_HPP_
#define SMART_POINTER_HPP_

/********************************************************************************/

/********************************************************************************/
namespace dp {
/********************************************************************************/

/** Define an abstraction for having a usage reference counter.
 */
class SmartPointer
{
public:
    SmartPointer () : _counterRef(0)  {}
    virtual ~SmartPointer ()  {}

    virtual void use    ()
    {
        __sync_fetch_and_add (&_counterRef, 1 );
        // DON'T DO _counterRef++  because possible real time issues...
    }

    virtual void forget ()
    {
        __sync_fetch_and_sub (&_counterRef, 1 );
        // DON'T DO _counterRef--  because possible real time issues...

        if (_counterRef<=0)  { delete this; }
    }

private:
    int _counterRef;
};

/********************************************************************************/

/** Small utility for locally getting a reference on a smart pointer. it creates
 *  a local (ie created in the execution stack) object that takes a reference on
 *  a smart pointer and get rid of it when the execution is out of the statements
 *  block holding the local object.
 */
class LocalObject
{
public:
    LocalObject (SmartPointer* ptr) : _ptr(ptr)  { if (_ptr)  {  _ptr->use();  } }
    ~LocalObject () { if (_ptr)  {  _ptr->forget ();  } }

private:
    SmartPointer* _ptr;
};

/********************************************************************************/

/** Macro for using the LocalObject class.
 */
#define LOCAL(object)  dp::LocalObject __##object (object)

/** */
#define SP_SETATTR(a)  \
{  \
    if (_##a != 0)  {  _##a->forget (); } \
    _##a = a; \
    if (_##a != 0)  {  _##a->use    (); } \
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* SMART_POINTER_HPP_ */
