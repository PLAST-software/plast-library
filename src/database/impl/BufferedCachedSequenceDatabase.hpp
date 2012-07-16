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

/** \file BufferedCachedSequenceDatabase.hpp
 *  \brief Sequence database in memory
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _BUFFERED_CACHED_SEQUENCE_DATABASE_HPP_
#define _BUFFERED_CACHED_SEQUENCE_DATABASE_HPP_

/********************************************************************************/

#include <database/impl/BufferedSequenceDatabase.hpp>
#include <vector>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief ISequenceDatabase implementation with all data kept in RAM.
  */
class BufferedCachedSequenceDatabase : public BufferedSequenceDatabase
{
public:

    /** */
    BufferedCachedSequenceDatabase (ISequenceIterator* refIterator, bool filterLowComplexity);

    /** Destructor. */
    virtual ~BufferedCachedSequenceDatabase ();

    /** */
    ISequence* getSequenceRefByIndex (size_t index)
    {
        return (index < _sequences.size() ? _sequences[index] : NULL);
    }

private:

    /** */
    BufferedCachedSequenceDatabase (
        const std::string& id,
        ISequenceCache* cache,
        size_t firstIdx,
        size_t lastIdx
    );

    /** */
    bool _isBuilt;
    void buildSequencesCache (void);

    /** */
    std::vector<ISequence*> _sequences;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _BUFFERED_CACHED_SEQUENCE_DATABASE_HPP_ */
