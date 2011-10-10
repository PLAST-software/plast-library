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

#include "SmallGapHitIteratorNull.hpp"

using namespace std;
using namespace os;
using namespace database;
using namespace seed;
using namespace indexation;

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SmallGapHitIteratorNull::SmallGapHitIteratorNull (
    indexation::IHitIterator*   realIterator,
    seed::ISeedModel*           model,
    algo::IScoreMatrix*         scoreMatrix,
    algo::IParameters*          parameters,
    algo::IAlignmentResult*     ungapResult
)
    : AbstractScoredHitIterator (realIterator, model, scoreMatrix, parameters, ungapResult)
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
SmallGapHitIteratorNull::~SmallGapHitIteratorNull ()
{
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
