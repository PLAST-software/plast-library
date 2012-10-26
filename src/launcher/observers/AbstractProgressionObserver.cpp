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
#include <os/impl/TimeTools.hpp>

#include <designpattern/api/Iterator.hpp>
#include <designpattern/impl/FileLineIterator.hpp>

#include <algo/core/api/IAlgoEnvironment.hpp>
#include <algo/core/api/IAlgoEvents.hpp>

#include <launcher/observers/AbstractProgressionObserver.hpp>

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace misc;
using namespace dp;
using namespace dp::impl;
using namespace os;
using namespace os::impl;
using namespace algo::core;
using namespace algo::hits;
using namespace alignment::core;

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
AbstractProgressionObserver::AbstractProgressionObserver ()
    : _percentThreshold(0), _got(0), _total(0),
      _currentPercentage(0), _globalPercentage(0),
      _currentAlgo (0), _totalAlgo(0),
      _ellapsedTime(0), _remainingTime(0),
      _currentNbAlignments(0), _nbAlignments(0),
      _usedMemory(0), _maxUsedMemory(0), _totalUsedMemory(0),
      _currentFrame(0), _nbSubjectFrames(1), _nbQueryFrames(1)
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
void AbstractProgressionObserver::update (dp::EventInfo* evt, dp::ISubject* subject)
{
    IterationStatusEvent* e1 = dynamic_cast<IterationStatusEvent*> (evt);
    if (e1 != 0)
    {
        if (e1->getStatus() == ITER_STARTING)
        {
            start ();
        }
        else if (e1->getStatus() == ITER_DONE)
        {
            _percentThreshold = 100;
            _got = _total;

            dump ();
        }
        else if (e1->getStatus() == ITER_ON_GOING)
        {
            _got   = e1->getCurrentIndex();
            _total = e1->getTotalNumber();

            if (_total > 0)
            {
                _currentPercentage = (float) _got / (float) _total;

                //float orfPercentage = (_currentFrame + _currentPercentage) / (_nbSubjectFrames * _nbQueryFrames);
                float orfPercentage = (_currentPercentage);

                _globalPercentage =  (_currentAlgo + orfPercentage) / _totalAlgo;

                if ( ((size_t) 100*_currentPercentage) >= _percentThreshold)
                {
                    dump ();
                    _percentThreshold += 1;  // could be configurable
                }
            }
        }
        return;
    }

    AlgorithmConfigurationEvent* e2 = dynamic_cast<AlgorithmConfigurationEvent*> (evt);
    if (e2 != 0)
    {
        _currentAlgo = e2->_current;
        _totalAlgo   = e2->_total;
        return;
    }

    AlgorithmReportEvent* e3 = dynamic_cast<AlgorithmReportEvent*> (evt);
    if (e3 != 0)
    {
        _currentFrame++;

        os::impl::TimeInfo* info = e3->getTimeInfo();
        if (info != 0)
        {
            _ellapsedTime  = info->getEntryByKey ("reading") + info->getEntryByKey ("algorithm");
            _remainingTime = _globalPercentage > 0 ?  ((1-_globalPercentage)*_ellapsedTime) / _globalPercentage : 0;
        }

        IAlignmentContainer*  alignmentResult  = e3->getAlignmentContainer();
        if (e3 != 0)
        {
            _currentNbAlignments = alignmentResult->getAlignmentsNumber();
            _nbAlignments += _currentNbAlignments;
        }

        /** We get an approximation of the used memory at the end of the algorithm. */
        _usedMemory = DefaultFactory::memory().getMemUsage();
        if (_usedMemory > _maxUsedMemory)  {  _maxUsedMemory = _usedMemory; }
        _totalUsedMemory += _usedMemory;

        /** We force the display of the information. */
        dump ();

        return;
    }

    AlgorithmReadingFrameEvent* e4 = dynamic_cast<AlgorithmReadingFrameEvent*> (evt);
    if (e4 != 0)
    {
        _nbSubjectFrames = e4->_subjectFrames.empty() ? 1 : e4->_subjectFrames.size();
        _nbQueryFrames   = e4->_queryFrames.empty()   ? 1 : e4->_queryFrames.size();

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
void AbstractProgressionObserver::start   (void)
{
    _percentThreshold   = 0;
    _currentPercentage  = 0;
    _got                = 0;
    _total              = 0;
    _globalPercentage   = 0;
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
string AbstractProgressionObserver::getTimeString (u_int32_t time_msec)
{
    char buffer[256];

    u_int32_t time_sec = time_msec / 1000;

    size_t h = time_sec /3600;
    size_t m = (time_sec - 3600*h) / 60;
    size_t s = time_msec - 1000*(h*3600 + m*60);

    snprintf (buffer, sizeof(buffer), "%02ld:%02ld:%2.2f", h, m, (float)s/1000.0);

    return string(buffer);
}

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/
