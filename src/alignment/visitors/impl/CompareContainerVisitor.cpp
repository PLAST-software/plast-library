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

#include <alignment/visitors/impl/CompareContainerVisitor.hpp>
#include <alignment/tools/impl/AlignmentOverlapCmd.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;

using namespace database;

using namespace alignment;
using namespace alignment::core;
using namespace alignment::tools::impl;

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
void CompareContainerVisitor::visitAlignmentsList (
    const database::ISequence* qry,
    const database::ISequence* sbj,
    std::list<core::Alignment>& alignments
)
{
    /** We retrieve the compared alignments. */
    list<Alignment>* comp = _dbComp->getContainer (qry, sbj);

    /** We compute the overlap between the two sets for the given threshold. */
    AlignmentOverlapCmd cmd (&alignments, comp, _overlapRange, _visitorCommon, _visitorDistinct);
    cmd.execute ();

    _commonSize   += cmd.getCommonSize   ();
    _specificSize += cmd.getSpecificSize ();

    //printf ("visitAlignmentsList: _commonSize=%ld  _specificSize=%ld \n", cmd.getCommonSize   (),  cmd.getSpecificSize ());
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
