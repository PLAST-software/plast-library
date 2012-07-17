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

#include <alignment/core/impl/HspContainer.hpp>
#include <os/impl/DefaultOsFactory.hpp>

#include <string.h>

using namespace std;

using namespace os;
using namespace os::impl;

using namespace misc;

#include <stdio.h>
#define DEBUG(a)    //printf a
#define VERBOSE(a)  //printf a

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
HspContainer::HspContainer (size_t dbQuerySize)
    : _diagonalNumber(0), _diagonalMask(0), _HspList(0), _HspListSize(0), _synchro(0),
      _firstRetrieve(true), _nbRetrieved(0), _currentDiagonal(0), _currentItem(0)
{
    size_t diagLength=0;

    /** We could set this parameter according to some maximum memory size for the list to be created. */
    _diagonalDivisor = 1024;

    /** We compute the number of bits needed for coding the dbQuerySize value. */
    for (diagLength = 1;  diagLength < dbQuerySize;  diagLength <<= 1)   {}

    if (diagLength < 1000000)   {  diagLength = diagLength << 6;  }

#if 1
    /** We compute the mask (as a series of 1) that spans all possible values in [0:dbQuerySize]. */
    _diagonalMask = diagLength - 1;

    /** We compute now the number of diagonals, ie the number of lists where the HSP will be stored. */
    _diagonalNumber = _diagonalMask/_diagonalDivisor + 10;

#else
    /** We compute now the number of diagonals, ie the number of lists where the HSP will be stored. */
    _diagonalNumber = diagLength/_diagonalDivisor;

    /** We compute the mask (as a series of 1) that spans all possible values in [0:dbQuerySize]. */
    _diagonalMask = _diagonalNumber - 1;

#endif

    /** We allocate the list (of list) of HSP. */
    _HspList = (LISTGAP **) DefaultFactory::memory().calloc (_diagonalNumber, sizeof(LISTGAP*));

    /** We initialize the number of HSP to 0. */
    _HspListSize = 0;

    /** We create a synchronizer for concurrent access. */
    _synchro = DefaultFactory::thread().newSynchronizer();

    DEBUG (("HspContainer::HspContainer:  dbQuerySize=0x%lx  _diagonalMask=0x%x   _diagonalDivisor=%d  _diagonalNumber=%d  listSize=%ld\n",
        dbQuerySize, _diagonalMask, _diagonalDivisor, _diagonalNumber, _diagonalNumber*sizeof(LISTGAP*)
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
HspContainer::~HspContainer ()
{
    DEBUG (("HspContainer::~HspContainer: _listGaplessAlignSize=%ld\n", _HspListSize));

    if (_HspList)
    {
        for (size_t i=0; i<_diagonalNumber; i++)
        {
            if (_HspList[i]==NULL)   {  continue;  }

            LISTGAP* gl_next = 0;
            for (LISTGAP* gl = _HspList[i];  gl != NULL;  )
            {
                gl_next = gl->next;
                DefaultFactory::memory().free (gl);
                gl = gl_next;
            }
            _HspList[i] = NULL;
        }

        DefaultFactory::memory().free (_HspList);
    }

    if (_synchro)  { delete _synchro; }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool HspContainer::insert (
    u_int64_t q_start,
    u_int64_t q_stop,
    u_int64_t s_start,
    u_int64_t s_stop,
    u_int32_t q_idx,
    u_int32_t s_idx,
    int16_t score
)
{
    LocalSynchronizer sync (_synchro);

    bool alreadyExist = false;

    u_int32_t d = 0;

    /** We retrieve the list of interest (and its diagonal). */
    LISTGAP** diagList = getItem (q_start, s_start, d, q_idx);

    LISTGAP* gl = *diagList;

    LISTGAP* prev_gl = 0;
    size_t k = 0;


    /** We try to find in the list the item of interest. */
    while ((gl!=NULL) && ( (d<gl->hsp.diag) || ((gl->hsp.diag==d) && (q_start>=gl->hsp.q_stop))) )
    {
        prev_gl=gl;
        gl = gl -> next;
        k++;
    }

//    VERBOSE (("HspContainer::insert (0):  qry=[%ld,%ld] (len=%ld)   sbj=[%ld,%ld]  delta=%d  d=%d\n",
//        q_start, q_stop,  q_stop-q_start+1,
//        s_start, s_start + q_stop - q_start,
//        delta, d
//    ));


    // we can add a new alignment to the list (ngl) if it's satisfied one of the following conditions
    //      gl = NULL  : no alignment identified
    //      d>gl->diag : as the diagonals are sorted in ascending order, we have not met d = gl->diag
    //      d=gl->diag and index1>gl->stop : a diagonal exists, but the index is outside the scope
    // k indicates the number of alignment on which stopped (k = 0 -> empty list)

    if ((gl==NULL) || (d>gl->hsp.diag) || ((gl->hsp.diag==d) && (q_start<gl->hsp.q_start)))
    {
        /** We create a new cell. */
        LISTGAP* ngl = (LISTGAP*) DefaultFactory::memory().malloc (sizeof(LISTGAP));

        ngl->hsp.diag      = d;
        ngl->hsp.q_start   = q_start;
        ngl->hsp.q_stop    = q_stop;
        ngl->hsp.s_start   = s_start;
        ngl->hsp.s_stop    = s_stop;
        ngl->hsp.q_idx     = q_idx;
        ngl->hsp.s_idx     = s_idx;
        ngl->hsp.score     = score;

//        VERBOSE (("HspContainer::insert (1):  qry=[%ld,%ld] (len=%ld)   sbj=[%ld,%ld] score=%d  delta=%d => diag=0x%x \n",
//            q_start, q_stop,  q_stop-q_start+1,
//            s_start, s_start + q_stop - q_start,
//            score,
//            delta,
//            d
//        ));

        // 4 cas sont a considerer suivant : k et gl
        if (gl == NULL)
        {
            if (k==0)
            {
                // gl = NULL et k = 0  --> empty list : connection to ngl (first element)
                ngl->next = (LISTGAP *) NULL;
                *diagList = ngl;
            }
            else
            {
                // gl = NULL et k = 1 --> insert ngl at the end of list
                ngl->next     =  (LISTGAP *) NULL;
                prev_gl->next = ngl;
            }
        }
        else
        {
            if (k==0)
            {
                // gl <> NULL et k = 0 --> insert ngl at beginning of list
                ngl->next = gl;
                *diagList = ngl;
            }
            else
            {
                // gl <> NULL et k > 0 --> insert ngl before gl
                ngl->next       = gl;
                prev_gl->next   = ngl;
            }
        }

        alreadyExist = false;
    }

    else
    {
        alreadyExist = true;
    }

    VERBOSE (("HspContainer::insert (0):  qry=[%ld,%ld] (len=%ld)   sbj=[%ld,%ld]  delta=%d  d=%d => alreadyExists=%d \n",
        q_start, q_stop,  q_stop-q_start+1,
        s_start, s_start + q_stop - q_start,
        delta, d,
        alreadyExist
    ));

    return alreadyExist;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool HspContainer::doesExist (u_int64_t q_start, u_int64_t s_start, u_int32_t delta)
{
#if 1
        /** We may want to lock the iteration of the list. In doing so, we may avoid some cell creations,
         * but we may slow down concurrent thread that access the list.
         */
        LocalSynchronizer local (_synchro);
#endif

      VERBOSE (("HspContainer::doesExist  q_start=%ld  s_start=%ld  delta=%d\n", q_start, s_start, delta));

    u_int32_t d = 0;

    /** We retrieve the list of interest (and its diagonal). */
    LISTGAP** diagList = getItem (q_start, s_start, d, delta);
    LISTGAP* gl = *diagList;

    /** Note: for optimization concerns, we prefer to directly use 'return' instead of managing
     *  a result variable until the end of the function (note perfect from coding rules point
     *  of view because of multiple return statements in the same function). */

    if (gl==NULL)   {  return false; }

    {
        while ( (gl!=NULL) &&  ( (d<gl->hsp.diag) || ( (gl->hsp.diag==d) && (q_start > gl->hsp.q_stop)) ) )
        {
            gl = gl -> next;
        }

        if (gl==NULL)   { return false;  }
    }

    if (d > gl->hsp.diag)   { return false; }

    if ((gl->hsp.diag == d) && (q_start < gl->hsp.q_start))    { return false;  }

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
IHspContainer::HSP* HspContainer::retrieve (u_int64_t& nbRetrieved)
{
    IHspContainer::HSP* result = 0;

    _synchro->lock();

    /** Specific stuff at first call. */
    if (_firstRetrieve == true)
    {
        clean ();

        _nbRetrieved     = 0;
        _currentDiagonal = 0;
        _currentItem     = 0;

        /** We look for the next non empty diagonal. */
        while (_currentDiagonal<_diagonalNumber &&  _HspList[_currentDiagonal]==NULL)   {  _currentDiagonal++;  }

        if (_currentDiagonal<_diagonalNumber)  {  _currentItem = _HspList[_currentDiagonal];  }

        _firstRetrieve = false;
    }

    if (_currentItem != 0)
    {
        result = &(_currentItem->hsp);

        nbRetrieved = ++_nbRetrieved;

        /** We setup the next item to be retrieved. */
        _currentItem = _currentItem->next;
        if (_currentItem == 0)
        {
            _currentDiagonal++;

            /** We look for the next non empty diagonal. */
            while (_currentDiagonal<_diagonalNumber &&  _HspList[_currentDiagonal]==NULL)   {  _currentDiagonal++;  }

            if (_currentDiagonal<_diagonalNumber)  {  _currentItem = _HspList[_currentDiagonal];  }
        }
    }

    _synchro->unlock ();

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
u_int64_t HspContainer::getItemsNumber ()
{
    _HspListSize = 0;

    if (_HspListSize == 0)
    {
        /** We loop each diagonal. */
        for (size_t i=0; i<_diagonalNumber; i++)
        {
            /** We loop every cell for the current diagonal. */
            for (LISTGAP* gl = _HspList[i];  gl != NULL;  gl = gl->next)
            {
                _HspListSize++;
            }
        }
    }

    return _HspListSize;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void  HspContainer::clean ()
{
    LISTGAP* prev    = 0;
    LISTGAP* current = 0;

    size_t nbRemoved = 0;

#if 1
    /** We loop each diagonal. */
    for (size_t i=0; i<_diagonalNumber; i++)
    {
        current = _HspList[i];
        prev    = current;

        if (current != 0)
        {
            for (current = current->next;  current != 0;  )
            {
                if (current->hsp.score == HSP::BAD_SCORE)
                {
                    prev->next = current->next;
                    DefaultFactory::memory().free (current);
                    current = prev->next;
                    nbRemoved++;
                }
                else
                {
                    prev = current;
                    current = current->next;
                }
            }
        }
    }
#endif

    DEBUG (("HspContainer::clean  nbRemoved=%ld \n", nbRemoved));
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
