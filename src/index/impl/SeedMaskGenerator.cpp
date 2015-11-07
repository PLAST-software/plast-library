#include <iostream>
#include <cmath>

#include <index/impl/SeedMaskGenerator.hpp>

#include <database/impl/FastaSequencePureIterator.hpp>

const uint64_t INF = (1 << 30);

inline void setInBitset(uint8_t* const bitset, uint64_t index, int value) {
  size_t elementIndex = (index >> 3);
  // The binary representation of 7 is 111, so and gives the last three bits
  // inside the index.
  int bitIndex = (index & 7);
  int setMask = (1 << bitIndex);
  int mask = (~(setMask));

  // We clear the old value and we put the value from the parameter.
  bitset[elementIndex] = ((bitset[elementIndex] & mask) | ((value << bitIndex) & setMask));
}

inline uint64_t getInBitset(uint8_t* const bitset, uint64_t index) {
  size_t elementIndex = (index >> 3);
  // the binary representation of 7 is 111, so and gives the last three bits
  // inside the index.
  int bitIndex = (index & 7);
  int getMask = (1 << bitIndex);

  return bitset[elementIndex] & getMask;
}

SeedMaskGenerator::SeedMaskGenerator(size_t kmerSize, std::string& queryPath, std::string& databasePath, size_t countToKeep)
    : kmerSize(kmerSize),
    queryPath(queryPath),
    databasePath(databasePath),
    countToKeep(countToKeep)
{
    bitsetSize = (1 << (2 * kmerSize - 3));

    bitset = new uint8_t[bitsetSize];
    memset(bitset, 0, bitsetSize * sizeof(uint8_t));

    compute();
}

SeedMaskGenerator::~SeedMaskGenerator()
{
    delete[] this->bitset;
}

void SeedMaskGenerator::compute()
{
    size_t kmerToCountMapSize = (1 << (2 * kmerSize));
    uint64_t* const subjectKmerToCountMap = new uint64_t[kmerToCountMapSize];
    memset(subjectKmerToCountMap, 0, sizeof(uint64_t) * kmerToCountMapSize);

    uint64_t commentMaxSize = 1024;
    uint64_t subjectFileSize = getFileSize(databasePath);

    int count = 0;
    database::impl::FastaSequencePureIterator subjectSequencesIterator(databasePath.c_str(), commentMaxSize, 0, subjectFileSize);
    for (subjectSequencesIterator.first(); !subjectSequencesIterator.isDone(); subjectSequencesIterator.next()) {
        countKmersInSequence(subjectSequencesIterator.currentItem(), subjectKmerToCountMap);
        count++;
    }

    uint64_t* const queryKmerToCountMap = new uint64_t[kmerToCountMapSize];
    memset(queryKmerToCountMap, 0, sizeof(uint64_t) * kmerToCountMapSize);
    uint64_t queryFileSize = getFileSize(queryPath);

    database::impl::FastaSequencePureIterator querySequencesIterator(queryPath.c_str(), commentMaxSize, 0, queryFileSize);
    count = 0;

    for (querySequencesIterator.first(); !querySequencesIterator.isDone(); querySequencesIterator.next()) {
        countKmersInSequence(querySequencesIterator.currentItem(), queryKmerToCountMap);
        count++;
    }

    count = 0;
    for (querySequencesIterator.first(); !querySequencesIterator.isDone(); querySequencesIterator.next()) {
        KmerWithCountPriorityQueue bestKmersWithCounts;

        findBestKmers(querySequencesIterator.currentItem(),
                queryKmerToCountMap,
                subjectKmerToCountMap,
                bestKmersWithCounts);

        addBestKmersToBitset(bestKmersWithCounts);
        count++;
    }

    delete[] subjectKmerToCountMap;
    delete[] queryKmerToCountMap;
}

const uint8_t* SeedMaskGenerator::getBitset()
{
    return this->bitset;
}

size_t SeedMaskGenerator::getBitsetSize()
{
    return this->bitsetSize;
}

uint64_t SeedMaskGenerator::getKmersUsedCount()
{
    uint64_t kmersUsed = 0;
    for(size_t i = 0; i < bitsetSize; i++) {
        kmersUsed += __builtin_popcount(bitset[i]);
    }

    return kmersUsed;
}

