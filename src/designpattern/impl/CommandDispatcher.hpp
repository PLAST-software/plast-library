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
 *   Define a default command invoker that uses some OS thread factory for
 *   creating threads in which commands will be executed.
 *
 *   Two implementations of the ICommandDispatcher interface:
 *      - parallel: use ICommandInvoker for parallelization
 *          (use a CommandInvoker factory)
 *      - serial:   just launch one command after the other
 *****************************************************************************/

#ifndef COMMAND_DISPATCHER_HPP_
#define COMMAND_DISPATCHER_HPP_

/********************************************************************************/

#include "ICommand.hpp"
#include "IThread.hpp"

/********************************************************************************/
namespace dp {
/********************************************************************************/

/** Default command invoker that uses an OS thread factory for creating threads.
 */
class DefaultCommandInvoker : public ICommandInvoker
{
public:

    DefaultCommandInvoker (os::IThreadFactory* threadFactory = 0);

    virtual ~DefaultCommandInvoker ();

    /** Command execution in current thread context. */
    virtual void executeCommand  (ICommand* command);

    /** Wait the end of the thread. */
    virtual void join ();

private:
    os::IThreadFactory* _threadFactory;
    os::IThread*        _thread;
    os::ISynchronizer*  _synchro;
    ICommand*           _command;

    static void* mainloop (void* data);
};

/********************************************************************************/

/** Default command invoker factory.
 */
class DefaultCommandInvokerFactory : public ICommandInvokerFactory
{
public:

    static ICommandInvokerFactory& singleton();

    virtual ~DefaultCommandInvokerFactory() {}

    ICommandInvoker* newCommandInvoker (const char* name = "", ...);
};

/********************************************************************************/

/** Launches commands through different ICommandInvoker => parallelization.
 *  A provided ICommandInvokerFactory is used for creating ICommandInvoker instances.
 */
class ParallelCommandDispatcher : public ICommandDispatcher
{
public:

    ParallelCommandDispatcher (ICommandInvokerFactory& factory, size_t nbUnits=0);
    virtual ~ParallelCommandDispatcher() {}

    /** Dispatch commands execution in some separate contexts (threads for instance).
     *  Returns the ellapsed time in msec.
     */
    u_int32_t dispatchCommands (std::list<ICommand*> commands, ICommand* postTreatment=0);

    size_t getExecutionUnitsNumber () { return _nbUnits; }

private:
    ICommandInvokerFactory& _factory;
    size_t _nbUnits;
};

/********************************************************************************/

/** A dispatcher that uses the calling thread, so no parallization.
 */
class SerialCommandDispatcher : public ICommandDispatcher
{
public:

    /** */
    virtual ~SerialCommandDispatcher() {}

    /** Dispatch commands execution in some separate contexts (threads for instance). */
    u_int32_t dispatchCommands (std::list<ICommand*> commands, ICommand* postTreatment=0);

    size_t getExecutionUnitsNumber () { return 1; }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* COMMAND_DISPATCHER_HPP_ */
