#ifndef _CACHED_SUB_DATABASE_HPP_
#define _CACHED_SUB_DATABASE_HPP_

/********************************************************************************/

#include <database/api/ISequenceDatabase.hpp>
#include <database/impl/AbstractSequenceIterator.hpp>

#include <set>
#include <vector>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/**
 *  \brief Database of not forcefully consequent sequences stored in memory
 *
 *  If a blacklist set is provided, the database will filter the sequences that
 *  it retrieves by id, possibly creating _holes_ of missing sequence indexes.
 */
class CachedSubDatabase : public ISequenceDatabase
{
public:
    CachedSubDatabase(ISequenceIterator* refIterator, std::set<u_int64_t>* blacklist = NULL);

    /** Destructor. */
    virtual ~CachedSubDatabase ();

    u_int64_t getSize ();

    /**
     * The actual number of sequences in the database, not a function of the index
     * of the last sequence.
     */
    size_t getSequencesNumber();

    bool getSequenceByIndex (size_t index, ISequence& sequence) ;

    /** Returns a sequence reference given its index. Note that all implementations may not support
     * this feature; indeed, this feature supposes that all ISequences instances that makes the
     * database are located in memory, so any reference on any of them is legal as long as the database
     * itself is alive.
     * \param[in]  index : the index of the wanted sequence
     * \return a reference to the sequence if found, NULL otherwise
     */
    ISequence* getSequenceRefByIndex (size_t index) ;

    /** Returns a sequence given an offset (in the database).
     *  Also returns the offset in the returned sequence and the actual offset in the database.
     *  \param[in] offset : offset of the sequence in the database
     *  \param[in] sequence : the filled sequence if successful
     *  \param[out] offsetInSequence : offset of the given offset in the sequence
     *  \param[out] offsetInDatabase : offset of the given offset in the database
     * \return true if the sequence has been retrieved, false otherwise.
     */
    bool getSequenceByOffset (
        u_int64_t offset,
        ISequence& sequence,
        u_int32_t& offsetInSequence,
        u_int64_t& offsetInDatabase
    ) ;

    /** Returns a sequence given a part of its string identifier (in the database).
     *  \param[in] id : part of the identifier
     *  \param[in] sequence : the filled sequence if successful
     * \return true if the sequence has been retrieved, false otherwise.
     */
    bool getSequenceByName (
        const std::string& id,
        ISequence& sequence
    ) ;

    /** Creates a Sequence iterator.
     * \return the created iterator.
     */
    ISequenceIterator* createSequenceIterator () ;

    /** Split the current database in several database. All the returned
     *  databases should represent the same set of ISequence instances than the source database.
     *  \param[in] nbSplit : number of parts we want to split the database
     *  \return a vector of split ISequenceDatabase instances.
     */
    std::vector<ISequenceDatabase*> split (size_t nbSplit) ;

    /** Return properties about the instance. These properties can provide information for statistical
     * purpose for instance.
     * \param[in] root : root string
     * \return a created IProperties instance
     */
    dp::IProperties* getProperties (const std::string& root) ;

    /** Returns some unique identifier for the database.
     *  For instance, implementations can rely on the uri of the database and the range
     *  used within it.
     * \return the identifier
     */
    std::string getId () ;

    /** Retrieve the identifiers of the sequences of the database.
     *  \param[out] ids : the set to be filled by the sequences identifiers.
     */
    void retrieveSequencesIdentifiers (std::set<std::string>& ids) ;

    /** Return the direction (PLUS or MINUS) of the strand for the sequences (meaningful only for nucleotides databases). */
    StrandId_e getDirection () ;

    /** Change the strand of the sequences (meaningful only for nucleotides databases). */
    void reverse () ;

    /** Accept method of the Visitor Design Pattern.
     * \param[in] v : the visitor to be accepted
     */
    void accept (DatabaseVisitor& v);

    /** see ISequenceIterator#transformComment for details
     *
     * The place for this is rather in the ISequence file, or somewhere else.
     */
    std::string transformComment (const ISequence& sequence);

protected:
    /** Identifier */
    std::string _id;

    void setId (const std::string& id);

private:

    ISequenceIterator* _sequenceIterator;

    void setSequenceIterator(ISequenceIterator* sequenceIterator) { SP_SETATTR(sequenceIterator); }

    std::set<u_int64_t>* _blacklist;

    std::vector<ISequence> _sequences;

    u_int64_t _size;

    /** */
    StrandId_e _direction;

    /**
     * Read and memorize the sequences from the database in _sequences.
     */
    void initializeCache();

    /** Reverse a sequence
     *
     *  This means that A turns into a T and C into a G (and the other way around)
     *  and that the resulting string is itself reversed.
     *
     *  NOTE ipetrov: The best place for this method is in the sequence class,
     *    but as the use of such function, if added, might break a number of
     *    things, I prefer to put it here */
    void reverseSequence(ISequence& sequence);
};

class CachedSubDatabaseIterator : public AbstractSequenceIterator
{
public:
    CachedSubDatabaseIterator(std::vector<ISequence> const & sequences);

    ~CachedSubDatabaseIterator();

    void first();
    dp::IteratorStatus  next();
    bool isDone();
    const ISequence* currentItem ();
    ISequenceIterator* clone ();

private:

    std::vector<ISequence> const & _sequences;
    std::vector<ISequence>::const_iterator _it;
};

}}

#endif /* _CACHED_SUB_DATABASE_HPP_ */
