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

/** \file IteratorGet.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementation of an Iterator as a unique "get" method
 */

#ifndef _ITERATOR_GET_HPP_
#define _ITERATOR_GET_HPP_

#include <designpattern/api/Iterator.hpp>
#include <designpattern/api/ICommand.hpp>

#include <os/impl/DefaultOsFactory.hpp>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Iterate items through a single method.
 *
 * The Iterator interface provides a mean to iterate items in the same way the Design Patterns book does.
 * With such an interface, clients use iteration loop where they control themselves the loop through
 * the first/isDone/next trinity.
 *
 * Another way to see the iteration loop is that the iterator provides at demand one item through some 'get' method.
 * We therefore define a IteratorGet class that merely defines a 'get' method that returns the current item and then
 * goes to the next one. As a matter of fact, it uses a referenced Iterator instance, provided at construction.
 *
 * One can see this way to proceed as a contraction of first/isDone/next/currentItem in a single atomic 'get' method,
 * 'first' being done (only once) at first call to 'get'.
 *
 * The 'get' method has the following properties:
 *   - return false if the iteration is finished, true otherwise
 *   - return a reference on the currently iterated item
 *   - return how many items have been iterated so far
 *
 *  It is important to notice the following: the current object is a reference (see get prototype) on an instance
 *  whose lifetime must be greater than the use of references by the clients, otherwise clients may suffer some
 *  dead references. Another implementation of IteratorGet (say IteratorCopy) could provide a copy of each iterated item,
 *  which would allow to use reference iterators that only iterate transient objects (like FASTA sequences iteration).
 *
 * The true advantage (compared to a mere Iterator) relies in the atomicity of the 'get' method.
 * Smart implementation of IteratorGet can ensure that the 'get' method is protected against concurrent access
 * by several threads. Hence, a single IteratorGet instance can be shared and used by several clients that
 * run on distinct threads (or ICommand in our object point of view).
 *
 * This is a simple mean to achieve parallelism:
 *  - write some implementation of Iterator that loops over specific objects
 *  - encapsulate an instance of this implementation in a IteratorGet instance
 *  - create several ICommand instances that reference the single IteratorGet instance
 *  - dispatch the ICommand through some parallel dispatcher.
 *
 *  A shortcut for doing this may be achieved through the IteratorCommand class.
 *
 * Sample code showing how to create an iterator, encapsulate it, and iterate it with 'get' method.
 * \code
 *
 *   // We create some iterator.
 *   list<int> l;
 *   l.push_back (1);
 *   l.push_back (2);
 *   l.push_back (4);
 *
 *   // We create our get iterator
 *   IteratorGet<const ISequence*>* it = new IteratorGet<const ISequence*> (new ListIterator<int> (l));
 *
 *   const ISequence* seq = 0;
 *   size_t nbRetrieved = 0;
 *
 *   // we loop over the sequences. Note that we could have split this in several ICommand instances.
 *   while (it->get (seq, nbRetrieved))
 *   {
 *      // do something with the retrieved sequence.
 *   }
 *  \endcode
 *
 * \see Iterator
 * \see IteratorCommand
 */
template <class T> class IteratorGet : public dp::SmartPointer
{
public:

    /** Constructor.
     * \param[in] iterator : the referenced iterator.
     */
    IteratorGet (Iterator<T>* iterator)  : _iterator(0), _synchro(0), _nbRetrieved(0)
    {
        setIterator (iterator);

        _synchro = os::impl::DefaultFactory::singleton().thread().newSynchronizer();
    }

    /** Destructor. */
    virtual ~IteratorGet ()
    {
        setIterator (0);
        if (_synchro)  { delete _synchro; }
    }

    /** Retrieve the current object in the iteration.
     * \param[out] item: the retrieved object
     * \param[out] nbRetrieved: the number of objects retrieved so far.
     * \return true if the iteration is running, false if the iteration is over. */
    bool get (T& item, size_t& nbRetrieved)
    {
        bool result = false;

        if (_synchro)  { _synchro->lock (); }

        if (_nbRetrieved == 0)  {  _iterator->first ();  }
        else                    {  _iterator->next  ();  }

        result = ! _iterator->isDone();
        if (result)
        {
            item = _iterator->currentItem();
            nbRetrieved = ++ _nbRetrieved;
        }

        if (_synchro)  { _synchro->unlock (); }

        return result;
    }

private:

    /** Referenced iterator. */
    Iterator<T>* _iterator;
    void setIterator (Iterator<T>* iterator)  { SP_SETATTR (iterator); }

    /** Synchronizer. */
    os::ISynchronizer* _synchro;

    size_t _nbRetrieved;
};

/********************************************************************************/

/** \brief ICommand that iterates items from an IteratorGet instance
 *
 * It may be useful to have a specific implementation of ICommand that iterates over the items
 * of a given IteratorGet instance, instead of having to write the classical iteration loop.
 *
 * Here, we refine the ICommand::execute method as a template method with a primitive
 * IteratorCommand::execute; this method is called each time an item has been retrieved from the
 * iterator.
 *
 * From implementors point of view, they just have to inherit from IteratorCommand and refine
 * the IteratorCommand::execute method.
 *
 * Sample of use:
 *
 * \code
 *
 * // We create a command that will do something on an integer value
 *   class MySeqCmd : public IteratorCommand<int>
 *   {
 *   public:
 *       MySeqCmd (IteratorGet<int>* it) : IteratorCommand<int> (it)  {}
 *       void execute (int& currentValue, size_t& nbGot)   {  // do something with the current value  }
 *   };
 *
 *   void foo ()
 *   {
 *      // We create some iterator.
 *      list<int> l;   for (int i=1; i<=1000; i++)  {  l.push_back (2*i+1);  }
 *
 *      // We create our get iterator
 *      IteratorGet<const ISequence*>* it = new IteratorGet<const ISequence*> (new ListIterator<int> (l));
 *
 *      // We create some IteratorCommand that will share our sequences iterator.
 *      list<ICommand*> commands;
 *      for (size_t i=1; i <= 8; i++)  {  commands.push_back (new MySeqCmd (it));  }
 *
 *      // We dispatch the commands
 *      ParallelCommandDispatcher().dispatchCommands (commands,0);
 *   }
 * \endcode
 */
template <class T> class IteratorCommand : public dp::ICommand
{
public:

    /** Constructor. */
    IteratorCommand (IteratorGet<T>* iterator)  : _iteratorGet(0)  {  setIteratorGet (iterator);  }

    /** Destructor. */
    virtual ~IteratorCommand ()  {  setIteratorGet (0);  }

    /** \copydoc ICommand::execute. This method is implemented as a Design Pattern Template Method. */
    void execute ()
    {
        preExecute ();

        size_t nbGot;
        for (T current ;  _iteratorGet->get (current, nbGot); )   {  execute (current, nbGot);  }

        postExecute ();
    }

protected:

    /** Primitive of the ICommand::execute method.
     * \param[in] current : current item of the iteration.
     * \param[in] nbGot : number of items so far retrieved during the iteration.
     */
    virtual void execute (T& current, size_t& nbGot) = 0;

    /** Method called before the execution of the Template Method ICommand::execute */
    virtual void preExecute  ()  {}

    /** Method called after the execution of the Template Method ICommand::execute */
    virtual void postExecute ()  {}

private:

    /** Referenced iterator. */
    IteratorGet<T>* _iteratorGet;

    /** Smart setter on the referenced iterator. */
    void setIteratorGet (IteratorGet<T>* iteratorGet)  { SP_SETATTR (iteratorGet); }
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _ITERATOR_GET_HPP_ */
