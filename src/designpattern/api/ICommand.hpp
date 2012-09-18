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

/** \file ICommand.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Command Design Pattern definition
 *
 *  This file holds interfaces related to the Design Pattern Command.
 *  It mainly provides means to:
 *      - execute simple action in specific thread
 *      - dispatch a list of actions in several threads
 */

#ifndef ICOMMAND_HPP_
#define ICOMMAND_HPP_

#include <designpattern/api/SmartPointer.hpp>
#include <misc/api/types.hpp>
#include <stddef.h>
#include <string>
#include <list>

/********************************************************************************/
/** \brief Design Pattern tools (Observer, SmartPointer, Command...) */
namespace dp {
/********************************************************************************/

/** \brief Interface of what a command is.
 *
 *  This class represents the Design Pattern Command whose purpose is to encapsulate
 *  some processing into a uniform calling procedure, a 'execute' method.
 *
 *  The command concept provides a uniform way for running some work.
 *  This is achieved by refining the execute() method of the ICommand interface.
 *  Doing so makes the actual job encapsulated inside the execute() body; clients
 *  that want to use commands only have to know how to launch them: calling execute().
 *
 *  This uniformization also allows to define some interface that know how to manipulate
 *  commands instance; we call this interface ICommandInvoker and it merely knows how to
 *  run a ICommand instance and wait until the command is finished. From this point of view,
 *  the ICommandInvoker can be seen as an oriented object abstraction of the thread concept.
 *
 *  The further step is to introduce an interface that can manage a list of commands.
 *  For instance, in a dual core architecture, it is possible to launch
 *  two instances of commands in two separated threads, which means we got a parallelization
 *  scheme. It is therefore interesting to define an interface that takes a list of commands
 *  an dispatch them in different threads; we call such an interface a command dispatcher.
 *  Note that an implementation of such a dispatcher can parallelize the commands, but another
 *  implementation can only serialize the commands; so, job parallelization or serialization
 *  is just a matter of choice in the actual implementation of the dispatcher interface.
 *
 *  PLAST will make a huge use of commands and commands dispatchers; in particular, since it is
 *  based on seeds iteration, PLAST will use a list of iterators (each of them iterating a
 *  subset of seeds), each one being iterated in a specific command. So using a parallel
 *  commands dispatcher on this list of commands will provide a parallelization scheme using
 *  the multicore architecture of modern computers.
 *
 *  Sample of use:
 * \code
 * class MyCommand : public ICommand
 * {
 * public:
 *     void execute ()  { printf ("I am doing something there\n"); }
 * };
 * \endcode
 *
 * \see ICommandInvoker
 * \see ICommandDispatcher
 */
class ICommand : public SmartPointer
{
public:
    /** Method that executes some job. */
    virtual void execute () = 0;
};

/********************************************************************************/

/** \brief Interface for ICommand factories
 *
 * This interface defines a factory for creating ICommand instances.
 *
 * It also provides a tool for creating a list of commands from a list of factories.
 * This may be useful for classes that need some data whose retrieval may take a long time.
 * Instead of being themselves a ICommand, they delegate the data retrieval to some ICommand
 * that they know how to instantiate.
 */
class ICommandFactory : public SmartPointer
{
public:

    /** Create a command. A name is provided for discriminating the kind of ICommand to be instantiated.
     * \param[in] name : name used for solving the type of ICommand to create
     * \return the created command if any.
     */
    virtual ICommand* createCommand (const std::string& name) = 0;

    /** Create a list of commands from a list of command factories.
     * \param[in] factories : list of factories used for creating ICommand instances
     * \param[in] name : discriminant for creating ICommand instances.
     * \return the list of ICommand instances.
     */
    static std::list<ICommand*> createCommandsList (const std::list<ICommandFactory*>& factories, const std::string& name)
    {
        std::list<ICommand*> result;

        for (std::list<ICommandFactory*>::const_iterator it = factories.begin(); it != factories.end(); it++)
        {
            result.push_back ((*it)->createCommand(name));
        }

        return result;
    }
};

/********************************************************************************/

/** \brief Execution of ICommand instances
 *
 *  Used for executing ICommand (i.e. call to 'execute').
 *
 *  Note that we add a join() method for synchronization purpose, ie. the join() method
 *  will wait until the command (launched by executeCommand()) is finished.
 *
 *  According to the implementation, a command could be launched in a brand new thread or only
 *  launched in the current thread.
 *
 *  This interface may be seen as an oriented object view of the thread concept.
 *
 *  \see ICommand
 *  \see ICommandDispatcher
 */
class ICommandInvoker : public SmartPointer
{
public:

