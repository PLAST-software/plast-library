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

/** \file IHspContainer.hpp
 *  \brief Interface for some HSP container.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IHSP_CONTAINER_HPP_
#define _IHSP_CONTAINER_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <misc/api/types.hpp>

#include <vector>
#include <iostream>

/********************************************************************************/
namespace alignment {
namespace core      {
/********************************************************************************/

/**
 */
class IHspContainer : public dp::SmartPointer
{
public:

    /** */
    struct HSP
    {
        u_int64_t  q_start;      // start position of alignment in the query
        u_int64_t  q_stop;       // stop position of alignment in the query
        u_int64_t  s_start;      // start position of alignment in the bank
        u_int64_t  s_stop;       // stop position of alignment in the bank
        u_int32_t  diag;         // no de diag max ou est situe  l'alignement
        u_int32_t  q_idx;
        u_int32_t  s_idx;
        int32_t    score;

        static const int32_t BAD_SCORE = ~0;

        /** */
        void invalidate ()  { score = BAD_SCORE; }

        /** */
        friend std::ostream& operator<< (std::ostream& s, const HSP& h)
        {
            return s << "[HSP"
                 << "  q_start=" << h.q_start
                 << "  q_stop="  << h.q_stop
                 << "  q_idx="   << h.q_idx
                 << "  s_start=" << h.s_start
                 << "  s_stop="  << h.s_stop
                 << "  s_idx="   << h.s_idx
                 << "  score="   << h.score
                 << "  diag="    << h.diag
                 << "]";
        }

    };

    /** */
    virtual bool insert (
        const misc::Range64& qry,
        const misc::Range64& sbj,
        u_int32_t qryId,
        u_int32_t seqId,
        int32_t score
    ) =  0;

    /** */
    virtual bool insert (
        u_int64_t q_start,
        u_int64_t q_stop,
        u_int64_t s_start,
        u_int64_t s_stop,
        u_int32_t qryId,
        u_int32_t seqId,
        int32_t score
    ) =  0;

	virtual void merge (std::vector<IHspContainer*> v) = 0;

    /** */
    virtual bool insert (HSP* hsp) = 0;

    /** */
    virtual u_int32_t getDbSize () = 0;

    /** */
    virtual bool doesExist (u_int64_t q_start, u_int64_t s_start, u_int32_t delta) = 0;

    /** */
    virtual HSP* retrieve (size_t& nbRetrieved) = 0;

    /** */
    virtual void resetRetrieve () = 0;

    /** */
    virtual u_int64_t getItemsNumber () = 0;
};

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _IHSP_CONTAINER_HPP_ */
