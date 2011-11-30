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

#include <seed/impl/AbstractSeedIterator.hpp>
#include <os/impl/DefaultOsFactory.hpp>

#include <stdlib.h>

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
AbstractSeedIterator::AbstractSeedIterator (ISeedModel* model, size_t firstIdx, size_t lastIdx, bool hasNextValidMethod)
    : _model        (model),
      _hasNextValidMethod (hasNextValidMethod),
      _span         (_model->getSpan()),
      _alphabetSize (_model->getAlphabet()->size),
      _currentItem  (_model->getSpan()),
      _firstIdx(firstIdx),
      _lastIdx(lastIdx),
      _currentIdx(0),
      _synchro (0),
      _nbRetrieved (0)
{
    /** We use the model. */
    _model->use ();

    /** We create a synchronizer. */
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
AbstractSeedIterator::~AbstractSeedIterator ()
{
    /** We unset the model. */
    _model->forget();

    if (_synchro)  { delete _synchro; }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AbstractSeedIterator::setData (const database::IWord& data)
{
    _data = data;

    /** We check that we have something to iterate. If not, we make sure that the starting index is
     *  greater than the last one. */
    if (_data.letters.size >= _span)
    {
        _firstIdx = 0;
        _lastIdx  = _data.letters.size - _span;
    }
    else
    {
        /** We have data size not big enough for getting at least one word for the given seed model. */
        _firstIdx = 1;
        _lastIdx  = 0;
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
void AbstractSeedIterator::iterate (void* aClient, Method method)
{
    IteratorClient* client = (IteratorClient*) aClient;   // not very nice...

    DEBUG (("AbstractSeedIterator::iterate BEGIN\n"));

    /** We have two ways for iterating, according to a non null 'findNextValidItem' method.
     *  This may avoid the cost of a method call to 'findNextValidItem' when not needed. */

    if (_hasNextValidMethod == false)
    {
        for (_currentIdx = _firstIdx; _currentIdx <= _lastIdx; _currentIdx++)
        {
            /** We update the item to be provided to the client. */
            updateItem();

            /** We call the callback. */
            (client->*method) (&_currentItem);
        }
    }
    else
    {
        _currentIdx = _firstIdx;
        if (findNextValidItem())  { updateItem(); }

        while (_currentIdx <= _lastIdx)
        {
            /** We call the callback. */
            (client->*method) (&_currentItem);

            _currentIdx++;
            if (findNextValidItem())  { updateItem(); }
        }
    }

    DEBUG (("AbstractSeedIterator::iterate END\n"));
}


/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
