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

/** \file SystemCommand.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Command for launching system commands
 */

#ifndef _SYSTEM_COMMAND_HPP_
#define _SYSTEM_COMMAND_HPP_

/********************************************************************************/

#include <designpattern/api/ICommand.hpp>
#include <os/api/IFile.hpp>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief ICommand implementation for launching system commands (ie through 'system' calls).
 *
 * This is the object-oriented version (through the ICommand interface) of the 'system' call.
 *
 * \code
 * void foo ()
 * {
 *      list<ICommand*> commands;
 *      commands.push_back (new SystemCommand ("head -%d %s", 20, "/tmp/bar"));
 *      commands.push_back (new SystemCommand ("tail -%d %s", 20, "/tmp/bar"));
 *
 *      SerialCommandDispatcher().dispatchCommands (commands, 0);
 * }
 * \endcode
 */
class SystemCommand : public ICommand
{
public:

    /** Constructor.
     * \param[in] format : format (in the printf way) of the string of the command
     * \param[in] ... : ellipsis for the format
     */
    SystemCommand (const char* format, ...);

    /** Constructor.
     * \param[in] cmd : string of the command to be executed by the 'system' call
     */
    SystemCommand (const std::string& cmd);

    /** Constructor. Possibility to specify an output file in which the output of the command will be dumped
     * \param[in] output : file where system output should be dumped in
     * \param[in] format : format (in the printf way) of the string of the command
     * \param[in] ... : ellipsis for the format
     */
    SystemCommand (os::IFile* output, const char* format, ...);

    /** Destructor. */
    virtual ~SystemCommand ();

    /** \copydoc ICommand::execute. */
    void execute ();

    /** Returns the execution time of the command.
     * \return the execution time.
     */
    u_int32_t getEllapsedTime () { return _ellapsedTime; }

    /** Return the status of the system call.
     * \return return the result of the 'system' call.
     */
    int getStatus ()  { return _status; }

    /** Return a string holding the command to be executed.
     * \return the command string to be executed.
     */
    std::string toString () { return _buffer; }

private:

    os::IFile* _outputfile;

    char _tmpFilename[128];

    char _buffer[4*1024];

    /** */
    u_int32_t _ellapsedTime;

    int _status;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _SYSTEM_COMMAND_HPP_ */
