#define DEBUG(a)  printf a
#define INFO(a)   printf a

#include <database/impl/FastaSequencePureIterator.hpp>
#include <database/impl/AbstractSequenceIterator.hpp>
#include <designpattern/impl/FileLineIterator.hpp>
#include <database/impl/BasicSequenceBuilder.hpp>
#include <misc/api/types.hpp>

#include <misc/api/macros.hpp>

/********************************************************************************/
namespace database { namespace impl  {

FastaSequencePureIterator::FastaSequencePureIterator (const char* filename,
        size_t commentMaxSize,
        u_int64_t offset0,
        u_int64_t offset1)
    :  _rawData(0), _rawDataEnd(0), _isDone(0), _commentMaxSize(commentMaxSize),
    sequenceDbOffset(0), _fileIterator(filename, commentMaxSize, offset0, offset1)
{
    size_t maxDataSize = offset1 - offset0;
    _rawData = new char[maxDataSize];
    memset(_rawData, 0, maxDataSize * sizeof(char));
    AbstractSequenceIterator::setBuilder(new BasicSequenceBuilder(SUBSEED));
}

/** Destructor. */
FastaSequencePureIterator::~FastaSequencePureIterator ()
{
    delete[] _rawData;
}

/** \copydoc AbstractSequenceIterator::first */
void FastaSequencePureIterator::first()
{
    _rawDataEnd = 0;

    tmpSequence.index = -1;
    _fileIterator.first ();
    _isDone = false;

    next();
}

/** \copydoc AbstractSequenceIterator::next */
dp::IteratorStatus FastaSequencePureIterator::next()
{
    size_t commentStart = _rawDataEnd;
    u_int64_t currentSequenceOffset = 0;

    char* buffer = 0;

    _isDone = _fileIterator.isDone();

    /** We retrieve the builder => shortcut and optimization (avoid method call) */
    ISequenceBuilder* builder = getBuilder();

    /** We look for a comment. */
    for ( ; !_fileIterator.isDone(); _fileIterator.next())
    {
        /** We retrieve the current file line. */
        buffer = _fileIterator.currentItem();

        if  (buffer && buffer[0]=='>')  {
            // from the current position we subtract the length of the read
            // text + 1 for the newline at the end, which is also read, but
            // not counted by strlen.
            currentSequenceOffset = _fileIterator.tell() - strlen(buffer) - 1;
            break;
        }
    }

    /** We may have found a comment. */
    if (buffer == 0 || !builder)
    {
        return dp::ITER_UNKNOWN;
    }

    /** We may have to skip space characters between the '>' and the actual comment. */
    while (*(++buffer) == ' ')   {}

    size_t len = strlen (buffer);

    builder->setComment (buffer, MIN (len, _commentMaxSize) );

    /** We reset the data size. */
    builder->resetData ();

    /** We read the next line (the current one still points to the comment line). */
    _fileIterator.next();

    for ( ; !_fileIterator.isDone(); _fileIterator.next())
    {
        /** We retrieve the current file line. */
        buffer = _fileIterator.currentItem();

        if (buffer && buffer[0]=='>')  { break; }

        /** We add the character to the sequence buffer. */
        builder->addData ((LETTER*)buffer, strlen(buffer), ASCII);
    }

    ISequence* currentSequence = builder->getSequence();
    const char* comment = currentSequence->comment;
    size_t commentLength = strlen(comment);
    // We would like to keep the '\0' character
    size_t currentSequenceDataStart = commentLength + commentStart + 1;
    size_t commentBytesCount = commentLength * sizeof(char);

    copyToRawData(_rawDataEnd, comment, commentBytesCount);

    size_t dataLength = currentSequence->getLength();
    size_t dataBytes = dataLength * sizeof(LETTER);
    copyToRawData(currentSequenceDataStart, currentSequence->getData(), dataBytes);

    updateSequence(commentStart, currentSequenceDataStart, dataLength, currentSequenceOffset);

    _rawDataEnd = currentSequenceDataStart + dataBytes;

    return dp::ITER_UNKNOWN;
}

void FastaSequencePureIterator::copyToRawData(size_t start, const void* from, size_t lengthBytes)
{
    memcpy(_rawData + start, from, lengthBytes);
}

void FastaSequencePureIterator::updateSequence(size_t commentStart, size_t dataStart,
        size_t dataSize, u_int64_t dbOffset)
{
    tmpSequence.index++;
    tmpSequence.data.setReference(dataSize, _rawData + dataStart);
    tmpSequence.comment = _rawData + commentStart;
    tmpSequence.offsetInDb = dbOffset;
}

/** \copydoc AbstractSequenceIterator::isDone */
bool FastaSequencePureIterator::isDone()
{
    return _isDone;
}

/** \copydoc AbstractSequenceIterator::currentItem */
ISequence* FastaSequencePureIterator::currentItem()
{
    return &tmpSequence;
}

/** \copydoc AbstractSequenceIterator::clone */
ISequenceIterator* FastaSequencePureIterator::clone ()
{
    return 0;
}

/** \copydoc ISequenceIterator::setBuilder */
void FastaSequencePureIterator::setBuilder (ISequenceBuilder* builder)
{
    throw "Unsupported operation FastaSequencePureIterator::setBuilder";
}

}}