void SeedMaskGenerator::countKmersInSequence(database::ISequence* data, uint64_t* const kmerToCountMap)
{
    size_t dataSize = data->getLength();
    const LETTER* letters = data->getData();

    for (size_t startIndex = 0; startIndex + kmerSize - 1 < dataSize; startIndex++) {
        uint64_t kmerIndex = getKmerIndex(letters + startIndex);

        if (kmerIndex < INF) {
            kmerToCountMap[kmerIndex]++;
        }
    }
}

void SeedMaskGenerator::findBestKmers(database::ISequence* data,
        uint64_t* const queryKmerToCountMap,
        uint64_t* const kmerToCountMap,
        KmerWithCountPriorityQueue& bestKmersWithCounts)
{
    size_t usedSize = (1 << (2 * kmerSize - 3));
    uint8_t* const used = new uint8_t[usedSize];
    memset(used, 0, usedSize * sizeof(uint8_t));

    size_t sequenceSize = data->getLength();
    int kmersHistogramIndex = 0;
    uint64_t* kmersHistogram = new uint64_t[sequenceSize];
    uint64_t* kmersHashes = new uint64_t[sequenceSize];

    // We iterate the kmers.
    size_t dataSize = data->getLength();
    const LETTER* letters = data->getData();

    for (size_t startIndex = 0; startIndex + kmerSize - 1 < dataSize; startIndex++) {
        // NOTE ipetrov: It is not necessary to recalculate this every time from scratch.
        uint64_t kmerValueInt = getKmerIndex(letters + startIndex);

        if (kmerValueInt == INF) {
            continue;
        }

        uint64_t kmerQueryCount = queryKmerToCountMap[kmerValueInt];
        uint64_t kmerDbCount = kmerToCountMap[kmerValueInt];
        uint64_t kmerCount = kmerQueryCount * kmerDbCount;
        if (kmerCount == 0) {
            kmerCount = INF;
        }

        kmersHistogram[kmersHistogramIndex] = kmerCount;
        kmersHashes[kmersHistogramIndex] = kmerValueInt;

        kmersHistogramIndex++;
    }

    int groupSize = kmersHistogramIndex / (countToKeep + 1);
    if (groupSize == 0) groupSize = 1;

    int bestSubsetSize = std::max(1UL, std::min(countToKeep, kmersHistogramIndex / (2 * kmerSize)));
    uint64_t ** const bestScores = new uint64_t*[bestSubsetSize];
    int ** const prevs = new int*[bestSubsetSize];
    for (int i = 0; i < bestSubsetSize; i++) {
        bestScores[i] = new uint64_t[kmersHistogramIndex];
        prevs[i] = new int[kmersHistogramIndex];
    }

    bestScores[0][0] = kmersHistogram[0];
    prevs[0][0] = -1;
    for (int i = 1; i < kmersHistogramIndex; i++) {
        bestScores[0][i] = kmersHistogram[i];
        prevs[0][i] = -1;
    }
    for (int i = 1; i < bestSubsetSize; i++) {
        for (int j = 0; j < kmersHistogramIndex; j++) {
            bestScores[i][j] = INF;
            prevs[i][j] = -1;

            for (int k = j - kmerSize; k >= 0; k--) {
                if (kmersHistogram[j] + bestScores[i - 1][k] < bestScores[i][j]) {
                    bestScores[i][j] = kmersHistogram[j] + bestScores[i - 1][k];
                    prevs[i][j] = k;
                }
            }
        }
    }

    long* bestIndexes = new long[kmersHistogramIndex];

    size_t currentOptimalIndex = 0;
    int start = 0;
    uint64_t best = INF;

    for (int i = 0; i < kmersHistogramIndex; i++) {
        if (bestScores[bestSubsetSize - 1][i] < best) {
            best = bestScores[bestSubsetSize - 1][i];
            start = i;
        }
    }

    if (best < INF) {
        int level = bestSubsetSize - 1;
        int current = start;
        do {
            bestIndexes[currentOptimalIndex++] = current;
            current = prevs[level][current];
            level--;
        } while(level >= 0);
    }

    int alreadyChosen = currentOptimalIndex;

    for (size_t startIndex = 0; startIndex + kmerSize - 1 < dataSize; startIndex++) {
        const LETTER* kmer = letters + startIndex;
        uint64_t kmerValueInt = getKmerIndex(kmer);

        if (kmerValueInt == INF) {
            continue;
        }

        if (getInBitset(used, kmerValueInt)) {
            continue;
        }

        setInBitset(used, kmerValueInt, 1);
        uint64_t kmerQueryCount = queryKmerToCountMap[kmerValueInt];
        uint64_t kmerDbCount = kmerToCountMap[kmerValueInt];
        uint64_t kmerCount = kmerQueryCount * kmerDbCount;
        if (kmerCount == 0) {
            continue;
        }
        if (kmerCount > 0 && countToKeep - alreadyChosen > 0 &&
                (bestKmersWithCounts.size() < countToKeep - alreadyChosen
                 || (bestKmersWithCounts.top().first > kmerCount))) {
            if (bestKmersWithCounts.size() >= countToKeep - alreadyChosen) {
                bestKmersWithCounts.pop();
            }
            uint64_t reverseIndex = getReverseComplimentaryKmerIndex(kmer);
            KmerWithCount currentKmerWithCount(kmerCount, KmerIndexes(kmerValueInt, reverseIndex));
            bestKmersWithCounts.push(currentKmerWithCount);
        }
    }

    for (int i = 0; i < alreadyChosen; i++) {
        size_t kmerStartPosition = bestIndexes[i];

        uint64_t forwardIndex = kmersHashes[kmerStartPosition];
        uint64_t reverseIndex = getReverseComplimentaryKmerIndex(forwardIndex);

        KmerIndexes kmerIndexes(forwardIndex, reverseIndex);
        KmerWithCount currentKmerWithCount(kmersHistogram[kmerStartPosition], kmerIndexes);
        bestKmersWithCounts.push(currentKmerWithCount);
    }

    for (int i = 0; i < bestSubsetSize; i++) {
        delete[] bestScores[i];
        delete[] prevs[i];
    }

    delete[] bestScores;
    delete[] kmersHistogram;
    delete[] kmersHashes;
    //delete[] minimums;
    delete[] bestIndexes;
    delete[] used;
}

