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

#include <alignment/visitors/impl/ReverseStrandVisitor.hpp>

#include <misc/api/types.hpp>

#include <database/impl/ReadingFrameSequenceDatabase.hpp>

#include <stdio.h>
#define DEBUG(a)  //a

using namespace std;
using namespace misc;
using namespace database;
using namespace database::impl;
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
ReverseStrandVisitor::ReverseStrandVisitor (IAlignmentContainerVisitor* ref,Alignment::DbKind kind)
    :  AlignmentsProxyVisitor(ref), _kind(kind)
{
    DEBUG (cout << "ReverseStrandVisitor::ReverseStrandVisitor   ref=" << ref << "  kind=" << kind << endl);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void ReverseStrandVisitor::visitAlignment  (Alignment* align, const misc::ProgressInfo& progress)
{
	/** Shortcuts. */
    const Range32& range  = align->getRange (_kind);
    u_int32_t 	   seqLen = align->getSequence(_kind)->getLength();

    misc::Range32 newRange (seqLen - range.begin - 1, seqLen - range.end - 1);

    /** We update some information of the alignment. */
    align->setRange (_kind, newRange);
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
