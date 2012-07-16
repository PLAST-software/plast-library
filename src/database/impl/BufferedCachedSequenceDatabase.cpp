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

#include <database/impl/BufferedCachedSequenceDatabase.hpp>
#include <designpattern/impl/Property.hpp>
#include <misc/api/macros.hpp>

#include <stdlib.h>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace os;
using namespace os::impl;

/********************************************************************************/
namespace database { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BufferedCachedSequenceDatabase::BufferedCachedSequenceDatabase (ISequenceIterator* refIterator, bool filterLowComplexity)
    : BufferedSequenceDatabase (refIterator, filterLowComplexity), _isBuilt(false)
{
    DEBUG (("BufferedSequenceDatabase::BufferedSequenceDatabase  this=%p  iter=%p\n", this, refIterator));

    /** We force the building of the sequences cache. */
    buildSequencesCache ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BufferedCachedSequenceDatabase::BufferedCachedSequenceDatabase (
    const string& id,
    ISequenceCache* cache,
    size_t firstIdx,
    size_t lastIdx
)
    :  BufferedSequenceDatabase (id, cache, firstIdx, lastIdx), _isBuilt(false)
{
    DEBUG (("BufferedCachedSequenceDatabase::BufferedCachedSequenceDatabase  this=%p  [%ld,%ld] \n", this, _firstIdx, _lastIdx));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BufferedCachedSequenceDatabase::~BufferedCachedSequenceDatabase ()
{
    DEBUG (("BufferedCachedSequenceDatabase::~BufferedCachedSequenceDatabase  this=%p  _sequences.size=%ld\n",
        this,
        _sequences.size()
    ));

    /** We delete the sequences instances. */
    for (vector<ISequence*>::iterator it = _sequences.begin(); it != _sequences.end(); it++)
    {
        delete *it;
    }
    _sequences.clear ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BufferedCachedSequenceDatabase::buildSequencesCache ()
{
    DEBUG (("BufferedCachedSequenceDatabase::buildSequencesCache  this=%p  _isBuilt=%d\n", this, _isBuilt));

    if (_isBuilt == false)
    {
        /** Shortcut. */
        size_t nbSeq = getSequencesNumber();

        for (size_t i=0; i<nbSeq; i++)
        {
            ISequence* newSeq = new ISequence ();

            if (this->getSequenceByIndex (i, *newSeq) == true)
            {
                _sequences.push_back (newSeq);
            }
        }

        /** Note that we should have  _sequences.size() == getSequencesNumber() */

        _isBuilt = true;
    }
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

