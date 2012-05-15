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
 * and dumps the execution percentage into a file as an integer value in range [0:100]
 *
 * This is useful for external clients (other applications) that want to know the
 * progression status of the algorithm.
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

class AlignmentProgressionObserver : public AbstractObserver
{
public:

    AlignmentProgressionObserver (const std::string& filename);
    virtual ~AlignmentProgressionObserver ();

    void update (dp::EventInfo* evt, dp::ISubject* subject);

protected:
    FILE*     _file;
    u_int32_t _count;
    u_int32_t _t0;

    os::ISynchronizer* _synchro;

    std::vector<u_int32_t> ungapAlignsNb;
    std::vector<u_int32_t> gapAlignNb;
    std::vector<float>     execTime;
};

/********************************************************************************/

class ResourcesObserver : public AbstractProgressionObserver
{
public:

    ResourcesObserver (const std::string& filename);
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
