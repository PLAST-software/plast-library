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
ReverseStrandVisitor::ReverseStrandVisitor (IAlignmentContainerVisitor* ref,Alignment::DbKind kind, StrandId_e strand)
    :  AlignmentsProxyVisitor(ref), _kind(kind), _strand (strand)
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
	if (_strand == MINUS)
	{
		/** Shortcuts. */
	    const Range32& range  = align->getRange (_kind);
	    u_int32_t 	   seqLen = align->getSequence(_kind)->getLength();

	    misc::Range32 newRange (seqLen - range.begin - 1, seqLen - range.end - 1);

	    /** We update some information of the alignment. */
	    align->setRange (_kind, newRange);
	}

    /** NOTE ! We force the query frame to be always 1. */
    align->setFrame (Alignment::QUERY, 1);
    align->setFrame (_kind,            _strand==PLUS ? 1 : -1);
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
