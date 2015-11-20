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

#include <index/api/ISeedMaskGenerator.hpp>

#include <index/impl/FakeDatabaseNucleotideIndex.hpp>
#include <index/impl/SeedMaskGenerator.hpp>

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

KmersCountToSeedMaskGenerator FakeDatabaseNucleotideIndex::_seedMaskGenerators;

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
    std::string subjectUri,
    std::string queryUri,
    long kmersPerSequence)
    : AbstractDatabaseIndex (database, model),
    _maskOut(0),
    _maskSize(0),
    _subjectUri(subjectUri),
    _queryUri(queryUri),
    _kmersPerSequence(kmersPerSequence),
    _synchro(0)
{
    // compute the mask size
    size_t alphabetSize = getModel()->getAlphabet()->size;
    size_t maxSeedsNumber = pow((double)alphabetSize,(double)getModel()->getSpan());
    _maskSize = maxSeedsNumber / WORD_SIZE;
    _maskOut = new word_t [_maskSize];
    // init the mask with 0 for all entries
    memset (_maskOut, 0, sizeof(word_t)*_maskSize);

    _synchro = os::impl::DefaultFactory::thread().newSynchronizer();
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
    delete _synchro;
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
    ISeedMaskGenerator* maskGenerator = getSeedMaskGenerator(_kmersPerSequence);
    LOCAL(maskGenerator);

    size_t generatedMaskSize = maskGenerator->getBitsetSize();

    if (generatedMaskSize != _maskSize * sizeof(word_t)) {
        std::cerr << "Expected mask size was "
            << _maskSize * sizeof(word_t)
            << " but was "
            << generatedMaskSize
            << std::endl;
        throw "Internal error. FakeDatabaseNucleotideIndex::build";
    }

    memcpy(_maskOut, maskGenerator->getBitset(), generatedMaskSize);
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
    //return NULL;
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

ISeedMaskGenerator* FakeDatabaseNucleotideIndex::getSeedMaskGenerator(u_int64_t kmersPerSequence)
{
    if (!_seedMaskGenerators.contains(kmersPerSequence)) {
        os::LocalSynchronizer lock(_synchro);
        ISeedMaskGenerator* seedMaskGenerator = new SeedMaskGenerator(getModel()->getSpan(), _queryUri, _subjectUri, kmersPerSequence);
        // This object should live at least as much as the algorithm environment.
        // To ensure this, currently it lives for the duration of the program.
        // It is freed when _seedMaskGenerators is destroyed.
        seedMaskGenerator->use();
        _seedMaskGenerators[kmersPerSequence] = seedMaskGenerator;
    }

    return _seedMaskGenerators[kmersPerSequence];
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/




