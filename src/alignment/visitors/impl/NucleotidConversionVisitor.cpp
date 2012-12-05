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
NucleotidConversionVisitor::NucleotidConversionVisitor (
    IAlignmentContainerVisitor* ref,
    Alignment::DbKind kind
)
    :  AlignmentsProxyVisitor(ref), _kind(kind), _nucleotidDb(0)
{
    DEBUG (cout << "NucleotidConversionVisitor::NucleotidConversionVisitor   ref=" << ref << "  kind=" << kind << endl);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void NucleotidConversionVisitor::visitAlignment  (Alignment* align, const misc::ProgressInfo& progress)
{
    Range32 oldRange = align->getRange (_kind);

    /** We convert [start,end] in terms of nucleotide sequence. */
    Range32 newRange (
        3*oldRange.begin - 1,
        3*oldRange.begin + 3*oldRange.getLength() - 2
    );

    /** We may have to reverse the indexes according to the strand direction. */
    if (_isTopFrame == true)
    {
        newRange.begin += _frameShift;
        newRange.end   += _frameShift;
    }
    else
    {
        newRange.begin = _nucleotidSequence.getLength() - newRange.begin - 1 + _frameShift;
        newRange.end   = _nucleotidSequence.getLength() - newRange.end   - 1 + _frameShift;
    }

    DEBUG (cout << "NucleotidConversionVisitor::visitAlignment FOUND   old=" << oldRange << "  new=" << newRange << endl);

    /** We update some information of the alignment. */
    align->setRange        (_kind, newRange);
    align->setFrame        (_kind, _frameShift);

    /** The 'kind' sequence has been translated from nucleotides to amino acids => just remember this at alignment level. */
    align->setIsTranslated (_kind, true);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
const ISequence* NucleotidConversionVisitor::retrieveNucleotidSequence (const database::ISequence*  proteinSequence)
{
    const ISequence* result = 0;

    /** We first look for the nucleotide database. */
    ReadingFrameSequenceDatabase* orfDb = dynamic_cast<ReadingFrameSequenceDatabase*> (proteinSequence->database);
    if (orfDb != 0)
    {
        _nucleotidDb = orfDb->getNucleotidDatabase();
        _frameShift  = orfDb->getFrameShift();
        _isTopFrame  = orfDb->isTopFrame();
    }

    if (_nucleotidDb != 0)
    {
        /**  Note that we use a ISequence cache in order to avoid too many
         *  ISequenceDatabase::getSequenceByIndex calls which may be time consuming.  */
        Key key (_nucleotidDb, proteinSequence->index);

        map<Key,database::ISequence>::iterator look = _nucleotidSequences.find (key);
        if (look != _nucleotidSequences.end())
        {
            _nucleotidSequence = look->second;
        }
        else
        {
            _nucleotidDb->getSequenceByIndex (proteinSequence->index, _nucleotidSequence);
            _nucleotidSequences[key] = _nucleotidSequence;
        }
        result = &_nucleotidSequence;
    }

    else
    {
        result = 0;
    }

    return result;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
