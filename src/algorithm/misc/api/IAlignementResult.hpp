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

#ifndef _IALIGNMENT_RESULT_HPP_
#define _IALIGNMENT_RESULT_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"
#include "Iterator.hpp"
#include "IOccurrenceIterator.hpp"

#include <list>
#include <map>
#include <iostream>
#include <sstream>
#include <set>
#include <string>

#include <stdio.h>

#define MAX(a,b)  ((a)>(b) ? (a) : (b))

/********************************************************************************/
namespace algo  {
/********************************************************************************/
class Alignment
{
public:

    Alignment (
        const indexation::ISeedOccurrence* occur1,
        const indexation::ISeedOccurrence* occur2,
        int leftOffsetInQuery,
        int leftOffsetInSubject,
        int rightOffsetInQuery,
        int rightOffsetInSubject,
        int score
    )  :   _occur1(occur1), _occur2(occur2), _score(score), _evalue(0), _bitscore(0), _nbGap(0), _identity(0), _nbMis(0)
    {
        configure (occur1, occur2, leftOffsetInQuery, leftOffsetInSubject, rightOffsetInQuery, rightOffsetInSubject);
    }

    Alignment (const indexation::ISeedOccurrence* occur1, const indexation::ISeedOccurrence* occur2)
        :  _occur1(occur1), _occur2(occur2), _score(0), _evalue(0), _bitscore(0), _nbGap(0), _identity(0), _nbMis(0)
    {
        configure (occur1, occur2, 0, 0, 0, 0);
    }

    Alignment ()  :  _score(0), _evalue(0), _bitscore(0), _nbGap(0), _identity(0), _nbMis(0)
    {
    }

    const indexation::ISeedOccurrence* _occur1;
    const indexation::ISeedOccurrence* _occur2;

    u_int32_t _subjectIdx;
    u_int32_t _queryIdx;

    int32_t _subjectStartInSeq;
    int32_t _queryStartInSeq;
    int32_t _subjectEndInSeq;
    int32_t _queryEndInSeq;

    int64_t _subjectStartInDb;
    int64_t _queryStartInDb;
    int64_t _subjectEndInDb;
    int64_t _queryEndInDb;

    u_int32_t _length;

    int _score;

    double _evalue;
    double _bitscore;

    size_t _nbGap;
    size_t _identity;
    size_t _nbMis;

    std::string toString ()  const
    {
        std::stringstream ss;

        ss << "[ALIGNMENT " <<
                "Q(" << _queryIdx   << ") " << _queryStartInSeq+1   << " " <<  _queryEndInSeq+1  <<
                "   " <<
                "S(" << _subjectIdx << ") " << _subjectStartInSeq+1 << " " <<  _subjectEndInSeq+1 <<
                "  len=" << _length << " nbgap=" << _nbGap << " identity=" << _identity << " nbMiss=" << _nbMis <<
                "]";

        return ss.str ();
    }

private:

    void configure (
        const indexation::ISeedOccurrence* occur1,
        const indexation::ISeedOccurrence* occur2,
        u_int32_t leftOffsetInQuery,
        u_int32_t leftOffsetInSubject,
        u_int32_t rightOffsetInQuery,
        u_int32_t rightOffsetInSubject
    )
    {
        /** Shortcuts. */
        u_int32_t size1 = occur1->sequence.data.letters.size;
        u_int32_t size2 = occur2->sequence.data.letters.size;

        /** Shortcuts. */
        int32_t offInSeq1 = occur1->offsetInSequence;
        int32_t offInSeq2 = occur2->offsetInSequence;

        _subjectIdx = occur1->sequence.index;
        _queryIdx   = occur2->sequence.index;

        _subjectStartInSeq = offInSeq1 - leftOffsetInSubject;
        _queryStartInSeq   = offInSeq2 - leftOffsetInQuery;

        _subjectEndInSeq   = offInSeq1 + rightOffsetInSubject;
        _queryEndInSeq     = offInSeq2 + rightOffsetInQuery;

        /** Shortcuts. */
        _subjectStartInDb = occur1->offsetInDatabase - leftOffsetInSubject;
        _queryStartInDb   = occur2->offsetInDatabase - leftOffsetInQuery;

        _subjectEndInDb   = occur1->offsetInDatabase + rightOffsetInSubject;
        _queryEndInDb     = occur2->offsetInDatabase + rightOffsetInQuery;

        _length = MAX (_subjectEndInSeq - _subjectStartInSeq + 1, _queryEndInSeq - _queryStartInSeq + 1);

#if 0
        printf ("NEW ALIGNMENT '%s'   size1=%ld  offInSeq1=%ld  rightOffsetInSubject=%ld  (%ld)\n",
            toString().c_str(),
            size1,
            offInSeq1,
            rightOffsetInSubject,
            (offInSeq1 + rightOffsetInSubject - 1)
        );
#endif
    }
};

/********************************************************************************/

/** We define a Design Pattern Visitor that can visit IAlignment result instances.
 */
class AlignmentResultVisitor : public dp::SmartPointer
{
public:
    virtual void visitQuerySequence   (const database::ISequence* seq)    = 0;
    virtual void visitSubjectSequence (const database::ISequence* seq)    = 0;
    virtual void visitAlignment       (const Alignment* align) = 0;
};

/********************************************************************************/

class IAlignmentResult : public dp::SmartPointer
{
public:

    virtual bool doesExist (
        const indexation::ISeedOccurrence* subjectOccur,
        const indexation::ISeedOccurrence* queryOccur) = 0;

    virtual bool doesExist (const Alignment& align) = 0;

    /** Insert an item. Returns true if insertion ok, false otherwise. */
    virtual bool insert (Alignment& align, void* context) = 0;

    virtual void shrink (void) = 0;

    virtual u_int32_t getSize () = 0;

    virtual void accept (AlignmentResultVisitor* visitor) {}

    /** Return properties about the instance. */
    virtual dp::IProperties* getProperties (const std::string& root, size_t nbDump) = 0;

    /** Read from a file. */
    virtual void readFromFile (
        const char* fileuri,
        std::map<std::string,int>& subjectComments,
        std::map<std::string,int>& queryComments
    ) = 0;
};

/********************************************************************************/
}; /* end of namespaces. */
/********************************************************************************/

#endif /* _IALIGNMENT_RESULT_HPP_ */
