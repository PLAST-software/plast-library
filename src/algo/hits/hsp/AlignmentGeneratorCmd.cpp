/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

#include <algo/hits/hsp/AlignmentGeneratorCmd.hpp>

#include <misc/api/PlastStrings.hpp>

#include <string.h>
#include <math.h>

using namespace std;

using namespace dp;

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
    IAlignmentSplitter*                     alignmentSplitter,
    algo::core::IScoreMatrix*               scoreMatrix,
    algo::core::IParameters*                params,
    dp::IObserver* 							observer
)
    : _db1(0), _db2(0), _queryInfo(0), _globalStats(0), _hspContainer (0), _alignHspContainer(0), _alignmentContainer(0),
      _splitter(0)
{
    setDb1                (db1);
    setDb2                (db2);
    setQueryInfo          (queryInfo);
    setGlobalStats        (globalStats);
    setHspContainer       (hspContainer);
    setAlignmentContainer (alignmentContainer);
    setSplitter           (alignmentSplitter);

    /** We register the provided observer. */
    this->addObserver (observer);
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
    size_t nbHsp=0;

    IHspContainer::HSP* hsp = 0;

    size_t nbTotal       = _hspContainer->getItemsNumber();
    size_t notifyModulus = MAX (1, nbTotal / 100);  // notify each percentage.

    /** We loop over all HSP. */
     while ( (hsp = _hspContainer->retrieve (nbHsp)) != 0)
     {
    	 /** We notify about the execution % of the loop. */
    	 if (nbHsp % notifyModulus == 0)
    	 {
    	     this->notify (new IterationStatusEvent (ITER_ON_GOING, nbHsp,  nbTotal, MSG_HITS_MSG2, nbHsp,  nbTotal));
    	 }

         /** We look for the query sequence. */
         ISequence* seqQry = _db2->getSequenceRefByIndex (hsp->q_idx);
         if (!seqQry)  { throw MSG_HITS_MSG1; }

         ISequence* seqSbj = _db1->getSequenceRefByIndex (hsp->s_idx);
         if (!seqSbj)  {  throw MSG_HITS_MSG1;  }

         int score = hsp->score;

         /** We retrieve statistical information for the current query sequence. */
         IQueryInformation::SequenceInfo& info = _queryInfo->getSeqInfo (*seqQry);

         if (score >= info.cut_offs)
         {
        	 /** We create the alignment object. */
             Alignment align (
                 seqQry, seqSbj,
                 misc::Range32 (hsp->q_start - seqQry->offsetInDb, hsp->q_stop - seqQry->offsetInDb),
                 misc::Range32 (hsp->s_start - seqSbj->offsetInDb, hsp->s_stop - seqSbj->offsetInDb)
             );

             /** We split the alignment into HSPs. */
             IAlignmentSplitter::SplitOutput output;
             _splitter->splitAlign (align, output);

             //double evalue = (double) info.eff_searchsp * exp((-_globalStats->lambda * (double) score) + _globalStats->logK);
             double evalue = _globalStats->scoreToEvalue((double) info.eff_searchsp, (double) score, seqQry->getLength(), seqSbj->getLength());

             /** We complete missing alignment information. */
             align.setEvalue       (evalue);
             align.setBitScore     (_globalStats->rawToBitsValue((double)score));
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

     }  /* end of while ( (hsp = ... */

     /** We send a notification about the % of execution. Here, we force to be at 100%. */
     this->notify (new IterationStatusEvent (ITER_ON_GOING, nbTotal, nbTotal, MSG_HITS_MSG2, nbTotal, nbTotal));
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
