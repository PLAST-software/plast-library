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

/** \file BargraphObserver.hpp
 *  \brief Define common class for target observer implementations.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _BARGRAPH_OBSERVER_HPP
#define _BARGRAPH_OBSERVER_HPP

/********************************************************************************/

#include <launcher/observers/AbstractProgressionObserver.hpp>

#include <stdio.h>
#include <string>

/********************************************************************************/
/** \brief PLAST command line. */
namespace launcher {
/** \brief Observers definitions */
namespace observers {
/********************************************************************************/

/** \brief Observer that display a progression bargraph.
 *
 * The purpose of this observer is to retrieve the PLAST algorithm progression
 * and to display it as a progression bargraph on the console.
 *
 * This is useful because the PLAST algorithm can take a long time, so we can
 * have then a precise idea of what have been executed so far.
 */
class BargraphObserver : public AbstractProgressionObserver
{
public:

    /** Constructor.
     * \param[in] prefix : text to be displayed just before the bargraph.
     * \param[in] file   : file where to display the bargraph; stdout by default
     * \param[in] nbChar : number of characters displayed for the bargraph
     */
    BargraphObserver (const char* prefix="", FILE* file=stdout, size_t nbChar=0);

    /** Destructor. */
    virtual ~BargraphObserver ();

protected:

    /** \copydoc AbstractProgressionObserver::start */
    void start (void);

    /** \copydoc AbstractProgressionObserver::dump */
    void dump (void);

    FILE* _file;
    char* _output;
    std::string _prefix;

    size_t _nbChar;
    size_t _actualSize;

    void fillTimeBuffer (u_int32_t t, char* buffer);
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _BARGRAPH_OBSERVER_HPP */
