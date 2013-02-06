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
BasicAlignmentContainer::BasicAlignmentContainer (size_t nbHitPerQuery, size_t nbHspPerHit)
    :  _nbSeqLevel1(0), _nbSeqLevel2(0), _nbHitPerQuery(nbHitPerQuery), _nbHspPerHit(nbHspPerHit)
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
        ContainerLevel2*  containerLevel2 = (*itLevel1).second.second;

        for (ContainerLevel2::iterator itLevel2 = containerLevel2->begin(); itLevel2 != containerLevel2->end(); itLevel2++)
        {
            /** Shortcuts. */
            ISequence*       seqLevel2       = (*itLevel2).second.first;
            ContainerLevel3* containerLevel3 = (*itLevel2).second.second;

            if (seqLevel2)        { delete seqLevel2;       }
            if (containerLevel3)  { delete containerLevel3; }
        }

        if (seqLevel1)        { delete seqLevel1;       }
        if (containerLevel2)  { delete containerLevel2; }
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
bool BasicAlignmentContainer::insertFirstLevel (const database::ISequence* firstLevelSeq)
{
    bool result = true;

    /** We look for the first level entry. */
    Key firstKey (firstLevelSeq->database,firstLevelSeq->index);
    ContainerLevel1::iterator lookFirstLevel = _containerLevel1.find (firstKey);
    if (lookFirstLevel == _containerLevel1.end())
    {
        /** We copy the provided sequence. */
        firstLevelSeq = new ISequence (*firstLevelSeq);
        _nbSeqLevel1 ++;

        /** We add a new pair into the map. */
        _containerLevel1 [firstKey] = ValueLevel1 ((ISequence*)firstLevelSeq, new ContainerLevel2());
    }

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
bool BasicAlignmentContainer::insert (Alignment& align, void* context)
{
    /** We need to be protected against concurrent accesses. */
    LocalSynchronizer local (_synchro);

    /** We determine first and second level sequences for the search. */
    ISequence* seqLevel1 = (ISequence*) align.getSequence(Alignment::QUERY);
    ISequence* seqLevel2 = (ISequence*) align.getSequence(Alignment::SUBJECT);

    /** Some check. */
    if (seqLevel1 == 0  ||  seqLevel2 == 0)  { return false; }

    ContainerLevel2* containerLevel2 = 0;
    ContainerLevel3* containerLevel3 = 0;

    /** We look for the first level entry. */
    Key firstKey (seqLevel1->database,seqLevel1->index);
    ContainerLevel1::iterator lookFirstLevel = _containerLevel1.find (firstKey);
    if (lookFirstLevel == _containerLevel1.end())
    {
        /** We copy the provided sequence. */
        seqLevel1       = new ISequence (*seqLevel1);
        containerLevel2 = new ContainerLevel2 ();
        _nbSeqLevel1 ++;

        /** We add a new pair into the map. */
        _containerLevel1 [firstKey] = ValueLevel1 (seqLevel1, containerLevel2);
    }
    else
    {
        /** We retrieve the sequence and the map. */
        seqLevel1       = lookFirstLevel->second.first;
        containerLevel2 = lookFirstLevel->second.second;
    }

    /** We look for the second level entry. */
    Key secondKey (seqLevel2->database,seqLevel2->index);
    ContainerLevel2::iterator lookSecondLevel = containerLevel2->find (secondKey);
    if (lookSecondLevel == containerLevel2->end())
    {
        /** We copy the provided sequence. */
        seqLevel2       = new ISequence (*seqLevel2);
        containerLevel3 = new ContainerLevel3 ();
        _nbSeqLevel2 ++;

        /** We add a new pair into the map. */
        (*containerLevel2) [secondKey] = ValueLevel2 (seqLevel2, containerLevel3);
    }
    else
    {
        /** We retrieve the sequence and the map. */
        seqLevel2       = lookSecondLevel->second.first;
        containerLevel3 = lookSecondLevel->second.second;
    }

    /** We can update the alignment sequences references.
     *  WARNING ! it is important to update these two references, whatever the following
     *  'isInContainer' result is, since the current ISequence references in the alignment
     *  may be lost later.
     */
    align.setSequence (Alignment::QUERY,   seqLevel1);
    align.setSequence (Alignment::SUBJECT, seqLevel2);

    /** Now, we have the list of alignments for the first and second levels entries. */
    bool found = isInContainer (containerLevel3, align.getRange(Alignment::SUBJECT), align.getRange(Alignment::QUERY));

    if (!found)
    {
        /** Finally, we can simply add the alignment into the container. */
        containerLevel3->push_back (align);

        _nbAlignments ++;
    }
    //else  {  printf ("BasicAlignmentContainer::insert FOUND\n"); }

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
void BasicAlignmentContainer::merge (const std::vector<IAlignmentContainer*> containers)
{
	for (size_t i=0; i<containers.size(); i++)
	{
		BasicAlignmentContainer* current = dynamic_cast<BasicAlignmentContainer*> (containers[i]);

		if (current == 0)  { continue; }

		for (ContainerLevel1::iterator itLevel1 = current->_containerLevel1.begin();
			itLevel1 != current->_containerLevel1.end();  ++itLevel1
		)
		{
	        ContainerLevel2*  containerLevel2  = (*itLevel1).second.second;

	        for (ContainerLevel2::iterator itLevel2 = containerLevel2->begin(); itLevel2 != containerLevel2->end(); ++itLevel2)
	        {
	            ContainerLevel3* containerLevel3 = (*itLevel2).second.second;

	            for (ContainerLevel3::iterator itLevel3 = containerLevel3->begin(); itLevel3 != containerLevel3->end(); ++itLevel3)
	            {
	            	this->insert (*itLevel3, 0);
	            }
	        }
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
        ContainerLevel2*  containerLevel2  = (*itLevel1).second.second;

        /** We call the visitor. */
        visitor->visitQuerySequence (seqLevel1, level1Progress);

        misc::ProgressInfo level2Progress (1, containerLevel2->size ());

        for (ContainerLevel2::iterator itLevel2 = containerLevel2->begin(); itLevel2 != containerLevel2->end(); itLevel2++, ++level2Progress)
        {
            /** Shortcuts. */
            ISequence*       seqLevel2       = (*itLevel2).second.first;
            ContainerLevel3* containerLevel3 = (*itLevel2).second.second;

            /** We call the visitor for the subject. */
            visitor->visitSubjectSequence (seqLevel2, level2Progress);

            /** We call the visitor for the list of alignments. */
            visitor->visitAlignmentsList (seqLevel1, seqLevel2, *containerLevel3);
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

/** We need a functor for sorting alignments. */
struct SortAlignmentsFunctor  { bool operator() (const Alignment& i, const Alignment& j)
{
    return i.getBitScore() > j.getBitScore();
}};

/** We need a functor for sorting hits. */
struct SortHitsFunctor  { bool operator() (
    const BasicAlignmentContainer::ContainerLevel3* i,
    const BasicAlignmentContainer::ContainerLevel3* j
)
{
    /** Shortcuts. */
    const Alignment& a = i->front();
    const Alignment& b = j->front();

    return (a.getBitScore() >  b.getBitScore()) ||
           (a.getBitScore() == b.getBitScore()  &&  a.getFrame(Alignment::SUBJECT) > b.getFrame(Alignment::SUBJECT));
}};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BasicAlignmentContainer::shrink ()
{
    /** We need to be protected against concurrent accesses. */
    LocalSynchronizer local (_synchro);

    SortHitsFunctor        sortHit;
    SortAlignmentsFunctor  sortAlign;

    /** The shrink process consists, for each query:
     *
     *      1) loop all matched subject and sort the alignments by evalue for each [query,subject]
     *         Note that, once sorted, we can keep only N best alignments (see _nbHspPerHit value)
     *
     *      2) once the alignments are sorted, we can sort the subjects by evalue
     *          (ie. evalue of the first alignment of the pair [query,subject].
     *
     *  Once everything is sorted (alignments and subjects), we can keep only N best subjects (see
     *  _nbHitPerQuery value).
     */

    /************************************************/
    /** MAIN LOOP ON QUERIES.                       */
    /************************************************/
    for (ContainerLevel1::iterator itLevel1 = _containerLevel1.begin(); itLevel1 != _containerLevel1.end();  itLevel1++)
    {
        /** Shortcuts. */
        ContainerLevel2*  containerLevel2  = (*itLevel1).second.second;

        /******************************************************************************/
        /** FIRST: we loop each second level and then sort/erase unwanted alignments. */
        /******************************************************************************/
        for (ContainerLevel2::iterator itLevel2 = containerLevel2->begin(); itLevel2 != containerLevel2->end(); itLevel2++)
        {
            /** Shortcut. */
            ContainerLevel3* containerLevel3 = (*itLevel2).second.second;

            /** We sort the alignments container with a functor (sort by evalue). */
            containerLevel3->sort (sortAlign);

            /** Once sorted, we can afford to keep a specific number of alignments for the current pair [query,subject]. */
            if (_nbHspPerHit > 0)
            {
                /** We loop the '_nbHspPerHit' first alignments. */
                size_t k=0;
                list<Alignment>::iterator it;
                for (it=containerLevel3->begin(); k<_nbHspPerHit && it!=containerLevel3->end(); it++, k++)   {}

                /** We can now delete extra alignments. */
                containerLevel3->erase (it, containerLevel3->end());
            }
        }

        /********************************************************************************/
        /** SECOND: we loop each second level and then sort them and cut unwanted ones. */
        /********************************************************************************/
        for (ContainerLevel2::iterator ita = containerLevel2->begin(); ita != containerLevel2->end(); ita++)
        {
			ContainerLevel2::iterator itbest = ita;
        	ContainerLevel2::iterator itb    = ita;

        	/** We have to iterate all the successive subjects after the 'ita' iterator. We have thus a O(n^2)
        	 * algorithm for sorting our subjects instances; this may be not optimal but should be good enough
        	 * as a starting point.
        	 */
            for (itb++; itb != containerLevel2->end(); itb++)
            {
                /** We check if we have not found a better hit than the current best. */
                if (sortHit (itb->second.second, itbest->second.second) == true)  {  itbest = itb;  }
            }

            /** If a best hit has been found, we swap it with the current one.
             *  At the end of this double loop, our hits should be sorted.
             *  Note that items swapped are pairs of  two pointers, so it should not take too many time.
             *  At least, we assure that we have no copy of alignments list.
             */
			if (itbest != ita)  {  std::swap ( (*containerLevel2) [itbest->first], (*containerLevel2) [ita->first]);  }
        }

        /** Once sorted, we can afford to keep a specific number of hits for the current query. */
        if (_nbHitPerQuery > 0)
        {
            /** We loop the '_nbHitPerQuery' first subjects. */
            size_t k=0;
        	ContainerLevel2::iterator it;
        	for (it=containerLevel2->begin(); k<_nbHitPerQuery && it!=containerLevel2->end(); it++, k++)   {}

        	/** Now, we have reach the first unwanted hit => we have to delete it and all the next items holding:
        	 * 		-> the 'subject ISequence instance
        	 * 		-> the list of alignments for each [qry,sbj] couple
        	 */
            for (ContainerLevel2::iterator itLevel2 = it; itLevel2 != containerLevel2->end(); itLevel2++)
            {
                /** Shortcuts. */
                ISequence*       seqLevel2       = (*itLevel2).second.first;
                ContainerLevel3* containerLevel3 = (*itLevel2).second.second;

                if (seqLevel2)        { delete seqLevel2;       }
                if (containerLevel3)  { delete containerLevel3; }
            }

        	/** We can now delete extra subjects from the container itself. */
        	containerLevel2->erase (it, containerLevel2->end());
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
    ContainerLevel2* containerLevel2 = (lookFirstLevel->second).second;

    /** We look for the second level entry. */
    Key secondKey (seqLevel2->database,seqLevel2->index);
    ContainerLevel2::iterator lookSecondLevel = containerLevel2->find (secondKey);

    if (lookSecondLevel == containerLevel2->end())  { return 0; }

    /** We return the result. */
    return  (lookSecondLevel->second).second;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
