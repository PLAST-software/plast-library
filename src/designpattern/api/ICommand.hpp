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
 *   This file holds interfaces related to the Design Pattern Command.
 *   It mainly provides means to:
 *      - execute simple action in specific thread
 *      - dispatch a list of actions in several threads
 *****************************************************************************/

#ifndef ICOMMAND_HPP_
#define ICOMMAND_HPP_

#include <stddef.h>
#include "SmartPointer.hpp"
#include <list>
#include "types.hpp"

/********************************************************************************/
namespace dp {
/********************************************************************************/

/** Design Pattern Command.
 */
class ICommand : public SmartPointer
{
public:
    virtual void execute () = 0;
};

/********************************************************************************/

/** Design Pattern Command.
 *  Used for executing ICommand (i.e. call to 'execute')
 *  Note that we add a 'join' method for synchronization purpose.
 */
class ICommandInvoker : public SmartPointer
{
public:

    /** */
    virtual ~ICommandInvoker () {}

    /** Command execution in some thread context. */
    virtual void executeCommand  (ICommand* command) = 0;

    /** Wait the end of the thread. */
    virtual void join () = 0;
};

/********************************************************************************/

/** Factory of ICommandInvoker instances.
 *  Implementations can use current context or can use a specific thread; it should
 *  depend on the 'os' abstraction package.
 */
class ICommandInvokerFactory : public SmartPointer
{
public:
    virtual ICommandInvoker* newCommandInvoker (const char* name = "", ...) = 0;
};

/********************************************************************************/

/** Interface that launches several commands. According to the implementation, this
 *  can be done in a serial or in a parallelized way.
 *  Note that a post treatment command can be provided and will be launched when all
 *  the commands have finished.
 */
class ICommandDispatcher : public SmartPointer
{
public:

    /** Dispatch commands execution in some separate contexts (threads for instance). */
    virtual u_int32_t dispatchCommands (std::list<ICommand*> commands, ICommand* postTreatment=0) = 0;

    /** Returns the number of execution units for this dispatcher.
     *  For instance, it could be the number of cores in a multi cores architecture. */
    virtual size_t getExecutionUnitsNumber () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* ICOMMAND_HPP_ */
