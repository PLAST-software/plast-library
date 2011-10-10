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

/********************************************************************************/

#include <stdio.h>

/********************************************************************************/

#define CHECKPTR(ptr)  ((ptr) != 0)

#define DEBUG(a)  //printf a

#define MIN(a,b)  ((a) < (b) ? (a) :  (b))
#define MAX(a,b)  ((a) < (b) ? (b) :  (a))
#define ABS(a)    ((a) > 0   ? (a) : -(a))

/********************************************************************************/

#endif /* _MACROS_HPP_ */
