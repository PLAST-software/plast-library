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

/** \file ISeed.hpp
 *  \brief Definition of the concept of seed
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  We define here the concept of seed that is central in the PLAST algorithm.
 *
 *  PLAST algorithm begins by finding small similarities between the two compared
 *  databases; such common similarities are called seeds.
 *
 *  Seeds may be belong to a seeds model, ie. some object that knows how to create
 *  and iterate seeds (see ISeedModel class).
 */

#ifndef _ISEED_HPP_
#define _ISEED_HPP_

/********************************************************************************/

#include <database/api/IAlphabet.hpp>
#include <database/api/IWord.hpp>
#include <misc/api/types.hpp>

#include <stdlib.h>
#include <string.h>

/********************************************************************************/
/** \brief Seed based concepts. */
namespace seed {
/********************************************************************************/

/** Definition of a hash code for encoding seeds. */
typedef u_int32_t SeedHashCode;

/** Define a constant encoding a bad or unknown hash code. */
const SeedHashCode BAD_SEED_HASH_CODE = ~0;

/** \brief Definition of a seed.
 *
 *  A seed is a small word, for instance containing 4 amino acids in case of protein
 *  database. For instance, "PQAD" is a seed.
 *
 *  This structure defines the information hold by a seed, at least the letters that compose
 *  the seed. Note that we can associate a hash code for these letters, which can speed up
 *  the algorithm by using integers rather strings.
 */
struct ISeed
{
    /** Seed content as a IWord structure. */
    database::IWord kmer;

    /** Perfect hash code encoding the kmer. Note that the hash function may be minimal or not. */
    SeedHashCode code;

    /** Offset of the seed in the buffer holding it (likely a sequence of a database). */
    Offset  offset;

    /** Constructor. */
    ISeed () : code(BAD_SEED_HASH_CODE), offset(0)   {}

    /** Constructor.
     * \param[in] span : span of the seed
     * \param[in] buffer : buffer containing the letters defining the seed.
     */
    ISeed (size_t span, const database::LETTER* buffer=0) : kmer(span, buffer), code(BAD_SEED_HASH_CODE), offset(0)   {}

    /** Affectation operator.
     * \param[in] s : the seed used for affectation
     * \return the affected instance
     */
    ISeed& operator= (const ISeed& s)
    {
        if (this != &s)
        {
            kmer   = s.kmer;
            code   = s.code;
            offset = s.offset;
        }

        return *this;
    }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEED_HPP_  */
