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

#ifndef _TYPES_HPP_
#define _TYPES_HPP_

/********************************************************************************/

#if 0
    #include <sys/types.h>
#else

# define __intN_t(N, MODE)      typedef int int##N##_t __attribute__ ((__mode__ (MODE)))
# define __u_intN_t(N, MODE)    typedef unsigned int u_int##N##_t __attribute__ ((__mode__ (MODE)))

__intN_t (8, __QI__);
__intN_t (16, __HI__);
__intN_t (32, __SI__);
__intN_t (64, __DI__);

__u_intN_t (8, __QI__);
__u_intN_t (16, __HI__);
__u_intN_t (32, __SI__);
__u_intN_t (64, __DI__);


//    typedef char                int8_t;
//    typedef short int           int16_t;
//    typedef int                 int32_t;
//    typedef long int            int64_t;
//
//    typedef unsigned char       u_int8_t;
//    typedef unsigned short int  u_int16_t;
//    typedef unsigned int        u_int32_t;
//    typedef unsigned long int   u_int64_t;
#endif

/********************************************************************************/

typedef unsigned long long Offset;

/********************************************************************************/
namespace types {
/********************************************************************************/

enum ReadingFrame_e
{
    FRAME_1,
    FRAME_2,
    FRAME_3,
    FRAME_4,
    FRAME_5,
    FRAME_6
};

enum AlgoKind_e
{
    ENUM_PLASTP,
    ENUM_TPLASTN,
    ENUM_PLASTX,
    ENUM_TPLASTX
};

enum SeedModelKind_e
{
    ENUM_BasicSeedModel,
    ENUM_SubSeedModel
};

enum ScoreMatrixKind_e
{
    ENUM_BLOSUM62,
    ENUM_BLOSUM50
};

enum AlgoInitializerKind_e
{
    ENUM_AlgoInitializer,
    ENUM_AlgoInitializerSortedSeeds
};

enum UngapHitIteratorKind_e
{
    ENUM_UngapHitIterator,
    ENUM_UngapHitIteratorSSE16,
    ENUM_UngapHitIteratorSSE8,
    ENUM_UngapHitIteratorNull
};

enum SmallGapHitIteratorKind_e
{
    ENUM_SmallGapHitIterator,
    ENUM_SmallGapHitIteratorSSE8,
    ENUM_SmallGapHitIteratorNull
};

enum CommandDispatcherKind_e
{
    ENUM_DispatcherSerial,
    ENUM_DispatcherParallel
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _TYPES_HPP_ */
