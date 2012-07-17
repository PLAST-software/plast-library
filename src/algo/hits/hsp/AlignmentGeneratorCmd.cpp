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

#include <algo/hits/hsp/AlignmentGeneratorCmd.hpp>

#include <alignment/tools/impl/AlignmentSplitter.hpp>

#include <string.h>
#include <math.h>

using namespace std;

using namespace os;
using namespace os::impl;

using namespace misc;

using namespace database;

using namespace statistics;

using namespace alignment::core;

using namespace alignment::tools;
using namespace alignment::tools::impl;

#include <stdio.h>
#define DEBUG(a)    //printf a
#define VERBOSE(a)  //printf a

#define LOG2 0.69314718055994530941

/********************************************************************************/
namespace algo {
namespace hits {
namespace hsp  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlignmentGeneratorCmd::AlignmentGeneratorCmd (
    database::ISequenceDatabase*            db1,
    database::ISequenceDatabase*            db2,
    statistics::IQueryInformation*          queryInfo,
    statistics::IGlobalParameters*          globalStats,
    IHspContainer*                          hspContainer,
    IAlignmentContainer*                    alignmentContainer,
    algo::core::IScoreMatrix*               scoreMatrix,
    algo::core::IParameters*                params
)
    : _db1(0), _db2(0), _queryInfo(0), _globalStats(0), _hspContainer (0), _alignHspContainer(0), _alignmentContainer(0),
      _parameters(0), _splitter(0)
{
    setDb1                (db1);
    setDb2                (db2);
    setQueryInfo          (queryInfo);
    setGlobalStats        (globalStats);
    setHspContainer       (hspContainer);
    setAlignmentContainer (alignmentContainer);
    setParameters         (params);
    setSplitter           (new AlignmentSplitter (scoreMatrix, _parameters->openGapCost, _parameters->extendGapCost));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlignmentGeneratorCmd::~AlignmentGeneratorCmd ()
{
    setDb1                (0);
    setDb2                (0);
    setQueryInfo          (0);
    setGlobalStats        (0);
    setHspContainer       (0);
    setAlignHspContainer  (0);
    setAlignmentContainer (0);
    setParameters         (0);
    setSplitter           (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlignmentGeneratorCmd::execute ()
{
    u_int64_t nbHsp=0;

    IHspContainer::HSP* hsp = 0;

    /** We loop over all HSP. */
     while ( (hsp = _hspContainer->retrieve (nbHsp)) != 0)
     {
         ISequence seqQry, seqSbj;

         u_int32_t offsetInQry, offsetInSbj;
         u_int64_t actualOffsetInDbQry, actualOffsetInDbSbj;

         u_int64_t qryStart = hsp->q_start;
         u_int64_t sbjStart = hsp->s_start;

         /** We look for the query sequence. */
         bool foundQry = _db2->getSequenceByOffset (qryStart, seqQry, offsetInQry, actualOffsetInDbQry);
         if (!foundQry)  { printf ("QRY NOT FOUND...\n"); }

         bool foundSbj = _db1->getSequenceByOffset (sbjStart, seqSbj, offsetInSbj, actualOffsetInDbSbj);
         if (!foundSbj)  { printf ("SBJ NOT FOUND...\n"); }

         int score = hsp->score;

         /** We retrieve statistical information for the current query sequence. */
         IQueryInformation::SequenceInfo& info = _queryInfo->getSeqInfo (seqQry);

         if (score >= info.cut_offs)
         {
        	 /** We create the alignment object. */
             Alignment align (
                 &seqQry, &seqSbj,
                 misc::Range32 (hsp->q_start - seqQry.offsetInDb, hsp->q_stop - seqQry.offsetInDb),
                 misc::Range32 (hsp->s_start - seqSbj.offsetInDb, hsp->s_stop - seqSbj.offsetInDb)
             );

             /** We split the alignment into HSPs. */
             IAlignmentSplitter::SplitOutput output;
             _splitter->splitAlign (align, output);

             double evalue = (double) info.eff_searchsp * exp((-_globalStats->lambda * (double) score) + _globalStats->logK);

             /** We complete missing alignment information. */
             align.setEvalue       (evalue);
             align.setBitScore     ((_globalStats->lambda * (double)score - _globalStats->logK) / LOG2);
             align.setScore        (score);
             align.setLength       (output.alignSize);
             align.setNbIdentities (output.identity);
             align.setNbPositives  (output.positive);
             align.setNbMisses     (output.nbMis);
             align.setNbGaps       (Alignment::QUERY,   output.nbGapQry);
             align.setNbGaps       (Alignment::SUBJECT, output.nbGapSbj);

             /** We add the alignment into the final container. */
             _alignmentContainer->insert (align, 0);
         }
     }
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
