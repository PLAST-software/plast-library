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

/*****************************************************************************
 *****************************************************************************/

#ifndef _IDATABASE_INDEX_HPP_
#define _IDATABASE_INDEX_HPP_

/********************************************************************************/

#include "ISeedModel.hpp"
#include "ISequenceDatabase.hpp"
#include "ISequenceIterator.hpp"
#include "IOccurrenceIterator.hpp"

/********************************************************************************/
namespace indexation {
/********************************************************************************/

class IDatabaseIndex : public dp::SmartPointer
{
public:

    /** Returns the sequences database. */
    virtual database::ISequenceDatabase* getDatabase () = 0;

    /** Returns the seed model used for the indexation. */
    virtual seed::ISeedModel* getModel () = 0;

    /** Builds the index for the database with the choosen seed model. */
    virtual void build () = 0;

    /** Creates an iterator on occurrences for a given seed key. */
    virtual IOccurrenceIterator* createOccurrenceIterator (const seed::ISeed* seed, size_t neighbourhoodSize=0) = 0;

    /** Creates an iterator on occurrences for a given seed key. */
    virtual IOccurrenceBlockIterator* createOccurrenceBlockIterator (
        const seed::ISeed* seed,
        size_t neighbourhoodSize,
        size_t blockSize
    ) = 0;

    /** Returns the number of occurrences for a given seed. */
    virtual size_t getOccurrenceNumber (const seed::ISeed* seed) = 0;

    /** Add/Remove a child index (Design Pattern Composite). */
    virtual void addChildIndex    (IDatabaseIndex* child) = 0;
    virtual void removeChildIndex (IDatabaseIndex* child) = 0;

    /** Merge children indexes. */
    virtual void merge (void) = 0;

    /** Return properties about the instance. */
    virtual dp::IProperties* getProperties (const std::string& root) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IDATABASE_INDEX_HPP_  */
