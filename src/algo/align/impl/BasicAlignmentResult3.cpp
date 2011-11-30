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

#include <algo/align/impl/BasicAlignmentResult3.hpp>

#include <iostream>
#include <vector>
#include <algorithm>

#include <stdio.h>
#define DEBUG(a) // printf a

using namespace std;
using namespace dp;
using namespace os;
using namespace os::impl;
using namespace database;

/********************************************************************************/
namespace algo  {
namespace align {
namespace impl  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool BasicAlignmentResult3::insertInContainer (Alignment& a1, void* context, AlignmentsContainer& container)
{
    bool alreadyKnown = false;

    for (AlignmentsContainer::iterator itAlign = container.begin(); !alreadyKnown && itAlign != container.end(); itAlign++)
    {
        Alignment& a2 = *itAlign;

#if 0
        alreadyKnown =
            (       a1._subjectStartInSeq >= a2._subjectStartInSeq
                &&  a1._subjectEndInSeq   <= a2._subjectEndInSeq   )  &&
            (       a1._queryStartInSeq   >= a2._queryStartInSeq
                &&  a1._queryEndInSeq     <= a2._queryEndInSeq
            );

#else
        alreadyKnown =
            (       a1._subjectStartInSeq == a2._subjectStartInSeq
                &&  a1._subjectEndInSeq   == a2._subjectEndInSeq   )  &&
            (       a1._queryStartInSeq   == a2._queryStartInSeq
                &&  a1._queryEndInSeq     == a2._queryEndInSeq
            );
#endif

    } /* end of for (AlignmentsList::iterator itAlign ... */

    if (!alreadyKnown)   {  container.push_back (a1);  }

    return !alreadyKnown;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool BasicAlignmentResult3::doesExist (const Alignment& align)
{
    bool found = false;

    LocalSynchronizer local (_synchro);

    /** We look for the instance corresponding to the current query sequence. */
    SubjectEntries& queryEntry = _queryEntries [align._queryIdx];

    /** Optimization. */
    if (queryEntry.empty())  { return false; }

    /** We look for the subject entry. */
    std::map<u_int32_t,AlignmentsContainer>::iterator it = queryEntry.find (align._subjectIdx);

    /** Optimization. */
    if (it == queryEntry.end())  { return false; }

    /** Now, we should have the correct list. */
    AlignmentsContainer& l = it->second;

    /** Optimization. */
    if (l.empty())  { return false; }

    for (AlignmentsContainer::iterator itAlign = l.begin();  !found &&  itAlign != l.end(); itAlign++)
    {
        Alignment& current = *itAlign;

#if 1
        found = (Alignment::overlap (current, align) > 0.9)
                ||
                ((align._subjectStartInSeq >= current._subjectStartInSeq  &&  align._subjectEndInSeq <= current._subjectEndInSeq)  &&
                (align._queryStartInSeq   >= current._queryStartInSeq    &&  align._queryEndInSeq   <= current._queryEndInSeq));

#else
        found =
            (align._subjectStartInSeq >= current._subjectStartInSeq  &&  align._subjectEndInSeq <= current._subjectEndInSeq)  &&
            (align._queryStartInSeq   >= current._queryStartInSeq    &&  align._queryEndInSeq   <= current._queryEndInSeq);

        found =
            (align._subjectStartInSeq == current._subjectStartInSeq  &&  align._subjectEndInSeq == current._subjectEndInSeq)  &&
            (align._queryStartInSeq   == current._queryStartInSeq    &&  align._queryEndInSeq   == current._queryEndInSeq);

#endif
    }

    return found;
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

    for (size_t queryIdx=0; queryIdx<_queryEntries.size(); queryIdx++)
    {
        SubjectEntries& queryEntry = _queryEntries [queryIdx];

        if (queryEntry.empty())  { continue; }

        /** We loop over subject entries. */
        for (std::map<u_int32_t,AlignmentsContainer>::iterator it = queryEntry.begin();  it != queryEntry.end();  it++)
        {
            AlignmentsContainer& container = it->second;

            if (container.size() <= 1)  { continue; }

            char* removeTable = 0;

            removeTable = (char*) DefaultFactory::memory().malloc (container.size());
            memset (removeTable, 0, container.size());

            for (size_t k=0; k<container.size(); k++)
            {
                Alignment& a1 = container[k];

                if (removeTable[k] != 0)  { continue; }

                for (size_t l=k+1; l<container.size(); l++)
                {
                    if (removeTable[l] != 0)  { continue; }

                    Alignment& a2 = container[l];

                    size_t shift = MIN (a1._length, a2._length) / _shiftDivisor;

                    if ((a1._subjectEndInSeq   + shift >= a2._subjectEndInSeq   - shift) &&
                        (a1._subjectStartInSeq - shift <= a2._subjectStartInSeq + shift) &&
                        (a1._queryEndInSeq     + shift >= a2._queryEndInSeq     - shift) &&
                        (a1._queryStartInSeq   - shift <= a2._queryStartInSeq   + shift) )
                    {
                        if (a1._length > a2._length)
                        {
                            removeTable[l] = 1;
                        }
                        else
                        {
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
                            removeTable[l] = 1;
                        }
                        else
                        {
                            removeTable[k] = 1;
                        }
                    }
                }

            }  /* end of for (size_t k=0; k<container.size(); k++) */

#if 1
            size_t idx = 0;
            for (vector<Alignment>::iterator it = container.begin(); it != container.end(); )
            {
                if (removeTable[idx] == 1)
                {
                    //printf ("remove\n");
                    count++;
                    _nbAlignments--;
                    it = container.erase (it);
                }
                else
                {
                    it++;
                }

                idx++;
            }
#endif
            for (size_t k=0; k<container.size(); k++)  {  if (removeTable[k] == 1)  { count++; }  }

            DefaultFactory::memory().free (removeTable);
        }


        for (std::map<u_int32_t,AlignmentsContainer>::iterator it = queryEntry.begin();  it != queryEntry.end();  it++)
        {
            AlignmentsContainer& container = it->second;
            sort (container.begin(), container.end(), mysortfunction);
        }
    }

//    printf ("BasicAlignmentResult::shrink  nbAlignments=%ld  count=%ld  diff=%ld \n",
//        _nbAlignments, count, (_nbAlignments-count));
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
