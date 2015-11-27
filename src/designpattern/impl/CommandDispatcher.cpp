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

#include <designpattern/impl/CommandDispatcher.hpp>
#include <os/impl/DefaultOsFactory.hpp>
#include <os/impl/CommonOsImpl.hpp>
#include <misc/api/macros.hpp>

using namespace std;
using namespace os;
using namespace os::impl;

#include <stdio.h>
#define DEBUG(a)  //printf a

/********************************************************************************/
namespace dp {  namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DefaultCommandInvoker::DefaultCommandInvoker (IThreadFactory* threadFactory)
    : _threadFactory(threadFactory), _thread(0), _synchro(0), _command(0)
{
    _synchro = DefaultFactory::thread().newSynchronizer();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DefaultCommandInvoker::~DefaultCommandInvoker ()
{
    if (_synchro != 0)  { delete _synchro; }
    if (_thread  != 0)  { delete _thread;  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DefaultCommandInvoker::executeCommand  (ICommand* command)
{
    /** We check that we don't have already a running command. */
    if (_command == 0 && _threadFactory != 0)
    {
        /** We memorize the command. */
        _command = command;

        /** We create a thread in which the command will be executed. */
        _thread = _threadFactory->newThread (mainloop, this);
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DefaultCommandInvoker::join ()
{
    /** We delegate the work to the OS thread. */
    _thread->join();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void* DefaultCommandInvoker::mainloop (void* data)
{
    /** We recover the THIS instance. */
    DefaultCommandInvoker* THIS = (DefaultCommandInvoker*) data;

    if (CHECKPTR(THIS) && CHECKPTR(THIS->_command))
    {
        /** We execute the command. */
        (THIS->_command)->use     ();
        (THIS->_command)->execute ();
        (THIS->_command)->forget  ();

        THIS->_command = 0;
    }

    return NULL;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ParallelCommandDispatcher::ParallelCommandDispatcher (size_t nbUnits)
    : _nbUnits(nbUnits)
{
    /** If the default value was provided, we try to guess the number of cores. */
    if (_nbUnits == 0)  {  _nbUnits = DefaultFactory::thread().getNbCores();  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void  ParallelCommandDispatcher::dispatchCommands (list<ICommand*> commands, ICommand* postTreatment)
{
    DEBUG (("ParallelCommandDispatcher::dispatchCommands  START \n"));

    /** We need a list holding the threads we want to create for the command execution dispatch. */
    list<ICommandInvoker*> invokers;

    /** We loop over given commands. */
    for (list<ICommand*>::iterator it = commands.begin(); it != commands.end(); it++)
    {
        /** We create a thread and use it. */
        ICommandInvoker* t = newCommandInvoker ();
        t->use();

        /** We add it into the list. */
        invokers.push_back (t);

        /** We execute the current command. */
        t->executeCommand (*it);
    }

    DEBUG (("ParallelCommandDispatcher::dispatchCommands  COMMANDS LAUNCHED\n"));

    /** We loop over the created threads. */
    for (list<ICommandInvoker*>::iterator it = invokers.begin(); it != invokers.end(); it++)
    {
        /** We wait for the end of each thread. */
        (*it)->join ();
    }

    /** We loop over the created threads. */
    for (list<ICommandInvoker*>::iterator it = invokers.begin(); it != invokers.end(); it++)
    {
        /** We forget the thread. */
        (*it)->forget ();
    }

    DEBUG (("ParallelCommandDispatcher::dispatchCommands  COMMANDS JOINED\n"));

    /** We may have to do some post treatment. Note that we do it in the current thread. */
    DefaultCommandInvoker invoker (& SerialThreadFactory::singleton());
    invoker.executeCommand (postTreatment);

    DEBUG (("ParallelCommandDispatcher::dispatchCommands  FINISHED\n"));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ICommandInvoker* ParallelCommandDispatcher::newCommandInvoker ()
{
    return new DefaultCommandInvoker (& DefaultFactory::thread());
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SerialCommandDispatcher::dispatchCommands (std::list<ICommand*> commands, ICommand* postTreatment)
{
    DefaultCommandInvoker invoker (& SerialThreadFactory::singleton());

    for (list<ICommand*>::iterator it = commands.begin(); it != commands.end(); it++)
    {
    	DefaultCommandInvoker (& SerialThreadFactory::singleton()).executeCommand (*it);
    }

    /** We may have to do some post treatment. Note that we do it in the current thread. */
    DefaultCommandInvoker (& SerialThreadFactory::singleton()).executeCommand (postTreatment);
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
