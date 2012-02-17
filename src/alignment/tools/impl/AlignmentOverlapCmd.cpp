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

#define DEBUG(a)

/********************************************************************************/
namespace alignment {
namespace tools     {
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
void AlignmentOverlapCmd::execute ()
{
    /** We first reset the arguments to be computed. */
    _commonSize   = 0;
    _specificSize = 0;

    if (!_ref)   { return; }
    if (!_comp)  { _specificSize = _ref->size();  return; }

    vector<bool> marked (_comp->size(), false);

    struct Value  {  double overlap;   int index;   };
    Value best;

    for (list<Alignment>::const_iterator itRef = _ref->begin(); itRef != _ref->end(); itRef++)
    {
        best.overlap = 0;
        best.index   = 0;

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

        if (best.overlap >= _threshold)
        {
            /** We can associate the comp alignment to the ref alignment;
             * we hence tag the marked item in order to not take into account any more. */
            marked [best.index] = true;

            _commonSize++;
        }
        else
        {
            _specificSize++;
        }

    } /* end of for (list<Alignment>::const_iterator itRef... */
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
