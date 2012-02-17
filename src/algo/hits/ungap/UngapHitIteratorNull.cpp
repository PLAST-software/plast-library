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
