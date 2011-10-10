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

#ifndef _ISEED_HPP_
#define _ISEED_HPP_

/********************************************************************************/

#include "MemoryAllocator.hpp"
#include "IAlphabet.hpp"
#include "IWord.hpp"
#include "types.hpp"
#include <stdlib.h>
#include <string.h>

/********************************************************************************/
namespace seed {
/********************************************************************************/

/** Definition of a hash code for encoding seeds. */
typedef u_int32_t SeedHashCode;

/** Define a constant encoding a bad or unknown hash code. */
const SeedHashCode BAD_SEED_HASH_CODE = ~0;

/** Define the information hold by a seed. */
struct ISeed
{
    /** Seed content as a IWord structure. */
    database::IWord kmer;

    /** Perfect hash code encoding the kmer. Note that the hash function may be minimal or not. */
    SeedHashCode code;

    /** Offset of the seed in the buffer holding it (likely a sequence of a database). */
    Offset  offset;

    /** constructors. */
    ISeed () : code(BAD_SEED_HASH_CODE), offset(0)   {}

    ISeed (size_t span, const database::LETTER* buffer=0) : kmer(span, buffer), code(BAD_SEED_HASH_CODE), offset(0)   {}

    /** */
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
