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

/** \file IHspContainer.hpp
 *  \brief Interface for some HSP container.
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  This file contains material for mainly PlastN algorithm.
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

/** \brief Interface of what an HSP container can be
 *
 * This interface is very similar to the IAlignmentContainer interface. As a matter of
 * fact, IHspContainer has been used for implementing the PlastN algorithm. Since the
 * amino acids algorithms were already working (and using IAlignmentContainer), precaution
 * has been taken to not disturb this working algorithms and interface duplication has
 * been preferred.
 *
 * In the future, it would be nice to merge IHspContainer and IAlignmentContainer interfaces.
 *
 * \see IAlignmentContainer
 */
class IHspContainer : public dp::SmartPointer
{
public:

    /** Define an HSP with several information. */
    struct HSP
    {
        /** start position of alignment in the query database. */
        u_int64_t  q_start;

        /** stop position of alignment in the query database. */
        u_int64_t  q_stop;

        /** start position of alignment in the bank */
        u_int64_t  s_start;

        /** stop position of alignment in the bank */
        u_int64_t  s_stop;

        /** diagonal number of the HSP */
        u_int32_t  diag;

        /** index of the query sequence. */
        u_int32_t  q_idx;

        /** index of the subject sequence. */
        u_int32_t  s_idx;

        /** score of the HSP */
        int32_t    score;

        static const int32_t BAD_SCORE = ~0;

        /** Invalidate the HSP by setting a dummy score. */
        void invalidate ()  { score = BAD_SCORE; }

        /** Overload output stream operator for HSP.
         * \param[in] s : the output stream object
         * \param[in] h : the HSP to be output
         * \return the output stream object
         */
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

    /** Insert an HSP in the container.
     * \param[in] qry : range of absolute offsets [begin,end] of the HSP in the query database
     * \param[in] sbj : range of absolute offsets [begin,end] of the HSP in the query database
     * \param[in] qryId : index of the query sequence
     * \param[in] seqId : index of the subject sequence
     * \param[in] score : score of the HSP to be inserted.
     * \return true if insertion is ok, false otherwise (like already existing HSP)
     */
    virtual bool insert (
        const misc::Range64& qry,
        const misc::Range64& sbj,
        u_int32_t qryId,
        u_int32_t seqId,
        int32_t score
    ) =  0;

    /** Insert an HSP in the container.
     * \param[in] q_start : absolute beginning offset of the HSP in the query database
     * \param[in] q_stop  : absolute ending  offset of the HSP in the query database
     * \param[in] s_start : absolute beginning offset of the HSP in the subject database
     * \param[in] s_stop  : absolute ending  offset of the HSP in the subject database
     * \param[in] qryId : index of the query sequence
     * \param[in] seqId : index of the subject sequence
     * \param[in] score : score of the HSP to be inserted.
     * \return true if insertion is ok, false otherwise (like already existing HSP)
     */
    virtual bool insert (
        u_int64_t q_start,
        u_int64_t q_stop,
        u_int64_t s_start,
        u_int64_t s_stop,
        u_int32_t qryId,
        u_int32_t seqId,
        int32_t score
    ) =  0;

    /** Merge several containers in the current one.
     * \param[in] v : the vector of HSP containers to be merged.
     */
	virtual void merge (std::vector<IHspContainer*> v) = 0;

    /** Insert an HSP into the container.
     * \param[in] hsp : the HSP to be inserted
     * \return true if insertion is ok, false otherwise (like already existing HSP)
     */
    virtual bool insert (HSP* hsp) = 0;

    /** Returns the size of the query database.
     * \return the database size */
    virtual u_int32_t getDbSize () = 0;

    /** Tells whether the provided HSP already exists or not in this container.
     * \param[in] q_start : absolute beginning offset of the HSP in the query database
     * \param[in] s_start : absolute beginning offset of the HSP in the subject database
     * \param[in] delta   : value for discriminating the diagonal
     */
    virtual bool doesExist (u_int64_t q_start, u_int64_t s_start, u_int32_t delta) = 0;

    /** Returns one HSP (kind of iterator)
     * \param[out] nbRetrieved : number of HSP so far iterated
     * \return the current iterated HSP.
     */
    virtual HSP* retrieve (size_t& nbRetrieved) = 0;

    /** Reset the iteration of the container. */
    virtual void resetRetrieve () = 0;

    /** Return the total number of HSP in this container.
     * \return the number of HSP.
     */
    virtual u_int64_t getItemsNumber () = 0;
};

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _IHSP_CONTAINER_HPP_ */
