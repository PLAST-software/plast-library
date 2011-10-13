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

#include "TokenizerIterator.hpp"
#include "MemoryAllocator.hpp"

using namespace std;

#include <stdio.h>
#define DEBUG(a)  //printf a

/********************************************************************************/
namespace dp {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
TokenizerIterator::TokenizerIterator (const char* text, const char* seperator)
    : _sep(seperator), _str(0), _loop(0)
{
    DEBUG (("TokenizerIterator::TokenizerIterator: text='%s'  sep='%s'\n", text, seperator));

    if (text != 0)  { _str = os::MemoryAllocator::singleton().strdup (text); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
TokenizerIterator::~TokenizerIterator ()
{
    os::MemoryAllocator::singleton().free (_str);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void TokenizerIterator::first()
{
    DEBUG (("TokenizerIterator::first\n"));

    if (_str)  {  _loop = strtok (_str, _sep.c_str());  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
dp::IteratorStatus TokenizerIterator::next()
{
    _loop = strtok (NULL, _sep.c_str());
    return ITER_UNKNOWN;
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
