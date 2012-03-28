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

/** \file AbstractAlgorithm.hpp
 *  \brief Abstract implementation of the IAlgorithm interface
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ABSTRACT_ALGORITHM_HPP_
#define _ABSTRACT_ALGORITHM_HPP_

/********************************************************************************/

#include <os/api/ITime.hpp>
#include <os/impl/TimeTools.hpp>

#include <designpattern/impl/Observer.hpp>

#include <database/api/ISequenceDatabase.hpp>
#include <database/api/IDatabaseQuickReader.hpp>

#include <seed/api/ISeedModel.hpp>

#include <algo/core/api/IAlgorithm.hpp>
#include <algo/core/api/IAlgoConfig.hpp>
#include <algo/core/api/IAlgoParameters.hpp>
#include <algo/core/api/IDatabasesProvider.hpp>

#include <alignment/core/api/IAlignmentContainer.hpp>
#include <alignment/core/api/IAlignmentContainerVisitor.hpp>
#include <alignment/filter/api/IAlignmentFilter.hpp>

#include <vector>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

/** \brief Default implementation of IAlgorithm
 *
 * This implementation of the IAlgorithm interface provides a execute() method
 * holding a full skeleton for the PLAST algorithm.
 *
 * This execute() method does the following:
 *      - reading the subject and query databases (ie. creation of ISequenceDatabase instances)
 *      - creation of statistical information for the query database (ie. creation of IQueryInformation instance)
 *      - indexation of the subject and query databases (ie. creation of IIndexator instance)
 *      - creation of ungap and gap alignments list that will be filled throughout the algorithm execution
 *      - creation of the global hits iterator
 *      - split of this global hits iterator in smaller ones (which provides our parallelization scheme)
 *      - iteration of the created hits iterator through ICommand instances (HitIterationCommand)
 *
 * The execute() method (from dp::ICommand) is a Template method, since it uses
 * several primitives for creating databases and iterators for instance.
 *
 * Ideally, the 'execute' method should be generic enough for being not changed. All
 * the variant parts should be:
 *      - the different instances used for providing the service (see getters/setters)
 *      - the primitive of the Template Method.
 *
 * In particular, this execute() method should work for 'plastp', 'tplastn', 'plastx' and
 * maybe some other in the future.
 *
 * Note that a resultVisitor instance is provided to the constructor.
 * This visitor will be visited at the end of the algorithm by the gap alignments list. For instance,
 * one can provide a file output visitor, and so we will get as a PLAST result a file holding all
 * the found alignments between the subject and the query databases.
 */
class AbstractAlgorithm : public IAlgorithm
{
public:

    /** Constructor.
     * \param[in] config : factory used for creating needed instances.
     * \param[in] reader : reader holding information about the subject database (needed for computing query cutoffs)
     * \param[in] params : used for parametrization
     * \param[in] resultVisitor : the visitor used for visiting the resulting gap alignments list.
     */
    AbstractAlgorithm (
        IConfiguration*                               config,
        database::IDatabaseQuickReader*               reader,
        IParameters*                                  params,
        alignment::filter::IAlignmentFilter*          filter,
        alignment::core::IAlignmentContainerVisitor*  resultVisitor,
        algo::core::IDatabasesProvider*               dbProvider,
        bool&                                         isRunning
    );

    /** Destructor. */
    virtual ~AbstractAlgorithm ();

    /** Implementation of the ICommand interface that provides the actual steps of the PLAST algorithm
     * (indexation, hits iteration...)
     */
    void execute (void);

    /** \copydoc IAlgorithm::getConfig */
    IConfiguration*                         getConfig           ()  { return _config;           }

    /** \copydoc IAlgorithm::getReader */
    database::IDatabaseQuickReader*         getReader           ()  { return _reader;           }

    /** \copydoc IAlgorithm::getParams */
    IParameters*                            getParams           ()  { return _params;           }

    /** \copydoc IAlgorithm::getFilter */
    alignment::filter::IAlignmentFilter*    getFilter           ()  { return _filter;           }

    /** \copydoc IAlgorithm::getResultVisitor */
    alignment::core::IAlignmentContainerVisitor* getResultVisitor    ()  { return _resultVisitor;    }

    /** \copydoc IAlgorithm::getDatabasesProvider */
    algo::core::IDatabasesProvider*         getDatabasesProvider ()  { return _dbProvider; }

