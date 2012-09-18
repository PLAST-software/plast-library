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

/** \file FileProgressionObserver.hpp
 *  \brief Define common class for target observer implementations.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _FILE_PROGRESSION_OBSERVER_HPP
#define _FILE_PROGRESSION_OBSERVER_HPP

/********************************************************************************/

#include <launcher/observers/AbstractProgressionObserver.hpp>

#include <stdio.h>
#include <string>
#include <vector>

/********************************************************************************/
/** \brief PLAST command line. */
namespace launcher {
/** \brief Observers definitions */
namespace observers {
/********************************************************************************/

/** \brief  Dump the progression percentage into a file.
 *
 * This observer receives information about algorithm progression
 * and dumps these information into a XML file.
 *
 * This is useful for external clients (other applications) that want to know the
 * progression status of the algorithm. They can access to the XML file (here, the
 * file system serves as the common resource between PLAST and third parties programs).
 */
class FileProgressionObserver : public AbstractProgressionObserver
{
public:

    /** Constructor.
     * \param[in] filename : uri of the file where the percentage has to be dumped.
     */
    FileProgressionObserver (const std::string& filename);

    /** Destructor. */
    virtual ~FileProgressionObserver ();

protected:

    /** \copydoc AbstractProgressionObserver::dump */
    void dump (void);

    std::string _filename;
    size_t      _dumpablePercentage;
};


/********************************************************************************/

/** \brief Dump into a file information about HSP number, alignments number and execution time
 *
 * This observers reacts on the AlignmentProgressionEvent event. From this event, it keeps
 * the number of HSP, alignments and current execution time.
 *
 * During the destruction, the whole gathered information is dumped into a file (one line per
 * AlignmentProgressionEvent received):
 *  - column 1: raw index i (starting at 0)
 *  - column 2: percentage of algorithm execution (ie raw index i divided by total number of lines)
 *  - column 3: execution time at index i
 *  - column 4: number of HSP found at index i
 *  - column 5: number of alignments found at index i
 *  - column 6: percentage of execution time (ie execution time at index i divided by total time)
 *  - column 7: percentage of found HSP (ie number of HSP found at index i divided by total number of HSPs)
 *  - column 8: percentage of found alignments (ie number of alignments found at index i divided by total number of alignments)
 *
 *  Such statistics file makes it possible to trace graphs like column 6, 7 and 8 in function of column 2.
 *  One can then observe that many alignments are found quickly (read with a low percentage of execution, or in other
 *  words with a low percentage of processed seeds).
 */
class AlignmentProgressionObserver : public AbstractObserver
{
public:

    /** Constructor.
     * \param[in] filename : name of the file where to put the statistics information into.
     */
    AlignmentProgressionObserver (const std::string& filename);

    /** Destructor. */
    virtual ~AlignmentProgressionObserver ();

    /** \copydoc AbstractObserver::update */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

protected:

    /** File for dumping statistics. */
    FILE*     _file;

    /** Counts the number of received AlignmentProgressionEvent notifications. */
    u_int32_t _count;

    /** Keeps the time of reception of the first AlignmentProgressionEvent notification. */
    u_int32_t _t0;

    /** We may need some synchronizer for managing concurrent accesses. */
    os::ISynchronizer* _synchro;

    /** Container holding successive number of HSPs. */
    std::vector<u_int32_t> ungapAlignsNb;

    /** Container holding successive number of alignments. */
    std::vector<u_int32_t> gapAlignNb;

    /** Container holding successive execution times. */
    std::vector<float>     execTime;
};

/********************************************************************************/

/** \brief Dump into a file information about algorithm observables and resources observables
 *
 * This observer reacts on the AlgorithmReportEvent event. It dumps into a file several
 * information extracted from this event.
 */
class ResourcesObserver : public AbstractProgressionObserver
{
public:

    /** Constructor.
     * \param[in] filename : name of the file where to put the statistics information into.
     */
    ResourcesObserver (const std::string& filename);

    /** Destructor. */
    virtual ~ResourcesObserver ();

    /** \copydoc AbstractObserver::update */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

protected:
    FILE*     _file;
    size_t    _lastAlgo;
    u_int64_t _lastNbAlign;
    u_int32_t _lastTime;
    u_int32_t _totalTime;
    void dump (void) {}
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _FILE_PROGRESSION_OBSERVER_HPP */
