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

#include "BasicAlignmentResult.hpp"
#include "ReadingFrameSequenceDatabase.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include "macros.hpp"

#include <stdio.h>
#define DEBUG(a) // printf a

using namespace std;
using namespace dp;
using namespace os;
using namespace database;

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
BasicAlignmentResult::BasicAlignmentResult (ISequenceDatabase* subjectDb, ISequenceDatabase* queryDb)
    : _subjectDb(0), _queryDb(0), _nbAlignments(0),
      _shiftDivisor (20)
{
    setSubjectDb (subjectDb);
    setQueryDb   (queryDb);

    /** We resize the vector holding query entries. */
    if (_queryDb != 0)  {  _queryEntries.resize (_queryDb->getSequencesNumber());  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicAlignmentResult::~BasicAlignmentResult ()
{
    setSubjectDb (0);
    setQueryDb   (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool BasicAlignmentResult::doesExist (const Alignment& align)
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

        found =
            (align._subjectStartInSeq >= current._subjectStartInSeq  &&  align._subjectEndInSeq <= current._subjectEndInSeq)  &&
            (align._queryStartInSeq   >= current._queryStartInSeq    &&  align._queryEndInSeq   <= current._queryEndInSeq);
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
bool BasicAlignmentResult::doesExist (
    const indexation::ISeedOccurrence* subjectOccur,
    const indexation::ISeedOccurrence* queryOccur
)
{
    bool found = false;

//    LocalSynchronizer local (_synchro);

    /** We look for the instance corresponding to the current query sequence. */
    SubjectEntries& queryEntry = _queryEntries [queryOccur->sequence.index];

    /** Optimization. */
    if (queryEntry.empty())  { return false; }

    /** We look for the subject entry. */
    std::map<u_int32_t,AlignmentsContainer>::iterator it = queryEntry.find (subjectOccur->sequence.index);

    /** Optimization. */
    if (it == queryEntry.end())  { return false; }

    /** Now, we should have the correct list. */
    AlignmentsContainer& l = it->second;

    /** Optimization. */
    if (l.empty())  { return false; }

    for (AlignmentsContainer::iterator itAlign = l.begin(); itAlign != l.end(); itAlign++)
    {
#if 0
        Alignment& current = *itAlign;
        found =
            (align._subjectStartInSeq >= current._subjectStartInSeq  &&  align._subjectEndInSeq <= current._subjectEndInSeq)  &&
            (align._queryStartInSeq >= current._queryStartInSeq  &&  align._queryEndInSeq <= current._queryEndInSeq);
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
bool BasicAlignmentResult::insert (Alignment& align, void* context)
{
    bool result = false;

    /** We have to protect the containers against concurrent accesses. */
    LocalSynchronizer local (_synchro);

    /** We look for the instance corresponding to the current query sequence. */
    SubjectEntries& queryEntry = _queryEntries [align._queryIdx];

    /** Optimization. */
    if (queryEntry.empty())
    {
        /** We look for the subject entry. */
        queryEntry [align._subjectIdx].push_back (align);

        result = true;
    }
    else
    {
        /** We look for the hits for the specific query/subject.  */
        std::map<u_int32_t,AlignmentsContainer>::iterator it = queryEntry.find (align._subjectIdx);

        if (it == queryEntry.end())
        {
            queryEntry [align._subjectIdx].push_back (align);

            result = true;
        }
        else
        {
            result = insertInContainer (align, context, it->second);
        }
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
bool BasicAlignmentResult::insertInContainer (Alignment& a1, void* context, AlignmentsContainer& container)
{
    bool alreadyKnown = false;

    for (AlignmentsContainer::iterator itAlign = container.begin(); !alreadyKnown && itAlign != container.end(); )
    {
        Alignment& a2 = *itAlign;

#if 0

        size_t shift = MIN (a1._length, a2._length) / _shiftDivisor;

        if ((a1._subjectEndInSeq   + shift >= a2._subjectEndInSeq   - shift) &&
            (a1._subjectStartInSeq - shift <= a2._subjectStartInSeq + shift) &&
            (a1._queryEndInSeq     + shift >= a2._queryEndInSeq     - shift) &&
            (a1._queryStartInSeq   - shift <= a2._queryStartInSeq   + shift) )
        {
            itAlign = container.erase (itAlign);
            _nbAlignments --;
        }

        else if ((a2._subjectEndInSeq  + shift >= a1._subjectEndInSeq   - shift) &&
                (a2._subjectStartInSeq - shift <= a1._subjectStartInSeq + shift) &&
                (a2._queryEndInSeq     + shift >= a1._queryEndInSeq     - shift) &&
                (a2._queryStartInSeq   - shift <= a1._queryStartInSeq   + shift) )
        {
            alreadyKnown = true;
        }
        else
        {
            itAlign++;
        }

#else

#if 1
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
        itAlign++;
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
static bool mysortfunction (const Alignment& i, const Alignment& j)
{
    return (i._evalue <  j._evalue)  ||
           (i._evalue == j._evalue  &&  i._bitscore < j._bitscore);

//    return (i._queryStartInDb< j._queryStartInDb) ||
//           (i._queryStartInDb==j._queryStartInDb  &&  i._queryEndInDb<j._queryEndInDb);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BasicAlignmentResult::shrink (void)
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

            removeTable = (char*) MemoryAllocator::singleton().malloc (container.size());
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

            MemoryAllocator::singleton().free (removeTable);
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

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BasicAlignmentResult::accept (AlignmentResultVisitor* visitor)
{
    ReadingFrameSequenceDatabase* subjectRF = dynamic_cast<ReadingFrameSequenceDatabase*> (_subjectDb);
    ReadingFrameSequenceDatabase* queryRF   = dynamic_cast<ReadingFrameSequenceDatabase*> (_queryDb);

    ISequenceDatabase* actualSubjectDb = (subjectRF ? subjectRF->getNucleotidDatabase() : _subjectDb);
    ISequenceDatabase* actualQueryDb   = (queryRF   ? queryRF->getNucleotidDatabase()   : _queryDb);

    /** We loop the query sequences. */
    for (size_t i=0; i<_queryEntries.size(); i++)
    {
        /** We retrieve the query sequence. */
        ISequence query;
        if (actualQueryDb->getSequenceByIndex (i, query))
        {
            /** We call the visitor. */
            visitor->visitQuerySequence (&query);

            /** We retrieve the subject entries. */
            SubjectEntries& subjects = _queryEntries[i];

            /** We loop the subjects entries. */
            for (SubjectEntries::iterator itSubjects = subjects.begin(); itSubjects != subjects.end(); itSubjects++)
            {
                ISequence subject;
                if (actualSubjectDb->getSequenceByIndex (itSubjects->first, subject))
                {
                    /** We call the visitor. */
                    visitor->visitSubjectSequence (&subject);

                    /** We loop over alignments. */
                    AlignmentsContainer& alignments = itSubjects->second;
                    for (AlignmentsContainer::iterator itAlign = alignments.begin(); itAlign != alignments.end(); itAlign++)
                    {
                        /** We get a copy of the currently iterated alignment (copy because we may have to modify it). */
                        Alignment align = *itAlign;

                        /** Alignment information have been computed by comparing two proteins.
                         *  Now, we may have to go back to the initial nucleotid alphabet, so we have to translate
                         *  alignment information */

                        if (subjectRF != 0)
                        {
                            size_t subjectLength = align._subjectEndInSeq - align._subjectStartInSeq + 1;

                            /** We convert [start,end] in terms of nucleotid sequence. */
                            align._subjectStartInSeq = 3*align._subjectStartInSeq + subjectRF->getFrameShift();
                            align._subjectEndInSeq   = align._subjectStartInSeq + 3*subjectLength - 1;

                            /** We may have to reverse the indexes according to the ORF. */
                            if (subjectRF->isTopFrame() == false)
                            {
                                align._subjectStartInSeq = subject.data.letters.size - align._subjectStartInSeq - 1;
                                align._subjectEndInSeq   = subject.data.letters.size - align._subjectEndInSeq   - 1;
                            }
                        }

                        if (queryRF != 0)
                        {
                            size_t queryLength = align._queryEndInSeq - align._queryStartInSeq + 1;

                            /** We convert [start,end] in terms of nucleotid sequence. */
                            align._queryStartInSeq = 3*align._queryStartInSeq + queryRF->getFrameShift();
                            align._queryEndInSeq   = align._queryStartInSeq + 3*queryLength - 1;

                            /** We may have to reverse the indexes according to the ORF. */
                            if (queryRF->isTopFrame() == false)
                            {
                                align._queryStartInSeq = query.data.letters.size - align._queryStartInSeq - 1;
                                align._queryEndInSeq   = query.data.letters.size - align._queryEndInSeq   - 1;
                            }
                        }

                        /** We call the visitor. */
                        visitor->visitAlignment (&align);
                    }
                }
            }
        }
    }
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
