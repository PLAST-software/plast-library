/*
 * SeedIndex.cpp
 *
 *  Created on: Jun 20, 2011
 *      Author: edrezen
 */

#include "AbstractSequenceIterator.hpp"

#include <stdio.h>
#define DEBUG(a)  //printf a

/********************************************************************************/
namespace database {
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
} /* end of namespaces. */
/********************************************************************************/

