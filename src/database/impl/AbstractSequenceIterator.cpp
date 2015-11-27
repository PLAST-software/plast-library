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

#include <database/impl/AbstractSequenceIterator.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

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
AbstractSequenceIterator::AbstractSequenceIterator ()
    : _sequenceBuilder(0), _encoding (SUBSEED)
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
void AbstractSequenceIterator::setBuilder (ISequenceBuilder* builder)
{
    if (_sequenceBuilder)  { _sequenceBuilder->forget ();  }
    _sequenceBuilder = builder;
    if (_sequenceBuilder)  { _sequenceBuilder->use ();  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
//std::vector<ISequenceIterator*> AbstractSequenceIterator::split (size_t nbIterators)
//{
//    std::vector<ISequenceIterator*> res;
//
//    /** By default, just add ourself into the container. */
//    res.push_back (this);
//
//    /** Note that we have to use the instance because it is used by the container. */
//    this->use ();
//
//    /** We return the resulting container. */
//    return res;
//}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

