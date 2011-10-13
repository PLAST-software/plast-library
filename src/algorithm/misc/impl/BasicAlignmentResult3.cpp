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

#include "BasicAlignmentResult3.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include "macros.hpp"

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace os;

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicAlignmentResult3::BasicAlignmentResult3 (size_t nbQuerySequences)
    : _nbAlignments(0)
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicAlignmentResult3::~BasicAlignmentResult3 ()
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool BasicAlignmentResult3::doesExist (
    const indexation::ISeedOccurrence* subjectOccur,
    const indexation::ISeedOccurrence* queryOccur
)
{
    return false;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool BasicAlignmentResult3::insert (Alignment& align, void* context)
{
    bool result = false;

    /** We have to protect the containers against concurrent accesses. */
    LocalSynchronizer local (_synchro);

    /** We build a pair holding query/subject sequences indexes of the alignment. */
    QuerySubjectIndexes idx (align._queryIdx, align._subjectIdx);

    /** We look for the hits for the specific query/subject.  */
    Entries::iterator it = _entries.find (idx);

    if (it == _entries.end())
    {
        _entries[idx].insert (pair<float,Alignment> (align._evalue, align));
        result = true;
    }
    else
    {
        AlignmentsContainer& l = it->second;

        bool alreadyKnown = false;
        for (AlignmentsContainer::iterator itAlign = l.begin(); !alreadyKnown && itAlign != l.end(); itAlign++)
        {
            Alignment& current = itAlign->second;

            /** We allow some tolerance about the boundaries of the two compared alignments. */
            int32_t shift = MIN (align._length, current._length) / 20;

            alreadyKnown =
                (       align._subjectStartInSeq + shift >= current._subjectStartInSeq - shift
                    &&  align._subjectEndInSeq   - shift <= current._subjectEndInSeq   + shift )  &&
                (       align._queryStartInSeq   + shift >= current._queryStartInSeq   - shift
                    &&  align._queryEndInSeq     - shift <= current._queryEndInSeq     + shift
                );

        } /* end of for (AlignmentsList::iterator itAlign ... */

        if (!alreadyKnown)   {  l.insert (pair<float,Alignment> (align._evalue, align)); }

        /** We set the result status. */
        result = ! alreadyKnown;
    }

    /** We may have to increase the number of alignments; */
    if (result == true)  {  _nbAlignments++;  }

    /** We return the result. */
    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BasicAlignmentResult3::shrink (void)
{
    size_t count = 0;

    LocalSynchronizer local (_synchro);

    for (Entries::iterator it = _entries.begin(); it != _entries.end(); it++)
    {
        AlignmentsContainer& container = it->second;

        if (container.size() <= 1)  { continue; }

        char* removeTable = 0;

        removeTable = (char*) MemoryAllocator::singleton().malloc (container.size());
        memset (removeTable, 0, container.size());

        size_t k=0;
        for (AlignmentsContainer::iterator itAlign = container.begin(); itAlign != container.end(); itAlign++, k++)
        {
            Alignment& a1 = itAlign->second;

            if (removeTable[k] != 0)  { continue; }

            size_t l=k+1;
            AlignmentsContainer::iterator itAlign2 = itAlign;
            for (itAlign2++; itAlign2 != container.end(); itAlign2++, l++)
            {
                if (removeTable[l] != 0)  { continue; }

                Alignment& a2 = itAlign2->second;

                size_t shift = MIN (a1._length, a2._length) / 20;
//shift = 0;

                if ((a1._subjectEndInSeq   + shift >= a2._subjectEndInSeq   - shift) &&
                    (a1._subjectStartInSeq - shift <= a2._subjectStartInSeq + shift) &&
                    (a1._queryEndInSeq     + shift >= a2._queryEndInSeq     - shift) &&
                    (a1._queryStartInSeq   - shift <= a2._queryStartInSeq   + shift) )
                {
                    if (a1._length > a2._length)
                    {
//                            printf ("1. BEST %s \n", a1.toString().c_str());
//                            printf ("1. WEAK %s \n", a2.toString().c_str());
//                            printf ("\n");
                        removeTable[l] = 1;
                    }
                    else
                    {
//                            printf ("2. BEST %s \n", a2.toString().c_str());
//                            printf ("2. WEAK %s \n", a1.toString().c_str());
//                            printf ("\n");
                        removeTable[k] = 1;
                    }
                }

                else if ((a2._subjectEndInSeq   + shift >= a1._subjectEndInSeq   - shift) &&
                         (a2._subjectStartInSeq - shift <= a1._subjectStartInSeq + shift) &&
                         (a2._queryEndInSeq     + shift >= a1._queryEndInSeq     - shift) &&
                         (a2._queryStartInSeq   - shift <= a1._queryStartInSeq   + shift) )
                {
                    if (a1._length > a2._length)
                    {
//                            printf ("3. BEST %s \n", a1.toString().c_str());
//                            printf ("3. WEAK %s \n", a2.toString().c_str());
//                            printf ("\n");
                        removeTable[l] = 1;
                    }
                    else
                    {
//                            printf ("4. BEST %s \n", a2.toString().c_str());
//                            printf ("4. WEAK %s \n", a1.toString().c_str());
//                            printf ("\n");
                        removeTable[k] = 1;
                    }
                }
            }

        }  /* end of for (size_t k=0; k<container.size(); k++) */

#if 1
        size_t idx = 0;
        AlignmentsContainer::iterator tmp;
        for (AlignmentsContainer::iterator it = container.begin(); it != container.end(); )
        {
            if (removeTable[idx] == 1)
            {
                tmp = it;
                ++ tmp;

                //printf ("remove\n");
                count++;
                _nbAlignments--;
                container.erase (it);
                it = tmp;
            }
            else
            {
                it++;
            }

            idx++;
        }
#endif
        for (size_t k=0; k<container.size(); k++)  {  if (removeTable[k] == 1)  { count++; }  }

        MemoryAllocator::singleton().free (removeTable);
    }

    size_t imin=100000, imax=0, sum=0;

    for (Entries::iterator it = _entries.begin(); it != _entries.end(); it++)
    {
        AlignmentsContainer& container = it->second;
        size_t s = container.size();
        sum += s;

        if (s > imax) { imax = s; }
        if (s < imin) { imin = s; }
    }

//    printf ("BasicAlignmentResult3::shrink  nbAlignments=%ld  count=%ld  diff=%ld  min=%ld max=%ld\n",
//        _nbAlignments, count, (_nbAlignments-count), imin, imax);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BasicAlignmentResult3::accept (AlignmentResultVisitor* visitor)
{
    for (Entries::iterator it = _entries.begin(); it != _entries.end(); it++)
    {
        QuerySubjectIndexes idx = it->first;
    }
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
