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


#ifndef _IALGO_INITIALIZER_HPP_
#define _IALGO_INITIALIZER_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"
#include "IHitIterator.hpp"
#include "IDatabaseIndex.hpp"
#include "IStatistics.hpp"

#include "IAlgoParameters.hpp"
#include "IScoreMatrix.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class IAlgoConfigurator : public dp::SmartPointer, public dp::Subject
{
public:

    virtual void setSubjectDatabase  (database::ISequenceDatabase* db) = 0;
    virtual void setQueryDatabase    (database::ISequenceDatabase* db) = 0;

    virtual IParameters*  getParameters  () = 0;
    virtual seed::ISeedModel* getSeedModel   () = 0;
    virtual IScoreMatrix*     getScoreMatrix () = 0;

    virtual database::ISequenceDatabase* getSubjectDatabase () = 0;
    virtual database::ISequenceDatabase* getQueryDatabase   () = 0;

    virtual indexation::IDatabaseIndex* getIndexDatabase () = 0;
    virtual indexation::IDatabaseIndex* getIndexQuery    () = 0;

    virtual statistics::IQueryInformation* getQueryInformation () = 0;

    virtual indexation::IHitIterator* createHitIterator () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALGO_INITIALIZER_HPP_ */
