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

#include <alignment/core/impl/ReaderAlignmentContainer.hpp>

#include <iostream>
#include <vector>
#include <algorithm>

#include <stdio.h>
#define DEBUG(a) //printf a

using namespace std;
using namespace dp;
using namespace os;
using namespace os::impl;
using namespace database;

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
ReaderAlignmentContainer::ReaderAlignmentContainer ()
{
    DEBUG (("ReaderAlignmentContainer::ReaderAlignmentContainer\n"));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ReaderAlignmentContainer::~ReaderAlignmentContainer ()
{
    DEBUG (("ReaderAlignmentContainer::~ReaderAlignmentContainer\n"));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void ReaderAlignmentContainer::setComments ()
{
    /** We need to be protected against concurrent accesses. */
    LocalSynchronizer local (_synchro);

    for (ContainerLevel1::iterator itLevel1 = _containerLevel1.begin(); itLevel1 != _containerLevel1.end();  itLevel1++)
    {
        /** Shortcuts. */
        ISequence*        seqLevel1        = (*itLevel1).second.first;
        ContainerLevel2*  containerLevel2  = (*itLevel1).second.second;

        seqLevel1->comment = _queryComments[seqLevel1->index].c_str();

        for (ContainerLevel2::iterator itLevel2 = containerLevel2->begin(); itLevel2 != containerLevel2->end(); itLevel2++)
        {
            /** Shortcuts. */
            ISequence*  seqLevel2 = (*itLevel2).second.first;

            seqLevel2->comment = _subjectComments[seqLevel2->index].c_str();
        }
    }
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
