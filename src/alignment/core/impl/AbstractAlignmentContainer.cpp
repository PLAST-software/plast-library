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

#include <alignment/core/impl/AbstractAlignmentContainer.hpp>

#include <designpattern/impl/Property.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace os;
using namespace os::impl;
using namespace dp;
using namespace dp::impl;

/********************************************************************************/
namespace alignment {
namespace core      {
namespace impl      {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractAlignmentContainer::AbstractAlignmentContainer ()
    : _synchro(0), _nbAlignments(0)
{
    _synchro = DefaultFactory::thread().newSynchronizer();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractAlignmentContainer::~AbstractAlignmentContainer ()
{
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
IProperties* AbstractAlignmentContainer::getProperties (const std::string& root)
{
    IProperties* props = new Properties();

    props->add (0, root,   "%ld", getSize());
    props->add (1, "size", "%ld", getSize());

    return props;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
