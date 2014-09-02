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
 * Provides a way to convert a database made of nucleotides into a database made of
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

/** \brief Interpret a nucleotide database as an amino acid database
 *
 *  Sequence database that refers another database (supposed to be a nucleotide database)
 *  and interprets this reference as an amino acid database for one of the six reading
 *  frames.
 *
 *  So, from one ISequenceDatabase database, it is possible to create 6 ReadingFrameSequenceDatabase
 *  instances that maps the referenced database into the 6 possible ORF.
 *
 *  From the client point of view, she/he just has to use a ReadingFrameSequenceDatabase instance
 *  as a ISequenceDatabase instance, ie. the nucleotide/amino acid translation is encapsulated within
 *  the ReadingFrameSequenceDatabase implementation.
 *
 *  This class is a 'memory' database since it inherits from the BufferedSequenceDatabase class.
 *
 *  The actual building of the amino acid sequences is done by providing (during construction) a
 *  ReadingFrameSequenceIterator instance to the base class BufferedSequenceDatabase constructor.
 *
 *  Code sample:
 *  \code
 *  void sample ()
 *  {
 *      // we create a (memory cached) nucleotide database from a FASTA file
 *      ISequenceDatabase* nucleotidDb = new BufferedSequenceDatabase (
 *          new FastaSequenceIterator ("tursiops_ADN.fa", 100),
 *          false
 *      );
 *
 *      // we create an amino acid database from the nucleotide database by reading it for one Open Reading Frame
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
     * \param[in] frame : opening reading frame used for translating the referenced nucleotide database
     * \param[in] nucleotidDatabase : nucleotide database to be translated
     * \param[in] filterLowComplexity : tells whether one should filter out regions of low complexity
     */
    ReadingFrameSequenceDatabase (misc::ReadingFrame_e frame, ISequenceDatabase* nucleotidDatabase, int filterLowComplexity);

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
    int8_t getFrameShift ()  {  return isTopFrame() ? (_frame + 1) : (2-_frame);  }

    /** Return the referenced nucleotide database.
     * \return the nucleotide database.
     */
    ISequenceDatabase* getNucleotidDatabase ()  { return _nucleotidDatabase; }

    /** \copydoc BufferedSequenceDatabase::getId */
    std::string getId ();

protected:

    /** The nucleotide database. */
    ISequenceDatabase*      _nucleotidDatabase;

    /** The current frame. */
    misc::ReadingFrame_e   _frame;

    /** Smart setter for _frame attribute. */
    void setNucleotidDatabase (ISequenceDatabase* nucleotidDatabase)  { SP_SETATTR (nucleotidDatabase); }
};

/********************************************************************************/

/** \brief Define a command for building ReadingFrameSequenceDatabase instances
 *
 *  It is often required to have at the same time the 6 ReadingFrameSequenceDatabase instances
 *  corresponding to the 6 possible frames.
 *
 *  Since the instantiation of such an instance is done by interpreting the initial nucleotide
 *  database for a given frame, it is possible to parallelize this process of creation.
 *
 *  Here, we define a ICommand implementation that only creates a ReadingFrameSequenceDatabase
 *  instance for a given frame. If we instantiate 6 times this class with all possible frames,
 *  we just have to dispatch the commands to a parallel commands dispatcher and then we get
 *  our parallelization.
 *
 * \code
 * void foo (ISequenceDatabase* db)
 * {
 *    // We create a list of commands
 *    list<ICommand*> commands;
 *    for (size_t i=FRAME_1; i<=FRAME_6; i++)
 *    {
 *        ICommand* cmd = new ReadingFrameSequenceCommand (db, i, false);
 *        cmd->use ();
 *
 *        // We add the command to the list to be dispatched.
 *        commands.push_back (cmd);
 *    }
 *
 *    // We dispatch the commands with a parallel dispatcher
 *    ParallelCommandDispatcher().dispatchCommands (commands, 0);
 *
 *    // We retrieve the created databases.
 *    for (list<ICommand*>::iterator it = commands.begin(); it != commands.end(); it++)
 *    {
 *        // We retrieve the created database
 *        ReadingFrameSequenceCommand* cmd = dynamic_cast<ReadingFrameSequenceCommand*> (*it);
 *
 *        // here, we can do anything we want with the built database
 *        if (cmd != 0)  {  ISequenceDatabase* currentDb = cmd->getResult();   }
 *
 *        // We forget the command
 *        (*it)->forget();
 *    }
 * }
 * \endcode
 *
 * \see ReadingFrameSequenceDatabase
 */
class ReadingFrameSequenceCommand : public dp::ICommand
{
public:

    /** Constructor.
     * \param[in] nucleotidDatabase : the nucleotide database to be transformed in amino acid database
     * \param[in] frame : frame used for the transformation
     * \param[in] filtering : true if low information regions must be tagged
     */
    ReadingFrameSequenceCommand (
        database::ISequenceDatabase*  nucleotidDatabase,
        misc::ReadingFrame_e          frame,
        int                           filtering
    ) : _nucleotidDatabase(nucleotidDatabase), _frame(frame), _filtering(filtering), _resultDatabase(0) {}

    /** \copydoc dp::ICommand::execute */
    void execute ()
    {
        _resultDatabase = new ReadingFrameSequenceDatabase (_frame, _nucleotidDatabase, _filtering);
        /* remove the getSize because of multi thread concurrence access */
        //_resultDatabase->getSize();
    }

    /** Return the (amino acid) resulting database.
     * \return the read database
     */
    database::ISequenceDatabase*  getResult () { return _resultDatabase; }

private:

    database::ISequenceDatabase*  _nucleotidDatabase;
    misc::ReadingFrame_e          _frame;
    int                           _filtering;
    database::ISequenceDatabase*  _resultDatabase;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _READING_FRAME_SEQUENCE_DATABASE_HPP_ */
