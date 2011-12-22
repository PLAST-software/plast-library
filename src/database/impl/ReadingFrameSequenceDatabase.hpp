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

/** \file ReadingFrameSequenceDatabase.hpp
 *  \brief Sequence database for one reading frame
 *  \date 07/11/2011
 *  \author edrezen
 *
 * Provides a way to convert a database made of nucleotids into a database made of
 * amino acids.
 */

#ifndef _READING_FRAME_SEQUENCE_DATABASE_HPP_
#define _READING_FRAME_SEQUENCE_DATABASE_HPP_

/********************************************************************************/

#include <designpattern/api/ICommand.hpp>

#include <database/impl/BufferedSequenceDatabase.hpp>
#include <database/impl/ReadingFrameSequenceIterator.hpp>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Interpret a nucleotid database as an amino acid database
 *
 *  Sequence database that refers another database (supposed to be a nucleotid database)
 *  and interprets this reference as an amino acid database for one of the six reading
 *  frames.
 *
 *  So, from one ISequenceDatabase database, it is possible to create 6 ReadingFrameSequenceDatabase
 *  instances that maps the referenced database into the 6 possible ORF.
 *
 *  From the client point of view, she/he just has to use a ReadingFrameSequenceDatabase instance
 *  as a ISequenceDatabase instance, ie. the nucleotid/amino acid translation is encapsulated within
 *  the ReadingFrameSequenceDatabase implementation.
 *
 *  This class is a 'memory' database since it inherits from the BufferedSequenceDatabase class.
 *
 *  Code sample:
 *  \code
 *  void sample ()
 *  {
 *      // we create a (memory cached) nucloetid database from a FASTA file
 *      ISequenceDatabase* nucleotidDb = new BufferedSequenceDatabase (
 *          new FastaSequenceIterator ("tursiops_ADN.fa", 100),
 *          false
 *      );
 *
 *      // we create an amino acid database from the nucleotid database by reading it for one Open Reading Frame
 *      ISequenceDatabase* aminoAcidDb = new ReadingFrameSequenceDatabase (FRAME_2, nucleotidDb, false);
 *
 *      // we try to retrieve the third protein sequence
 *      ISequence sequence;
 *      if (aminoAcidDb->getSequenceByIndex (2, sequence) == true)
 *      {
 *          // we can do anything we want with the retrieved protein sequence.
 *      }
 *  }
 *  \endcode
 */
class ReadingFrameSequenceDatabase : public BufferedSequenceDatabase
{
public:

    /** Constructor.
     * \param[in] frame : opening reading frame used for translating the referenced nucleotid database
     * \param[in] nucleotidDatabase : nucleotid database to be translated
     * \param[in] filterLowComplexity : tells whether one should filter out regions of low complexity
     */
    ReadingFrameSequenceDatabase (misc::ReadingFrame_e frame, ISequenceDatabase* nucleotidDatabase, bool filterLowComplexity);

    /** Destructor. */
    virtual ~ReadingFrameSequenceDatabase ();

    /** Returns the opening reading frame.
     * \return the frame.
     */
    misc::ReadingFrame_e getFrame () { return _frame; }

    /** Tells if the current frame is 'top', ie. one of the direct reading (frames 1,2,3).
     * \return true if top, false otherwise.
     */
    bool isTopFrame ()  { return (_frame>= misc::FRAME_1 && _frame <= misc::FRAME_3); }

    /** Returns the frame shift.
     * \return the frame shift
     */
    size_t getFrameShift ()  {  return isTopFrame() ? (_frame - 0) : (_frame -3);  }

    /** Return the referenced nucleotid database.
     * \return the nucleotid database.
     */
    ISequenceDatabase* getNucleotidDatabase ()  { return _nucleotidDatabase; }

protected:

    /** The nucleotid database. */
    ISequenceDatabase*      _nucleotidDatabase;

    /** The current frame. */
    misc::ReadingFrame_e   _frame;

    /** Smart setter for _frame attribute. */
    void setNucleotidDatabase (ISequenceDatabase* nucleotidDatabase)  { SP_SETATTR (nucleotidDatabase); }
};

/********************************************************************************/

/** \brief Define a command for reading a orf database.
 *
 * This implementation is useful for reading several frames in different threads.
 */
class ReadingFrameSequenceCommand : public dp::ICommand
{
public:

    /** Constructor.
     */
    ReadingFrameSequenceCommand (
        database::ISequenceDatabase*  nucleotidDatabase,
        misc::ReadingFrame_e          frame,
        bool                          filtering
    ) : _nucleotidDatabase(nucleotidDatabase), _frame(frame), _filtering(filtering), _resultDatabase(0) {}

    /** \copydoc dp::ICommand::execute */
    void execute ()
    {
        _resultDatabase = new ReadingFrameSequenceDatabase (_frame, _nucleotidDatabase, _filtering);
        _resultDatabase->getSize();
    }

    /** Return the (amino acid) resulting database.
     * \return the read database
     */
    database::ISequenceDatabase*  getResult () { return _resultDatabase; }

private:
    database::ISequenceDatabase*  _nucleotidDatabase;
    misc::ReadingFrame_e          _frame;
    bool                          _filtering;
    database::ISequenceDatabase*  _resultDatabase;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _READING_FRAME_SEQUENCE_DATABASE_HPP_ */
