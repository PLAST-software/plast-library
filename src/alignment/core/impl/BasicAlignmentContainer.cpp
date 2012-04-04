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

#include <alignment/core/impl/BasicAlignmentContainer.hpp>

#include <iostream>
#include <vector>
#include <algorithm>

#include <stdio.h>
#define DEBUG(a) //printf a

using namespace std;
using namespace dp;
using namespace os;
using namespace os::impl;
using namespace database;

/********************************************************************************/
namespace alignment {
namespace core      {
namespace impl      {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicAlignmentContainer::BasicAlignmentContainer ()
    :  _nbSeqLevel1(0), _nbSeqLevel2(0)
{
    DEBUG (("BasicAlignmentContainer::BasicAlignmentContainer\n"));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicAlignmentContainer::~BasicAlignmentContainer ()
{
    DEBUG (("BasicAlignmentContainer::~BasicAlignmentContainer\n"));

    /** We have to get rid of all copied ISequence instances. */
    for (ContainerLevel1::iterator itLevel1 = _containerLevel1.begin(); itLevel1 != _containerLevel1.end();  itLevel1++)
    {
        /** Shortcuts. */
        ISequence*        seqLevel1       = (*itLevel1).second.first;
        ContainerLevel2&  containerLevel2 = (*itLevel1).second.second;

        if (seqLevel1)  {  delete seqLevel1;  }

        for (ContainerLevel2::iterator itLevel2 = containerLevel2.begin(); itLevel2 != containerLevel2.end(); itLevel2++)
        {
            /** Shortcuts. */
            ISequence* seqLevel2 = (*itLevel2).second.first;

            if (seqLevel2)  {  delete seqLevel2;  }
        }
    }

    _containerLevel1.clear ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool BasicAlignmentContainer::doesExist (const Alignment& align)
{
    bool found = false;

    /** We need to be protected against concurrent accesses. */
    LocalSynchronizer local (_synchro);

    /** We retrieve the list of alignments for the [query,subject] pair of the alignment. */
    ContainerLevel3* containerLevel3 = getContainer (align.getSequence(Alignment::QUERY), align.getSequence(Alignment::SUBJECT));

    found = isInContainer (containerLevel3, align.getRange(Alignment::SUBJECT), align.getRange(Alignment::QUERY));

    /** We return true if we found the provided alignment, false otherwise. */
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
bool BasicAlignmentContainer::doesExist (
    const indexation::ISeedOccurrence* subOccur,
    const indexation::ISeedOccurrence* qryOccur,
    u_int32_t bandLength
)
{
    bool found = false;

    /** We need to be protected against concurrent accesses. */
    LocalSynchronizer local (_synchro);

    /** We determine first and second level sequences for the search. */
    const ISequence* seqLevel1  = & (qryOccur->sequence);
    const ISequence* seqLevel2  = & (subOccur->sequence);

    ContainerLevel3* containerLevel3 = getContainer (seqLevel1, seqLevel2);
    if (containerLevel3 != 0)
    {
        size_t subLen = seqLevel2->data.letters.size;
        size_t qryLen = seqLevel1->data.letters.size;

        misc::Range32 sbjRange (
            subOccur->offsetInSequence >= bandLength           ? subOccur->offsetInSequence - bandLength : 0,
            subOccur->offsetInSequence  + bandLength <  subLen ? subOccur->offsetInSequence + bandLength : subLen-1
        );

        misc::Range32 qryRange (
            qryOccur->offsetInSequence >= bandLength           ? qryOccur->offsetInSequence - bandLength : 0,
            qryOccur->offsetInSequence  + bandLength <  qryLen ? qryOccur->offsetInSequence + bandLength : qryLen-1
        );

        found = isInContainer (containerLevel3, sbjRange, qryRange);
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
bool BasicAlignmentContainer::insert (Alignment& align, void* context)
{
    //if (_filter && _filter->isOk(align) == false)  {  return false;  }

    /** We need to be protected against concurrent accesses. */
    LocalSynchronizer local (_synchro);

    /** We determine first and second level sequences for the search. */
    ISequence* firstLevelSeq  = (ISequence*) align.getSequence(Alignment::QUERY);
    ISequence* secondLevelSeq = (ISequence*) align.getSequence(Alignment::SUBJECT);

    /** Some check. */
    if (firstLevelSeq == 0  ||  secondLevelSeq == 0)  { return false; }

    ContainerLevel2* containerLevel2 = 0;
    ContainerLevel3* containerLevel3 = 0;

    /** We look for the first level entry. */
    Key firstKey (firstLevelSeq->database,firstLevelSeq->index);
    ContainerLevel1::iterator lookFirstLevel = _containerLevel1.find (firstKey);
    if (lookFirstLevel == _containerLevel1.end())
    {
        /** We copy the provided sequence. */
        firstLevelSeq = new ISequence (*firstLevelSeq);
        _nbSeqLevel1 ++;

        /** We add a new pair into the map. */
        pair<ISequence*,ContainerLevel2>& p =
            _containerLevel1 [firstKey] = pair<ISequence*,ContainerLevel2> (firstLevelSeq, ContainerLevel2());

        /** We get a reference on the added map. */
        containerLevel2 = & (p.second);
    }
    else
    {
        /** We retrieve the sequence. */
        firstLevelSeq = lookFirstLevel->second.first;

        /** We found the inner map. */
        containerLevel2 = & (lookFirstLevel->second.second);
    }

    /** We look for the second level entry. */
    Key secondKey (secondLevelSeq->database,secondLevelSeq->index);
    ContainerLevel2::iterator lookSecondLevel = containerLevel2->find (secondKey);
    if (lookSecondLevel == containerLevel2->end())
    {
        /** We copy the provided sequence. */
        secondLevelSeq = new ISequence (*secondLevelSeq);
        _nbSeqLevel2 ++;

        /** We add a new pair into the map. */
        pair<ISequence*,ContainerLevel3>& p =
            (*containerLevel2) [secondKey] = pair<ISequence*,ContainerLevel3> (secondLevelSeq, ContainerLevel3());

        /** We get a reference on the added container. */
        containerLevel3 = & (p.second);
    }
    else
    {
        /** We retrieve the sequence. */
        secondLevelSeq = lookSecondLevel->second.first;

        /** We found the inner map. */
        containerLevel3 = & (lookSecondLevel->second.second);
    }

    /** We can update the alignment sequences references.
     *  WARNING ! it is important to update these two references, whatever the following
     *  'isInContainer' result is, since the current ISequence references in the alignment
     *  may be lost later.
     */
    align.setSequence (Alignment::QUERY, firstLevelSeq);
    align.setSequence (Alignment::SUBJECT, secondLevelSeq);

    /** Now, we have the list of alignments for the first and second levels entries. */
    bool found = isInContainer (containerLevel3, align.getRange(Alignment::SUBJECT), align.getRange(Alignment::QUERY));

    if (!found)
    {
        /** Finally, we can simply add the alignment into the container. */
        containerLevel3->push_back (align);

        _nbAlignments ++;
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
void BasicAlignmentContainer::accept (IAlignmentContainerVisitor* visitor)
{
    DEBUG (("BasicAlignmentContainer::accept  _nbSeqLevel1=%ld  _nbSeqLevel2=%ld\n", _nbSeqLevel1, _nbSeqLevel2));

    /** We need to be protected against concurrent accesses. */
    LocalSynchronizer local (_synchro);

    misc::ProgressInfo level1Progress (1, _containerLevel1.size ());

    for (ContainerLevel1::iterator itLevel1 = _containerLevel1.begin(); itLevel1 != _containerLevel1.end();  itLevel1++, ++level1Progress)
    {
        /** Shortcuts. */
        ISequence*        seqLevel1        = (*itLevel1).second.first;
        ContainerLevel2&  containerLevel2  = (*itLevel1).second.second;

        /** We call the visitor. */
        visitor->visitQuerySequence (seqLevel1, level1Progress);

        misc::ProgressInfo level2Progress (1, containerLevel2.size ());

        for (ContainerLevel2::iterator itLevel2 = containerLevel2.begin(); itLevel2 != containerLevel2.end(); itLevel2++, ++level2Progress)
        {
            /** Shortcuts. */
            ISequence*       seqLevel2       = (*itLevel2).second.first;
            ContainerLevel3& containerLevel3 = (*itLevel2).second.second;

            /** We call the visitor for the subject. */
            visitor->visitSubjectSequence (seqLevel2, level2Progress);

            /** We call the visitor for the list of alignments. */
            visitor->visitAlignmentsList (seqLevel1, seqLevel2, containerLevel3);
        }
    }

    /** We may want to have extra process to be done by the visitor at the end of the visit. */
    visitor->postVisit (this);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
std::list<Alignment>* BasicAlignmentContainer::getContainer (
    const database::ISequence* seqLevel1,
    const database::ISequence* seqLevel2
)
{
    if (seqLevel1 == 0  ||  seqLevel2 == 0)  { return 0; }

    /** We look for the first level entry. */
    Key firstKey (seqLevel1->database,seqLevel1->index);
    ContainerLevel1::iterator lookFirstLevel = _containerLevel1.find (firstKey);
    if (lookFirstLevel == _containerLevel1.end())  { return 0; }

    /** Shortcuts (more readable). */
    ContainerLevel2& containerLevel2 = (lookFirstLevel->second).second;

    /** We look for the second level entry. */
    Key secondKey (seqLevel2->database,seqLevel2->index);
    ContainerLevel2::iterator lookSecondLevel = containerLevel2.find (secondKey);

    if (lookSecondLevel == containerLevel2.end())  { return 0; }

    /** We return the result. */
    return  & ((lookSecondLevel->second).second);
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
