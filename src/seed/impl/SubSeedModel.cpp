/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

#include <seed/impl/SubSeedModel.hpp>
#include <os/impl/DefaultOsFactory.hpp>
#include <designpattern/impl/FileLineIterator.hpp>

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace os;
using namespace os::impl;
using namespace dp;
using namespace dp::impl;
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
SubSeedModel::SubSeedModel ()
    : AbstractSeedModel(0), _allSeedsTable(0), _seedsMaxNumber(1), _currentNumber(0)
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
SubSeedModel::SubSeedModel (const char* filename)
    : AbstractSeedModel(0), _allSeedsTable(0), _seedsMaxNumber(1), _currentNumber(0)
{
    /** We need to read the file. */
    FileLineIterator it (filename);

    /** The first line should be the span. */
    it.first ();
    _span = misc::atoi (it.currentItem());
    it.next ();

    /** We can now build some attributes. */
    initialize (_span);

    /** We read the entries. */
    for (; !it.isDone() &&  _currentNumber<_span; it.next())   {  addEntry (it.currentItem());  }

    DEBUG (("SubSeedModel::SubSeedModel  span=%ld  maxSeeds=%ld \n", _span, _seedsMaxNumber));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SubSeedModel::SubSeedModel (size_t nbEntries, ...)
    : AbstractSeedModel(nbEntries), _allSeedsTable(0), _seedsMaxNumber(1), _currentNumber(0)
{
    /** We build some attributes. */
    initialize (_span);

    /** We parse the ellipsis. */
    va_list varg;
    va_start (varg, nbEntries);
    do {
        const char* entry = va_arg(varg, const char*);
        addEntry (entry);
        nbEntries--;
    } while (nbEntries!=0);
    va_end(varg);

    DEBUG (("SubSeedModel::SubSeedModel  span=%ld  maxSeeds=%ld \n", _span, _seedsMaxNumber));

    //dump ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SubSeedModel::SubSeedModel (const std::vector<std::string>& subseedStrings)
    : AbstractSeedModel(subseedStrings.size()), _allSeedsTable(0), _seedsMaxNumber(1), _currentNumber(0)
{
    /** We build some attributes. */
    initialize (subseedStrings.size());

    for (size_t i=0; i<subseedStrings.size(); i++)
    {
        addEntry (subseedStrings[i].c_str());
    }

    DEBUG (("SubSeedModel::SubSeedModel  span=%ld  maxSeeds=%ld \n", _span, _seedsMaxNumber));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SubSeedModel::initialize (size_t span)
{
    /** We get a reference on the alphabet given the encoding scheme. */
    _alphabet = EncodingManager::singleton().getAlphabet (SUBSEED);

    /** We allocate the equivalence table. */
    _equivalenceTable = (char **) DefaultFactory::memory().malloc (sizeof(char *)*_span);
    for (size_t i=0; i<_span; i++)  {  _equivalenceTable[i] = (char *) DefaultFactory::memory().malloc (sizeof(char)*_alphabet->size);  }

    _equivalentsTable.resize (_span);

    /** We set the conversion table; input encoding is ASCII. */
    _convertTable = EncodingManager::singleton().getEncodingConversion (ASCII, SUBSEED);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SubSeedModel::~SubSeedModel ()
{
    /** We deallocate the equivalence table. */
    for (size_t i=0; i<_span; i++)
    {
        DefaultFactory::memory().free (_equivalenceTable[i]);
    }

    DefaultFactory::memory().free (_equivalenceTable);

    DefaultFactory::memory().free (_allSeedsTable);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SubSeedModel::addEntry (const char* sequence)
{
    size_t idx=0;
    size_t nbFoundLetters=0;

    DEBUG (("SubSeedModel::addEntry  current=%ld  seq='%s'\n", _currentNumber, sequence));

    while (sequence[idx] != 0)
    {
        /** We get the current representant of the sequence. */
        LETTER equivalent = _convertTable [(int)sequence [idx++]];

        if (_alphabet->isValid (equivalent))
        {
            //DEBUG (("       current_representant=%d   '%c' \n", equivalent, sequence[idx-1]));

            nbFoundLetters++;

            _equivalenceTable [_currentNumber][(int)equivalent] = equivalent;

            /** We memorize the current representant for the current letter position. */
            _equivalentsTable[_currentNumber].push_back (equivalent);

            while (sequence[idx] != ','  &&  sequence[idx] != 0)
            {
                LETTER current_letter = _convertTable [(int)sequence [idx++]];

                if (_alphabet->isValid(current_letter))
                {
                    _equivalenceTable [_currentNumber][(int)current_letter] = equivalent;
                }
            }

            if (sequence[idx] == ',')  {  idx++; }
        }
    }

    DEBUG (("_equivalentsTable[%ld].len=%ld\n", _currentNumber, _equivalentsTable[_currentNumber].size()));

    /** We update the total distinct seeds. */
    _seedsMaxNumber *= _equivalentsTable[_currentNumber].size();

    /** We increase the number of letters. */
    _currentNumber++;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SubSeedModel::dump (void)
{
    const LETTER* convert = EncodingManager::singleton().getEncodingConversion (SUBSEED, ASCII);

    for (size_t i=0; i<_span; i++)
    {
        printf ("[%ld/%ld]  ", i, _span);

        for (size_t j=0; j<_alphabet->size; j++)
        {
            //printf ("[%c->%c] ", convert[j], convert[_equivalenceTable[i][j]] );
            printf ("[%ld->%d] ", j, _equivalenceTable[i][j] );
        }
        printf ("\n");
    }

    for (size_t i=0; i<_span; i++)
    {
        vector<LETTER>& currentContainer = _equivalentsTable[i];
        for (size_t j=0; j<currentContainer.size(); j++)
//        for (list<LETTER>::iterator it = currentList.begin(); it != currentList.end(); it++)
        {
            LETTER l =  currentContainer[j];
            printf ("[%c] ", convert[(int)l]);
        }
        printf ("\n");
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
const database::LETTER* SubSeedModel::getAllSeedsTable ()
{
    if (_allSeedsTable == 0)
    {
        _allSeedsTable = (LETTER*) DefaultFactory::memory().calloc (_span*(_seedsMaxNumber+1), sizeof(LETTER));
        size_t idx = 0;

        size_t* increments = (size_t*) DefaultFactory::memory().calloc (_span, sizeof(size_t));

        for (size_t i=0; i<_span; i++)  {  increments[i] = 0;  }

        /** We loop over all possible seeds. */
        for (size_t i=0; i<_seedsMaxNumber; i++)
        {
            /** We configure the current kmer. */
            for (size_t j=0; j<_span; j++)
            {
                _allSeedsTable [idx++] = _equivalentsTable [j][increments[j]];
            }

            /** We update the increments indexes. */
            for (int k=_span-1; k>=0; k--)
            {
                increments[k]++;
                if (increments[k] >= _equivalentsTable [k].size())
                {
                    increments[k] = 0;
                }
                else
                {
                    break;
                }
            }
        }

        /** Some clean up. */
        DefaultFactory::memory().free (increments);
    }

    /** We return the result. */
    return _allSeedsTable;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISeedIterator* SubSeedModel::createSeedsIterator (const database::IWord& data)
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
ISeedIterator* SubSeedModel::createAllSeedsIterator ()
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
SubSeedModel::DataSeedIterator::DataSeedIterator (SubSeedModel* model, const database::IWord& data)
    : AbstractSeedIterator (model,0,0, true), _specificModel(model)
{
    /** A little optimization (for avoiding too many method calls). */
    _equivalenceTable = _specificModel->_equivalenceTable;

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
bool SubSeedModel::DataSeedIterator::findNextValidItem (void)
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

        /** We look whether there is a bad letter in the current seed.
         *  We also translate the output kmer in the same loop.
         */
        for (size_t i=0;  i <_span; i++)
        {
            DEBUG (("DataSeedIterator::findNextValidLetter: data[%ld]=%d  \n" ,i, _data.letters.data[i+_currentIdx]));

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
                out[i] = _equivalenceTable [i] [(int)l];
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

    /** We may have to update the "currentItem". */
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
SubSeedModel::DataSeedIteratorWithTokenizer::DataSeedIteratorWithTokenizer (SubSeedModel* model, const database::IWord& data)
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
bool SubSeedModel::DataSeedIteratorWithTokenizer::findNextValidItem (void)
{
    bool found = false;

    /** Shortcuts. */
    LETTER* out    = _currentItem.kmer.letters.data;
    LETTER* buffer = _data.letters.data;

    if (_begin <= _end)
    {
        for (size_t i=0; i<_span; i++)
        {
            int l = buffer[i+_currentIdx];
            out[i] = _equivalenceTable [i] [l];
        }
        _begin++;
        found = true;
    }
    else
    {
        _currentToken++;
        found = updateBound();

        if (found)
        {
            for (size_t i=0; i<_span; i++)
            {
                int l = buffer[i+_currentIdx];
                out[i] = _equivalenceTable [i] [l];
            }
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
SubSeedModel::AllSeedsIterator::AllSeedsIterator (SubSeedModel* model, size_t firstIdx, size_t lastIdx)
    : AbstractSeedIterator (model, firstIdx, lastIdx, false),
      _specificModel(model),
      _allSeedsTable(0),
      _totalNumber(0)
{
    _allSeedsTable = _model->getAllSeedsTable();

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

    /** We compute the total number of seed. */
    _totalNumber = _lastIdx - _firstIdx + 1;

    /** We fill the indexes vector. */
    _seedsIdx.resize (_totalNumber);
    for (size_t i=0; i<_totalNumber; i++)  {  _seedsIdx[i] = i; }

    DEBUG (("AllSeedsIterator::AllSeedsIterator [1] first=%ld  last=%ld  total=%ld \n", _firstIdx, _lastIdx, _totalNumber));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SubSeedModel::AllSeedsIterator::AllSeedsIterator (SubSeedModel* model, const std::vector<size_t>& seedsIdx)
: AbstractSeedIterator (model, 0, 0, false),
  _specificModel(model),
  _allSeedsTable(0),
  _totalNumber(0)
{
    /** We create the table holding all possible seeds. */
    _allSeedsTable = _model->getAllSeedsTable();

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

    DEBUG (("AllSeedsIterator::AllSeedsIterator [2] first=%ld  last=%ld  total=%ld \n", _firstIdx, _lastIdx, _totalNumber));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SubSeedModel::AllSeedsIterator::~AllSeedsIterator ()
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
ISeedIterator* SubSeedModel::AllSeedsIterator::extract (size_t firstIdx, size_t lastIdx)
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
ISeedIterator* SubSeedModel::AllSeedsIterator::createFilteredIterator (const std::vector<size_t>& seedsIdx)
{
    return  new AllSeedsIterator (_specificModel, seedsIdx);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SubSeedModel::AllSeedsIterator::updateItem ()
{
    /** We retrieve the actual index of the seed. */
    size_t actualIdx = _seedsIdx [_currentIdx - _firstIdx];

    /** We set the kmer. */
    memcpy (_currentItem.kmer.letters.data, &(_allSeedsTable[_span*actualIdx]), _span);

    /** We set the index. */
    _currentItem.offset  = _span*actualIdx;

    /** We set the hash code. */
    _currentItem.code = _specificModel->getHashCode (_currentItem.kmer);
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
