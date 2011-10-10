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

#ifndef _IALGO_UNGAP_ALIGNMENT_HPP_
#define _IALGO_UNGAP_ALIGNMENT_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"
#include "IHitIterator.hpp"
#include "ISeedModel.hpp"
#include "IScoreMatrix.hpp"
#include "IAlgoParameters.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** Define an interface of what ungap alignment algorithm is: something that provides
 *  a way for iterating Hit instances.
 *  The idea is to get as input some HitIterator that is transformed into another one,
 *  possibly after filtering out some hits.
 */
class IAlgoUngapAlignment : public dp::SmartPointer
{
public:

    /** Creates a hits iterator that loops over ungap alignement hits. */
    virtual indexation::IHitIterator* createHitIterator (
        indexation::IHitIterator*   inputIterator,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         scoreMatrix,
        algo::IParameters*      parameters
    ) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_UNGAP_ALIGNMENT_HPP_ */
