/*
 * SeedIndex.cpp
 *
 *  Created on: Jun 20, 2011
 *      Author: edrezen
 */

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

