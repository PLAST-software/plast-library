/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

#include <alignment/tools/impl/AlignmentOverlapCmd.hpp>

using namespace std;
using namespace alignment;
using namespace alignment::core;

#define DEBUG(a) //a

/********************************************************************************/
namespace alignment {
namespace tools     {
namespace impl      {
/********************************************************************************/

static Alignment::DbKind KIND = Alignment::QUERY;

#define PLUS   1
#define MINUS -1

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
static bool compare_alignments_plus (const Alignment* i, const Alignment* j)
{
    return  (i->getRange(KIND).begin < j->getRange(KIND).begin);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
static bool compare_alignments_minus (const Alignment* i, const Alignment* j)
{
    return  (i->getRange(KIND).end < j->getRange(KIND).end);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
static inline int8_t getAlignmentDirection (const Alignment& a, Alignment::DbKind kind)
{
    return (a.getRange(kind).begin < a.getRange(kind).end) ? PLUS : MINUS;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractAlignmentOverlapCmd::AbstractAlignmentOverlapCmd (
    std::list<core::Alignment>* ref,
    std::list<core::Alignment>* comp,
    misc::Range<double> overlapRange
) : _overlapRange(overlapRange)
{
    /** We split the alignments lists into sorted partitions. */
    partition (ref,  refParts);
    partition (comp, compParts);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AbstractAlignmentOverlapCmd::partition (std::list<core::Alignment>* in, Partition& out)
{
    if (!in)  { return; }

    /** We split the comp in partitions. */
    for (std::list<core::Alignment>::iterator it = in->begin(); it != in->end(); it++)
    {
        /** Shortcuts. */
        int8_t qryDir = getAlignmentDirection(*it,Alignment::QUERY);
        int8_t sbjDir = getAlignmentDirection(*it,Alignment::SUBJECT);

              if (qryDir== PLUS && sbjDir== PLUS)  {  out.LL.push_back (&(*it)); }
        else  if (qryDir== PLUS && sbjDir==MINUS)  {  out.LR.push_back (&(*it)); }
        else  if (qryDir==MINUS && sbjDir== PLUS)  {  out.RL.push_back (&(*it)); }
        else  if (qryDir==MINUS && sbjDir==MINUS)  {  out.RR.push_back (&(*it)); }
    }

    /** We sort the partitions. */
    if (KIND == Alignment::QUERY)
    {
        out.LL.sort (compare_alignments_plus);
        out.LR.sort (compare_alignments_plus);
        out.RL.sort (compare_alignments_minus);
        out.RR.sort (compare_alignments_minus);
    }
    else
    {
        out.LL.sort (compare_alignments_plus);
        out.LR.sort (compare_alignments_minus);
        out.RL.sort (compare_alignments_plus);
        out.RR.sort (compare_alignments_minus);
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
void AbstractAlignmentOverlapCmd::execute ()
{
    if (KIND == Alignment::QUERY)
    {
        execute (refParts.LL, compParts.LL, PLUS);
        execute (refParts.LR, compParts.LR, PLUS);
        execute (refParts.RL, compParts.RL, MINUS);
        execute (refParts.RR, compParts.RR, MINUS);
    }
    else
    {
        execute (refParts.LL, compParts.LL, PLUS);
        execute (refParts.LR, compParts.LR, MINUS);
        execute (refParts.RL, compParts.RL, PLUS);
        execute (refParts.RR, compParts.RR, MINUS);
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
void AbstractAlignmentOverlapCmd::execute (list<Alignment*>& ref, list<Alignment*>& comp, int direction)
{
    double bestOverlap = 0;

    list<Alignment*>::iterator itCompLeft = comp.begin();
    list<Alignment*>::iterator itDelete   = comp.end();

    for (list<Alignment*>::iterator itRef = ref.begin(); itRef != ref.end(); itRef++)
    {
        Alignment* refAlign = *itRef;

        /** We check that we are not to far on the right. */
        for (; itCompLeft != comp.end(); itCompLeft++)
        {
            Alignment* compAlign = *itCompLeft;

            if (direction == PLUS)
            {
                if (compAlign->getRange(KIND).end >= refAlign->getRange(KIND).begin)  { break; }
            }
            else
            {
                if (compAlign->getRange(KIND).begin >= refAlign->getRange(KIND).end)  { break; }
            }
        }

        bestOverlap = 0;
        itDelete    = comp.end();

        /** We loop over alignments of the comp list. */
        for (list<Alignment*>::iterator itComp=itCompLeft; itComp != comp.end(); itComp++)
        {
            Alignment* compAlign = *itComp;

            /** We check that we are not to far on the right. */
            if (direction == PLUS)
            {
                if (compAlign->getRange(KIND).begin > refAlign->getRange(KIND).end)  { break; }
            }
            else
            {
                if (compAlign->getRange(KIND).end > refAlign->getRange(KIND).begin)  { break; }
            }

            /** We compute the overlap of the two Alignment instances. */
            double overlap = Alignment::overlap (*refAlign, *compAlign);

            /** We may memorize this overlap value and the current index. */
            if (overlap > bestOverlap)
            {
                bestOverlap = overlap;

                itDelete = itComp;
            }

        } /* end of for (list<Alignment>::const_iterator itComp... */

        if (_overlapRange.includes (bestOverlap) == true)
        {
            handleCommonAlignment (*itRef);
        }
        else
        {
            handleDistinctAlignment (*itRef);
        }

        if (itDelete != comp.end())
        {
            if (itCompLeft == itDelete)
            {
                itCompLeft = comp.erase (itDelete);
            }
            else
            {
                comp.erase (itDelete);
            }
        }

    } /* end of for (list<Alignment>::const_iterator itRef... */
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
