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

#ifndef _READING_FRAME_SEQUENCE_DATABASE_HPP_
#define _READING_FRAME_SEQUENCE_DATABASE_HPP_

/********************************************************************************/

#include "BufferedSequenceDatabase.hpp"
#include "ReadingFrameSequenceIterator.hpp"

/********************************************************************************/
namespace database {
/********************************************************************************/

/** Sequence database that reads another database for providing one of the six reading
 *  frames. */
class ReadingFrameSequenceDatabase : public BufferedSequenceDatabase
{
public:

    /** Constructor. */
    ReadingFrameSequenceDatabase (types::ReadingFrame_e frame, ISequenceDatabase* nucleotidDatabase, bool filterLowComplexity);

    /** Destructor. */
    virtual ~ReadingFrameSequenceDatabase ();

    types::ReadingFrame_e getFrame () { return _frame; }

    bool isTopFrame ()  { return (_frame>= types::FRAME_1 && _frame <= types::FRAME_3); }

    size_t getFrameShift ()  {  return isTopFrame() ? (_frame - 0) : (_frame -3);  }

    ISequenceDatabase* getNucleotidDatabase ()  { return _nucleotidDatabase; }

protected:

    ISequenceDatabase*      _nucleotidDatabase;
    types::ReadingFrame_e   _frame;

    void setNucleotidDatabase (ISequenceDatabase* nucleotidDatabase)  { SP_SETATTR (nucleotidDatabase); }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _READING_FRAME_SEQUENCE_DATABASE_HPP_ */
