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

#include <os/impl/DefaultOsFactory.hpp>

#include <misc/api/macros.hpp>

#include <database/impl/ReadingFrameSequenceDatabase.hpp>

#include <algo/align/impl/BasicAlignmentResult.hpp>

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
using namespace database::impl;

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
static inline bool isIncluded (const Alignment& a1, const Alignment& a2)
{
    char delta = 0; //MIN (a1._length, a2._length) / 100;

    return (a1._subjectDb == a2._subjectDb)  &&  (a1._queryDb == a2._queryDb)  &&

    (      a1._subjectStartInSeq >= (a2._subjectStartInSeq - delta)
       &&  a1._subjectEndInSeq   <= (a2._subjectEndInSeq   + delta) ) &&

    (      a1._queryStartInSeq   >= (a2._queryStartInSeq - delta)
       &&  a1._queryEndInSeq     <= (a2._queryEndInSeq   + delta)
    );
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
static inline bool isIncluded (
    int32_t subjectStartInSeq,
    int32_t queryStartInSeq,
    int32_t subjectEndInSeq,
    int32_t queryEndInSeq,
    const Alignment& a2
)
{
    char delta = 0; //MIN (a1._length, a2._length) / 100;

    return
    (      subjectStartInSeq >= (a2._subjectStartInSeq - delta)
       &&  subjectEndInSeq   <= (a2._subjectEndInSeq   + delta) ) &&

    (      queryStartInSeq   >= (a2._queryStartInSeq - delta)
       &&  queryEndInSeq     <= (a2._queryEndInSeq   + delta)
    );
}

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

        found = isIncluded (align, current);
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
    const indexation::ISeedOccurrence* subOccur,
    const indexation::ISeedOccurrence* qryOccur,
    size_t bandLength
)
{
    bool found = false;

    LocalSynchronizer local (_synchro);

    /** We look for the instance corresponding to the current query sequence. */
    SubjectEntries& queryEntry = _queryEntries [qryOccur->sequence.index];

    /** Optimization. */
    if (queryEntry.empty())  { return false; }

    /** We look for the subject entry. */
    std::map<u_int32_t,AlignmentsContainer>::iterator it = queryEntry.find (subOccur->sequence.index);

    /** Optimization. */
    if (it == queryEntry.end())  { return false; }

    /** Now, we should have the correct list. */
    AlignmentsContainer& l = it->second;

    /** Optimization. */
    if (l.empty())  { return false; }

    size_t subLen = subOccur->sequence.data.letters.size;
    size_t qryLen = qryOccur->sequence.data.letters.size;

    int32_t subStartInSeq = (subOccur->offsetInSequence >= bandLength ? subOccur->offsetInSequence - bandLength : 0);
    int32_t qryStartInSeq = (qryOccur->offsetInSequence >= bandLength ? qryOccur->offsetInSequence - bandLength : 0);
    int32_t subEndInSeq   = (subOccur->offsetInSequence  + bandLength <  subLen ? subOccur->offsetInSequence + bandLength : subLen-1);
    int32_t qryEndInSeq   = (qryOccur->offsetInSequence  + bandLength <  qryLen ? qryOccur->offsetInSequence + bandLength : qryLen-1);

    for (AlignmentsContainer::iterator itAlign = l.begin();  !found &&  itAlign != l.end(); itAlign++)
    {
        found = isIncluded (subStartInSeq, qryStartInSeq, subEndInSeq, qryEndInSeq, *itAlign);
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

    for (AlignmentsContainer::iterator itAlign = container.begin(); !alreadyKnown && itAlign != container.end(); itAlign++)
    {
        alreadyKnown =  isIncluded (a1, *itAlign);

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
bool BasicAlignmentResult::mysortfunction (const Alignment& i, const Alignment& j)
{
    return (i._evalue <  j._evalue)  ||
           (i._evalue == j._evalue  &&  i._bitscore < j._bitscore);
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
                        if (a1._bitscore > a2._bitscore)
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
                        if (a1._bitscore > a2._bitscore)
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

    } /* end of for (size_t queryIdx=0; queryIdx<_queryEntries.size(); queryIdx++) */
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BasicAlignmentResult::accept (IAlignmentResultVisitor* visitor)
{
    /** We loop the query sequences. */
    for (size_t i=0; i<_queryEntries.size(); i++)
    {
        /** We retrieve the query sequence. */
        ISequence query;
        if (_queryDb->getSequenceByIndex (i, query) == false)  { continue; }

        /** We call the visitor. */
        visitor->visitQuerySequence (&query);

        /** We retrieve the subject entries. */
        SubjectEntries& subjects = _queryEntries[i];

        /** We loop the subjects entries. */
        for (SubjectEntries::iterator itSubjects = subjects.begin(); itSubjects != subjects.end(); itSubjects++)
        {
            ISequence subject;
            if (_subjectDb->getSequenceByIndex (itSubjects->first, subject) == false)  { continue; }

            /** We call the visitor. */
            visitor->visitSubjectSequence (&subject);

            /** We loop over alignments. */
            AlignmentsContainer& alignments = itSubjects->second;
            for (AlignmentsContainer::iterator itAlign = alignments.begin(); itAlign != alignments.end(); itAlign++)
            {
                /** We get a copy of the currently iterated alignment (copy because we may have to modify it). */
                Alignment align = *itAlign;

                ReadingFrameSequenceDatabase* subjectRF = dynamic_cast<ReadingFrameSequenceDatabase*> (align._subjectDb);
                ReadingFrameSequenceDatabase* queryRF   = dynamic_cast<ReadingFrameSequenceDatabase*> (align._queryDb);

                /** Alignment information have been computed by comparing two proteins.
                 *  Now, we may have to go back to the initial nucleotid alphabet, so we have to translate
                 *  alignment information.
                 *
                 *  IMPORTANT: be aware that variables 'query' and 'subject' represent sequence in protein alphabet...
                 */

                if (subjectRF != 0)
                {
                    /** We retrieve the corresponding nucleotid sequence. It is required for having the exact size of the
                     *  initial nucleotid sequence.
                     */
                    ISequence nucleotidSubject;
                    if (subjectRF->getNucleotidDatabase()->getSequenceByIndex (itSubjects->first, nucleotidSubject) == false)  { continue; }

                    size_t subjectLength = align._subjectEndInSeq - align._subjectStartInSeq + 1;

                    /** We convert [start,end] in terms of nucleotid sequence. */
                    align._subjectStartInSeq = 3*align._subjectStartInSeq + subjectRF->getFrameShift();
                    align._subjectEndInSeq   = align._subjectStartInSeq + 3*subjectLength - 1;

                    /** We may have to reverse the indexes according to the ORF. */
                    if (subjectRF->isTopFrame() == false)
                    {
                        align._subjectStartInSeq = nucleotidSubject.data.letters.size - align._subjectStartInSeq - 1;
                        align._subjectEndInSeq   = nucleotidSubject.data.letters.size - align._subjectEndInSeq   - 1;
                    }
                }

                if (queryRF != 0  &&  queryRF->getNucleotidDatabase() != 0)
                {
                    /** We retrieve the corresponding nucleotid sequence. It is required for having the exact size of the
                     *  initial nucleotid sequence.
                     */
                    ISequence nucleotidQuery;
                    if (queryRF->getNucleotidDatabase()->getSequenceByIndex (i, nucleotidQuery) == false)  { continue; }

                    size_t queryLength = align._queryEndInSeq - align._queryStartInSeq + 1;

                    /** We convert [start,end] in terms of nucleotid sequence. */
                    align._queryStartInSeq = 3*align._queryStartInSeq + queryRF->getFrameShift();
                    align._queryEndInSeq   = align._queryStartInSeq + 3*queryLength - 1;

                    /** We may have to reverse the indexes according to the ORF. */
                    if (queryRF->isTopFrame() == false)
                    {
                        align._queryStartInSeq = nucleotidQuery.data.letters.size - align._queryStartInSeq - 1;
                        align._queryEndInSeq   = nucleotidQuery.data.letters.size - align._queryEndInSeq   - 1;
                    }
                }

                /** We call the visitor. */
                visitor->visitAlignment (&align);

            } /* end of for (AlignmentsContainer::iterator itAlign ... */

        } /* end of for (SubjectEntries::iterator itSubjects = ... */

    }  /* end of for (size_t i=0; i<_queryEntries.size(); i++) */

    /** We may want to have extra process to be done by the visitor at the end of the visit. */
    visitor->finish();
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
