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
    this->add (new OptionOneParam (STR_OPTION_ALGO_TYPE,                STR_HELP_ALGO_TYPE));
    this->add (new OptionOneParam (STR_OPTION_SUBJECT_URI,              STR_HELP_SUBJECT_URI));
    this->add (new OptionOneParam (STR_OPTION_QUERY_URI,                STR_HELP_QUERY_URI));
    this->add (new OptionOneParam (STR_OPTION_OUTPUT_FILE,              STR_HELP_OUTPUT_FILE));
    this->add (new OptionOneParam (STR_OPTION_EVALUE,                   STR_HELP_EVALUE));
    this->add (new OptionOneParam (STR_OPTION_UNGAP_NEIGHBOUR_LENGTH,   STR_HELP_UNGAP_NEIGHBOUR_LENGTH));
    this->add (new OptionOneParam (STR_OPTION_UNGAP_SCORE_THRESHOLD,    STR_HELP_UNGAP_SCORE_THRESHOLD));
    //this->add (new OptionOneParam ("-z",                              "Effective length of the database (use zero for the real size)"));
    this->add (new OptionOneParam (STR_OPTION_SMALLGAP_THRESHOLD,       STR_HELP_SMALLGAP_THRESHOLD));
    this->add (new OptionOneParam (STR_OPTION_SMALLGAP_BAND_WITH,       STR_HELP_SMALLGAP_BAND_WITH));
    this->add (new OptionOneParam (STR_OPTION_NB_PROCESSORS,            STR_HELP_NB_PROCESSORS));
    this->add (new OptionOneParam (STR_OPTION_OPEN_GAP_COST,            STR_HELP_OPEN_GAP_COST));
    this->add (new OptionOneParam (STR_OPTION_EXTEND_GAP_COST,          STR_HELP_EXTEND_GAP_COST));
    this->add (new OptionOneParam (STR_OPTION_X_DROPOFF_UNGAPPED,       STR_HELP_X_DROPOFF_UNGAPPED));
    this->add (new OptionOneParam (STR_OPTION_X_DROPOFF_GAPPED,         STR_HELP_X_DROPOFF_GAPPED));
    this->add (new OptionOneParam (STR_OPTION_X_DROPOFF_FINAL,          STR_HELP_X_DROPOFF_FINAL));
    this->add (new OptionOneParam (STR_OPTION_INDEX_NEIGHBOUR_THRESHOLD,  STR_HELP_INDEX_NEIGHBOUR_THRESHOLD));
    this->add (new OptionOneParam (STR_OPTION_FILTER_QUERY,             STR_HELP_FILTER_QUERY));
    this->add (new OptionOneParam (STR_OPTION_SCORE_MATRIX,             STR_HELP_SCORE_MATRIX));
    this->add (new OptionOneParam (STR_OPTION_STRAND,                   STR_HELP_STRAND));
    this->add (new OptionOneParam (STR_OPTION_REWARD,                   STR_HELP_REWARD));
    this->add (new OptionOneParam (STR_OPTION_PENALTY,                  STR_HELP_PENALTY));

    this->add (new OptionOneParam (STR_OPTION_FORCE_QUERY_ORDERING,     STR_HELP_FORCE_QUERY_ORDERING));

    //this->add (new OptionOneParam ("-S", "Subset seed"));
    //this->add (new OptionOneParam ("-R", "Query strands to search against database (for plastx and tplastx)"));
    //this->add (new OptionOneParam ("-C", "Use composition-based score adjustments as in Bioinformatics 21:902-911 for plastp or tplastn [T/F]"));

    this->add (new OptionOneParam (STR_OPTION_MAX_DATABASE_SIZE,        STR_HELP_MAX_DATABASE_SIZE));
    this->add (new OptionOneParam (STR_OPTION_MAX_HIT_PER_QUERY,        STR_HELP_MAX_HIT_PER_QUERY));
    this->add (new OptionOneParam (STR_OPTION_MAX_HSP_PER_HIT,          STR_HELP_MAX_HSP_PER_HIT));
    //this->add (new OptionOneParam (STR_OPTION_MAX_HIT_PER_ITERATION,    STR_HELP_MAX_HIT_PER_ITERATION));

    this->add (new OptionOneParam (STR_OPTION_OUTPUT_FORMAT,            STR_HELP_OUTPUT_FORMAT));

    this->add (new OptionOneParam (STR_OPTION_STRANDS_LIST,             STR_HELP_STRANDS_LIST));

    this->add (new OptionOneParam (STR_OPTION_CODON_STOP_OPTIM,         STR_HELP_CODON_STOP_OPTIM));

    /** Factories. */
    this->add (new OptionOneParam (STR_OPTION_FACTORY_DISPATCHER,       STR_HELP_FACTORY_DISPATCHER));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_STATISTICS,       STR_HELP_FACTORY_STATISTICS));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_INDEXATION,       STR_HELP_FACTORY_INDEXATION));
    //this->add (new OptionOneParam ("-factory-hit-seeds",              "Factory that creates seeds hits iterator."));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_HIT_UNGAP,        STR_HELP_FACTORY_HIT_UNGAP));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_HIT_SMALLGAP,     STR_HELP_FACTORY_HIT_SMALLGAP));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_HIT_FULLGAP,      STR_HELP_FACTORY_HIT_FULLGAP));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_HIT_COMPOSITION,  STR_HELP_FACTORY_HIT_COMPOSITION));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_GAP_RESULT,       STR_HELP_FACTORY_GAP_RESULT));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_UNGAP_RESULT,     STR_HELP_FACTORY_UNGAP_RESULT));
    this->add (new OptionOneParam (STR_OPTION_FACTORY_SPLITTER,         STR_HELP_FACTORY_SPLITTER));

    this->add (new OptionOneParam (STR_OPTION_OPTIM_FILTER_UNGAP,       STR_HELP_OPTIM_FILTER_UNGAP));

    this->add (new OptionNoParam  (STR_OPTION_INFO_BARGRAPH,            STR_HELP_INFO_BARGRAPH));
    this->add (new OptionOneParam (STR_OPTION_INFO_BARGRAPH_SIZE,       STR_HELP_INFO_BARGRAPH_SIZE));
    this->add (new OptionOneParam (STR_OPTION_INFO_PROGRESSION,         STR_HELP_INFO_PROGRESSION));
    this->add (new OptionNoParam  (STR_OPTION_INFO_VERBOSE,             STR_HELP_INFO_VERBOSE));
    this->add (new OptionOneParam (STR_OPTION_INFO_FULL_STATS,          STR_HELP_INFO_FULL_STATS));
    this->add (new OptionOneParam (STR_OPTION_INFO_STATS,               STR_HELP_INFO_STATS));
    this->add (new OptionOneParam (STR_OPTION_INFO_STATS_FORMAT,        STR_HELP_INFO_STATS_FORMAT));
    this->add (new OptionNoParam  (STR_OPTION_INFO_STATS_AUTO,          STR_HELP_INFO_STATS_AUTO));
    this->add (new OptionOneParam (STR_OPTION_INFO_ALIGNMENT_PROGRESS,  STR_HELP_INFO_ALIGNMENT_PROGRESS));
    this->add (new OptionOneParam (STR_OPTION_INFO_RESOURCES_PROGRESS,  STR_HELP_INFO_RESOURCES_PROGRESS));

    this->add (new OptionOneParam (STR_OPTION_INFO_CONFIG_FILE,         STR_HELP_INFO_CONFIG_FILE));

    this->add (new OptionOneParam (STR_OPTION_XML_FILTER_FILE,          STR_HELP_XML_FILTER_FILE));

    this->add (new OptionOneParam (STR_OPTION_SEEDS_USE_RATIO,          STR_HELP_SEEDS_USE_RATIO));
    this->add (new OptionOneParam (STR_OPTION_INDEX_FILTER_SEED,        STR_HELP_INDEX_FILTER_SEED));

    this->add (new OptionOneParam (STR_OPTION_COMPLETE_SUBJECT_DB_STATS_FILE,       STR_HELP_COMPLETE_SUBJECT_DB_STATS_FILE));

    this->add (new OptionOneParam (STR_OPTION_WORD_SIZE,                			STR_HELP_WORD_SIZE));
    this->add (new OptionOneParam (STR_OPTION_KMERS_BITSET_PATH, STR_HELP_KMERS_BITSET_PATH));

    this->add (new OptionNoParam  (STR_OPTION_HELP, STR_HELP_HELP));
}

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/

