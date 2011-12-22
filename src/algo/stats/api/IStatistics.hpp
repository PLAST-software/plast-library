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

/** \file IStatistics.hpp
 *  \brief Interfaces for statistics management
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef ISTATISTICS_HPP_
#define ISTATISTICS_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <database/api/ISequence.hpp>

/********************************************************************************/
/** \brief Statistics concepts */
namespace statistics  {
/********************************************************************************/

/** \brief Interface that provides global statistical information.
 */
class IGlobalParameters : public dp::SmartPointer
{
public:

    /** */
    double evalue;

    /** */
    double K;

    /** */
    double H;

    /** */
    double logK;

    /** */
    double ln2;

    /** */
    double alpha;

    /** */
    double lambda;

    /** */
    double beta;
};

/********************************************************************************/

/** \brief Exception class for bad statistical parameters
 */
class GlobalParametersFailure
{
public:

    /** Constructor with some informative string.
     * \param[in] message : a description of the exception.
     */
    GlobalParametersFailure (const char* message) : _message(message)  {}

    /** Returns the description message.
     * \return the message
     */
    const char* getMessage ()  { return _message.c_str(); }

private:

    /** The informative message. */
    std::string _message;
};

/********************************************************************************/

/** \brief Interface that provides statistical information for query sequences.
 */
class IQueryInformation : public dp::SmartPointer
{
public:

    /** \brief Statistics for a sequence
     */
    struct SequenceInfo
    {
        int         sequence_length;
        int         length_adjust;
        int         cut_offs;
        long long   eff_searchsp;
    };

    /** Return some statistical information for a given sequence.
     * \param[in] seq : the sequence we want statistical information for
     * \return the wanted statistical information
     */
    virtual SequenceInfo& getSeqInfo (const database::ISequence& seq) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* ISTATISTICS_HPP_ */
