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

#include <database/impl/ReadingFrameSequenceDatabase.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace misc;

/********************************************************************************/
namespace database { namespace impl  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ReadingFrameSequenceDatabase::ReadingFrameSequenceDatabase (
    ReadingFrame_e      frame,
    ISequenceDatabase*  nucleotidDatabase,
    int                 filterLowComplexity
)
    : BufferedSequenceDatabase (
            new ReadingFrameSequenceIterator (frame, nucleotidDatabase->createSequenceIterator()),
            filterLowComplexity
        ),
      _nucleotidDatabase(0),
      _frame (frame)
{
    /** We set the nucleotid database. */
    setNucleotidDatabase (nucleotidDatabase);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ReadingFrameSequenceDatabase::~ReadingFrameSequenceDatabase ()
{
    /** We unset the nucleotid database. */
    setNucleotidDatabase (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
string ReadingFrameSequenceDatabase::getId ()
{
    stringstream ss;
    ss << _nucleotidDatabase->getId() << "frame" << _frame;
    return ss.str();
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

