/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.0, released May 2015                                          *
 *   Copyright (c) 2015                                                      *
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


#include <index/impl/FakeDatabaseNucleotideIndex.hpp>

#include <os/impl/DefaultOsFactory.hpp>

#include <math.h>
#include <cstdlib>

using namespace database;
using namespace seed;
using namespace indexation;

/********************************************************************************/
namespace indexation { namespace impl {
/********************************************************************************/

enum { WORD_SIZE = sizeof(FakeDatabaseNucleotideIndex::word_t) * 8 };


/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
FakeDatabaseNucleotideIndex::FakeDatabaseNucleotideIndex (ISequenceDatabase* database,
    ISeedModel* model,
    std::string kmersBitsetPath)
    : AbstractDatabaseIndex (database, model), _maskOut(0), _maskSize(0), kmersBitsetPath(kmersBitsetPath)
{
	// compute the mask size
	size_t alphabetSize = getModel()->getAlphabet()->size;
	size_t maxSeedsNumber = pow((double)alphabetSize,(double)getModel()->getSpan());
	_maskSize = maxSeedsNumber / WORD_SIZE;
	_maskOut = new word_t [_maskSize];
	// init the mask with 0 for all entries
	memset (_maskOut, 0, sizeof(word_t)*_maskSize);
}


/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
FakeDatabaseNucleotideIndex::~FakeDatabaseNucleotideIndex ()
{
    delete[] _maskOut;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void FakeDatabaseNucleotideIndex::build ()
{
    FILE* inFile = fopen(kmersBitsetPath.c_str(), "rb");

    if (!inFile)
    {
        throw "Error reading bitset file";
    }

    // get length of file:
    fseek (inFile, 0, SEEK_END);
    size_t length = ftell(inFile);
    rewind(inFile);

    if (length != _maskSize * sizeof(word_t))
    {
        throw "File size incorrect";
    }

    size_t bytesRead = fread((char*)_maskOut, sizeof(char), length, inFile);

    if (bytesRead != length)
    {
        // Incorrect size read => Error
        throw "Bitset file read error";
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IDatabaseIndex::IndexEntry& FakeDatabaseNucleotideIndex::getEntry (const seed::ISeed* seed)
{
//	return NULL;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t FakeDatabaseNucleotideIndex::getOccurrenceNumber (const seed::ISeed* seed)
{
    return 0;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
u_int64_t FakeDatabaseNucleotideIndex::getTotalOccurrenceNumber ()
{
    return 0;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/




