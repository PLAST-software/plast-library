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

/** \file IAlgoEvents.hpp
 *  \brief Define some information linked to notifications.
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  We define here a bunch of EventInfo subclasses that provide different kind
 *  of information about PLAST algorithm execution.
 */

#ifndef _IALGO_EVENTS_HPP_
#define _IALGO_EVENTS_HPP_

/********************************************************************************
    Definition of some dp::EventInfo classes used by the IAlgorithm for sending
    some information (about the algorithm itself) to potential listeners.
********************************************************************************/

#include <designpattern/api/IObserver.hpp>

#include <os/impl/TimeTools.hpp>

#include <database/api/ISequenceDatabase.hpp>

#include <algo/core/api/IAlgorithm.hpp>

#include <alignment/core/api/IAlignmentContainer.hpp>

/********************************************************************************/
/** \brief PLAST algorithm concepts. */
namespace algo {
/** \brief Concept for configuring and running PLAST. */
namespace core {
/********************************************************************************/

/** \brief Enumerate different PLAST iteration states. */
enum EventProgressionStatus_e
{
    ENUM_STARTED,
    ENUM_ONGOING,
    ENUM_FINISHED
};

/********************************************************************************/

/** \brief Provides iteration status
 *
 * This class provides notification information for the current state of the iteration
 * in the PLAST algorithm (iteration here means that PLAST iterates hits between the two
 * compared databases). Note that an informative textual message is available.
 */
class AlgoEventWithStatus : public dp::EventInfo
{
public:

    /** Constructor.
     * \param[in] message : informative message
     * \param[in] status  : status of the hits based iteration.
     */
    AlgoEventWithStatus (const char* message, EventProgressionStatus_e status)
        : dp::EventInfo(0), _message(message), _status (status)
    {}

    /** A informative textual message. */
    std::string _message;

    /** Status of the PLAST algorithm. */
    EventProgressionStatus_e _status;
};

/********************************************************************************/

/** \brief Provides information about the algorithm.
 *
 * This class provides notification information about a IAlgorithm instance. In particular,
 * a notification carrying a AlgorithmReportEvent instance is sent when the PLAST algorithm
 * is finished (or a part of the algorithm if it is composed of several IAlgorithm instances).
 *
 * Provided information is various here:
 *      - the algorithm itself
 *      - subject and query databases
 *      - time statistics about algorithm execution (time for indexing, iterating...)
 *      - the list of ungap alignments
 *      - the list of gap alignments (of interest for the end user).
 */
class AlgorithmReportEvent : public dp::EventInfo
{
public:

    /** Constructor.
     * \param[in] algo : the algorithm concerned by the notification
     * \param[in] subjectDb : the subject database
     * \param[in] queryDb : the query database
     * \param[in] timeInfo : time statistics about algorithm execution
     * \param[in] ungapResult : list of ungap alignments
     * \param[in] alignmentResult : list of gap alignments
     */
    AlgorithmReportEvent (
        algo::core::IAlgorithm*                 algo,
        database::ISequenceDatabase*            subjectDb,
        database::ISequenceDatabase*            queryDb,
        os::impl::TimeInfo*                     timeInfo,
        alignment::core::IAlignmentContainer*   ungapResult,
        alignment::core::IAlignmentContainer*   alignmentResult,
        alignment::filter::IAlignmentFilter*    alignmentFilter
    )
        : dp::EventInfo(0), _algo(0), _subjectDb(0), _queryDb(0), _timeInfo (0), _ungapResult(0), _alignmentResult(0), _alignmentFilter(0)
    {
        setAlgo             (algo);
        setSubjectDb        (subjectDb);
        setQueryDb          (queryDb);
        setTimeInfo         (timeInfo);
        setUngapResult      (ungapResult);
        setAlignmentResult  (alignmentResult);
        setAlignmentFilter  (alignmentFilter);
    }

