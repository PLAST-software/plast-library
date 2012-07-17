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

/** \file Statistics.hpp
 *  \brief Implementation of statistics management
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef STATISTICS_HPP_
#define STATISTICS_HPP_

/********************************************************************************/

#include <os/api/IThread.hpp>

#include <database/api/ISequenceDatabase.hpp>

#include <algo/core/api/IAlgoParameters.hpp>

#include <algo/stats/api/IStatistics.hpp>

#include <vector>
#include <map>
#include <stdio.h>

/********************************************************************************/
namespace statistics  {
/** \brief Implementation of statistics concepts */
namespace impl {
/********************************************************************************/

/** Code factorization.
 */
class AbstractGlobalParameters : public IGlobalParameters
{
public:

    /** Constructor.
     *  \param[in] parameters : parameters used for configuring the global statistics.
     */
    AbstractGlobalParameters (algo::core::IParameters* parameters) : _parameters(0)  { setParameters (parameters); }

    ~AbstractGlobalParameters () { setParameters (0); }

    /** Structure holding statistical information. */
    struct Info
    {
        double openGap;
        double extendGap;
        double lambda;
        double K;
        double H;
        double alpha;
        double beta;
        double theta;
    };

protected:

    /** Reference on the IParameters instance. */
    algo::core::IParameters* _parameters;
    void setParameters (algo::core::IParameters* parameters)  { SP_SETATTR(parameters); }

    /** Looks for statistical parameters according to a score matrix and open/extend gap costs.
     * \return true if parameters are found, false otherwise
     */
    static bool lookup (AbstractGlobalParameters* globalParams, void* table, size_t size);
};

/********************************************************************************/

/** \brief Implementation of IGlobalParameters interface
 *
 *  This class uses a IParameters instance (coming from command line options for instance)
 *  for customizing the generation of the statistical parameters
 *  (openGapCost, extendGapCost, etc...).
 */
class GlobalParameters : public AbstractGlobalParameters
{
public:

    /** Constructor.
     *  \param[in] parameters : parameters used for configuring the global statistics.
     */
    GlobalParameters (algo::core::IParameters* parameters)
        : AbstractGlobalParameters (parameters)  {  build(); }

protected:

    /** Computes statistics. */
    void build (void);
};

/********************************************************************************/

/** \brief Implementation of IQueryInformation interface
 *
 *  This class relies on several input information. In particular, query statistics
 *  are computed according to the subject database properties (data size, number of sequences).
 *
 *  It depends also on the global parameters and the user parameters.
 *
 *  It depends of course on the query database itself (in particular for getting the size of
 *  each query sequence).
 */
class QueryInformation : public IQueryInformation
{
public:

    /** Constructor.
     * \param[in] globalParams : global statistical parameters
     * \param[in] parameters : user parameters
     * \param[in] queryDb : query database
     * \param[in] subjectDbSize : data size of the subject database
     * \param[in] subjectNbSequences : number of sequences in the subject database
     */
    QueryInformation (
        IGlobalParameters*              globalParams,
        algo::core::IParameters*        parameters,
        database::ISequenceDatabase*    queryDb,
        u_int64_t                       subjectDbSize,
        u_int32_t                       subjectNbSequences
    );

    /** Destructor. */
    virtual ~QueryInformation ();

    /** \copydoc IQueryInformation::getSeqInfo */
    IQueryInformation::SequenceInfo& getSeqInfo (const database::ISequence& seq);

private:

    IGlobalParameters* _globalParams;
    void setGlobalParams (IGlobalParameters* globalParams)  { SP_SETATTR (globalParams); }

    algo::core::IParameters* _parameters;
    void setParameters (algo::core::IParameters* parameters)      { SP_SETATTR (parameters);  }

    database::ISequenceDatabase* _queryDb;
    void setQueryDb (database::ISequenceDatabase* queryDb)  { SP_SETATTR (queryDb);     }

    /** Computes statistics for the provided query database. */
    void build (void);

    u_int64_t _subjectDbSize;
    u_int32_t _subjectNbSequences;

    typedef std::vector<IQueryInformation::SequenceInfo> Container;

    /** We need a map whose key is a database. */
    std::map <database::ISequenceDatabase*, Container> _seqInfoMap;

    bool _isBuilt;
    os::ISynchronizer* _synchro;

    /** Ref. NCBI Blast. */
    bool computeLengthAdjustment (
        double      K,
        double      logK,
        double      alpha_d_lambda,
        double      beta,
        int         query_length,
        long long   db_length,
        int         db_num_seqs,
        int&        length_adjustment
    );

    /* Calculate the cutoff score, S, and the highest expected score. */
    int computeCutoffs (
        int&        S,           /* cutoff score */
        double&     E,           /* expected no. of HSPs scoring at or above S */
        long long   searchsp,    /* size of search space. */
        double      gap_decay_rate
    );
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* STATISTICS_HPP_ */
