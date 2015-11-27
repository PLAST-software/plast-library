/** \file FastaSequencePureIterator.hpp
 *  \brief Sequence iterator that returns the sequences from a part of a FASTA
 *  file, without mutating any external object (an external builder for example).
 *  \date 19/10/2015
 *  \author ipetrov
 */

#ifndef _FASTA_SEQUENCE_PURE_ITERATOR_HPP_
#define _FASTA_SEQUENCE_PURE_ITERATOR_HPP_

/********************************************************************************/

#include <database/impl/AbstractSequenceIterator.hpp>
#include <designpattern/impl/FileLineIterator.hpp>
#include <misc/api/types.hpp>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/
/**
 * An iterator over the sequences in a fasta file that could not be used with
 * expectations for side effects on the builder (which is the case with some
 * other implementations).
 *
 * NOTE ipetrov: If you change the sequences that you retrieve, this is at your
 *   own risk.
 */
class FastaSequencePureIterator : public AbstractSequenceIterator
{
public:

    /** Constructor.
     * \param[in] filename : path of the FASTA file to be read.
     * \param[in] commentMaxSize : maximum size of sequences comments
     * \param[in] offset0 : starting offset in the file
     * \param[in] offset1 : ending offset in the file
     */
    FastaSequencePureIterator ( const char* filename,
        size_t commentMaxSize = 2*1024,
        u_int64_t offset0 = 0,
        u_int64_t offset1 = 0
    );

    /** Destructor. */
    virtual ~FastaSequencePureIterator ();

    /** \copydoc AbstractSequenceIterator::first */
    void first();

    /** \copydoc AbstractSequenceIterator::next */
    dp::IteratorStatus next();

    /** \copydoc AbstractSequenceIterator::isDone */
    bool isDone();

    /** \copydoc AbstractSequenceIterator::currentItem */
    ISequence* currentItem();

    /** \copydoc AbstractSequenceIterator::clone */
    ISequenceIterator* clone ();

    /** Unsupported operation */
    void setBuilder (ISequenceBuilder* builder);

    /** Set the encoding scheme.
     * \param[in] encoding : the encoding scheme
     */
    void setEncoding (Encoding encoding);

    /** Get the encoding scheme.
     * \return the encoding scheme
     */
    Encoding getEncoding ();

    /** \copydoc ISequenceIterator::transformComment */
    std::string transformComment (const char* comment) {return comment; };

private:
    /** For memory performance reasons we are going to store the data in a flat
     *    array and each sequence will be refering to positions in this array.
     *    If we don't do this, a number of sequence will contain any given letter.
     *    from the input file. */
    char* _rawData;

    size_t _rawDataEnd;

    bool _isDone;

    /** Maximum size of a sequence comment. */
    size_t _commentMaxSize;

    u_int64_t sequenceDbOffset;

    ISequence tmpSequence;

    /** File line iterator for reading the file line by line. */
    dp::impl::FileLineIterator _fileIterator;

    void copyToRawData(size_t start, const void* from, size_t lengthBytes);

    void updateSequence(size_t commentStart, size_t dataStart, size_t dataLength, u_int64_t dbOffset);
};

}}

#endif /* _FASTA_SEQUENCE_PURE_ITERATOR_HPP_ */
