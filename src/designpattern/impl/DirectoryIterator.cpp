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

#include <designpattern/impl/DirectoryIterator.hpp>

#include <misc/api/macros.hpp>
#include <string.h>

using namespace std;

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
DirectoryIterator::DirectoryIterator (const char* dirname, const char* match)
    : _dirname (dirname ? dirname : "."), _match (match ? match : ""),
      _dp(0), _dirp(0)
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
DirectoryIterator::~DirectoryIterator ()
{
    if (_dp)  { closedir(_dp); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DirectoryIterator::first ()
{
    _dp  = opendir (_dirname.c_str());

    next ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
dp::IteratorStatus DirectoryIterator::next()
{
    if (_dp)
    {
        while ( (_dirp = readdir(_dp)) != 0)
        {
            if (_match.empty())  { break; }

            else if (strstr (_dirp->d_name, _match.c_str()) != 0)
            {
                break;
            }
        }
    }

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
bool DirectoryIterator::isDone()
{
    return (_dp != 0)  &&  (_dirp == 0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
const char* DirectoryIterator::currentItem()
{
    return (_dirp != 0 ? _dirp->d_name : 0);
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
