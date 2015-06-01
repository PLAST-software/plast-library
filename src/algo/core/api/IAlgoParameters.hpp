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
#include <misc/api/CompleteSubjectDatabaseStats.hpp>
#include <misc/api/PlastStrings.hpp>

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
    int  filterQuery;

    /** Value for the threshold size of a sequence for launching the filtering algorithm. */
    int  filterQueryThreshold;

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
    double   evalue;

    /** Ungapped X drop off. */
    int XdroppofUnGap;

    /** X drop off. */
    int XdroppofGap;

    /** Final X drop off. */
    int finalXdroppofGap;

    /** Index neighbor threshold for nucleotid indexation optimization. */
    int index_neighbor_threshold;

    /** URI (filepath) of the file where the algorithm results (alignments list). */
    std::string outputfile;

    /** List of strands to be used. */
    std::vector<misc::ReadingFrame_e> strands;

    /** */
    size_t nbHitPerQuery;
    size_t nbAlignPerHit;

    /** Reward score (used by plastn). */
    int reward;

    /** Penalty score (used by plastn). */
    int penalty;

    /** Strand to be used by plastn: O for both, 1 for plus, -1 for minus. */
    int strand;

    /** Stats of the complete subject database */
    misc::CompleteSubjectDatabaseStats completeSubjectDatabaseStats;

    /** Path to the bitset file for the present kmers */
    std::string kmersBitsetPath;

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

        result->add (0, STR_PARAM_params);

        result->add (1, STR_PARAM_seedModelKind, "%d", seedModelKind);
        result->add (1, STR_PARAM_seedSpan,      "%d", seedSpan);

        result->add (1, STR_PARAM_subseedStrings, "%d", subseedStrings.size());
        for (size_t i=0; i<subseedStrings.size(); i++)
        {
            result->add (2, "str", subseedStrings[i]);
        }

        result->add (1, STR_PARAM_matrixKind, "%d", matrixKind);

        result->add (1, STR_PARAM_subject, "");
        result->add (2, STR_PARAM_uri,   subjectUri);
        result->add (3, STR_PARAM_range, "");
        result->add (4, STR_PARAM_begin, "%ld", subjectRange.begin);
        result->add (4, STR_PARAM_end,   "%ld", subjectRange.end);

        result->add (1, STR_PARAM_subject, "");
        result->add (2, STR_PARAM_uri,   queryUri);
        result->add (3, STR_PARAM_range, "");
        result->add (4, STR_PARAM_begin, "%ld", queryRange.begin);
        result->add (4, STR_PARAM_end,   "%ld", queryRange.end);

        result->add (1, STR_PARAM_filterQuery, "%d", filterQuery);

        result->add (1, STR_PARAM_ungapNeighbourLength, "%d", ungapNeighbourLength);
        result->add (1, STR_PARAM_ungapScoreThreshold,  "%d", ungapScoreThreshold);
        result->add (1, STR_PARAM_smallGapBandLength,   "%d", smallGapBandLength);
        result->add (1, STR_PARAM_smallGapBandWidth,    "%d", smallGapBandWidth);
        result->add (1, STR_PARAM_smallGapThreshold,    "%d", smallGapThreshold);
        result->add (1, STR_PARAM_openGapCost,          "%d", openGapCost);
        result->add (1, STR_PARAM_extendGapCost,        "%d", extendGapCost);
        result->add (1, STR_PARAM_evalue,               "%g", evalue);
        result->add (1, STR_PARAM_XdroppofUngap,        "%d", XdroppofUnGap);
        result->add (1, STR_PARAM_XdroppofGap,          "%d", XdroppofGap);
        result->add (1, STR_PARAM_finalXdroppofGap,     "%d", finalXdroppofGap);
        result->add (1, STR_PARAM_strands, "%d", strands.size());

        result->add (1, STR_PARAM_outputfile, outputfile);

        result->add (1, STR_PARAM_strands, "%d", strands.size());
        result->add (1, STR_PARAM_kmersBitsetPath, kmersBitsetPath);

        return result;
    }
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_PARAMETERS_HPP_ */
