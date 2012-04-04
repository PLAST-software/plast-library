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

#include <alignment/visitors/impl/FilterContainerVisitor.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;

using namespace database;

using namespace alignment;
using namespace alignment::core;

/********************************************************************************/
namespace alignment {
namespace visitors  {
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
void FilterContainerVisitor::visitAlignmentsList (
    const database::ISequence* qry,
    const database::ISequence* sbj,
    std::list<core::Alignment>& alignments
)
{
    if (_filter != 0)
    {
        size_t removed = 0;

        /** Shortcut. */
        misc::ProgressInfo& alignProgress = _extraInfo.alignProgress;

        /** We resume the alignment progress. */
        alignProgress.set (1, alignments.size());

        for (list<Alignment>::iterator it = alignments.begin(); it != alignments.end(); ++alignProgress)
        {
            /** Shortcut. */
            Alignment& al = (*it);

            /** We reference the extra info for the current alignment. */
            al.setExtraInfo (&_extraInfo);

            if (_filter->isOk (*it) == false) {  it = alignments.erase (it);   removed++;   }
            else                              {  it++;                                      }

            /** We dereference the extra info from the current alignment. */
            al.setExtraInfo (0);
        }

        /** We increase the number of removed alignments. */
        _nbRemoved += removed;
    }
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
