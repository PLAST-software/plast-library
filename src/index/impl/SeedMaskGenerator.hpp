#ifndef _SEED_MASK_GENERATOR_HPP
#define _SEED_MASK_GENERATOR_HPP

#include <iostream>
#include <queue>

#include <database/api/ISequence.hpp>

#include <designpattern/api/SmartPointer.hpp>

#include <index/api/ISeedMaskGenerator.hpp>

typedef database::LETTER LETTER;
typedef u_int8_t uint8_t;
typedef u_int32_t uint32_t;
typedef u_int64_t uint64_t;
// the forward and reverse indexes
typedef std::pair<uint64_t, uint64_t> KmerIndexes;
typedef std::pair<uint64_t, KmerIndexes> KmerWithCount;

/**
 * Comparator for instances of KmerWithCount that compares only their count field.
 */
class KmerComparator {
  public:
    bool operator()(const KmerWithCount& first, const KmerWithCount& second) {
      return first.first < second.first;
    }
};

typedef std::priority_queue<KmerWithCount, std::vector<KmerWithCount>, KmerComparator> KmerWithCountPriorityQueue;

class SeedMaskGenerator : public dp::SmartPointer, public ISeedMaskGenerator {
public:
    SeedMaskGenerator(size_t kmerSize, std::string& queryPath, std::string& databasePath, size_t countToKeep);

    virtual ~SeedMaskGenerator();

    /**
     * Returns the underlying bitset
     *
     * Note that if the object is destroyed, the underlying bitset will be deleted.
     */
    const uint8_t* getBitset();

    /**
     * Returns the size in bytes of the underlying bitset
     */
    size_t getBitsetSize();

    /**
     * Compute the number of kmers that are used/present in the bitset.
     */
    uint64_t getKmersUsedCount();

private:
    size_t kmerSize;

    uint8_t* bitset;

    uint32_t bitsetSize;

    std::string& queryPath;

    std::string& databasePath;

    size_t countToKeep;

    /**
     * Fill in the bitset, using the information from the provided database, the
     * kmerSize and the count of the kmers that we should try to keep for each
     * sequence in the database.
     */
    void compute();

    /**
     * Add the number of occurrences of each kmer from a sequence to the kmerToCountMap
     */
    void countKmersInSequence(database::ISequence* data,
            uint64_t* const kmerToCountMap);

    /**
     * Selects the best countToKeep kmers for a given sequence
     */
    void findBestKmers(database::ISequence* data,
            uint64_t* const queryKmerToCountMap,
            uint64_t* const kmerToCountMap,
            KmerWithCountPriorityQueue& bestKmersWithCounts);

    /**
     * Computes the index of a kmer in the way that plast/klast computes it
     */
    uint64_t getKmerIndex(const LETTER* kmer);

    /**
     * Computes the index of the inverse complimentary kmer of the provided one
     * in the way that plast/klast computes it
     */
    uint64_t getReverseComplimentaryKmerIndex(const LETTER* kmer);

    /**
     * Computes the index of the inverse complimentary kmer of the provided one
     * in the way that plast/klast computes it
     */
    uint64_t getReverseComplimentaryKmerIndex(uint64_t kmerIndex);

    /**
     * Sets kmers to be present in the bitset
     */
    void addBestKmersToBitset(KmerWithCountPriorityQueue& bestKmersWithCounts);

    /**
     * Find the size in bytes of a file with the given filepath/filename
     */
    uint64_t getFileSize(std::string& filename);
};

#endif /* SEED_MASK_GENERATOR_HPP */
