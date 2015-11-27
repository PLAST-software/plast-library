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

/** \file FastaSequenceOutput.hpp
 *  \brief Dump of sequences in a FASTA file
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _FASTA_SEQUENCE_OUTPUT_HPP_
#define _FASTA_SEQUENCE_OUTPUT_HPP_

/********************************************************************************/

#include <misc/api/types.hpp>

#include <designpattern/api/SmartPointer.hpp>

#include <os/api/IFile.hpp>

#include <database/api/ISequenceIterator.hpp>

#include <list>
#include <string>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Dump sequences into a FASTA file
 *
 * This class is a small utility for dumping sequences into an output FASTA file.
 *
 * Note: by now, the length of the data lines is set to 60 and is not a parameter
 * of the constructor, which could be improved.
 */
class FastaSequenceOutput : public dp::SmartPointer
{
public:

    /** Constructor.
     * \param[in] iterator : iteration of sequences to be dumped into the file
     * \param[in] filename : uri of the dump file
     * \param[in] comments : list of sequences comments; if not empty, it overrides the iterated comments
     */
    FastaSequenceOutput (ISequenceIterator* iterator, const char* filename, const std::list<std::string>& comments);

    /** Constructor.
     * \param[in] iterator : iteration of sequences to be dumped into the file
     * \param[in] filename : uri of the dump file
     */
    FastaSequenceOutput (ISequenceIterator* iterator, const char* filename);

    /** Destructor. */
    virtual ~FastaSequenceOutput ();

    /** Dump the sequences defined by the provided iterator in a file.
     */
    void dump ();

private:

    /** Iterator whose sequences have to be dumped into a file. */
    ISequenceIterator* _iterator;

    /** Smart setter for \ref _iterator attribute. */
    void setIterator (ISequenceIterator* iterator)  { SP_SETATTR(iterator); }

    /** Callback to be called during sequences iteration.
     * \param[in] seq : the currently iterated sequence
     */
    void dumpSequence (const ISequence* seq);

    /** File name */
    std::string _filename;

    /** File where the sequences are dumped. */
    os::IFile* _file;

    /** List of comments to be used for file dump. */
    const std::list<std::string> _comments;

    std::list<std::string>::const_iterator _itComments;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _FASTA_SEQUENCE_OUTPUT_HPP_ */
