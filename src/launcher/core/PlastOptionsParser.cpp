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

#include <launcher/core/PlastOptionsParser.hpp>
#include <misc/api/PlastStrings.hpp>

using namespace std;
using namespace misc;
using namespace misc::impl;

#define DEBUG(a)  //printf a

/********************************************************************************/
namespace launcher {
namespace core     {
/********************************************************************************/

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
PlastOptionsParser::PlastOptionsParser ()
{
    build ();
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void PlastOptionsParser::build ()
{
    this->add (new OptionOneParam (STR_OPTION_ALGO_TYPE,                "Program Name [plastp, tplastn, plastx or tplastx]"));
    this->add (new OptionOneParam (STR_OPTION_SUBJECT_URI,              "Subject database file"));
    this->add (new OptionOneParam (STR_OPTION_QUERY_URI,                "Query database file"));
    this->add (new OptionOneParam (STR_OPTION_OUTPUT_FILE,              "PLAST report Output File"));
    this->add (new OptionOneParam (STR_OPTION_EVALUE,                   "Expectation value"));
    this->add (new OptionOneParam (STR_OPTION_UNGAP_NEIGHBOUR_LENGTH,   "Size of neighbourhood peforming ungapped extension"));
    this->add (new OptionOneParam (STR_OPTION_UNGAP_SCORE_THRESHOLD,    "Ungapped threshold trigger a small gapped extension"));
    //this->add (new OptionOneParam ("-z",                              "Effective length of the database (use zero for the real size)"));
    this->add (new OptionOneParam (STR_OPTION_SMALLGAP_THRESHOLD,       "threshold for small gapped extension"));
    this->add (new OptionOneParam (STR_OPTION_SMALLGAP_BAND_WITH,       "bandwith for small gapped extension"));
    this->add (new OptionOneParam (STR_OPTION_NB_PROCESSORS,            "Number of processors to use"));
    this->add (new OptionOneParam (STR_OPTION_OPEN_GAP_COST,            "Cost to open a gap"));
    this->add (new OptionOneParam (STR_OPTION_EXTEND_GAP_COST,          "Cost to extend a gap"));
    this->add (new OptionOneParam (STR_OPTION_X_DROPOFF_GAPPED,         "X dropoff value for gapped alignment (in bits) (zero invokes default behavior)"));
    this->add (new OptionOneParam (STR_OPTION_X_DROPOFF_FINAL,          "X dropoff value for final gapped alignment in bits (0.0 invokes default behavior)"));
    this->add (new OptionOneParam (STR_OPTION_FILTER_QUERY,             "Filter query sequence"));
    this->add (new OptionOneParam (STR_OPTION_SCORE_MATRIX,             "Score matrix (BLOSUM62 or BLOSUM50)"));

    //this->add (new OptionOneParam ("-S", "Subset seed"));
    //this->add (new OptionOneParam ("-R", "Query strands to search against database (for plastx and tplastx)"));
    //this->add (new OptionOneParam ("-C", "Use composition-based score adjustments as in Bioinformatics 21:902-911 for plastp or tplastn [T/F]"));

    this->add (new OptionOneParam (STR_OPTION_MAX_DATABASE_SIZE,        "Maximum allowed size (in bytes) for a database. If greater, database is segmented."));
    this->add (new OptionOneParam (STR_OPTION_MAX_HSP_PER_HIT,          "Maximum hits per query. 0 value will dump all hits (default)"));
    this->add (new OptionOneParam (STR_OPTION_MAX_HIT_PER_ITERATION,    "Maximum hits per iteration (for memory usage control). 1000000 by default"));

    this->add (new OptionOneParam (STR_OPTION_OUTPUT_FORMAT,            "Output format: 1 for tabulated (default)."));

    this->add (new OptionOneParam (STR_OPTION_STRANDS_LIST,             "List of the strands (ex: \"1,2,6\") to be used when using algo using nucleotids databases."));

    this->add (new OptionOneParam (STR_OPTION_CODON_STOP_OPTIM,          "size of the allowed range between the last invalid character and the next stop codon"));

    /** Factories. */
    this->add (new OptionOneParam (STR_OPTION_FACTORY_DISPATCHER,       "Factory that creates dispatcher."));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_INDEXATION,       "Factory that creates indexation builder."));
    //this->add (new OptionOneParam ("-factory-hit-seeds",              "Factory that creates seeds hits iterator."));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_HIT_UNGAP,        "Factory that creates ungap hits iterator."));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_HIT_SMALLGAP,     "Factory that creates small gap hits iterator."));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_HIT_FULLGAP,      "Factory that creates full gap hits iterator."));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_HIT_COMPOSITION,  "Factory that creates composition hits iterator."));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_GAP_RESULT,       "Factory that creates gap alignments result."));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_UNGAP_RESULT,     "Factory that creates ungap alignments result."));

    this->add (new OptionOneParam (STR_OPTION_OPTIM_FILTER_UNGAP,       "Optimization that filters out through ungap alignments."));

    this->add (new OptionNoParam  (STR_OPTION_INFO_BARGRAPH,            "Display a progress bar during execution."));
    this->add (new OptionOneParam (STR_OPTION_INFO_BARGRAPH_SIZE,       "Nb of characters of the bargraph."));
    this->add (new OptionOneParam (STR_OPTION_INFO_PROGRESSION,         "Dump in a file the current execution percentage."));
    this->add (new OptionNoParam  (STR_OPTION_INFO_VERBOSE,             "Display information during algorithm execution."));
    this->add (new OptionOneParam (STR_OPTION_INFO_FULL_STATS,          "Dump algorithm statistics."));
    this->add (new OptionOneParam (STR_OPTION_INFO_STATS,               "Dump generic statistics."));
    this->add (new OptionOneParam (STR_OPTION_INFO_STATS_FORMAT,        "Format of statistics: 'raw' (default) or 'xml'"));
    this->add (new OptionNoParam  (STR_OPTION_INFO_STATS_AUTO,          "Automatic stats file creation"));
    this->add (new OptionOneParam (STR_OPTION_INFO_ALIGNMENT_PROGRESS,  "Dump in a file the growing number of ungap/ungap alignments during algorithm."));
    this->add (new OptionOneParam (STR_OPTION_INFO_RESOURCES_PROGRESS,  "Dump in a file information about resources during algorithm."));

    this->add (new OptionOneParam (STR_OPTION_INFO_CONFIG_FILE,         "Pathname of the plast config file."));

    this->add (new OptionOneParam (STR_OPTION_XML_FILTER_FILE,          "Uri of a XML filter file."));

    this->add (new OptionNoParam  (STR_OPTION_HELP, "help"));
}

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/

