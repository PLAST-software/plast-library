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

using namespace database;
using namespace seed;
using namespace indexation;

/********************************************************************************/
namespace indexation { namespace impl {
/********************************************************************************/

/********************************************************************************/
inline static SeedHashCode revcomp (SeedHashCode x, size_t sizeKmer)
{
	u_int64_t res = x;

	res = ((res>> 2 & 0x3333333333333333) | (res & 0x3333333333333333) <<  2);
	res = ((res>> 4 & 0x0F0F0F0F0F0F0F0F) | (res & 0x0F0F0F0F0F0F0F0F) <<  4);
	res = ((res>> 8 & 0x00FF00FF00FF00FF) | (res & 0x00FF00FF00FF00FF) <<  8);
	res = ((res>>16 & 0x0000FFFF0000FFFF) | (res & 0x0000FFFF0000FFFF) << 16);
	res = ((res>>32 & 0x00000000FFFFFFFF) | (res & 0x00000000FFFFFFFF) << 32);
	res = res ^ 0xAAAAAAAAAAAAAAAA;

	return (res >> (2*(32-sizeKmer))) ;
}

/********************************************************************************/


enum { WORD_SIZE = sizeof(FakeDatabaseNucleotideIndex::word_t) * 8 };

inline int bindex(int b)  { return b / WORD_SIZE; }
inline int boffset(int b) { return b % WORD_SIZE; }

#define SETMASK(data,b)  data[bindex(b)] |= 1LL << (boffset(b))



/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
FakeDatabaseNucleotideIndex::FakeDatabaseNucleotideIndex (ISequenceDatabase* database, ISeedModel* model)
    : AbstractDatabaseIndex (database, model), _maskOut(0), _maskSize(0)
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

    for (size_t currentCode=0; currentCode<_maskSize; currentCode++)
    {
		/** We setup the mask */
	SETMASK (_maskOut, currentCode);
		SETMASK (_maskOut, revcomp(currentCode, getModel()->getSpan()));
//    	_maskOut[currentCode] = 1;
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
