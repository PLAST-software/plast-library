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

/** \file ObsfucatedString.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief String obfuscation
 */

#include <string>
#include <sys/types.h>

/********************************************************************************/
/** \brief PLAST command line. */
namespace misc {
/** \brief Tools (command line options management) */
namespace impl {
/********************************************************************************/

/** \brief Class that can encode/decode strings
 *
 * This class provides a simple algorithm for encoding strings, in order to make
 * them unreadable by humans.
 *
 * A string is encoded as an array of integers. Note that it uses a custom (and simple)
 * pseudo-random numbers generator.
 */
class ObsfucatedString 
{
public:

    /** Define a specific integer type for this class. */
    typedef unsigned long long LONG;

    /** Constructor.
     * \param[in] l : the first integer elements of the encoding array.
     * \param[in] ... : the rest of the integers of the encoding array.
     */
    ObsfucatedString (LONG l, ...);

    /** Destructor. */
    ~ObsfucatedString () {}

    /** Getting clear string.
     * \return the clear string.
     */
    std::string toString ()  { return s; }

    /** Obfuscation.
     * \param[in] s : the (clear) strings to be encoded.
     * \return the string holding the encoding array
     */
    static std::string obsfucate (const std::string& s);

private:

    /** The clear string. */
    std::string s;

    static void toBytes (LONG l, char* bytes, size_t length, int off);

    static LONG toLong (const char* bytes, size_t length, int off);

    /** Initialization of the pseudo random numbers generator.
     * \param[in] s : the initial seed of the generator.
     */
    static void srand (LONG s) { seed = s; }

    /** The initial seed of the pseudo random numbers generator. */
    static LONG seed;

    /** Return a pseudo random number. */
    static LONG rand ();
};

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/
