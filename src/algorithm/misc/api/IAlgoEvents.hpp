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

#ifndef _IALGO_EVENTS_HPP_
#define _IALGO_EVENTS_HPP_

/********************************************************************************
    Definition of some dp::EventInfo classes used by the IAlgorithm for sending
    some information (about the algorithm itself) to potential listeners.
********************************************************************************/

#include "Observer.hpp"
#include "IAlgorithm.hpp"
#include "ISequenceDatabase.hpp"
#include "ITime.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** */
enum EventProgressionStatus_e
{
    ENUM_STARTED,
    ENUM_ONGOING,
    ENUM_FINISHED
};

/********************************************************************************/

class AlgoEventWithStatus : public dp::EventInfo
{
public:
    AlgoEventWithStatus (const char* message, EventProgressionStatus_e status)
        : dp::EventInfo(0), _message(message), _status (status)
    {}

    std::string _message;
    EventProgressionStatus_e _status;
};

/********************************************************************************/

class AlgorithmReportEvent : public dp::EventInfo
{
public:
    AlgorithmReportEvent (
        algo::IAlgorithm*               algo,
        database::ISequenceDatabase*    subjectDb,
        database::ISequenceDatabase*    queryDb,
        os::TimeInfo*                   timeInfo,
        algo::IAlignmentResult*         ungapResult,
        algo::IAlignmentResult*         alignmentResult
    )
        : dp::EventInfo(0), _algo(0), _subjectDb(0), _queryDb(0), _timeInfo (0), _ungapResult(0), _alignmentResult(0)
    {
        setAlgo             (algo);
        setSubjectDb        (subjectDb);
        setQueryDb          (queryDb);
        setTimeInfo         (timeInfo);
        setUngapResult      (ungapResult);
        setAlignmentResult  (alignmentResult);
    }

    virtual ~AlgorithmReportEvent ()
    {
        setAlgo             (0);
        setSubjectDb        (0);
        setQueryDb          (0);
        setTimeInfo         (0);
        setUngapResult      (0);
        setAlignmentResult  (0);
    }

    algo::IAlgorithm*            getAlgo            ()  { return _algo;             }
    database::ISequenceDatabase* getSubjectDb       ()  { return _subjectDb;        }
    database::ISequenceDatabase* getQueryDb         ()  { return _queryDb;          }
    os::TimeInfo*                getTimeInfo        ()  { return _timeInfo;         }
    algo::IAlignmentResult*      getUngapResult     ()  { return _ungapResult;      }
    algo::IAlignmentResult*      getAlignmentResult ()  { return _alignmentResult;  }

private:
    algo::IAlgorithm* _algo;
    void setAlgo (algo::IAlgorithm* algo)  { SP_SETATTR (algo); }

    database::ISequenceDatabase* _subjectDb;
    void setSubjectDb (database::ISequenceDatabase* subjectDb)  { SP_SETATTR (subjectDb); }

    database::ISequenceDatabase* _queryDb;
    void setQueryDb (database::ISequenceDatabase* queryDb)  { SP_SETATTR (queryDb); }

    os::TimeInfo* _timeInfo;
    void setTimeInfo (os::TimeInfo* timeInfo)  { SP_SETATTR (timeInfo); }

    algo::IAlignmentResult* _ungapResult;
    void setUngapResult (algo::IAlignmentResult* ungapResult)  { SP_SETATTR(ungapResult); }

    algo::IAlignmentResult* _alignmentResult;
    void setAlignmentResult (algo::IAlignmentResult* alignmentResult)  { SP_SETATTR(alignmentResult); }
};

/********************************************************************************/

class AlignmentProgressionEvent : public dp::EventInfo
{
public:
    AlignmentProgressionEvent (
        dp::IterationStatusEvent* iterateEvent,
        algo::IAlignmentResult*   ungapResult,
        algo::IAlignmentResult*   gapResult
    )
        : dp::EventInfo(0), _iterateEvent(0), _ungapResult(0), _gapResult(0)
    {
        setIterateEvent (iterateEvent);
        setUngapResult  (ungapResult);
        setGapResult    (gapResult);
    }

    virtual ~AlignmentProgressionEvent ()
    {
        setIterateEvent (0);
        setUngapResult  (0);
        setGapResult    (0);
    }

    dp::IterationStatusEvent* getIterateEvent ()  { return _iterateEvent;   }
    algo::IAlignmentResult*   getUngapResult  ()  { return _ungapResult;    }
    algo::IAlignmentResult*   getGapResult    ()  { return _gapResult;      }

private:
    dp::IterationStatusEvent* _iterateEvent;
    void setIterateEvent (dp::IterationStatusEvent* iterateEvent)  { SP_SETATTR(iterateEvent); }

    algo::IAlignmentResult* _ungapResult;
    void setUngapResult (algo::IAlignmentResult* ungapResult)  { SP_SETATTR(ungapResult); }

    algo::IAlignmentResult* _gapResult;
    void setGapResult (algo::IAlignmentResult* gapResult)  { SP_SETATTR(gapResult); }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_EVENTS_HPP_ */
