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

#ifndef _IALGORITHM_HPP_
#define _IALGORITHM_HPP_

/********************************************************************************/

#include "ICommand.hpp"
#include "Observer.hpp"

#include "IAlgoConfig.hpp"
#include "IAlgoParameters.hpp"
#include "IAlgoIndexator.hpp"

#include "ISeedModel.hpp"

#include "IScoreMatrix.hpp"

#include "IHitIterator.hpp"

#include "ListIterator.hpp"

#include "ReadingFrameSequenceIterator.hpp"

#include "IAlignementResult.hpp"

#include <vector>
#include <string>

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** We define here what a plast algorithm is: a mere command (due to dp::ICommand
 * inheritance) supposed to provided the end user service, which consists in finding
 * alignments between a subject and a query databases.
 *
 * Note the several getter/setters that manage the instances used for providing the
 * service.
 */

class IAlgorithm : public dp::ICommand, public dp::Subject, public dp::IObserver
{
public:

    virtual IConfiguration*                 getConfig           () = 0;
    virtual IParameters*                    getParams           () = 0;
    virtual seed::ISeedModel*               getSeedsModel       () = 0;
    virtual algo::IScoreMatrix*             getScoreMatrix      () = 0;
    virtual statistics::IGlobalParameters*  getGlobalStatistics () = 0;
    virtual statistics::IQueryInformation*  getQueryInfo        () = 0;
    virtual IIndexator*                     getIndexator        () = 0;
    virtual indexation::IHitIterator*       getHitIterator      () = 0;

    virtual void setConfig           (IConfiguration*                 config) = 0;
    virtual void setParams           (IParameters*                    params) = 0;
    virtual void setSeedsModel       (seed::ISeedModel*               model)  = 0;
    virtual void setScoreMatrix      (algo::IScoreMatrix*             matrix) = 0;
    virtual void setGlobalStatistics (statistics::IGlobalParameters*  params) = 0;
    virtual void setQueryInfo        (statistics::IQueryInformation*  info) = 0;
    virtual void setIndexator        (IIndexator*                     indexator) = 0;
    virtual void setHitIterator      (indexation::IHitIterator*       iterator) = 0;

protected:

    /** Provides the list of databases to be used as source databases for the algorithm.
     *  It should be called twice, one for the subject databases configuration, and once
     *  for the query. */
    virtual dp::ListIterator<database::ISequenceDatabase*> createDatabaseIterator (
        IConfiguration*     config,
        const std::string&  uri,
        const Range&        range,
        bool                filtering,
        const std::vector<types::ReadingFrame_e>& frames
    ) = 0;

    /** Define the Hit iterator that will be used for building the alignments. */
    virtual indexation::IHitIterator* createHitIterator (
        IConfiguration* config,
        indexation::IHitIterator* sourceHits,
        algo::IAlignmentResult*   ungapAlignResult,
        algo::IAlignmentResult*   alignResult
    ) = 0;

    /** Define (for subject and query) the list of reading frames to be used. This is the way
     *  for differentiating the algorithm 'plastp', 'tplastn' and 'plastx'. For instance,
     *      'plastp' will return empty lists as result for the both method.
     *      'tplastn' should return a 6 frames list for getSubjectFrames and empty for the other
     *      'plastx'  should return a 6 frames list for getQueryFrames and empty for the other
     */
    virtual const std::vector<types::ReadingFrame_e>&  getSubjectFrames () = 0;
    virtual const std::vector<types::ReadingFrame_e>&  getQueryFrames   () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGORITHM_HPP_ */
