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

#include "ReadingFrameSequenceDatabase.hpp"
#include "macros.hpp"

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;

/********************************************************************************/
namespace database {
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
    bool                filterLowComplexity
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
void ReadingFrameSequenceDatabase::setNucleotidDatabase (ISequenceDatabase* nucleotidDatabase)
{
    if (CHECKPTR(_nucleotidDatabase))  { _nucleotidDatabase->forget (); }
    _nucleotidDatabase = nucleotidDatabase;
    if (CHECKPTR(_nucleotidDatabase))  { _nucleotidDatabase->use(); }
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

