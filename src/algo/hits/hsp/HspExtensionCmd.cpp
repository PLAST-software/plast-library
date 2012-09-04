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

#include <algo/hits/hsp/HspExtensionCmd.hpp>

#include <alignment/core/impl/HspContainer.hpp>

#include <alignment/tools/impl/AlignmentSplitter.hpp>

#include <designpattern/api/Iterator.hpp>

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
HspExtensionCmd::HspExtensionCmd (
    database::ISequenceDatabase*            db1,
    database::ISequenceDatabase*            db2,
    statistics::IQueryInformation*          queryInfo,
    IHspContainer*                          hspContainer,
    IHspContainer*                          hspContainer2,
    alignment::tools::ISemiGapAlign*        dynapro,
    algo::core::IParameters*                params,
    dp::IObserver* 							observer
)
    : _db1(0), _db2(0), _queryInfo(0), _hspContainer (0), _hspContainer2 (0),
      _dynapro(0), _parameters(0)
{
    setDb1                (db1);
    setDb2                (db2);
    setQueryInfo          (queryInfo);
    setHspContainer       (hspContainer);
    setHspContainer2      (hspContainer2);
    setDynapro            (dynapro);
    setParameters         (params);

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
HspExtensionCmd::~HspExtensionCmd ()
{
    setDb1                (0);
    setDb2                (0);
    setQueryInfo          (0);
    setHspContainer       (0);
    setHspContainer2      (0);
    setDynapro            (0);
    setParameters         (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void HspExtensionCmd::execute ()
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

         u_int64_t qryStart = hsp->q_start;
         u_int64_t sbjStart = hsp->s_start;

         /** We look for the query sequence. */
         ISequence* seqQry = _db2->getSequenceRefByIndex (hsp->q_idx);
         if (!seqQry)  { throw MSG_HITS_MSG1; }

         ISequence* seqSbj = _db1->getSequenceRefByIndex (hsp->s_idx);
         if (!seqSbj)  { throw MSG_HITS_MSG1; }

         /** We shift the two HSP ranges into sequence referential (instead of database). */
         qryStart -= seqQry->offsetInDb;
         sbjStart -= seqSbj->offsetInDb;

        int leftOffsetInQuery=0, leftOffsetInSubject=0, rightOffsetInQuery=0, rightOffsetInSubject=0;

        int scoreRight=0, scoreLeft=0;

         const char* qryData = seqQry->getData();
         const char* sbjData = seqSbj->getData();

         /** LEFT EXTENSION */
         scoreLeft = _dynapro->compute (
             qryData,
             sbjData,
             qryStart + 1,
             sbjStart + 1,
             & leftOffsetInQuery,
             & leftOffsetInSubject,
             1
         );

         /** RIGHT EXTENSION */
         scoreRight = _dynapro->compute (
             qryData + qryStart,
             sbjData + sbjStart,
             seqQry->getLength() - qryStart - 1,
             seqSbj->getLength() - sbjStart - 1,
             & rightOffsetInQuery,
             & rightOffsetInSubject,
             0
         );

         /** We compute the total score. */
         int score = scoreLeft + scoreRight;

         /** We retrieve statistical information for the current query sequence. */
         IQueryInformation::SequenceInfo& info = _queryInfo->getSeqInfo (*seqQry);

         if (score >= info.cut_offs)
         {
             _hspContainer2->insert (
                 hsp->q_start - leftOffsetInQuery   + 1,
                 hsp->q_start + rightOffsetInQuery,
                 hsp->s_start - leftOffsetInSubject + 1,
                 hsp->s_start + rightOffsetInSubject,
                 hsp->q_idx,
                 hsp->s_idx,
                 score
             );
         }

     } /* end of while ( (hsp = _hspContainer->retrieve... */

     /** We send a notification about the % of execution. Here, we force to be at 100%. */
     this->notify (new IterationStatusEvent (ITER_ON_GOING, nbTotal, nbTotal, MSG_HITS_MSG2, nbTotal, nbTotal));

     DEBUG (("HspExtensionCmd::execute  nbExists=%d\n", nbExists));
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
