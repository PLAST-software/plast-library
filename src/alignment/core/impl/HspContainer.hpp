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

/** \file HspContainer.hpp
 *  \brief Interface for some HSP container.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _HSP_CONTAINER_HPP_
#define _HSP_CONTAINER_HPP_

/********************************************************************************/

#include <alignment/core/api/IHspContainer.hpp>
#include <os/api/IThread.hpp>

#include <vector>
#include <stdio.h>

/********************************************************************************/
namespace alignment {
namespace core      {
namespace impl      {
/********************************************************************************/

/**
 */
class HspContainer : public IHspContainer
{
public:

    /** Constructor. */
    HspContainer (size_t dbQuerySize);

    /** Destructor. */
    virtual ~HspContainer ();

    /** */
    bool insert (
        u_int64_t q_start,
        u_int64_t q_stop,
        u_int64_t s_start,
        u_int64_t s_stop,
        u_int32_t qryId,
        u_int32_t seqId,
        int32_t score
    );

    /** */
    bool insert (
        const misc::Range64& qry,
        const misc::Range64& sbj,
        u_int32_t qryId,
        u_int32_t seqId,
        int32_t score
    )   {  return insert (qry.begin, qry.end, sbj.begin, sbj.end, qryId, seqId, score);  }

    /** */
    bool insert (HSP* hsp)
    {  return insert (hsp->q_start, hsp->q_stop, hsp->s_start, hsp->s_stop, hsp->q_idx, hsp->s_idx, hsp->score);  }

    /** */
    bool doesExist (u_int64_t q_start, u_int64_t s_start, u_int32_t delta);

    /** */
    HSP* retrieve (size_t& nbRetrieved);

    /** */
    void resetRetrieve ()  { _firstRetrieve = true; }

    /** */
    u_int64_t getItemsNumber ();

    /** */
    u_int32_t getDbSize ()  { return _dbSize; }

    /** */
	void merge (std::vector<IHspContainer*> v);

private:

    /** */
    u_int32_t _diagonalNumber;

    /** */
    u_int32_t _diagonalDivisor;

    /** */
    u_int32_t  _diagonalMask;

    // information about ungapped alignment list
    struct LISTGAP
    {
        HSP             hsp;
        struct LISTGAP* next;         // pointer to a structure identical (to manage the list)
    };

    LISTGAP** _HspList;
    size_t    _HspListSize;

    /** */
    LISTGAP** getItem (u_int64_t q_start, u_int64_t s_start, u_int32_t& d, u_int32_t delta)
    {
#if 1
        d =  (q_start - s_start) & _diagonalMask;
//
//        u_int64_t tmp = (q_start + delta*0 - s_start);
//        d =   tmp % _diagonalNumber;
#else
        if (q_start == s_start)  {  d = ((q_start - s_start) & _diagonalMask)  + (delta % _diagonalNumber);  }
        else                     {  d =  (q_start - s_start) & _diagonalMask;  }
#endif

//        printf ("getItem: Q[%ld]  S[%ld]  delta=%d  d=%ld  %ld\n",
//            q_start, s_start, delta,
//            d, (q_start == s_start ? ((d+delta) % _diagonalNumber) : d)
//        );


        return _HspList + ((d + delta) %  _diagonalNumber);
    }

    /** Synchronizer for preventing for concurrent accesses. */
    os::ISynchronizer* _synchro;

    /** */
    u_int32_t _dbSize;

    /** */
    bool      _firstRetrieve;
    size_t    _nbRetrieved;
    u_int32_t _currentDiagonal;
    LISTGAP*  _currentItem;

    /** */
    void clean ();
};

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _IHSP_CONTAINER_HPP_ */
