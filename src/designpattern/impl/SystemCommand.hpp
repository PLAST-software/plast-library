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
 */
class SystemCommand : public ICommand
{
public:

    /** Constructor. */
    SystemCommand (const char* format, ...);

    /** Constructor. */
    SystemCommand (const std::string& cmd);

    /** Constructor. */
    SystemCommand (os::IFile* output, const char* format, ...);

    /** Destructor. */
    virtual ~SystemCommand ();

    /** \copydoc ICommand::execute. */
    void execute ();

    /** */
    u_int32_t getEllapsedTime () { return _ellapsedTime; }

    /** */
    int getStatus ()  { return _status; }

    /** */
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
