#ifndef _ISEED_MASK_GENERATOR_HPP
#define _ISEED_MASK_GENERATOR_HPP

#include <designpattern/api/SmartPointer.hpp>

#include <misc/api/types.hpp>

/********************************************************************************/
/** \brief Genomic database indexation concepts. */
namespace indexation {
/********************************************************************************/

/**
 * \brief Interface for the generators of seed bitmasks
 *
 * Declares the operations for obtaining the result of the generation.
 */
class ISeedMaskGenerator : virtual public dp::ISmartPointer {
public:
    /**
     * Returns the underlying bitset
     *
     * Note that if the object is destroyed, the underlying bitset will be deleted.
     */
    virtual const u_int8_t* getBitset() = 0;

    /**
     * Returns the size in bytes of the underlying bitset
     */
    virtual size_t getBitsetSize() = 0;

    /**
     * Compute the number of kmers that are used/present in the bitset.
     */
    virtual u_int64_t getKmersUsedCount() = 0;
};

} // indexation

#endif /* ISEED_MASK_GENERATOR_HPP */
