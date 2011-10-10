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


#ifndef _ALGO_UNGAP_ALIGNMENT_HPP_
#define _ALGO_UNGAP_ALIGNMENT_HPP_

/********************************************************************************/

#include "IAlgoUngapAlignment.hpp"
#include "ISeedModel.hpp"
#include "IScoreMatrix.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** Define an interface of what ungap alignment algorithm is: something that provides
 *  a way for iterating Hit instances.
 *  The idea is to get as input some HitIterator that is transformed into another one,
 *  possibly after filtering out some hits.
 */
class AlgoUngapAlignment : public IAlgoUngapAlignment
{
public:

    /** */
    AlgoUngapAlignment ();

    virtual ~AlgoUngapAlignment ();

    /** Creates a hits iterator that loops over ungap alignement hits. */
    indexation::IHitIterator* createHitIterator (
        indexation::IHitIterator*   inputIterator,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         scoreMatrix,
        algo::IParameters*      parameters
    );
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALGO_UNGAP_ALIGNMENT_HPP_ */
