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

#include <alignment/visitors/impl/ShrinkContainerVisitor.hpp>

#include <alignment/tools/impl/AlignmentContainerShrinkCmd.hpp>

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
void ShrinkContainerVisitor::visitAlignmentsList (
    const database::ISequence* qry,
    const database::ISequence* sbj,
    std::list<core::Alignment>& alignments
)
{
    /** We may remove redundant alignments. */
    AlignmentContainerShrinkCmd cmd (alignments, _sort_cbk, _nbAlignToKeep);

    /** We execute the command. */
    cmd.execute ();

    /** We increase the number of removed alignments. */
    _nbRemoved += cmd.getNbRemoved();
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
