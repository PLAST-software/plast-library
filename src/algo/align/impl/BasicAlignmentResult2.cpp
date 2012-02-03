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

#include <database/api/ISequence.hpp>

#include <algo/align/impl/BasicAlignmentResult2.hpp>

#include <iostream>
#include <vector>
#include <algorithm>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace os;
using namespace os::impl;
using namespace database;

/********************************************************************************/
namespace algo   {
namespace align {
namespace impl   {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicAlignmentResult2::BasicAlignmentResult2 (size_t nbQuerySequences)
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
BasicAlignmentResult2::BasicAlignmentResult2 (vector<string>& subjectComments,  vector<string>& queryComments)
    : _nbAlignments(0), _subjectComments(subjectComments), _queryComments(queryComments)
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
BasicAlignmentResult2::~BasicAlignmentResult2 ()
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
bool BasicAlignmentResult2::doesExist (
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
bool BasicAlignmentResult2::insert (Alignment& align, void* context)
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
        _entries[idx].push_back (align);
        result = true;
    }
    else
    {
        AlignmentsContainer& l = it->second;

        bool alreadyKnown = false;
        for (AlignmentsContainer::iterator itAlign = l.begin(); !alreadyKnown && itAlign != l.end(); itAlign++)
        {
            Alignment& current = *itAlign;

            alreadyKnown =
                (       align._subjectStartInSeq == current._subjectStartInSeq
                    &&  align._subjectEndInSeq   == current._subjectEndInSeq   )  &&
                (       align._queryStartInSeq   == current._queryStartInSeq
                    &&  align._queryEndInSeq     == current._queryEndInSeq
                );

        } /* end of for (AlignmentsList::iterator itAlign ... */

        if (!alreadyKnown)   {  l.push_back (align);  }

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
void BasicAlignmentResult2::shrink (void)
{
    size_t count = 0;

    LocalSynchronizer local (_synchro);

    for (Entries::iterator it = _entries.begin(); it != _entries.end(); it++)
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

                size_t shift = MIN (a1._length, a2._length) / 20;

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

    for (Entries::iterator it = _entries.begin(); it != _entries.end(); it++)
    {
        AlignmentsContainer& container = it->second;
        sort (container.begin(), container.end(), mysortfunction);
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

//    printf ("BasicAlignmentResult2::shrink  nbAlignments=%ld  count=%ld  diff=%ld  min=%ld max=%ld\n",
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
void BasicAlignmentResult2::accept (IAlignmentResultVisitor* visitor)
{
    ISequence querySeq;
    ISequence subjectSeq;

    querySeq.index   = ~0;
    subjectSeq.index = ~0;

    for (Entries::iterator it = _entries.begin(); it != _entries.end(); it++)
    {
        QuerySubjectIndexes  idx       = it->first;
        AlignmentsContainer& container = it->second;

        if (querySeq.index != idx.first)
        {
            querySeq.index   = idx.first;
            subjectSeq.index = ~0;  // need to reset the subject index for a new found query
            querySeq.comment =  _queryComments.empty()==false ? _queryComments[querySeq.index].c_str() : "query?";
            visitor->visitQuerySequence (&querySeq);
        }

        if (subjectSeq.index != idx.second)
        {
            subjectSeq.index = idx.second;
            subjectSeq.comment = _subjectComments.empty()==false ? _subjectComments[subjectSeq.index].c_str() : "subject?";
            visitor->visitSubjectSequence (&subjectSeq);
        }

        for (size_t k=0; k<container.size(); k++)
        {
            Alignment& a = container[k];

            visitor->visitAlignment (&a);
        }
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BasicAlignmentResult2::readFromFile (
    const char* fileuri,
    std::map<std::string,int>& subjectComments,
    std::map<std::string,int>& queryComments
)
{
    AbstractAlignmentResult::readFromFile (fileuri, subjectComments, queryComments);

    if (_subjectComments.empty ())
    {
        _subjectComments.resize (subjectComments.size());

        for (map<string,int>::iterator it = subjectComments.begin(); it != subjectComments.end(); it++)
        {
            _subjectComments [it->second] = it->first;
        }
    }

    if (_queryComments.empty ())
    {
        _queryComments.resize (queryComments.size());

        for (map<string,int>::iterator it = queryComments.begin(); it != queryComments.end(); it++)
        {
            _queryComments [it->second] = it->first;
        }
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
list<Alignment> BasicAlignmentResult2::getAlignments (const ISequence* querySeq, const ISequence* subjectSeq)
{
    list<Alignment> result;

    if (querySeq && subjectSeq)
    {
        pair <u_int32_t,u_int32_t> p (querySeq->index, subjectSeq->index);

        Entries::iterator it = _entries.find (p);
        if (it != _entries.end())
        {
            for (vector<Alignment>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                result.push_back (*it2);
            }
        }
    }

    return result;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
