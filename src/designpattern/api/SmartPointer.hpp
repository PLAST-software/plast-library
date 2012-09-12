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

/** \file SmartPointer.hpp
*   \date 07/11/2011
*   \author edrezen
 *  \brief Smart Pointer Design Pattern definition
*
* Define tools for easing life cycle of objects. Also known as smart pointer.
*/

#ifndef SMART_POINTER_HPP_
#define SMART_POINTER_HPP_

/********************************************************************************/
/** \brief Design Pattern tools (Observer, SmartPointer, Command...) */
namespace dp {
/********************************************************************************/

/** \brief Tool for managing instances life cycle
 *
 *  The goal of this class is to share easily objects between clients.
 *
 *  This class has an integer attribute that acts as a reference counter, i.e. a
 *  token counting how many clients are interested by the instance.
 *
 *  This is useful for sharing instances; if a client is interested by using an
 *  instance 'obj', she/he may call 'obj->use()' which will increase the internal
 *  token number. When the client is no more interested by using the instance 'obj',
 *  she/he may call 'obj->forget()', which will decrease the internal token.
 *
 *  When the token becomes 0, the instance is automatically destroyed.
 *
 *  Note that use() and forget() are virtual; it may happen (for singleton management
 *  for instance) that some subclass needs to refine them.
 *
 *  This pattern is often known as Smart Pointer.
 */
class SmartPointer
{
public:
    /** Use an instance by taking a token on it */
    virtual void use    ()
    {
        __sync_fetch_and_add (&_counterRef, 1 );
        // DON'T DO _counterRef++  because possible real time issues...
    }

    /** Forget an instance by releasing a token on it */
    virtual void forget ()
    {
        __sync_fetch_and_sub (&_counterRef, 1 );
        // DON'T DO _counterRef--  because possible real time issues...

        if (_counterRef<=0)  { delete this; }
    }

protected:

    /** Constructor. */
    SmartPointer () : _counterRef(0)  {}

    /** Destruction of the instance will be automatically called when the reference counter becomes null.
     *  The destructor is private in order to avoid to directly delete the instance without using the
     *  'use/forget' mechanism.
     */
    virtual ~SmartPointer ()  {}

private:
    /** Counts the number of references of the instance.*/
    int _counterRef;
};

/********************************************************************************/

/** \brief Local usage of SmartPointer instance.
 *
 * Small utility for locally getting a reference on a smart pointer. It creates
 *  a local (ie created in the execution stack) object that takes a reference on
 *  a smart pointer and get rid of it when the execution is out of the statements
 *  block holding the local object.
 *
 *  Sample:
 *  \code
 *  void foo ()
 *  {
 *     {
 *        // we create an instance of a class that inherits from SmartPointer
 *        MyClass* object = new MyClass ();
 *
 *        // we want that this object lives only inside the englobing statements block.
 *        // note that we use the LOCAL macro
 *        LOCAL (object);
 *     }
 *
 *     // Here, the object should have been automtically deleted.
 *  }
 *  \endcode
 *
 *  \see LOCAL
 */
class LocalObject
{
public:
    /** Constructor.
     * \param[in] ptr : the instance we want locally manage.
     */
    LocalObject (SmartPointer* ptr) : _ptr(ptr)  { if (_ptr)  {  _ptr->use();  } }

    /** Destructor. */
    ~LocalObject () { if (_ptr)  {  _ptr->forget ();  } }

private:
    /** The SmartPointer instance we want local life cycle management. */
    SmartPointer* _ptr;
};

/********************************************************************************/

/** Macro that creates an instance of type LocalObject whose name is the prefix '__' followed by the provided argument.
 */
#define LOCAL(object)  dp::LocalObject __##object (object)

/** Macro that generates an instructions block that manages life cycle of a class attributes.
 * Dedicated for simply write clever setter methods.
 */
#define SP_SETATTR(a)  \
{  \
    if (_##a == a)  { return;  }   /* just to be sure that we don't reuse the same object */  \
    if (_##a != 0)  {  _##a->forget (); } \
    _##a = a; \
    if (_##a != 0)  {  _##a->use    (); } \
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* SMART_POINTER_HPP_ */