    /** \copydoc IAlgorithm::getSeedsModel */
    seed::ISeedModel*                       getSeedsModel       ()  { return _seedsModel;       }

    /** \copydoc IAlgorithm::getScoreMatrix */
    IScoreMatrix*                           getScoreMatrix      ()  { return _scoreMatrix;      }

    /** \copydoc IAlgorithm::getGlobalStatistics */
    statistics::IGlobalParameters*          getGlobalStatistics ()  { return _globalStats;      }

    /** \copydoc IAlgorithm::getQueryInfo */
    statistics::IQueryInformation*          getQueryInfo        ()  { return _queryInfo;        }

    /** \copydoc IAlgorithm::getIndexator */
    IIndexator*                             getIndexator        ()  { return _indexator;        }

    /** \copydoc IAlgorithm::getHitIterator */
    algo::hits::IHitIterator*               getHitIterator      ()  { return _hitIterator;      }

    /** \copydoc IAlgorithm::setConfig */
    void setConfig           (IConfiguration*                       config)             { SP_SETATTR (config);          }

    /** \copydoc IAlgorithm::setReader */
    void setReader           (database::IDatabaseQuickReader*       reader)             { SP_SETATTR (reader);          }

    /** \copydoc IAlgorithm::setParams */
    void setParams           (IParameters*                          params)             { SP_SETATTR (params);          }

    /** \copydoc IAlgorithm::setFilter */
    void setFilter           (alignment::filter::IAlignmentFilter*  filter)             { SP_SETATTR (filter);          }

    /** \copydoc IAlgorithm::setResultVisitor */
    void setResultVisitor    (alignment::core::IAlignmentContainerVisitor* resultVisitor) { SP_SETATTR (resultVisitor);   }

    /** \copydoc IAlgorithm::setDatabasesProvider */
    void setDatabasesProvider (algo::core::IDatabasesProvider* dbProvider)  { SP_SETATTR(dbProvider); }

    /** \copydoc IAlgorithm::setSeedsModel */
    void setSeedsModel       (seed::ISeedModel*                     seedsModel)         { SP_SETATTR (seedsModel);      }

    /** \copydoc IAlgorithm::setScoreMatrix */
    void setScoreMatrix      (IScoreMatrix*                         scoreMatrix)        { SP_SETATTR (scoreMatrix);     }

    /** \copydoc IAlgorithm::setGlobalStatistics */
    void setGlobalStatistics (statistics::IGlobalParameters*        globalStats)        { SP_SETATTR (globalStats);     }

    /** \copydoc IAlgorithm::setQueryInfo */
    void setQueryInfo        (statistics::IQueryInformation*        queryInfo)          { SP_SETATTR (queryInfo);       }

    /** \copydoc IAlgorithm::setIndexator */
    void setIndexator        (IIndexator*                           indexator)          { SP_SETATTR (indexator);       }

    /** \copydoc IAlgorithm::setHitIterator */
    void setHitIterator      (algo::hits::IHitIterator*             hitIterator)        { SP_SETATTR (hitIterator);     }

protected:

    /** \copydoc IAlgorithm::createHitIterator */
    algo::hits::IHitIterator* createHitIterator (
        IConfiguration*                         config,
        algo::hits::IHitIterator*               sourceHits,
        alignment::core::IAlignmentContainer*   ungapAlignResult,
        alignment::core::IAlignmentContainer*   alignResult
    );

    std::vector<misc::ReadingFrame_e>  _subjectFrames;
    /** \copydoc IAlgorithm::getSubjectFrames */
    const std::vector<misc::ReadingFrame_e>&  getSubjectFrames ()  { return _subjectFrames; }

    /** */
    std::vector<misc::ReadingFrame_e>  _queryFrames;
    /** \copydoc IAlgorithm::getQueryFrames */
    const std::vector<misc::ReadingFrame_e>&  getQueryFrames   ()  { return _queryFrames;   }

    static misc::ReadingFrame_e allframes[];
    static misc::ReadingFrame_e topframes[];
    static misc::ReadingFrame_e bottomframes[];

    virtual void hitUpdate (const algo::hits::IHitIterator* hit) {}

    /** Receives and forwards notifications.
     * \param[in] evt     : notification information
     * \param[in] subject : sender of the notification
     */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

