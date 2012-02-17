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

#include <misc/api/types.hpp>

#include <database/impl/ReadingFrameSequenceDatabase.hpp>

#include <alignment/visitors/impl/NucleotidConversionVisitor.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

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
NucleotidConversionVisitor::NucleotidConversionVisitor (
    IAlignmentContainerVisitor* ref,
    Alignment::DbKind kind
)
    :  AlignmentsProxyVisitor(ref), _kind(kind)
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void NucleotidConversionVisitor::visitAlignment  (Alignment* align)
{
    ISequence nucleotidSequence;
    size_t    frameShift;
    bool      isTopFrame;

    /** We retrieve the nucleotid sequence. */
    if (getNucleotidSequence (nucleotidSequence, align, frameShift, isTopFrame) == true)
    {
        Range32 oldRange = align->getRange (_kind);

        /** We convert [start,end] in terms of nucleotid sequence. */
        Range32 newRange (
            3*oldRange.begin + frameShift,
            3*oldRange.begin + frameShift + 3*oldRange.getLength() - 1
        );

        /** We may have to reverse the indexes according to the reading frame. */
        if (isTopFrame == false)
        {
            newRange.begin = nucleotidSequence.getLength() - newRange.begin - 1;
            newRange.end   = nucleotidSequence.getLength() - newRange.end   - 1;
        }

        /** We update the range in the alignment. */
        align->setRange (_kind, newRange);
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool NucleotidConversionVisitor::getNucleotidSequence (
    ISequence&  nuclotidSequence,
    Alignment*  align,
    size_t&     frameShift,
    bool&       isTopFrame
)
{
    bool found = false;

    ISequenceDatabase* nucleotidDb = 0;

    /** Shortcut. */
    const ISequence* sequence = align->getSequence (_kind);

    /** We first look for the nucleotid database. */
    ReadingFrameSequenceDatabase* orfDb = dynamic_cast<ReadingFrameSequenceDatabase*> (sequence->database);
    if (orfDb != 0)
    {
        nucleotidDb = orfDb->getNucleotidDatabase();
        frameShift  = orfDb->getFrameShift();
        isTopFrame  = orfDb->isTopFrame();
    }

    if (nucleotidDb != 0)
    {
        /**  Note that we use a ISequence cache in order to avoid too many
         *  ISequenceDatabase::getSequenceByIndex calls which may be time consuming.  */
        Key key (nucleotidDb, sequence->index);

        map<Key,database::ISequence>::iterator look = _nucleotidSequences.find (key);
        if (look != _nucleotidSequences.end())
        {
            nuclotidSequence = look->second;
        }
        else
        {
            ISequence nucleotidSequence;
            nucleotidDb->getSequenceByIndex (sequence->index, nucleotidSequence);
            nuclotidSequence = (_nucleotidSequences[key] = nucleotidSequence);
        }
        found = true;
    }

    else
    {
        found = false;
    }

    return found;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
