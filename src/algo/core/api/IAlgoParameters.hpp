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

/** \file IAlgoParameters.hpp
 *  \brief Interface holding information for parameterize PLAST algorithm.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALGO_PARAMETERS_HPP_
#define _IALGO_PARAMETERS_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/impl/Property.hpp>

#include <misc/api/types.hpp>

#include <string>
#include <vector>
#include <stdio.h>

/********************************************************************************/
/** \brief PLAST algorithm concepts. */
namespace algo {
/** \brief Concept for configuring and running PLAST. */
namespace core {
/********************************************************************************/

/********************************************************************************/

/** \brief Define several piece of information used for parameterizing the algorithm.
 *
 * The IParameters class groups many information used for the parametrization of the
 * PLAST algorithm. For instance, it holds the URI of the two databases to be
 * compared.
 *
 * Actually, an instance of this class is created through the IConfiguration::createDefaultParameters
 * method, which provides for a given algorithm type (plastp, plastx...) the default values
 * for the IParameters attributes.
 *
 * Then, it is possible to modify some attributes of the instance according to some source
 * of information (command line options set by the user, configuration file filled by the
 * user).
 *
 * Once it is configured, the IParameters instance is provided to several parts of the algorithm,
 * likely as argument of constructors.
 */
class IParameters : public dp::SmartPointer
{
public:

    /** Type of algorithm to be used. */
    misc::AlgoKind_e         algoKind;

    /** Kind of seed model to be used. Likely to be ENUM_SubSeedModel for PLAST algorithm. */
    misc::SeedModelKind_e    seedModelKind;

    /** Number of characters to be used for each seed. */
    size_t                    seedSpan;

    /** List of strings defining a subseed model. */
    std::vector<std::string>  subseedStrings;

    /** Kind of score matrix to be used. Likely to be ENUM_BLOSUM62 for PLAST algorithm. */
    misc::ScoreMatrixKind_e  matrixKind;

    /** URI (filepath) of the subject database. */
    std::string   subjectUri;

    /** [begin,end] offsets  as range to be read in the subject database file. */
    misc::Range64 subjectRange;

    /** URI (filepath) of the query database. */
    std::string   queryUri;

    /** [begin,end] offsets  as range to be read in the query database file. */
    misc::Range64 queryRange;

    /** Tells whether low information regions have to be filtered out from the query database. */
    bool    filterQuery;

    /** Size of the neighbourhoods during ungap part of the algorithm. */
    int     ungapNeighbourLength;

    /** Threshold score to be used for score computation during the ungap part of the algorithm. */
    int     ungapScoreThreshold;

    /** Length of the band to be explored in the small gap algorithm. */
    int     smallGapBandLength;

    /** Length of the wirth to be explored in the small gap algorithm. */
    int     smallGapBandWidth;

    /** Threshold score to be used for score computation during the small gap part of the algorithm. */
    int     smallGapThreshold;

    /** Cost for opening a gap. */
    int     openGapCost;

    /** Cost for extending a gap. */
    int     extendGapCost;

    /** Expected value to be used. */
    float   evalue;

    /** X drop off. */
    int XdroppofGap;

    /** Final X drop off. */
    int finalXdroppofGap;

    /** URI (filepath) of the file where the algorithm results (alignments list). */
    std::string outputfile;

    /** List of strands to be used. */
    std::vector<misc::ReadingFrame_e> strands;

    /** Clone the instance.
     * \return the cloned instance. */
    IParameters* clone ()
    {
        return new IParameters (*this);
    }

    /** Returns IProperties object. */
    dp::IProperties* getProperties ()
    {
        dp::IProperties* result = new dp::impl::Properties ();

        result->add (0, "params");

        result->add (1, "seedModelKind", "%d", seedModelKind);
        result->add (1, "seedSpan",      "%d", seedSpan);

        result->add (1, "subseedStrings", "%d", subseedStrings.size());
        for (size_t i=0; i<subseedStrings.size(); i++)
        {
            result->add (2, "str", subseedStrings[i]);
        }

        result->add (1, "matrixKind", "%d", matrixKind);

        result->add (1, "subject", "");
        result->add (2, "uri",   subjectUri);
        result->add (3, "range", "");
        result->add (4, "begin", "%ld", subjectRange.begin);
        result->add (4, "end",   "%ld", subjectRange.end);

        result->add (1, "subject", "");
        result->add (2, "uri",   queryUri);
        result->add (3, "range", "");
        result->add (4, "begin", "%ld", queryRange.begin);
        result->add (4, "end",   "%ld", queryRange.end);

        result->add (1, "filterQuery", "%d", filterQuery);

        result->add (1, "ungapNeighbourLength", "%d", ungapNeighbourLength);
        result->add (1, "ungapScoreThreshold",  "%d", ungapScoreThreshold);
        result->add (1, "smallGapBandLength",   "%d", smallGapBandLength);
        result->add (1, "smallGapBandWidth",    "%d", smallGapBandWidth);
        result->add (1, "smallGapThreshold",    "%d", smallGapThreshold);
        result->add (1, "openGapCost",          "%d", openGapCost);
        result->add (1, "extendGapCost",        "%d", extendGapCost);
        result->add (1, "evalue",               "%g", evalue);
        result->add (1, "XdroppofGap",          "%d", XdroppofGap);
        result->add (1, "finalXdroppofGap",     "%d", finalXdroppofGap);

        result->add (1, "outputfile", outputfile);

        result->add (1, "strands", "%d", strands.size());

        return result;
    }
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_PARAMETERS_HPP_ */
