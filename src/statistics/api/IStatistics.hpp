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

#ifndef ISTATISTICS_HPP_
#define ISTATISTICS_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"
#include <stddef.h>

/********************************************************************************/
namespace statistics  {
/********************************************************************************/

class IGlobalParameters : public dp::SmartPointer
{
public:

    double evalue;

    int    frm_sub;
    int    frm_qry;

    double K;
    double H;
    double logK;
    double ln2;
    double alpha;
    double lambda;
    double beta;
};

/********************************************************************************/

class IQueryInformation : public dp::SmartPointer
{
public:

    /** */
    struct SequenceInfo
    {
        int         sequence_length;
        int         length_adjust;
        int         cut_offs;
        long long   eff_searchsp;
    };

    virtual size_t        getNbSequences() = 0;
    virtual SequenceInfo& getSeqInfoByIndex (size_t i) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* ISTATISTICS_HPP_ */
