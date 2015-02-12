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

#include <os/impl/DefaultOsFactory.hpp>

#include <designpattern/api/Iterator.hpp>

#include <launcher/observers/BargraphObserver.hpp>

#include <database/api/ISequenceDatabase.hpp>

#include <string.h>

using namespace std;
using namespace dp;
using namespace os;
using namespace os::impl;

/********************************************************************************/
namespace launcher {
namespace observers {
/********************************************************************************/

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
BargraphObserver::BargraphObserver (const char* prefix, FILE* file, size_t nbChar)
    : _file(file), _output(0), _prefix (prefix ? prefix : ""), _nbChar (nbChar), _actualSize(0)
{
    if (_nbChar == 0)  {  _nbChar = 20; }

    _actualSize = _nbChar + 200;

    _output = (char*) DefaultFactory::memory().calloc (_actualSize, sizeof(char));
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
BargraphObserver::~BargraphObserver ()
{
    DefaultFactory::memory().free (_output);

    fprintf (_file, "\n");
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void BargraphObserver::start ()
{
    /** We need to call parent for house keeping. */
    AbstractProgressionObserver::start ();

    memset (_output, _actualSize, 0);
    _output[0] = '[';
    for (size_t i=1; i<=_nbChar; i++)  {   _output[i] = '.';  }
    _output[_nbChar+1] = ']';

    /** We want to display the bargraph on a new line. */
//    fprintf (_file, "\n");
//    fflush (_file);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void BargraphObserver::dump (void)
{
    size_t imax =  (_percentThreshold * _nbChar) / 100;

    for (size_t i=1; i<=_nbChar && i<=imax; i++)  { _output[i] = '='; }

    if (_file != 0)
    {
        char ellapsed[32];      fillTimeBuffer (_ellapsedTime/1000,                     ellapsed);
        char remaining[32];     fillTimeBuffer (_remainingTime/1000,                    remaining);
        char fulltime[32];      fillTimeBuffer ((_ellapsedTime+_remainingTime)/1000,    fulltime);

        char buf[512];

        snprintf (buf, sizeof(buf),
            "\r%s [%d/%d] %.1f%%  align=%d  time [%s - %s - %s]  mem=%.1fMo (max=%.1fMo tot=%.1fGo)  %s [%4d:%4d] %s %3d%% ",
            "algo",  (int)_currentAlgo + 1,  (int)_totalAlgo,
            (100 * _globalPercentage),
            (int)_nbAlignments,
            ellapsed,  remaining, fulltime,
            (float)_usedMemory / 1024.0,
            (float)_maxUsedMemory / 1024.0,
            (float)_totalUsedMemory / 1024.0 / 1024.0,
            _prefix.c_str(),
            (int)_got,
            (int)_total,
            _output,
            (int)(100 * _currentPercentage)
        );

        fprintf (_file, "%s", buf);

        fflush (_file);
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
void BargraphObserver::fillTimeBuffer (u_int32_t t, char* buffer)
{
    size_t h = t /3600;
    size_t m = (t - 3600*h) / 60;
    size_t s = (t - 3600*h - 60*m);

    sprintf (buffer, "%02ld:%02ld:%02ld", h, m, s);
}

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/
