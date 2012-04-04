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

#include <designpattern/impl/WrapperIterator.hpp>

#include <os/impl/DefaultOsFactory.hpp>

#include <misc/api/macros.hpp>

using namespace std;
using namespace os;
using namespace os::impl;

#include <stdio.h>
#define DEBUG(a)  //printf a

/********************************************************************************/
namespace dp { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
WrapperIterator::WrapperIterator (const std::string& text, size_t nbChars)
    : _txt (text), _nbChars(nbChars), _length(0), _currentIndex(0)
{
    _length = _txt.size();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
WrapperIterator::~WrapperIterator ()
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
void WrapperIterator::first()
{
    _currentIndex = 0;
    if (!isDone())  {  _wrap = _txt.substr (_currentIndex, getCharNb() );  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
dp::IteratorStatus WrapperIterator::next()
{
    _currentIndex += _nbChars;
    if (!isDone())  {  _wrap = _txt.substr (_currentIndex, getCharNb() );  }

    return ITER_UNKNOWN;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t WrapperIterator::getCharNb ()
{
    return  MIN (_nbChars,_length-_currentIndex);
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
