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

/** \file CommandDispatcher.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Command dispatcher implementation
 *
 *   Define a default command invoker that uses some OS thread factory for
 *   creating threads in which commands will be executed.
 *
 *   Two implementations of the ICommandDispatcher interface:
 *      - parallel: use ICommandInvoker for parallelization
 *          (use a CommandInvoker factory)
 *      - serial:   just launch one command after the other
 */

#ifndef COMMAND_DISPATCHER_HPP_
#define COMMAND_DISPATCHER_HPP_

/********************************************************************************/

#include <designpattern/api/ICommand.hpp>
#include <os/api/IThread.hpp>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Default command invoker that uses an OS thread factory for creating threads.
 *
 * This class implements the ICommandInvoker interface by using some OS specific capabilities.
 * It can be seen as an object oriented view of the thread concept.
 *
 * It won't be a good point to have several implementations of this interface, one per operating
 * system. A best way is to configure such an invoker by some OS abstraction layer, which is done
 * by providing a os::IThreadFactory instance.
 */
class DefaultCommandInvoker : public ICommandInvoker
{
public:

    /** Constructor.
     * \param[in]  threadFactory : the OS factory used for creating threads, mutex,...
     */
    DefaultCommandInvoker (os::IThreadFactory* threadFactory);

    /** Destructor. */
    virtual ~DefaultCommandInvoker ();

    /** \copydoc ICommandInvoker::executeCommand */
    virtual void executeCommand  (ICommand* command);

    /** \copydoc ICommandInvoker::join */
    virtual void join ();

private:

    /** Reference on the thread factory to be used (provided at construction). */
    os::IThreadFactory* _threadFactory;

    /** Thread created by the factory; used for executing a command. */
    os::IThread*        _thread;

    /** Synchronization object for dealing with concurrent access. */
    os::ISynchronizer*  _synchro;

    /** Reference on the command to be executed. */
    ICommand*           _command;

    /** Mainloop of the thread. */
    static void* mainloop (void* data);
};

/********************************************************************************/

/** \brief Launches commands in different threads
 *
 *  This implementation launches commands through different ICommandInvoker => parallelization.
 *  A provided ICommandInvokerFactory is used for creating ICommandInvoker instances.
 *
 */
class ParallelCommandDispatcher : public ICommandDispatcher
{
public:

    /** Constructor.
     * \param[in] nbUnits : number of threads to be used. If 0 is provided, one tries to guess the number of available cores.
     */
    ParallelCommandDispatcher (size_t nbUnits=0);

    /** Destructor. */
    virtual ~ParallelCommandDispatcher() {}

    /** \copydoc ICommandDispatcher::dispatchCommands */
    void dispatchCommands (std::list<ICommand*> commands, ICommand* postTreatment=0);

    /** \copydoc ICommandDispatcher::getExecutionUnitsNumber */
    size_t getExecutionUnitsNumber () { return _nbUnits; }

protected:

    /** \copydoc ICommandDispatcher::newCommandInvoker */
    ICommandInvoker* newCommandInvoker ();

private:

    /** Number of execution units to be used for command dispatching. */
    size_t _nbUnits;
};

/********************************************************************************/

/** \brief Launches commands in current threads
 *
 * A dispatcher that uses the calling thread, so no parallization.
 */
class SerialCommandDispatcher : public ICommandDispatcher
{
public:

    /** Destructor (defined because of presence of virtual methods). */
    virtual ~SerialCommandDispatcher() {}

    /** \copydoc ICommandDispatcher::dispatchCommands */
    void dispatchCommands (std::list<ICommand*> commands, ICommand* postTreatment=0);

    /** \copydoc ICommandDispatcher::getExecutionUnitsNumber */
    size_t getExecutionUnitsNumber () { return 1; }

protected:

    /** \copydoc ICommandDispatcher::newCommandInvoker */
    ICommandInvoker* newCommandInvoker ()  { return 0; }
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* COMMAND_DISPATCHER_HPP_ */
