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

#include <seed/impl/BasicSeedModel.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace database;

/********************************************************************************/
namespace seed { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicSeedModel::BasicSeedModel (Encoding encoding, size_t span)
    : AbstractSeedModel(span),_seedsMaxNumber(1)
{
    /** We compute the maximum number of seeds. */
    _seedsMaxNumber = 1;
    for (size_t i=1; i<=_span; i++)   {  _seedsMaxNumber *= _alphabet->size; }

    DEBUG (("BasicSeedModel::BasicSeedModel:  span=%d  alphabetSize=%d  maxNb=%ld\n", _span, _alphabet->size, _seedsMaxNumber));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicSeedModel::~BasicSeedModel ()
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISeedIterator* BasicSeedModel::createSeedsIterator (const database::IWord& data)
{
    return new DataSeedIterator (this, data);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISeedIterator* BasicSeedModel::createAllSeedsIterator ()
{
    return new AllSeedsIterator (this);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool BasicSeedModel::getSeedByString (const std::string& seedAscii, ISeed& seed)
{
    bool result = false;

    ISeedIterator* it = createAllSeedsIterator ();
    LOCAL(it);

    for (it->first(); result==false && !it->isDone(); it->next())
    {
        const ISeed* current = it->currentItem();

        if (current->kmer.toString().compare (seedAscii) == 0)
        {
            seed = *current;
            result = true;
        }
    }

    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicSeedModel::DataSeedIterator::DataSeedIterator (BasicSeedModel* model, const database::IWord& data)
    : AbstractSeedIterator (model,0,0,true), _specificModel(model)
{
    setData (data);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool BasicSeedModel::DataSeedIterator::findNextValidItem (void)
{
    bool found = false;

    /** Shortcuts. */
    LETTER* out    = _currentItem.kmer.letters.data;
    LETTER* buffer = _data.letters.data;

    /** We check that we have enough letters for building a correct word. */
    while (found==false && _currentIdx <= _lastIdx)
    {
        size_t badLetterIdx = 0;
        bool   isBadLetter  = false;

        /** We look whether there is a bad letter in the current seed. */
        for (size_t i=0;  i <_span; i++)
        {
            DEBUG (("DataSeedIterator::findNextValidLetter: data[%ld]=%d  \n" ,i, _data.letters[i+_currentIdx]));

            /** We retrieve the current letter. */
            LETTER l = buffer[i+_currentIdx];

            /** We check that we have no bad letter. */
            if (l >= (char)_alphabetSize)
            {
                isBadLetter  = true;
                badLetterIdx = i +_currentIdx;
            }
            else
            {
                /** We translate the output word. */
                out[i] = l;
            }
        }

        if (isBadLetter)
        {
            _currentIdx = badLetterIdx + 1;
        }
        else
        {
            found = true;
        }
    }

    DEBUG (("DataSeedIterator::findNextValidLetter: ------------------- _currentIdx=%ld  _lastIdx=%ld  found=%d\n", _currentIdx, _lastIdx, found));

    if (found)
    {
        /** We set the hash code. The output word is supposed to have been translated above. */
        _currentItem.code  = _specificModel->getHashCode (_currentItem.kmer);

        /** We set the index. */
        _currentItem.offset  = _currentIdx;
    }

    return found;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicSeedModel::AllSeedsIterator::AllSeedsIterator (BasicSeedModel* model, size_t firstIdx, size_t lastIdx)
    : AbstractSeedIterator (model, firstIdx, lastIdx, false), _specificModel(model), _totalNumber(0)
{
    /** By convention, if we have a first index greater than the last index,
     *  it means that we want to have the whole set of iterated items. */
    if (firstIdx > lastIdx)
    {
        _firstIdx = 0;
        _lastIdx  = _model->getSeedsMaxNumber() - 1;
    }
    else
    {
        _firstIdx = firstIdx;
        _lastIdx  = lastIdx;
    }

    _totalNumber = _lastIdx - _firstIdx + 1;

    DEBUG (("BasicSeedModel::AllSeedsIterator::AllSeedsIterator: [%ld,%ld] => [%ld,%ld]\n", firstIdx, lastIdx, _firstIdx, _lastIdx));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicSeedModel::AllSeedsIterator::~AllSeedsIterator ()
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISeedIterator* BasicSeedModel::AllSeedsIterator::extract (size_t firstIdx, size_t lastIdx)
{
    return new AllSeedsIterator (_specificModel, firstIdx, lastIdx);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BasicSeedModel::AllSeedsIterator::updateItem (void)
{
    /** A little shortcut. */
    LETTER* out = _currentItem.kmer.letters.data;

    size_t idx =_currentIdx;

    /** We convert the letters of the current data according to the model. */
    for (size_t i=0; i<_span; i++)
    {
        out[i] =  idx % _alphabetSize;
        idx   /= _alphabetSize;
    }

    _currentItem.code   = _currentIdx;
    _currentItem.offset = 0;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
