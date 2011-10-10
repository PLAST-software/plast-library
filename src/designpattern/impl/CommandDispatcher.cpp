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

#include "CommandDispatcher.hpp"
#include "DefaultOsFactory.hpp"

#include "macros.hpp"

using namespace std;
using namespace os;

/********************************************************************************/
namespace dp {
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
    _synchro = os::DefaultFactory::singleton().getThreadFactory().newSynchronizer();
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
    if (_command == 0)
    {
        /** We memorize the command. */
        _command = command;

        if (CHECKPTR(_threadFactory))
        {
            /** We create a thread in which the command will be executed. */
            _thread = _threadFactory->newThread (mainloop, this);
        }
        else
        {
            /** We have no thread factory, we launch directly the mainloop. */
            mainloop (this);
        }
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
    bool isOk = false;

    /** We retrieve the thread status in a synchronized way because of concurrent
     *  access (see 'mainloop'). */
    {
        LocalSynchronizer s (_synchro);
        isOk = _thread != 0;
    }

    /** We delegate the work to the OS thread. */
    if (isOk)  {  _thread->join();  }
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
    /** We recover the invoker instance. */
    DefaultCommandInvoker* invoker = (DefaultCommandInvoker*) data;

    if (CHECKPTR(invoker))
    {
        if (CHECKPTR(invoker->_command))
        {
            /** We execute the command. */
            (invoker->_command)->use     ();
            (invoker->_command)->execute ();
            (invoker->_command)->forget  ();

            invoker->_command = 0;
        }

        /** We suppress the thread. */
        if (CHECKPTR(invoker->_thread))
        {
            LocalSynchronizer s (invoker->_synchro);
            delete invoker->_thread;
            invoker->_thread  = 0;
        }
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
ICommandInvokerFactory& DefaultCommandInvokerFactory::singleton()
{
    static DefaultCommandInvokerFactory instance;
    return instance;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ICommandInvoker* DefaultCommandInvokerFactory::newCommandInvoker (const char* name, ...)
{
    return new DefaultCommandInvoker (& DefaultFactory::singleton().getThreadFactory());
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ParallelCommandDispatcher::ParallelCommandDispatcher (ICommandInvokerFactory& factory, size_t nbUnits)
    : _factory (factory), _nbUnits(nbUnits)
{
    /** If the default value was provided, we try to guess the number of cores. */
    if (_nbUnits == 0)  {  _nbUnits = DefaultFactory::singleton().getThreadFactory().getNbCores();  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
u_int32_t  ParallelCommandDispatcher::dispatchCommands (list<ICommand*> commands, ICommand* postTreatment)
{
    u_int32_t startTime = DefaultFactory::singleton().getTimeFactory().gettime();

    DEBUG (("ParallelCommandDispatcher::dispatchCommands  START (%d)\n", startTime));

    /** We need a list holding the threads we want to create for the command execution dispatch. */
    list<ICommandInvoker*> invokers;

    /** We loop over given commands. */
    for (list<ICommand*>::iterator it = commands.begin(); it != commands.end(); it++)
    {
        /** We create a thread and use it. */
        ICommandInvoker* t = _factory.newCommandInvoker ();
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
    DefaultCommandInvoker().executeCommand (postTreatment);

    DEBUG (("ParallelCommandDispatcher::dispatchCommands  FINISHED\n"));

    /** We return the elapsed time. */
    return  DefaultFactory::singleton().getTimeFactory().gettime() - startTime;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
u_int32_t SerialCommandDispatcher::dispatchCommands (std::list<ICommand*> commands, ICommand* postTreatment)
{
    u_int32_t startTime = DefaultFactory::singleton().getTimeFactory().gettime();

    DefaultCommandInvoker invoker;

    for (list<ICommand*>::iterator it = commands.begin(); it != commands.end(); it++)
    {
        invoker.executeCommand (*it);
    }

    /** We may have to do some post treatment. Note that we do it in the current thread. */
    invoker.executeCommand (postTreatment);

    /** We return the elapsed time. */
    return  DefaultFactory::singleton().getTimeFactory().gettime() - startTime;
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
