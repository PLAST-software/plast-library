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

#include <alignment/tools/impl/AlignmentOverlapCmd.hpp>

using namespace std;
using namespace alignment;
using namespace alignment::core;

#define DEBUG(a) a

/********************************************************************************/
namespace alignment {
namespace tools     {
namespace impl      {
/********************************************************************************/

int AlignmentOverlapCmd::_count = 0;

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
    if (!_ref)   { return; }

    vector<bool> marked;

    struct Value  {  double overlap;   int index;   };
    Value best;

    if (_comp)  { marked.resize(_comp->size(), false); }

    for (list<Alignment>::const_iterator itRef = _ref->begin(); itRef != _ref->end(); itRef++)
    {
        best.overlap = 0;
        best.index   = 0;

        if (_comp != 0)
        {
            int idx = 0;
            for (list<Alignment>::const_iterator itComp = _comp->begin(); itComp != _comp->end(); itComp++, idx++)
            {
                if (marked[idx] == false)
                {
                    /** We compute the overlap of the two Alignment instances. */
                    double overlap = Alignment::overlap (*itRef, *itComp);

                    /** We may memorize this overlap value and the current index. */
                    if (overlap > best.overlap)
                    {
                        best.overlap = overlap;
                        best.index   = idx;
                    }
                }

            } /* end of for (list<Alignment>::const_iterator itComp... */
        }

        if (_overlapRange.includes (best.overlap) == true)
        {
            /** We can associate the comp alignment to the ref alignment;
             * we hence tag the marked item in order to not take into account any more. */
            marked [best.index] = true;

            handleCommonAlignment (*itRef);
        }
        else
        {
            handleDistinctAlignment (*itRef);
        }

    } /* end of for (list<Alignment>::const_iterator itRef... */
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
