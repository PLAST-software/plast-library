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

#include <algo/hits/ungap/UngapHitIteratorNull.hpp>

using namespace seed;
using namespace algo::core;
using namespace alignment::core;

/********************************************************************************/
namespace algo     {
namespace hits     {
namespace ungapped {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
UngapHitIteratorNull::UngapHitIteratorNull (
    IHitIterator*         realIterator,
    ISeedModel*           model,
    IScoreMatrix*         scoreMatrix,
    IParameters*          parameters,
    IAlignmentContainer*  ungapResult
)
    : AbstractPipeHitIterator (realIterator, model, scoreMatrix, parameters, ungapResult)
{
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
