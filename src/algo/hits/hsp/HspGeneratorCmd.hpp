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

/** \file HspGeneratorCmd.hpp
 *  \brief Interface for some HSP generator.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _HSP_GENERATOR_CMD_HPP_
#define _HSP_GENERATOR_CMD_HPP_

/********************************************************************************/

#include <algo/core/api/IAlgoIndexator.hpp>

#include <alignment/core/api/IHspContainer.hpp>

#include <os/api/IThread.hpp>

#include <designpattern/impl/RangeIterator.hpp>
#include <designpattern/impl/Observer.hpp>

#include <stdio.h>

/********************************************************************************/
namespace algo {
namespace hits {
namespace hsp  {
/********************************************************************************/

class HSPGenerator : public dp::ICommand, public dp::impl::Subject
{
public:

    /** */
    HSPGenerator (
        algo::core::IIndexator* 			 indexator,
        alignment::core::IHspContainer* 	 hspContainer,
        dp::impl::RangeIterator<u_int32_t>&  rangeIterator,
        int32_t threshold,
        int32_t match,
        int32_t mismatch,
        int32_t xdrop,
        dp::IObserver* observer
    );

    /** */
    virtual ~HSPGenerator ();

    /** */
    void execute ();

private:

    algo::core::IIndexator* _indexator;
    void setIndexator (algo::core::IIndexator* indexator)  { SP_SETATTR(indexator); }

    alignment::core::IHspContainer* _hspContainer;
    void setHspContainer (alignment::core::IHspContainer* hspContainer)  { SP_SETATTR(hspContainer); }

    dp::impl::RangeIterator<u_int32_t>& _rangeIterator;

    int32_t _threshold;
    int32_t _match;
    int32_t _mismatch;
    int32_t _xdrop;

    size_t _span;
    size_t _extraSpan;

    database::LETTER _badLetter;

    /********************************************************************************/
    struct Entry
    {
        void set (const database::LETTER* aData, u_int64_t aOffsetInDb, u_int32_t aRightLen,  u_int32_t aLeftLen, u_int32_t aSeqId)
        {
            data       = aData;
            offsetInDb = aOffsetInDb;
            rightLen   = aRightLen;
            leftLen    = aLeftLen;
            seqId      = aSeqId;
        }

        const database::LETTER* data;
        u_int64_t               offsetInDb;
        u_int32_t               rightLen;
        u_int32_t               leftLen;
        u_int32_t               seqId;
    };

    /** */
    int computeExtensionRight (int code, const database::LETTER* s1, const database::LETTER* s2, size_t& length, size_t span, bool& alreadySeen);
    int computeExtensionLeft  (int code, const database::LETTER* s1, const database::LETTER* s2, size_t& length, size_t span, bool& alreadySeen);

    void dump (seed::SeedHashCode code, u_int32_t qryIdx, u_int32_t sbjIdx, u_int32_t off1, u_int32_t off2);

    static char CONVERT (database::LETTER i);
};

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _HSP_GENERATOR_CMD_HPP_ */
