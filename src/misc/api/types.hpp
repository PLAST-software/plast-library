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
 *   We define here some types used thoughout the code.
 *
 *   Important: we define typedefs such as int16_t or u_int64_t. It is a good idea to use such typedefs
 *   instead of direct 'unsigned long' or 'short' for instance, because the actual number of used bytes
 *   may depend on the operating system/architecture. Using u_int32_t for instance ensure that we get
 *   an unsigned integer on 4 bytes.
 *
 *   Note that we use the <sys/types.h> file on Linux and MacOs. Such file may not exist on Windows (on Mingw
 *   to be more precise), so we propose here a definition. This is not ideal and should be improved.
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

/** \brief Definition of an interval (inspired by std::pair). It is possible to define
 * 'reversed' range, ie. with a beginning greater than the end.
 */
template <class T> struct Range
{
    /** The template (integer) type. */
    typedef T type;

    /** Beginning of the range. */
    T begin;

    /** End of the range. */
    T end;

    /** Default constructor. */
    Range() : begin(T()), end(T()) {}

    /** Constructor taking the [begin,end] couple as arguments.
     * \param[in] x : beginning of the range.
     * \param[in] y : end of the range.
     */
    Range(const T& x, const T& y) : begin(x), end(y) {}

    /** Copy constructor. */
    template <class U>  Range (const Range<U> &p) : begin(p.begin), end(p.end) { }

    /** Returns the length of the range. */
    T getLength ()  const  { return (end >= begin ? end - begin + 1 : begin - end + 1); }

    /** Tells whether the provided 'r' range is included into the 'this' instance.
     * \param[in] r : range to be tested
     * \param[in] delta : not used (0 by default)
     * \return true if the provided range is inside of the current instance, false otherwise.
     */
    bool includes (const Range<T>& r, u_int8_t delta=0) const
    {
        return ((r.begin + delta) >= this->begin)  &&  (r.end <= (this->end + delta));
    }

    /** Tells whether the provided value is included into the 'this' instance.
     * \param[in] val : value to be tested
     * \return true if the provided value is inside the current range, false otherwise.
     */
    bool includes (const T& val) const   {   return (this->begin <= val)  &&  (val <= this->end);  }

    /** Dilatation  of a range by some factor.
     * \param[in] factor : dilatation factor
     * \return the dilated factor
     */
    Range inflate (float factor)
    {
        T delta = (T) ( (1-factor)/2.0 * (float)getLength());
        return Range (begin+delta, end-delta);
    }

    /** Shift the range from a given value.
     * \param[in] t : the shifting value
     * \return the shifted range.
     */
    Range shift (T t)  {  return Range (begin+t, end+t);  }

    /** Output stream for a Range object. */
    friend std::ostream& operator<< (std::ostream& s, const Range& o)
    {
        return s << '[' << o.begin << ':' << o.end << ']';
    }

    /** Input stream for a Range object. */
    friend std::istream& operator>> (std::istream& s, Range& o)
    {
        char c;
        return s >> c >> o.begin >> c >> o.end >> c;
    }

    /** Equality operator.
     * \param[in] r : the range to be compared to.
     * \return true if the ranges are the same (same beginning, same end), false otherwise. */
    bool operator== (const Range& r) const
    {
        return begin==r.begin && end==r.end;
    }

    /** InEquality operator.
     * \param[in] r : the range to be compared to.
     * \return false if the ranges are the same (same beginning, same end), true otherwise. */
    bool operator!= (const Range& r) const
    {
        return begin!=r.begin || end!=r.end;
    }
};

/** Define a range of signed 4 bytes integer. */
typedef Range<int32_t>    Range32;

/** Define a range of unsigned 8 bytes integer. */
typedef Range<u_int64_t>  Range64;

/** Define a range of unsigned 4 bytes integer. */
typedef Range<u_int32_t>  RangeU32;


/********************************************************************************/

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

/********************************************************************************/

/** Enumeration of possible ORF (opening read frames). */
enum ReadingFrame_e
{
    FRAME_1,
    FRAME_2,
    FRAME_3,
    FRAME_4,
    FRAME_5,
    FRAME_6
};

/** Enumeration of known PLAST algorithms. */
enum AlgoKind_e
{
    ENUM_PLASTP,
    ENUM_TPLASTN,
    ENUM_PLASTX,
    ENUM_TPLASTX,
    ENUM_PLASTN
};

/** Enumeration of different kinds of seeds models. */
enum SeedModelKind_e
{
    ENUM_BasicSeedModel,
    ENUM_SubSeedModel
};

/** Enumeration of different scoring matrixes. */
enum ScoreMatrixKind_e
{
    ENUM_BLOSUM62,
    ENUM_BLOSUM50,
    ENUM_NUCLEOTIDE_IDENTITY
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _TYPES_HPP_ */
