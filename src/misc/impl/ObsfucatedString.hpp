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

/** \file ObsfucatedString.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief String obsfucation
 */

#include <string>
#include <sys/types.h>

/********************************************************************************/
/** \brief PLAST command line. */
namespace misc {
/** \brief Tools (command line options management) */
namespace impl {
/********************************************************************************/

/**
 */
class ObsfucatedString 
{
public:

    typedef u_int64_t LONG;

    /** Constructor. */
    ObsfucatedString (LONG l, ...);

    /** Destructor. */
    ~ObsfucatedString () {}

    /** Getting clear string. */
    std::string toString ()  { return s; }

    /** Obsfucation. */
    static std::string obsfucate (const std::string& s);

private:

    /** */
    std::string s;

    /** */
    static void toBytes (LONG l, char* bytes, size_t length, int off);

    /** */
    static LONG toLong (const char* bytes, size_t length, int off);

    /** */
    static void srand (LONG s) { seed = s; }
    static LONG seed;

    static LONG rand ();
};

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/
