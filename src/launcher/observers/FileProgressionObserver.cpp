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

#include <algo/core/api/IAlgoEvents.hpp>

#include <launcher/observers/FileProgressionObserver.hpp>

#include <designpattern/impl/Property.hpp>

#include <stdio.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace os;
using namespace os::impl;
using namespace algo::core;

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
FileProgressionObserver::FileProgressionObserver (const std::string& filename)
    : _filename (filename), _dumpablePercentage(~0)
{
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
FileProgressionObserver::~FileProgressionObserver ()
{
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void FileProgressionObserver::dump (void)
{
    size_t percent = (size_t) (100*_globalPercentage);

    /** we check whether it has changed or not or has reach 100%. */
    if (_dumpablePercentage != percent  ||  percent >= 100)
    {
        _dumpablePercentage = percent;

        /** We build a properties instance. */
        Properties props;
        props.add (0, "progression", "");

        props.add (1, "alignments",   "");
        props.add (2, "number",  "%ld",  _nbAlignments);

        props.add (1, "exec", "");
        props.add (2, "local",  "%f",   _currentPercentage);
        props.add (2, "global", "%f",   _globalPercentage);

        props.add (1, "time",      "");
        props.add (2, "elapsed",   "%ld", _ellapsedTime  / 1000);
        props.add (2, "remaining", "%ld", _remainingTime / 1000);
        props.add (2, "total",     "%ld", (_ellapsedTime + _remainingTime) / 1000);

        props.add (1, "memory",    "");
        props.add (2, "current",   "%ld", _usedMemory);
        props.add (2, "max",       "%ld", _maxUsedMemory);
        props.add (2, "total",     "%ld", _totalUsedMemory);

        XmlDumpPropertiesVisitor v (_filename, false, false);
        props.accept (&v);
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
AlignmentProgressionObserver::AlignmentProgressionObserver (const std::string& filename)
    : _file (0), _count(0),_t0(0)
{
    _file = fopen (filename.c_str(), "w");
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
AlignmentProgressionObserver::~AlignmentProgressionObserver ()
{
    if (_file != 0)  { fclose (_file); }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void AlignmentProgressionObserver::update (dp::EventInfo* evt, dp::ISubject* subject)
{
    AlignmentProgressionEvent* e1 = dynamic_cast<AlignmentProgressionEvent*> (evt);
    if (e1 != 0)
    {
        if (_count==0)
        {
            _t0 = DefaultFactory::singleton().getTimeFactory().getclock();
        }
        _count ++;

        u_int32_t t1 = DefaultFactory::singleton().getTimeFactory().getclock();

        fprintf (_file, "%d  %.3f  %d  %d \n",
            _count,
            (t1-_t0) / 1000000.0,
            e1->getUngapResult()->getSize(),
            e1->getGapResult()->getSize()
        );

        return;
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
ResourcesObserver::ResourcesObserver (const std::string& filename)
    : _file (0), _lastAlgo(~0), _lastNbAlign(0), _lastTime(0), _totalTime(0)
{
    _file = fopen (filename.c_str(), "w");
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
ResourcesObserver::~ResourcesObserver ()
{
    if (_file)  { fclose (_file); }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void ResourcesObserver::update (dp::EventInfo* evt, dp::ISubject* subject)
{
    /** We call the parent method. */
    AbstractProgressionObserver::update (evt, subject);

    AlgorithmReportEvent* e1 = dynamic_cast<AlgorithmReportEvent*> (evt);
    if (e1 != 0)
    {
        if ( (_currentAlgo != _lastAlgo)  &&  (_file != 0) )
        {
            fprintf (_file, "%ld  %.3f  %lld  %lld  %.3f  %.3f  %.3f  %d  %lld  %lld \n",
                _currentAlgo,
                _globalPercentage,
                _nbAlignments - _lastNbAlign,
                _nbAlignments,
                (float) (_ellapsedTime - _lastTime) / 1000.0,
                (float)_ellapsedTime / 1000.0,
                (float) (_ellapsedTime+_remainingTime) / 1000.0,
                _usedMemory,
                _maxUsedMemory,
                _totalUsedMemory
             );

            fflush (_file);

            /** We update some values. */
            _lastAlgo    = _currentAlgo;
            _lastTime    = _ellapsedTime;
            _lastNbAlign = _nbAlignments;
        }

        return;
    }

}

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/
