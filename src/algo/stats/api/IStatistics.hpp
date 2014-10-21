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
#include <math.h>

/********************************************************************************/
/** \brief Statistics concepts */
namespace statistics  {
/********************************************************************************/

/** \brief Interface that provides global statistical information.
 */
class IGlobalParameters : public dp::SmartPointer
{
public:

	IGlobalParameters ()
	: evalue(0), K(0), H(0), logK(0), ln2(0), alpha(0), lambda(0), beta(0), C(0), Alpha_v(0), Sigma(0),
	  a_un(0), Alpha_un(0), G(0), b(0), Beta(0), Tau(0), db_length(0)
	{}

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

    /** */
    double C;

    /** */
    double Alpha_v;

    /** */
    double Sigma;

    /** */
    double a_un;

    /** */
    double Alpha_un;

    /** */
    double G;

    /** */
    double b;

    /** */
    double Beta;

    /** */
    double Tau;

    /** */
    u_int64_t db_length;

    inline double rawToBitsValue (const double& score)
    {
        return ((lambda * (double)score - logK) / M_LN2);
    }

    inline double bitsToRawValue (const double& score)
    {
        return (((score * M_LN2)+ logK) / lambda);
    }

    /** Returns the Evalue calculated with the score.
     * \param[in] effSearchSp : search space.
     * \param[in] score : score value
     * \param[in] qryLength : query length
     * \param[in] sbjLength : subject length
     * \return E-value
     */
    virtual double scoreToEvalue(double effSearchSp, double score, size_t qryLength, size_t sbjLength) = 0;

    /** Returns the score calculated with the E-value. This function is used to calculate the cutoffs
     * \param[out] cutoff : value of the cutoff.
     * \param[in] effSearchSp : search space.
     * \param[in] evalue : Evalue value
     * \param[in] qryLength : query length
     * \param[in] sbjLength : subject length
     * \return true or false
     */
    virtual bool evalueToCutoff(int&cutoff, double effSearchSp, double evalue, size_t qryLength, size_t sbjLength) = 0;

    virtual bool useCutoff() = 0;
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
