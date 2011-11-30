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

#ifndef _MACROS_HPP_
#define _MACROS_HPP_

/** \file macros.hpp
 *  \brief macros definition for PLAST.
 *  \date 07/11/2011
 *  \author edrezen
 *
 *   We define here a bunch of macros for easing every day work.
 */

/********************************************************************************/

#include <stdio.h>

/** \namespace misc
 * \brief Miscellanous definitions
 */

/********************************************************************************/
namespace misc {
/********************************************************************************/

/** Macro that checks validity of a pointer. */
#define CHECKPTR(ptr)  ((ptr) != 0)

/** Traces management. Minimal right now, should be improved. */
#define DEBUG(a)  //printf a

/** Mininum of two values. */
#define MIN(a,b)  ((a) < (b) ? (a) :  (b))

/** Maximum of two values. */
#define MAX(a,b)  ((a) < (b) ? (b) :  (a))

/** Absolute value. */
#define ABS(a)    ((a) > 0   ? (a) : -(a))

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _MACROS_HPP_ */
