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

#ifndef _ABSTRACT_ALGORITHM_HPP_
#define _ABSTRACT_ALGORITHM_HPP_

/********************************************************************************/
#include "Observer.hpp"

#include "IAlgorithm.hpp"
#include "IAlgoConfig.hpp"
#include "IAlgoParameters.hpp"
#include "ISequenceDatabase.hpp"
#include "ISeedModel.hpp"
#include "ITime.hpp"
#include "IDatabaseQuickReader.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** The 'execute' method (from dp::ICommand) is also a Template method, since it uses
 * several primitives for creating databases and iterators for instance.
 *
 * Ideally, the 'execute' method should be generic enough for being not changed. All
 * the variant parts should be:
 *      - the different instances used for providing the service (see getters/setters)
 *      - the primitive of the Template Method.
 *
 * In particular, this 'execute' method should work for 'plastp', 'tplastn', 'plastx' and
 * maybe some other in the future.
 */

class AbstractAlgorithm : public IAlgorithm
{
public:

    /** */
    AbstractAlgorithm (
        IConfiguration*                 config,
        database::IDatabaseQuickReader* reader,
        IParameters*                    params,
        AlignmentResultVisitor*         resultVisitor
    );
    virtual ~AbstractAlgorithm ();

    /** */
    void execute (void);

    /** */
    IConfiguration*                 getConfig           ()  { return _config;           }
    database::IDatabaseQuickReader* getReader           ()  { return _reader;           }
    IParameters*                    getParams           ()  { return _params;           }
    algo::AlignmentResultVisitor*   getResultVisitor    ()  { return _resultVisitor;    }
    seed::ISeedModel*               getSeedsModel       ()  { return _seedsModel;       }
    algo::IScoreMatrix*             getScoreMatrix      ()  { return _scoreMatrix;      }
    statistics::IGlobalParameters*  getGlobalStatistics ()  { return _globalStats;      }
    statistics::IQueryInformation*  getQueryInfo        ()  { return _queryInfo;        }
    IIndexator*                     getIndexator        ()  { return _indexator;        }
    indexation::IHitIterator*       getHitIterator      ()  { return _hitIterator;      }

    /** */
    void setConfig           (IConfiguration*                   config)             { SP_SETATTR (config);          }
    void setReader           (database::IDatabaseQuickReader*   reader)             { SP_SETATTR (reader);          }
    void setParams           (IParameters*                      params)             { SP_SETATTR (params);          }
    void setResultVisitor    (algo::AlignmentResultVisitor*     resultVisitor)      { SP_SETATTR (resultVisitor);   }
    void setSeedsModel       (seed::ISeedModel*                 seedsModel)         { SP_SETATTR (seedsModel);      }
    void setScoreMatrix      (algo::IScoreMatrix*               scoreMatrix)        { SP_SETATTR (scoreMatrix);     }
    void setGlobalStatistics (statistics::IGlobalParameters*    globalStats)        { SP_SETATTR (globalStats);     }
    void setQueryInfo        (statistics::IQueryInformation*    queryInfo)          { SP_SETATTR (queryInfo);       }
    void setIndexator        (IIndexator*                       indexator)          { SP_SETATTR (indexator);       }
    void setHitIterator      (indexation::IHitIterator*         hitIterator)        { SP_SETATTR (hitIterator);     }

protected:

    dp::ListIterator<database::ISequenceDatabase*> createDatabaseIterator (
        IConfiguration*     config,
        const std::string&  uri,
        const Range&        range,
        bool                filtering,
        const std::vector<database::ReadingFrame_e>& frames
    );

    indexation::IHitIterator* createHitIterator (
        IConfiguration*             config,
        indexation::IHitIterator*   sourceHits,
        algo::IAlignmentResult*     ungapAlignResult,
        algo::IAlignmentResult*     alignResult
    );

    /** */
    std::vector<database::ReadingFrame_e>  _subjectFrames;
    const std::vector<database::ReadingFrame_e>&  getSubjectFrames ()  { return _subjectFrames; }

    /** */
    std::vector<database::ReadingFrame_e>  _queryFrames;
    const std::vector<database::ReadingFrame_e>&  getQueryFrames   ()  { return _queryFrames;   }

    static database::ReadingFrame_e allframes[];
    static database::ReadingFrame_e topframes[];
    static database::ReadingFrame_e bottomframes[];

    virtual void hitUpdate (const indexation::IHitIterator* hit) {}

    /** Implementation of IObserver interface. */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

    IConfiguration*                 _config;
    database::IDatabaseQuickReader* _reader;
    IParameters*                    _params;
    algo::AlignmentResultVisitor*   _resultVisitor;
    seed::ISeedModel*               _seedsModel;
    algo::IScoreMatrix*             _scoreMatrix;
    statistics::IGlobalParameters*  _globalStats;
    statistics::IQueryInformation*  _queryInfo;
    IIndexator*                     _indexator;
    indexation::IHitIterator*       _hitIterator;

    /** */
    IAlignmentResult* _ungapAlignmentResult;
    void setUngapAlignmentResult (IAlignmentResult* ungapAlignmentResult)  { SP_SETATTR(ungapAlignmentResult); }

    /** */
    IAlignmentResult* _gapAlignmentResult;
    void setGapAlignmentResult (IAlignmentResult* gapAlignmentResult)  { SP_SETATTR(gapAlignmentResult); }

    /** */
    class AlgoTimeInfo : public os::TimeInfo
    {
    public:
        AlgoTimeInfo (AbstractAlgorithm* algo);
        void addEntry (const char* name);
    private:
        AbstractAlgorithm* _algo;
    };

    os::TimeInfo* createTimeInfo ()  { return new AlgoTimeInfo (this); }
};

/********************************************************************************/

class AlgorithmPlastp : public AbstractAlgorithm
{
public:
    AlgorithmPlastp (
        IConfiguration*                 config,
        database::IDatabaseQuickReader* reader,
        IParameters*                    params,
        AlignmentResultVisitor*         resultVisitor
    )
    : AbstractAlgorithm (config, reader, params, resultVisitor) {}
};

/********************************************************************************/

class AlgorithmPlastx : public AbstractAlgorithm
{
public:
    AlgorithmPlastx (
        IConfiguration*                 config,
        database::IDatabaseQuickReader* reader,
        IParameters*                    params,
        AlignmentResultVisitor*         resultVisitor
    )
    : AbstractAlgorithm (config, reader, params, resultVisitor)
    {
        _queryFrames.assign (allframes, allframes + 6);
    }
};

/********************************************************************************/

class AlgorithmTplastn : public AbstractAlgorithm
{
public:
    AlgorithmTplastn (
        IConfiguration*                 config,
        database::IDatabaseQuickReader* reader,
        IParameters*                    params,
        AlignmentResultVisitor*         resultVisitor
    )
    : AbstractAlgorithm (config, reader, params, resultVisitor)
    {
        _subjectFrames.assign (allframes, allframes + 6);
    }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_ALGORITHM_HPP_ */
