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

#ifndef _STRINGS_HPP_
#define _STRINGS_HPP_

/********************************************************************************/

/** We define here a bunch of string constants that will be shared for many aspects
 *  (command line options, configuration file...)
 */

#define STR_OPTION_ALGO_TYPE                "-p"
#define STR_OPTION_SUBJECT_URI              "-d"
#define STR_OPTION_QUERY_URI                "-i"
#define STR_OPTION_OUTPUT_FILE              "-o"
#define STR_OPTION_EVALUE                   "-e"
#define STR_OPTION_UNGAP_NEIGHBOUR_LENGTH   "-n"
#define STR_OPTION_UNGAP_SCORE_THRESHOLD    "-s"
#define STR_OPTION_SMALLGAP_THRESHOLD       "-g"
#define STR_OPTION_SMALLGAP_BAND_WITH       "-b"
#define STR_OPTION_NB_PROCESSORS            "-a"
#define STR_OPTION_OPEN_GAP_COST            "-G"
#define STR_OPTION_EXTEND_GAP_COST          "-E"
#define STR_OPTION_X_DROPOFF_GAPPED         "-X"
#define STR_OPTION_X_DROPOFF_FINAL          "-Z"
#define STR_OPTION_FILTER_QUERY             "-F"

#define STR_OPTION_OUTPUT_FORMAT            "-outfmt"
#define STR_OPTION_MAX_DATABASE_SIZE        "-max-database-size"
#define STR_OPTION_MAX_HIT_PER_QUERY        "-max-hit-per-query"
#define STR_OPTION_MAX_HIT_PER_ITERATION    "-max-hit-per-iteration"

#define STR_OPTION_STRANDS_LIST             "-strands-list"

#define STR_OPTION_FACTORY_DISPATCHER       "-factory-dispatcher"
#define STR_OPTION_FACTORY_INDEXATION       "-factory-indexation"
#define STR_OPTION_FACTORY_HIT_UNGAP        "-factory-hit-ungap"
#define STR_OPTION_FACTORY_HIT_SMALLGAP     "-factory-hit-smallgap"
#define STR_OPTION_FACTORY_HIT_FULLGAP      "-factory-hit-fullgap"
#define STR_OPTION_FACTORY_HIT_COMPOSITION  "-factory-hit-composition"
#define STR_OPTION_FACTORY_GAP_RESULT       "-factory-gap-result"
#define STR_OPTION_FACTORY_UNGAP_RESULT     "-factory-ungap-result"

#define STR_OPTION_OPTIM_FILTER_UNGAP       "-optim-filter-ungap"

#define STR_OPTION_INFO_BARGRAPH            "-bargraph"
#define STR_OPTION_INFO_BARGRAPH_SIZE       "-bargraph-size"
#define STR_OPTION_INFO_VERBOSE             "-verbose"
#define STR_OPTION_INFO_STATS               "-stats"

#define STR_OPTION_INFO_PROGRESSION         "-progression-file"

#define STR_OPTION_INFO_ALIGNMENT_PROGRESS  "-alignment-progress"

#define STR_OPTION_INFO_CONFIG_FILE         "-plastrc"

#define STR_OPTION_HELP                     "-h"

/********************************************************************************/

#endif /* _STRINGS_HPP_ */
