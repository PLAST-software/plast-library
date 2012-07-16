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

/** \file types.hpp
 *  \brief types definition for PLAST.
 *  \date 07/11/2011
 *  \author edrezen
 *
 *   We define here some types.
 */

#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#include <iostream>

#if defined(__LINUX__) or defined (__DARWIN__)
    #include <sys/types.h>
#else
    # define __intN_t(N, MODE)      typedef int int##N##_t              __attribute__ ((__mode__ (MODE)))
    # define __u_intN_t(N, MODE)    typedef unsigned int u_int##N##_t   __attribute__ ((__mode__ (MODE)))

    __intN_t (8, __QI__);
    __intN_t (16, __HI__);
    __intN_t (32, __SI__);
    __intN_t (64, __DI__);

    __u_intN_t (8, __QI__);
    __u_intN_t (16, __HI__);
    __u_intN_t (32, __SI__);
    __u_intN_t (64, __DI__);
#endif

/** */
typedef unsigned long long Offset;

typedef u_int32_t Size;

/** \namespace misc
 * \brief Miscellanous definitions
 */

/********************************************************************************/
namespace misc {
/********************************************************************************/

/** \brief Definition of an interval (inspired by std::pair).
 */
template <class T> struct Range
{
    typedef T type;

    T begin;
    T end;
    Range() : begin(T()), end(T()) {}
    Range(const T& x, const T& y) : begin(x), end(y) {}
    template <class U>  Range (const Range<U> &p) : begin(p.begin), end(p.end) { }
    T getLength ()  const  { return (end >= begin ? end - begin + 1 : begin - end + 1); }

    /** Tells whether the provided 'r' range is included into the 'this' instance. */
    bool includes (const Range<T>& r, u_int8_t delta=0) const
    {
        return ((r.begin + delta) >= this->begin)  &&  (r.end <= (this->end + delta));
    }

    /** Tells whether the provided value is included into the 'this' instance. */
    bool includes (const T& val) const   {   return (this->begin <= val)  &&  (val <= this->end);  }

    /** Dilatation  of a range by some factor. */
    Range inflate (float factor)
    {
        T delta = (T) ( (1-factor)/2.0 * (float)getLength());
        return Range (begin+delta, end-delta);
    }

    /** Shift. */
    Range shift (T t)  {  return Range (begin+t, end+t);  }

    /** */
    friend std::ostream& operator<< (std::ostream& s, const Range& o)
    {
        return s << '[' << o.begin << ':' << o.end << ']';
    }

    /** */
    friend std::istream& operator>> (std::istream& s, Range& o)
    {
        char c;
        return s >> c >> o.begin >> c >> o.end >> c;
    }

    bool operator== (const Range& r) const
    {
        return begin==r.begin && end==r.end;
    }

    bool operator!= (const Range& r) const
    {
        return begin!=r.begin || end!=r.end;
    }
};

/** Some shortcuts. */
typedef Range<int32_t>  Range32;
typedef Range<u_int64_t>  Range64;

/** Define a pair for [rank;number] that gives information about iteration progression. */
struct ProgressInfo
{
    u_int32_t rank;
    u_int32_t number;

    ProgressInfo (u_int32_t rk=0, u_int32_t nb=0) : rank(rk), number(nb) {}
    void set (u_int32_t rk, u_int32_t nb)  { rank = rk;  number = nb; }
    ProgressInfo& operator++()  { rank++;  return *this; }

    static ProgressInfo& null ()  { static ProgressInfo instance; return instance; }

    /** */
    friend std::ostream& operator<< (std::ostream& s, const ProgressInfo& o)
    {
        return s << '[' << o.rank << '/' << o.number << ']';
    }

    /** */
    friend std::istream& operator>> (std::istream& s, ProgressInfo& o)
    {
        char c;
        return s >> c >> o.rank >> c >> o.number >> c;
    }
};

/** */
enum ReadingFrame_e
{
    FRAME_1,
    FRAME_2,
    FRAME_3,
    FRAME_4,
    FRAME_5,
    FRAME_6
};

/** */
enum AlgoKind_e
{
    ENUM_PLASTP,
    ENUM_TPLASTN,
    ENUM_PLASTX,
    ENUM_TPLASTX,
    ENUM_PLASTN
};

/** */
enum SeedModelKind_e
{
    ENUM_BasicSeedModel,
    ENUM_SubSeedModel
};

/** */
enum ScoreMatrixKind_e
{
    ENUM_BLOSUM62,
    ENUM_BLOSUM50,
    ENUM_NUCLEOTIDE_IDENTITY
};

/** */
enum AlgoInitializerKind_e
{
    ENUM_AlgoInitializer,
    ENUM_AlgoInitializerSortedSeeds
};

/** */
enum UngapHitIteratorKind_e
{
    ENUM_UngapHitIterator,
    ENUM_UngapHitIteratorSSE16,
    ENUM_UngapHitIteratorSSE8,
    ENUM_UngapHitIteratorNull
};

/** */
enum SmallGapHitIteratorKind_e
{
    ENUM_SmallGapHitIterator,
    ENUM_SmallGapHitIteratorSSE8,
    ENUM_SmallGapHitIteratorNull
};

/** */
enum CommandDispatcherKind_e
{
    ENUM_DispatcherSerial,
    ENUM_DispatcherParallel
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _TYPES_HPP_ */
