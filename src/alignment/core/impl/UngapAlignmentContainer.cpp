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


#include <alignment/core/impl/UngapAlignmentContainer.hpp>

#include <alignment/tools/api/IAlignmentSplitter.hpp>

#include <misc/api/macros.hpp>

#include <iostream>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace database;
using namespace dp;
using namespace os;
using namespace os::impl;
using namespace algo::core;
using namespace alignment::tools;
//#define VERBOSE

/********************************************************************************/
namespace alignment {
namespace core      {
namespace impl      {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
UngapAlignmentResult::UngapAlignmentResult (size_t nbQuerySequences)
    : _diagGlobal(0), _listGaplessAlign(0), _listGaplessAlignSize(0)
{
    /** We could set this parameter according to some maximum memory size for the list to be created. */
    _DIVDIAG = 10;

    size_t diagLength = 0;
    size_t k = nbQuerySequences;

    diagLength = 1;
    while (diagLength < k)     {  diagLength = diagLength << 1;  }

    if  (diagLength<1000000)   {  diagLength = diagLength << 3;  }

    _diagGlobal = diagLength - 1;

    k = _diagGlobal/_DIVDIAG + 10;

    _listGaplessAlign = (LISTGAP **) DefaultFactory::memory().calloc (k, sizeof(LISTGAP*));

    _listGaplessAlignSize = 0;

    DEBUG (("UngapAlignmentResult::UngapAlignmentResult:  nbQuerySequences=%ld  _DIVDIAG=%d  _diagGlobal=%d   listSize=%d\n",
        nbQuerySequences, _DIVDIAG, _diagGlobal, k*sizeof(LISTGAP*)
    ));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
UngapAlignmentResult::~UngapAlignmentResult ()
{
    DEBUG (("UngapAlignmentResult::~UngapAlignmentResult: _listGaplessAlignSize=%ld\n", _listGaplessAlignSize));

    if (_listGaplessAlign)
    {
        size_t k = _diagGlobal/_DIVDIAG + 10;
        for(size_t i=0; i<k; i++)
        {
            if (_listGaplessAlign[i]==NULL)   {  continue;  }

            LISTGAP* gl_next = 0;
            for (LISTGAP* gl = _listGaplessAlign[i];  gl != NULL;  )
            {
                gl_next = gl->next;
                DefaultFactory::memory().free (gl);
                gl = gl_next;
            }
            _listGaplessAlign[i] = NULL;
        }

        DefaultFactory::memory().free (_listGaplessAlign);
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool UngapAlignmentResult::insert (Alignment& align, void* context)
{
    bool result = false;
    IAlignmentSplitter* splitter = (IAlignmentSplitter*) context;

    if (splitter != 0)
    {
        u_int32_t  splittab[10000];
        IAlignmentSplitter::SplitOutput output (splittab);

        size_t nbSplits = splitter->splitAlign (align, output);

        /** A little check. */
        if (nbSplits > 0  &&  nbSplits < ARRAYSIZE(splittab))
        {
            u_int32_t q_start=0, q_stop=0, s_start=0, s_stop=0;

            /** Note that we can have a range (in subject or query) that finishes by one or more '-'.
             *  In such a case, nbAlign won't divide 4, so we have to skip the split holding
             *  the '-'. We achieve this by taking the nearest lower integer modulo 4.
             */
            nbSplits = nbSplits & ~3;

            for (size_t i=0; i<nbSplits; i=i+4)
            {
                u_int64_t qryOffset = align.getInfo(Alignment::QUERY).getOffsetInDb();
                u_int64_t sbjOffset = align.getInfo(Alignment::SUBJECT).getOffsetInDb();

                q_start = qryOffset + splittab[i+2];
                q_stop  = qryOffset + splittab[i+0];

                s_start = sbjOffset + splittab[i+3];
                s_stop  = sbjOffset + splittab[i+1];

                if (q_stop - q_start >= 4)  // COULD BE MODIFIED: 4 ?
                {
                    result = addDiag (
                        q_start,
                        q_stop,
                        s_start,
                        s_stop,
                        align.getSequence(Alignment::QUERY)->index
                    );
                }
            }
        }
    }

    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool UngapAlignmentResult::doesExist (
    const indexation::ISeedOccurrence* subjectOccur,
    const indexation::ISeedOccurrence* queryOccur,
    u_int32_t bandSize
)
{
    u_int32_t d = 0;

    /** We retrieve the list of interest (and its diagonal). */
    LISTGAP* gl = getItem (
        queryOccur->offsetInDatabase,
        subjectOccur->offsetInDatabase,
        queryOccur->sequence.index,
        d
    );

    /** Note: for optimization concerns, we prefer to directly use 'return' instead of managing
     *  a result variable until the end of the function (note perfect from coding rules point
     *  of view because of multiple return statements in the same function). */

    if (gl==NULL)   {  return false; }

    {
#if 0
        /** We may want to lock the iteration of the list. In doing so, we may avoid some cell creations,
         * but we may slow down concurrent thread that access the list.
         */
        LocalSynchronizer local (_synchro);
#endif

        /** Shortcut. */
        u_int64_t qoffset = queryOccur->offsetInDatabase;

        while ( (gl!=NULL) &&  ( (d<gl->diag) || ( (gl->diag==d) && (qoffset > gl->stop)) ) )
        {
            gl = gl -> next;
        }

        if (gl==NULL)   { return false;  }
    }

    if (d > gl->diag)   { return false; }

    if ((gl->diag == d) && (queryOccur->offsetInDatabase < gl->start))    { return false;  }

    /** Being here means that we found our item. */
    return true;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool UngapAlignmentResult::addDiag (
    u_int64_t q_start,
    u_int64_t q_stop,
    u_int64_t s_start,
    u_int64_t s_stop,
    u_int32_t seqIdx
)
{
    bool alreadyExist = false;

    u_int32_t d = 0;

    /** We retrieve the list of interest (and its diagonal). */
    LISTGAP* gl = getItem (q_start, s_start, seqIdx, d);

    LISTGAP* prev_gl = 0;
    size_t k = 0;
    {
#if 0
        /** We may want to lock the iteration of the list. In doing so, we may avoid some cell creations,
         * but we may slow down concurrent thread that access the list.
         */
        LocalSynchronizer local (_synchro);
#endif

        /** We try to find in the list the item of interest. */
        while ((gl!=NULL) && ( (d<gl->diag) || ((gl->diag==d) && (q_start>=gl->stop))) )
        {
            prev_gl=gl;
            gl = gl -> next;
            k++;
        }
    }

    // we can add a new alignment to the list (ngl) if it's satisfied one of the following conditions
    //      gl = NULL  : no alignment identified
    //      d>gl->diag : as the diagonals are sorted in ascending order, we have not met d = gl->diag
    //      d=gl->diag and index1>gl->stop : a diagonal exists, but the index is outside the scope
    // k indicates the number of alignment on which stopped (k = 0 -> empty list)

    if ((gl==NULL) || (d>gl->diag) || ((gl->diag==d) && (q_start<gl->start)))
    {
        /** We create a new cell. */
        LISTGAP* ngl = (LISTGAP*) DefaultFactory::memory().malloc (sizeof(LISTGAP));

        ngl->diag  = d;
        ngl->start = q_start;
        ngl->stop  = q_stop;

        // 4 cas sont a considerer suivant : k et gl
        if (gl == NULL)
        {
            if (k==0)
            {
                // gl = NULL et k = 0  --> empty list : connection to ngl (first element)
                _synchro->lock ();
                ngl->next = (LISTGAP *) NULL;
                _listGaplessAlign[d/_DIVDIAG] = ngl;
                _synchro->unlock ();
            }
            else
            {
                // gl = NULL et k = 1 --> insert ngl at the end of list
                _synchro->lock ();
                ngl->next =  (LISTGAP *) NULL;
                prev_gl->next = ngl;
                _synchro->unlock ();
            }
        }
        else
        {
            if (k==0)
            {
                // gl <> NULL et k = 0 --> insert ngl at beginning of list
                _synchro->lock ();
                ngl->next = gl;
                _listGaplessAlign[d/_DIVDIAG] = ngl;
                _synchro->unlock ();
            }
            else
            {
                // gl <> NULL et k > 0 --> insert ngl before gl
                _synchro->lock ();
                ngl->next = gl;
                prev_gl->next = ngl;
                _synchro->unlock ();
            }
        }

        _listGaplessAlignSize++;

        alreadyExist = false;
    }

    else
    {
        alreadyExist = true;
    }

    return alreadyExist;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
