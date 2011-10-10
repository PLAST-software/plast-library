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


#ifndef _IALGO_TYPES_HPP_
#define _IALGO_TYPES_HPP_

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/********************************************************************************/
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

#endif /* _IALGO_TYPES_HPP_ */
