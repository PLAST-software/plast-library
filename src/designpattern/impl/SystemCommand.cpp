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

#include <designpattern/impl/SystemCommand.hpp>
#include <designpattern/impl/FileLineIterator.hpp>

#include <stdlib.h>

using namespace std;
using namespace os;
using namespace os::impl;

#include <stdio.h>
#define DEBUG(a)  //printf a

/********************************************************************************/
namespace dp { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SystemCommand::SystemCommand (const char* format, ...)
    : _outputfile(0),_ellapsedTime(0), _status(0)
{
    va_list ap;
    va_start  (ap, format);
    vsnprintf (_buffer, sizeof(_buffer), format, ap);
    va_end (ap);

    DEBUG (("SystemCommand::SystemCommand (1) _buffer='%s' \n", _buffer));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SystemCommand::SystemCommand (const std::string& request)
    : _outputfile(0), _ellapsedTime(0), _status(0)
{
    strcpy (_buffer, request.c_str());
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SystemCommand::SystemCommand (os::IFile* output, const char* format, ...)
    : _outputfile(output),_ellapsedTime(0), _status(0)
{
    /** */
    *_tmpFilename = 0;
    *_buffer      = 0;

    /** We retrieve the request string. */
    char tmp[1024];
    va_list ap;
    va_start  (ap, format);
    vsnprintf (tmp, sizeof(tmp), format, ap);
    va_end (ap);

    if (_outputfile != 0)
    {
        /** We create a temporary file name for gathering all command outputs. */
        snprintf (_tmpFilename, sizeof(_tmpFilename), "/tmp/%p", this);

        /** We modify the request. */
        snprintf (_buffer, sizeof(_buffer), "%s > %s", tmp, _tmpFilename);
    }
    else
    {
        /** We modify the request. */
        snprintf (_buffer, sizeof(_buffer), "%s", tmp);
    }

    DEBUG (("SystemCommand::SystemCommand (2) _buffer='%s'  _tmpFilename='%s' \n", _buffer, _tmpFilename));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SystemCommand::~SystemCommand ()
{
    /** We remove the temporary file. */
    if (_outputfile != 0)
    {
        char tmp[256];  snprintf (tmp, sizeof(tmp), "/bin/rm %s", _tmpFilename);
        system (tmp);
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
void SystemCommand::execute ()
{
    u_int32_t t0 = os::impl::DefaultFactory::time().gettime();

    /** We execute the command. */
    _status = system (_buffer);

    u_int32_t t1 = os::impl::DefaultFactory::time().gettime();

    _ellapsedTime = (t1 - t0);

    /** We may have to copy its output into the user provided file. */
    if (_outputfile != 0)
    {
        /** We use a file iterator. */
        FileLineIterator it (_tmpFilename, 1024);
        for (it.first(); !it.isDone(); it.next())
        {
            _outputfile->println (it.currentItem());
        }
    }
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
