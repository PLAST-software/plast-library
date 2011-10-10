/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

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
    ReadingFrameSequenceDatabase (ReadingFrame_e frame, ISequenceDatabase* nucleotidDatabase, bool filterLowComplexity);

    /** Destructor. */
    virtual ~ReadingFrameSequenceDatabase ();

    ReadingFrame_e getFrame () { return _frame; }

    bool isTopFrame ()  { return (_frame>= FRAME_1 && _frame <= FRAME_3); }

    size_t getFrameShift ()  {  return isTopFrame() ? (_frame - 0) : (_frame -3);  }

    ISequenceDatabase* getNucleotidDatabase ()  { return _nucleotidDatabase; }

protected:

    ISequenceDatabase* _nucleotidDatabase;
    ReadingFrame_e     _frame;

    void setNucleotidDatabase (ISequenceDatabase* _nucleotidDatabase);
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _READING_FRAME_SEQUENCE_DATABASE_HPP_ */
