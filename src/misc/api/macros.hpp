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

/** Mininum of two values. */
#define MIN(a,b)  ((a) < (b) ? (a) :  (b))

/** Maximum of two values. */
#define MAX(a,b)  ((a) < (b) ? (b) :  (a))

/** Absolute value. */
#define ABS(a)    ((a) > 0   ? (a) : -(a))

/** Size of an array (computed through sizeof). */
#define ARRAYSIZE(t)  (sizeof(t) / sizeof(t[0]))

#define CHAR_TO_INT32(x1,x2,x3,x4)	  ((((u_int32_t)x1<<24)&0xFF000000) | (((u_int32_t)x2<<16)&0x00FF0000) | \
									   (((u_int32_t)x3<<8)&0x0000FF00) | (((u_int32_t)x4)&0x000000FF))

#define CHAR_TO_INT64(x1,x2,x3,x4,x5,x6,x7,x8) \
									((((u_int64_t)x1<<56)&0xFF00000000000000) | (((u_int64_t)x2<<48)&0x00FF000000000000) | \
									 (((u_int64_t)x3<<40)&0x0000FF0000000000) | (((u_int64_t)x4<<32)&0x000000FF00000000) | \
									 (((u_int64_t)x5<<24)&0x00000000FF000000) | (((u_int64_t)x6<<16)&0x0000000000FF0000) | \
									 (((u_int64_t)x7<<8)&0x000000000000FF00)  |       (((u_int64_t)x8)&0x00000000000000FF))

/** Some macros for add checks in the code. When this flag is activated, the flags
 *  CHECK_BLOCK_BEGIN and CHECK_BLOCK_END are then used to include some code intended
 *  to control internals of the execution, and eventually launch an exception if something
 *  strange is happening.
 *
 *  Note that this control blocks may take some time at execution, so, when a version seems
 *  to be ok with the WITH_CHECK activated, one could choose to deactivate it in order to
 *  gain some execution time.
 *
 *  By default, the flag WITH_CHECK is activated.
 */
#define WITH_CHECK

#ifdef WITH_CHECK
    #define CHECK_BLOCK_BEGIN  if (true) {
    #define CHECK_BLOCK_END    }
#else
    #define CHECK_BLOCK_BEGIN  if (false)  {
    #define CHECK_BLOCK_END    }
#endif

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _MACROS_HPP_ */
