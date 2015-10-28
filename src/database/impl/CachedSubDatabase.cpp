#include <database/impl/CachedSubDatabase.hpp>
#include <database/api/ISequenceDatabase.hpp>
#include <designpattern/impl/Property.hpp>

#include <set>
#include <vector>

namespace database { namespace impl {

CachedSubDatabase::CachedSubDatabase(ISequenceIterator* refIterator, std::set<u_int64_t>* blacklist)
    : _sequenceIterator(0),
      _blacklist(blacklist),
      _size(0),
      _direction(ISequenceDatabase::PLUS)
{
    setSequenceIterator(refIterator);
    initializeCache();
    setId(refIterator->getId());
}

CachedSubDatabase::~CachedSubDatabase() {
    setSequenceIterator(0);
}

void CachedSubDatabase::initializeCache()
{
    int i = 0;
    _size = 0;
    for (_sequenceIterator->first(); !_sequenceIterator->isDone(); _sequenceIterator->next()) {
        _sequences.push_back(*_sequenceIterator->currentItem());
        ISequence& currentSequence = _sequences[_sequences.size() - 1];
        currentSequence.database = this;

        _size += currentSequence.getLength();
        if (_blacklist && _blacklist->find(currentSequence.offsetInDb) != _blacklist->end()) {
            currentSequence.data.letters.reset();
        }
        i++;
    }
}

u_int64_t CachedSubDatabase::getSize ()
{
    return _size;
}

bool CachedSubDatabase::getSequenceByIndex (size_t index, ISequence& sequence)
{
    ISequence* result = getSequenceRefByIndex(index);

    if (result == NULL) {
        return false;
    }

    sequence = *result;

    return true;
}

size_t CachedSubDatabase::getSequencesNumber()
{
    throw "Unsupported operation CachedSubDatabase::getSequencesNumber";
    return _sequences.size();
}

ISequence* CachedSubDatabase::getSequenceRefByIndex (size_t index)
{
    std::vector<ISequence>::iterator it = _sequences.begin();

    for (; it != _sequences.end(); ++it) {
        if (it->index == index) {
            return &(*it);
        }
    }
    throw "Unsupported operation CachedSubDatabase::getSequenceRefByIndex";

    return NULL;
}

bool CachedSubDatabase::getSequenceByOffset(
        u_int64_t offset,
        ISequence& sequence,
        u_int32_t& offsetInSequence,
        u_int64_t& offsetInDatabase)
{
    throw "Unsupported operation CachedSubDatabase::getSequenceByOffset";

    for (std::vector<ISequence>::iterator it = _sequences.begin(); it != _sequences.end(); ++it) {
        if (it->offsetInDb > offset) {
            if (it != _sequences.begin()) {
                it--;
                sequence = (*it);
                offsetInDatabase = offset;
                offsetInSequence = (offset - sequence.offsetInDb);

                return true;
            }

            break;
        }
    }

    return false;
}

bool CachedSubDatabase::getSequenceByName(const std::string& id, ISequence& sequence)
{
    throw "Unsupported operation CachedSubDatabase::getSequenceByName";
    for (std::vector<ISequence>::iterator it = _sequences.begin(); it != _sequences.end(); ++it) {
        if (it->getComment().find(id) != std::string::npos) {
            sequence = *it;

            return true;
        }
    }

    return false;
}

ISequenceIterator* CachedSubDatabase::createSequenceIterator ()
{
    return new CachedSubDatabaseIterator(this->_sequences);
}

std::vector<ISequenceDatabase*> CachedSubDatabase::split (size_t nbSplit)
{
    std::vector<ISequenceDatabase*> result;

    throw "Unsupported operation CachedSubDatabase::split";
}

dp::IProperties* CachedSubDatabase::getProperties (const std::string& root)
{
    throw "Unsupported operation CachedSubDatabase::getProperties";
}

std::string CachedSubDatabase::getId ()
{
    return _id;
}

void CachedSubDatabase::setId (const std::string& id)
{
    _id = id;
}

void CachedSubDatabase::retrieveSequencesIdentifiers (std::set<std::string>& ids)
{
    throw "Unsupported operation CachedSubDatabase::retrieveSequencesIdentifiers";

    for (std::vector<ISequence>::iterator it = _sequences.begin(); it != _sequences.end(); ++it) {
        std::string const & comment = it->getComment();

        char* locate = ISequence::searchIdSeparator (comment.c_str());

        if (locate != 0)
        {
            ids.insert (comment.substr (0, locate - comment.c_str()));
        }
        else
        {
            ids.insert (comment);
        }
    }
}

ISequenceDatabase::StrandId_e CachedSubDatabase::getDirection ()
{
    return _direction;
}

void CachedSubDatabase::reverseSequence(ISequence& sequence) {
    throw "Unsupported operation CachedSubDatabase::reverseSequences";

    IWord& data = sequence.data;
    LETTER* sequenceLetters = data.letters.data;
    size_t length = sequence.getLength();

    for (size_t i = 0; i < (length >> 1); i++) {
        std::swap (sequenceLetters[i], sequenceLetters[length - 1 - i]);
    }

    for (size_t i = 0; i < length; i++) {
        /** Residues above value 4 (like N) are not reversed. */
        if (sequenceLetters[i]<4)
        {
            /** Note here the way we get the complement of a nucleotide: comp(c) -> (c+2)%4
             * It is possible since we have A=0, C=1, G=3, T=2.
             */
            sequenceLetters[i] = (sequenceLetters[i] + 2) & 3;
        }
    }
}

void CachedSubDatabase::reverse ()
{
    for (std::vector<ISequence>::iterator it = _sequences.begin(); it != _sequences.end(); ++it) {
        reverseSequence(*it);
    }

    _direction = (_direction == ISequenceDatabase::PLUS ?  ISequenceDatabase::MINUS : ISequenceDatabase::PLUS);
}

void CachedSubDatabase::accept (DatabaseVisitor& v)
{
    v.visitCachedSubDatabase(*this);
}

std::string CachedSubDatabase::transformComment(const ISequence& sequence) {
    return _sequenceIterator->transformComment(sequence.comment);
}

CachedSubDatabaseIterator::CachedSubDatabaseIterator(std::vector<ISequence> const & sequences)
    : _sequences(sequences) {}

CachedSubDatabaseIterator::~CachedSubDatabaseIterator() {}

void CachedSubDatabaseIterator::first()
{
    _it = _sequences.begin();
}

dp::IteratorStatus CachedSubDatabaseIterator::next()
{
    if (isDone()) {
        return dp::ITER_DONE;
    }

    _it++;

    return dp::ITER_UNKNOWN;
}

bool CachedSubDatabaseIterator::isDone()
{
    return _it == _sequences.end();
}

const ISequence* CachedSubDatabaseIterator::currentItem ()
{
    return &(*_it);
}

ISequenceIterator* CachedSubDatabaseIterator::clone () {
    return new CachedSubDatabaseIterator(this->_sequences);
}

}}
