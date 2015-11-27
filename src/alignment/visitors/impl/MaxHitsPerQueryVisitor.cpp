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

#include <alignment/visitors/impl/MaxHitsPerQueryVisitor.hpp>

#include <database/api/ISequenceDatabase.hpp>

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
void MaxHitsPerQueryVisitor::visitQuerySequence (
    const database::ISequence*  seq,
    const misc::ProgressInfo&   progress
)
{
    if (_ref)  { _ref->visitQuerySequence (seq, progress); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void MaxHitsPerQueryVisitor::visitSubjectSequence (
    const database::ISequence*  seq,
    const misc::ProgressInfo&   progress
)
{
    _currentHitsNb = 0;

    if (_ref)  { _ref->visitSubjectSequence (seq, progress); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void MaxHitsPerQueryVisitor::visitAlignment (
    core::Alignment*            align,
    const misc::ProgressInfo&   progress
)
{
    _currentHitsNb ++;

    if (_currentHitsNb <= _maxHitsPerQuery  &&  _ref)
    {
        _ref->visitAlignment (align, progress);
    }
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