    /** Destructor. */
    virtual ~ICommandInvoker () {}

    /** Launch the execution of a provided command.
     * \param[in] command : the command to be executed
     */
    virtual void executeCommand  (ICommand* command) = 0;

    /** Wait until the launched command is finished. */
    virtual void join () = 0;
};

/********************************************************************************/

/** \brief Dispatching of commands.
 *
 *  Interface that launches several commands. Implementors could rely on the ICommandInvoker
 *  interface for actually launching the ICommand instances.
 *
 *  According to the implementation, the dispatching can be done in a serial or in
 *  a parallelized way.
 *
 *  Note that a post treatment command can be provided and will be launched when all
 *  the commands have finished.
 *
 *  This interface is a core concept in PLAST seeds based algorithm; PLAST indeed will create
 *  a list of ICommand instances, each of them iterating a set of seeds. It has then just to
 *  use a ICommandDispatcher instance for launching the whole iterators and therefore, parallelization
 *  of the algorithm is just a matter of choice in a specific implementation of this interface.
 *
 *  Note that this interface could also be implemented for dispatching commands over a network in order
 *  to use a grid of computers. We could imagine that the commands dispatching consists in
 *  launching some RCP calls, or creating some web services calls. The important point is that, from the
 *  client point of view, her/his code should not change, except the actual kind of ICommandDispatcher instance
 *  provided to the algorithm.
 *
 *  According to the previous note, PLAST is a good candidate for network dispatching. Indeed, when databases
 *  to be compared are too big to be in memory, they are split in small blocks and the full PLAST algorithm
 *  consists in a list of ICommand, each of them dealing with some couple of databases blocks. With the appropriate
 *  "network" command dispatcher, this list of commands could be processed over a network without modifying
 *  the current PLAST algorithm.
 *
 *  Sample of use:
 *  \code
 *  // We define a command class
 *  class MyCommand : public ICommand
 *  {
 *  public:
 *      MyCommand (int i) : _i(i) {}
 *      void execute ()  { printf ("Going to sleep %d...\n", _i);  sleep (_i);  }
 *  private:
 *      int _i;
 *  };
 *
 *  int main (int argc, char* argv[])
 *  {
 *      // We create a list of commands
 *      std::list<ICommand*> commands;
 *      commands.push_back (new MyCommand(2));
 *      commands.push_back (new MyCommand(5));
 *      commands.push_back (new MyCommand(3));
 *
 *      // We create a commands dispatcher that parallelizes the execution of the commands.
 *      ICommandDispatcher* dispatcher = new ParallelCommandDispatcher ();
 *
 *      // We launch the 3 commands.
 *      dispatcher->dispatchCommands (commands, 0);
 *
 *      // Here, we should be waiting until the second command (that waits for 5 seconds) is finished.
 *
 *      return 0;
 *  }
 *  \endcode
 *
 *  \see ICommand
 *  \see ICommandInvoker
 */
class ICommandDispatcher : public SmartPointer
{
public:

    /** Dispatch commands execution in some separate contexts (threads for instance).
     *  Once the commands are launched, this dispatcher waits for the commands finish.
     *  Then, it may have to execute a post treatment command (if any).
     * \param[in] commands      : commands to be executed
     * \param[in] postTreatment : command to be executed after all the commands are done
     */
    virtual void dispatchCommands (std::list<ICommand*> commands, ICommand* postTreatment=0) = 0;

    /** Returns the number of execution units for this dispatcher.
     *  For instance, it could be the number of cores in a multi cores architecture.
     *  \return the number of execution units.
     */
    virtual size_t getExecutionUnitsNumber () = 0;

protected:

    /** Creation of a new ICommandInvoker instance.
     * \return the created instance
     */
    virtual ICommandInvoker* newCommandInvoker () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* ICOMMAND_HPP_ */