uint64_t SeedMaskGenerator::getKmerIndex(const LETTER* kmer)
{
    uint64_t result = 0;
    for (int i = kmerSize - 1; i >= 0; i--) {
        LETTER currentLetter = kmer[i];
        if (currentLetter > 3) {
            return INF;
        }
        result = ((result << 2) | (currentLetter & 3));
    }

    uint64_t backwardResult = getReverseComplimentaryKmerIndex(result);

    return (backwardResult < result) ? backwardResult : result;
}

uint64_t SeedMaskGenerator::getReverseComplimentaryKmerIndex(const LETTER* kmer)
{
    uint64_t result = 0;
    for (size_t i = 0; i < kmerSize; i++) {
        LETTER currentLetter = kmer[i];
        if (currentLetter > 3) {
            return INF;
        }
        // NOTE ipetrov: the second part of the expression should be equivalent to
        // (kmerIndex & 3) ^ 2
        result = ((result << 2) | ((currentLetter + 2) & 3));
    }

    uint64_t forwardResult = getReverseComplimentaryKmerIndex(result);

    return (forwardResult > result) ? forwardResult : result;
}

uint64_t SeedMaskGenerator::getReverseComplimentaryKmerIndex(uint64_t kmerIndex)
{
    if (kmerIndex == INF) {
        return INF;
    }

    uint64_t result = 0;
    for (size_t i = 0; i < kmerSize; i++) {
        // NOTE ipetrov: the second part of the expression should be equivalent to
        // (kmerIndex & 3) ^ 2
        result = ((result << 2) | (((kmerIndex & 3) + 2) & 3));
        kmerIndex = (kmerIndex >> 2);
    }

    return result;
}

void SeedMaskGenerator::addBestKmersToBitset(KmerWithCountPriorityQueue& bestKmersWithCounts)
{
    while(!bestKmersWithCounts.empty()) {
        KmerWithCount kmerWithCounts = bestKmersWithCounts.top();
        KmerIndexes& kmerIndexes = kmerWithCounts.second;
        bestKmersWithCounts.pop();
        uint64_t& forwardIndex = kmerIndexes.first;
        uint64_t& reverseIndex = kmerIndexes.second;

        setInBitset(bitset, forwardIndex, 1);

        setInBitset(bitset, reverseIndex, 1);
    }
}

uint64_t SeedMaskGenerator::getFileSize(std::string& filename)
{
    FILE* inFile = fopen(filename.c_str(), "rb");

    if (!inFile) {
        throw "Error reading bitset file";
    }

    fseek (inFile, 0, SEEK_END);
    size_t length = ftell(inFile);

    fclose(inFile);

    return length;
}
