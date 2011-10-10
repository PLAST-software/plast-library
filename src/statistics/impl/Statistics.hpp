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

#ifndef STATISTICS_HPP_
#define STATISTICS_HPP_

/********************************************************************************/

#include "IStatistics.hpp"
#include "ISequenceDatabase.hpp"
#include "IAlgoParameters.hpp"

#include <vector>

#include <stdio.h>

/********************************************************************************/
namespace statistics  {
/********************************************************************************/

class GlobalParameters : public IGlobalParameters
{
public:

    GlobalParameters (algo::IParameters* parameters);

private:

    algo::IParameters* _parameters;
    void setParameters (algo::IParameters* parameters);

    /** Computes statistics. */
    void build (void);
};

/********************************************************************************/

class QueryInformation : public IQueryInformation
{
public:

    QueryInformation (
        IGlobalParameters*              globalParams,
        algo::IParameters*              parameters,
        database::ISequenceDatabase*    queryDb,
        u_int64_t                       subjectDbSize,
        u_int32_t                       subjectNbSequences
    );

    virtual ~QueryInformation ();

    /** */
    struct SequenceInfo
    {
        int         sequence_length;
        int         length_adjust;
        int         cut_offs;
        long long   eff_searchsp;
    };

    size_t getNbSequences()  { return (_queryDb ? _queryDb->getSequencesNumber() : 0);  }

    IQueryInformation::SequenceInfo& getSeqInfoByIndex (size_t i)  {

        if (i>=_seqInfo.size())
        {
            // TBD launch an exception ?
        }
        return _seqInfo[i];
    }

private:

    IGlobalParameters* _globalParams;
    void setGlobalParams (IGlobalParameters* globalParams)  { SP_SETATTR (globalParams); }

    algo::IParameters* _parameters;
    void setParameters (algo::IParameters* parameters)      { SP_SETATTR (parameters);  }

    database::ISequenceDatabase* _queryDb;
    void setQueryDb (database::ISequenceDatabase* queryDb)  { SP_SETATTR (queryDb);     }

    /** Computes statistics for the provided query database. */
    void build (void);

    u_int64_t _subjectDbSize;
    u_int32_t _subjectNbSequences;

    /** We need a vector for holding information for each sequence. */
    std::vector<IQueryInformation::SequenceInfo> _seqInfo;

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
} /* end of namespaces. */
/********************************************************************************/

#endif /* STATISTICS_HPP_ */