    /** Destructor. */
    virtual ~AlgorithmReportEvent ()
    {
        setAlgo             (0);
        setSubjectDb        (0);
        setQueryDb          (0);
        setTimeInfo         (0);
        setUngapResult      (0);
        setAlignmentResult  (0);
        setAlignmentFilter  (0);
    }

    /** Getter for the algorithm.
     * \return the algorithm
     */
    algo::core::IAlgorithm*         getAlgo            ()  { return _algo;             }

    /** Getter for the subject database.
     * \return the subject database
     */
    database::ISequenceDatabase*    getSubjectDb       ()  { return _subjectDb;        }

    /** Getter for the query database.
     * \return the query database
     */
    database::ISequenceDatabase*    getQueryDb         ()  { return _queryDb;          }

    /** Getter for the time statistics.
     * \return the time statistics
     */
    os::impl::TimeInfo*             getTimeInfo        ()  { return _timeInfo;         }

    /** Getter for the list of ungap alignments.
     *  \return list of ungap alignments
     */
    alignment::core::IAlignmentContainer* getUngapContainer     ()  { return _ungapResult;      }

    /** Getter for the list of gap alignments.
     * \return list of ungap alignments
     */
    alignment::core::IAlignmentContainer* getAlignmentContainer ()  { return _alignmentResult;  }

    /** Getter for the alignments filter (if any)
     * \return the filter
     */
    alignment::filter::IAlignmentFilter* getAlignmentFilter ()  { return _alignmentFilter;  }

private:
    algo::core::IAlgorithm* _algo;
    void setAlgo (algo::core::IAlgorithm* algo)  { SP_SETATTR (algo); }

    database::ISequenceDatabase* _subjectDb;
    void setSubjectDb (database::ISequenceDatabase* subjectDb)  { SP_SETATTR (subjectDb); }

    database::ISequenceDatabase* _queryDb;
    void setQueryDb (database::ISequenceDatabase* queryDb)  { SP_SETATTR (queryDb); }

    os::impl::TimeInfo* _timeInfo;
    void setTimeInfo (os::impl::TimeInfo* timeInfo)  { SP_SETATTR (timeInfo); }

    alignment::core::IAlignmentContainer* _ungapResult;
    void setUngapResult (alignment::core::IAlignmentContainer* ungapResult)  { SP_SETATTR(ungapResult); }

    alignment::core::IAlignmentContainer* _alignmentResult;
    void setAlignmentResult (alignment::core::IAlignmentContainer* alignmentResult)  { SP_SETATTR(alignmentResult); }

    alignment::filter::IAlignmentFilter* _alignmentFilter;
    void setAlignmentFilter (alignment::filter::IAlignmentFilter* alignmentFilter)  { SP_SETATTR(alignmentFilter); }
};

/********************************************************************************/

/** \brief Provides information about the ungap/gap alignments list.
 *
 * This class provides notification information the ungap and gap alignments list.
 *
 * It serves as a decorator for a IterationStatusEvent instance (by adding ungap/gap
 * alignments information).
 */
class AlignmentProgressionEvent : public dp::EventInfo
{
public:

    /** Constructor.
     * \param[in] iterateEvent : the information to be decorated
     * \param[in] ungapResult  : the list of ungap alignments
     * \param[in] gapResult    : the list of gap alignments
     */
    AlignmentProgressionEvent (
        dp::IterationStatusEvent*       iterateEvent,
        alignment::core::IAlignmentContainer* ungapResult,
        alignment::core::IAlignmentContainer* gapResult
    )
        : dp::EventInfo(0), _iterateEvent(0), _ungapResult(0), _gapResult(0)
    {
        setIterateEvent (iterateEvent);
        setUngapResult  (ungapResult);
        setGapResult    (gapResult);
    }

    /** Destructor. */
    virtual ~AlignmentProgressionEvent ()
    {
        setIterateEvent (0);
        setUngapResult  (0);
        setGapResult    (0);
    }

    /** Getter on the decorated event
     * \return the decorated event.
     */
    dp::IterationStatusEvent*      getIterateEvent ()  { return _iterateEvent;   }

