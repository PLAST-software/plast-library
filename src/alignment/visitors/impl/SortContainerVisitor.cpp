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

#include <alignment/visitors/impl/SortContainerVisitor.hpp>

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
SortContainerVisitor::SortContainerVisitor (bool (*sort_cbk) (const core::Alignment& i, const core::Alignment& j))
    : _sort_cbk (sort_cbk)
{
    if (_sort_cbk == 0)  {  _sort_cbk = mysortfunction; }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SortContainerVisitor::visitAlignmentsList (
    const database::ISequence* qry,
    const database::ISequence* sbj,
    std::list<core::Alignment>& alignments
)
{
    /** We may have to sort the list at the end of the process. */
    if (_sort_cbk != 0) {  alignments.sort (_sort_cbk); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool SortContainerVisitor::mysortfunction (const Alignment& i, const Alignment& j)
{
    return (i.getEvalue() <  j.getEvalue())  ||
           (i.getEvalue() == j.getEvalue()  &&  i.getBitScore() < j.getBitScore());
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
