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

#include <misc/api/macros.hpp>
#include <seed/impl/BasicSeedModel.hpp>

#include <stdio.h>
#define DEBUG(a)    //printf a
#define VERBOSE(a)

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
    //return new DataSeedIteratorWithTokenizer (this, data);

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
        for (size_t i=0; i <_span; i++)
        {
            VERBOSE (("DataSeedIterator::findNextValidLetter: data[%ld]=%d  \n" ,i, _data.letters[i+_currentIdx]));

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

    } /* end of while (found==false && _currentIdx <= _lastIdx) */

    VERBOSE (("DataSeedIterator::findNextValidLetter: ------------------- _currentIdx=%ld  _lastIdx=%ld  found=%d\n", _currentIdx, _lastIdx, found));

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
BasicSeedModel::DataSeedIteratorWithTokenizer::DataSeedIteratorWithTokenizer (BasicSeedModel* model, const database::IWord& data)
    : DataSeedIterator (model, data), _delta(0), _tokenizer(data), _begin(0), _end(0)
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
bool BasicSeedModel::DataSeedIteratorWithTokenizer::findNextValidItem (void)
{
    bool found = false;

    /** Shortcuts. */
    LETTER* out    = _currentItem.kmer.letters.data;
    LETTER* buffer = _data.letters.data;

    if (_begin <= _end)
    {
        memcpy (out, buffer+_begin, _span);
        _begin++;
        found = true;
    }
    else
    {
        _currentToken++;
        found = updateBound();

        if (found)
        {
            memcpy (out, buffer+_begin, _span);
            _begin++;
            found = true;
        }
        else
        {
            /** We should be done. */
            _currentIdx = _lastIdx + 1;
            found = false;
        }
    }

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

    /** We fill the indexes vector. */
    _seedsIdx.resize (_totalNumber);
    for (size_t i=0; i<_totalNumber; i++)  {  _seedsIdx[i] = i; }

    DEBUG (("BasicSeedModel::AllSeedsIterator::AllSeedsIterator 1: [%ld,%ld] => [%ld,%ld]\n", firstIdx, lastIdx, _firstIdx, _lastIdx));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicSeedModel::AllSeedsIterator::AllSeedsIterator (BasicSeedModel* model, const std::vector<size_t>& seedsIdx)
    : AbstractSeedIterator (model, 0, 0, false),
      _specificModel(model), _totalNumber(0)
{
    if (seedsIdx.empty() == false)
    {
        _firstIdx = 0;
        _lastIdx  = seedsIdx.size()-1;
    }
    else
    {
        /** We have nothing to iterate, just be sure that the first is greater than the last. */
        _firstIdx = 1;
        _lastIdx  = 0;
    }

    /** We compute the total number of seed. */
    _totalNumber = seedsIdx.size();

    /** We copy the vector of seeds indexes. */
    _seedsIdx = seedsIdx;

    DEBUG (("BasicSeedModel::AllSeedsIterator::AllSeedsIterator 2: seedsIdx.size()=%ld\n", seedsIdx.size() ));
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
ISeedIterator* BasicSeedModel::AllSeedsIterator::createFilteredIterator (const std::vector<size_t>& seedsIdx)
{
    DEBUG (("BasicSeedModel::AllSeedsIterator::createFilteredIterator   seedsIdx.size()=%ld\n", seedsIdx.size() ));

    /** We just clone, we don't filter anything. TO BE IMPROVED... */
    return new AllSeedsIterator (_specificModel, seedsIdx);
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

    /** We retrieve the actual index of the seed. */
    size_t actualIdx = _seedsIdx [_currentIdx - _firstIdx];

    /** We convert the letters of the current data according to the model. */
    for (size_t i=0; i<_span; i++)
    {
        out[i] =  actualIdx % _alphabetSize;
        actualIdx   /= _alphabetSize;
    }

    _currentItem.code   = actualIdx;
    _currentItem.offset = 0;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