    /** Getter on the ungap alignments list.
     * \return the ungap alignments list.
     */
    alignment::core::IAlignmentContainer* getUngapResult  ()  { return _ungapResult;    }

    /** Getter on the gap alignments list.
     * \return the gap alignments list.
     */
    alignment::core::IAlignmentContainer* getGapResult    ()  { return _gapResult;      }

private:
    dp::IterationStatusEvent* _iterateEvent;
    void setIterateEvent (dp::IterationStatusEvent* iterateEvent)  { SP_SETATTR(iterateEvent); }

    alignment::core::IAlignmentContainer* _ungapResult;
    void setUngapResult (alignment::core::IAlignmentContainer* ungapResult)  { SP_SETATTR(ungapResult); }

    alignment::core::IAlignmentContainer* _gapResult;
    void setGapResult (alignment::core::IAlignmentContainer* gapResult)  { SP_SETATTR(gapResult); }
};

/********************************************************************************/

/** \brief Provides information about the gap alignments list.
 *
 */
class AlignmentsContainerEvent : public dp::EventInfo
{
public:

    /** Constructor.
     * \param[in] iterateEvent : the information to be decorated
     * \param[in] ungapResult  : the list of ungap alignments
     * \param[in] gapResult    : the list of gap alignments
     */
    AlignmentsContainerEvent (alignment::core::IAlignmentContainer* container)
        : dp::EventInfo(0), _container(0)
    {
        setContainer (container);
    }

    /** Destructor. */
    virtual ~AlignmentsContainerEvent ()
    {
        setContainer (0);
    }

    /** Getter on the container.
     * \return the container.
     */
    alignment::core::IAlignmentContainer* getContainer    ()  { return _container;      }

private:
    alignment::core::IAlignmentContainer* _container
    ;
    void setContainer (alignment::core::IAlignmentContainer* container)  { SP_SETATTR(container); }
};

/********************************************************************************/

/** \brief Provides information about Open Reading Frames (ORF)
 *
 * This class provides notification information the Open Reading Frames (ORF) used
 * by the algorithm for the subject and query databases.
 */
class AlgorithmReadingFrameEvent : public dp::EventInfo
{
public:

    /** Constructor.
     * \param[in] algo : the algorithm
     * \param[in] subjectFrames : ORF for the subject database
     * \param[in] queryFrames   : ORF for the query database
     * */
    AlgorithmReadingFrameEvent (
        algo::core::IAlgorithm* algo,
        const std::vector<misc::ReadingFrame_e>& subjectFrames,
        const std::vector<misc::ReadingFrame_e>& queryFrames
    )
    :  dp::EventInfo(0), _subjectFrames(subjectFrames), _queryFrames(queryFrames)  {}

    /** ORF for the subject database */
    const std::vector<misc::ReadingFrame_e> _subjectFrames;

    /** ORF for the query database */
    const std::vector<misc::ReadingFrame_e> _queryFrames;
};

/********************************************************************************/

/** \brief Notification that a cancel is requested.
 */
class CancelRequestedEvent : public dp::EventInfo
{
public:

    /** Constructor.  */
    CancelRequestedEvent () :  dp::EventInfo(1) {}
};

/********************************************************************************/

/** \brief Notification that provides time information
 */
class TimeInfoEvent : public dp::EventInfo
{
public:

    /** Constructor.  */
    TimeInfoEvent (const std::string& title, os::impl::TimeInfo* timeInfo)
        :  dp::EventInfo(0), _title(title), _timeInfo(0)   { setTimeInfo   (timeInfo);  }

    /** Destructor. */
    ~TimeInfoEvent ()   { setTimeInfo (0);  }

    /** */
    std::string getTitle ()  { return _title; }

    /** */
    os::impl::TimeInfo* getTimeInfo ()  { return _timeInfo; }

private:

    std::string _title;

    os::impl::TimeInfo* _timeInfo;
    void setTimeInfo (os::impl::TimeInfo* timeInfo)  { SP_SETATTR (timeInfo); }
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_EVENTS_HPP_ */
