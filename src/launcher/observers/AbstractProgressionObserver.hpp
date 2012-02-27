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

/** \file AbstractProgressionObserver.hpp
 *  \brief Abstract class for observer interested by PLAST algorithm progression
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ABSTRACT_PROGRESSION_OBSERVER_HPP
#define _ABSTRACT_PROGRESSION_OBSERVER_HPP

/********************************************************************************/

#include <launcher/observers/AbstractObserver.hpp>

#include <stdio.h>
#include <string>

/********************************************************************************/
/** \brief PLAST command line. */
namespace launcher {
/** \brief Observers definitions */
namespace observers {
/********************************************************************************/

/** \brief Abstract class for observer interested by PLAST algorithm progression
 *
 * The idea of this class is to maintain a set of attributes according to notifications
 * received from the PlastLibrary. These attributes are related to:
 *      - current and global percentage of algorithm execution
 *      - ellapsed time and remaining time
 *      - used memory
 *      - number of found alignments
 *      - opening reading frames information
 *
 * This class implements the update() method as listening the following notifications:
 *  - IterationStatusEvent
 *  - AlgorithmConfigurationEvent
 *  - AlgorithmReportEvent
 *  - AlgorithmReadingFrameEvent
 *
 * According to the received events, the update() method can call the primitives start()
 * and/or dump() (which means that we implement update() as a Template Method).
 *
 * The method start() is called when something is starting (like hits iteration).
 *
 * The method dump() is called when one of the attributes has changed.
 * Concrete implementions of dump() must be provided by subclasses. For instance, the
 * BargraphObserver implementation will dump some progression bargraph in the console where
 * the PLAST binary has been launched.
 */
class AbstractProgressionObserver : public AbstractObserver
{
public:

    /** Default constructor. */
    AbstractProgressionObserver ();

    /** \copydoc AbstractObserver::update */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

protected:

    /** Value giving the threshold under which one considers that something has changed.
     * This value is increased by one at each change, so it should reach 100 at the end.
     */
    size_t    _percentThreshold;

    /** Value telling how many hits have been iterated so far. Retrieved from the
     * IterationStatusEvent class.
     */
    u_int64_t _got;

    /** Value telling how many hits will be iterated. Retrieved from the
     * IterationStatusEvent class.
     */
    u_int64_t _total;

    /** Percentage ratio of the hits iterator. Actually, it is computed as \ref _got / \ref _total.
     */
    float _currentPercentage;

    /** The current hits iteration represents the PLAST one algorithm progression. Actually, running
     * PLAST may lead to the execution of several IAlgorithm instances. So we need to have a global
     * percentage of the whole execution, which is the purpose of \ref _globalPercentage.
     *
     * This ratio is a combination of the current algorithm progress ratio, of the total number of
     * algorithms to be executed. It may also depends on the fact we have several opening frames
     * to take into account.
     */
    float _globalPercentage;

    /** Index of the current IAlgorithm instance being executed. Retrieved from AlgorithmConfigurationEvent
     * event.
     */
    size_t _currentAlgo;

    /** Total number of IAlgorithm instances to be executed. Retrieved from AlgorithmConfigurationEvent
     * event.
     */
    size_t _totalAlgo;

    /** Provide the execution of the IAlgorithm instance that just finished.
     * Retrieved from AlgorithmReportEvent event.
     */
    u_int32_t _ellapsedTime;

    /** Estimation of the remaining time. It depends to the number of IAlgorithm instances to be executed.
     */
    u_int32_t _remainingTime;

    /** Gives the number of found alignments for the IAlgorithm instance that just finished.
     * Retrieved from AlgorithmReportEvent event.
     */
    u_int64_t _nbAlignments;

    /** Gives the memory usage at the end of an algorithm execution.
     */
    u_int32_t _usedMemory;

    /** Gives the maximum memory usage.
     */
    u_int64_t _maxUsedMemory;

    /** Gives the total memory usage.
     */
    u_int64_t _totalUsedMemory;

    /** Current frame number.
     */
    size_t _currentFrame;

    /** Number of reading frames to be used for the subject database.
     * Retrieved from AlgorithmReadingFrameEvent event.
     */
    size_t _nbSubjectFrames;

    /** Number of reading frames to be used for the query database.
     * Retrieved from AlgorithmReadingFrameEvent event.
     */
    size_t _nbQueryFrames;

    /** Primitive of the update() Template Method. Called when some hits iteration begins. */
    virtual void start   (void);

    /** Primitive of the update() Template Method. Called when some attributes have changed
     * after analyzing incoming notifications from PlastLibrary.
     */
    virtual void dump (void) = 0;

    /** Return a formatted string for a time given as seconds since 01.01.1970
     * \param[in] time_msec : the time in milli seconds
     * \return the formatted string.
     */
    std::string getTimeString (u_int32_t time_msec);
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_PROGRESSION_OBSERVER_HPP */
