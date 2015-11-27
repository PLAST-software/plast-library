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
BufferedCachedSequenceDatabase::BufferedCachedSequenceDatabase (ISequenceIterator* refIterator, int filterLowComplexity)
    : BufferedSequenceDatabase (refIterator, filterLowComplexity), _isBuilt(false)
{
    DEBUG (("BufferedCachedSequenceDatabase::BufferedCachedSequenceDatabase  this=%p  iter=%p\n", this, refIterator));

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
BufferedCachedSequenceDatabase::~BufferedCachedSequenceDatabase ()
{
    DEBUG (("BufferedCachedSequenceDatabase::~BufferedCachedSequenceDatabase  this=%p  _sequences.size=%ld\n",
        this,
        _sequences.size()
    ));

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
    DEBUG (("BufferedCachedSequenceDatabase::BufferedCachedSequenceDatabase  this=%p  _isBuilt=%d\n", this, _isBuilt));

    if (_isBuilt == false)
    {
        /** Shortcut. */
        size_t nbSeq = getSequencesNumber();

        _sequences.resize (nbSeq);

        for (size_t i=0; i<nbSeq; i++)  {  this->getSequenceByIndex (i, _sequences[i]);  }

        /** Note that we should have  _sequences.size() == getSequencesNumber() */

        _isBuilt = true;
    }
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