    IConfiguration*                                 _config;
    database::IDatabaseQuickReader*                 _reader;
    IParameters*                                    _params;
    alignment::filter::IAlignmentFilter*            _filter;
    alignment::core::IAlignmentContainerVisitor*    _resultVisitor;
    IDatabasesProvider*                             _dbProvider;
    seed::ISeedModel*                               _seedsModel;
    IScoreMatrix*                                   _scoreMatrix;
    statistics::IGlobalParameters*                  _globalStats;
    statistics::IQueryInformation*                  _queryInfo;
    IIndexator*                                     _indexator;
    algo::hits::IHitIterator*                       _hitIterator;

    /** */
    alignment::core::IAlignmentContainer* _ungapAlignmentResult;
    void setUngapAlignmentResult (alignment::core::IAlignmentContainer* ungapAlignmentResult)  { SP_SETATTR(ungapAlignmentResult); }

    /** */
    alignment::core::IAlignmentContainer* _gapAlignmentResult;
    void setGapAlignmentResult (alignment::core::IAlignmentContainer* gapAlignmentResult)  { SP_SETATTR(gapAlignmentResult); }

    /**  */
    bool& _isRunning;

    /** */
    class AlgoTimeInfo : public os::impl::TimeInfo
    {
    public:
        AlgoTimeInfo (AbstractAlgorithm* algo);
        void addEntry (const char* name);
    private:
        AbstractAlgorithm* _algo;
    };

    os::impl::TimeInfo* createTimeInfo ()  { return new AlgoTimeInfo (this); }
};

/********************************************************************************/

/** \brief Implementation of the plastp algorithm (protein/protein)
 *
 * The plastp algorithm just inherits from the AbstractAlgorithm class.
 *
 * Note: it means that the AbstractAlgorithm class implements by default a protein/protein
 * comparison.
 */
class AlgorithmPlastp : public AbstractAlgorithm
{
public:

    /** \copydoc AbstractAlgorithm */
    AlgorithmPlastp (
        IConfiguration*                                 config,
        database::IDatabaseQuickReader*                 reader,
        IParameters*                                    params,
        alignment::filter::IAlignmentFilter*            filter,
        alignment::core::IAlignmentContainerVisitor*    resultVisitor,
        algo::core::IDatabasesProvider*                 dbProvider,
        bool&                                           isRunning
    )
    : AbstractAlgorithm (config, reader, params, filter, resultVisitor, dbProvider, isRunning) {}
};

/********************************************************************************/

/** \brief Implementation of the plastx algorithm (protein/ADN)
 *
 * The plastx algorithm inherits from the AbstractAlgorithm class and specifies
 * what are the reading frames to be used for the query database.
 */
class AlgorithmPlastx : public AbstractAlgorithm
{
public:

    /** \copydoc AbstractAlgorithm */
    AlgorithmPlastx (
        IConfiguration*                                 config,
        database::IDatabaseQuickReader*                 reader,
        IParameters*                                    params,
        alignment::filter::IAlignmentFilter*            filter,
        alignment::core::IAlignmentContainerVisitor*    resultVisitor,
        algo::core::IDatabasesProvider*                 dbProvider,
        bool&                                           isRunning
    )
    : AbstractAlgorithm (config, reader, params, filter, resultVisitor, dbProvider, isRunning)
    {
        if (params->strands.empty() )
        {
            _queryFrames.assign (allframes, allframes + 6);
        }
        else
        {
            _queryFrames.assign (params->strands.begin(), params->strands.end());
        }
    }
};

/********************************************************************************/

/** \brief Implementation of the tplastn algorithm (ADN/protein)
 *
 * The tplastn algorithm inherits from the AbstractAlgorithm class and specifies
 * what are the reading frames to be used for the subject database.
 */
class AlgorithmTplastn : public AbstractAlgorithm
{
public:

    /** \copydoc AbstractAlgorithm */
    AlgorithmTplastn (
        IConfiguration*                                 config,
        database::IDatabaseQuickReader*                 reader,
        IParameters*                                    params,
        alignment::filter::IAlignmentFilter*            filter,
        alignment::core::IAlignmentContainerVisitor*    resultVisitor,
        algo::core::IDatabasesProvider*                 dbProvider,
        bool&                                           isRunning
    )
    : AbstractAlgorithm (config, reader, params, filter, resultVisitor, dbProvider, isRunning)
    {
        if (params->strands.empty() )
        {
            _subjectFrames.assign (allframes, allframes + 6);
        }
        else
        {
            _subjectFrames.assign (params->strands.begin(), params->strands.end());
        }
    }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_ALGORITHM_HPP_ */
