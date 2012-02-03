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

/** \file PlastStrings.hpp
 *  \brief Strings definition for PLAST.
 *  \date 07/11/2011
 *  \author edrezen
 *
 *   We define here a bunch of string constants that will be shared for many aspects
 *  (command line options, configuration file...)
 */

#ifndef _PLAST_STRINGS_HPP_
#define _PLAST_STRINGS_HPP_

/** \namespace misc
 * \brief Miscellanous definitions
 */

/********************************************************************************/
namespace misc {
/********************************************************************************/

/** Command Line option telling which kind of comparison we are interested in.
 *  String value (one of: plastp, tplastn, plastx)
 */
#define STR_OPTION_ALGO_TYPE                "-p"

/** Command Line option giving the file path of the subject database.
 *  String value.
 */
#define STR_OPTION_SUBJECT_URI              "-d"

/** Command Line option giving the file path of the query database.
 *  String value.
 */
#define STR_OPTION_QUERY_URI                "-i"

/** Command Line option giving the file path of the output file.
 *  String value.
 */
#define STR_OPTION_OUTPUT_FILE              "-o"

/** Command Line option giving the maximum evalue to be used by PLAST.
 *  Float value.
 */
#define STR_OPTION_EVALUE                   "-e"

/** Command Line option giving the neighbourhood size (ie. number of amino acids or nucleotids)
 *  used for computing left and right scores during the ungap algorithm.
 *  Integer value.
 */
#define STR_OPTION_UNGAP_NEIGHBOUR_LENGTH   "-n"

/** Command Line option giving the threshold score used for computing score
 *  during the ungap algorithm.
 *  Integer value.
 */
#define STR_OPTION_UNGAP_SCORE_THRESHOLD    "-s"

/** Command Line option giving the threshold score used for computing score
 *  during the small gap algorithm.
 *  Integer value.
 */
#define STR_OPTION_SMALLGAP_THRESHOLD       "-g"

/** Command Line option giving the bandwith size used during the small gap algorithm.
 *  Integer value.
 */
#define STR_OPTION_SMALLGAP_BAND_WITH       "-b"

/** Command Line option giving the number of cores to be used for computation.
 *  Integer value.
 */
#define STR_OPTION_NB_PROCESSORS            "-a"

/** Command Line option giving the cost for opening a gap.
 *  Integer value.
 */
#define STR_OPTION_OPEN_GAP_COST            "-G"

/** Command Line option giving the cost for extending a gap.
 *  Integer value.
 */
#define STR_OPTION_EXTEND_GAP_COST          "-E"

/** Command Line option giving the dropoff value for full gap algorithm.
 *  Integer value.
 */
#define STR_OPTION_X_DROPOFF_GAPPED         "-X"

/** Command Line option giving the dropoff value for composition gap algorithm.
 *  Integer value.
 */
#define STR_OPTION_X_DROPOFF_FINAL          "-Z"

/** Command Line option telling if the algorithm should filter out low information parts
 *  in the query database.
 *  String value (F or T).
 */
#define STR_OPTION_FILTER_QUERY             "-F"

/** Command Line option telling which score matrix is wanted
 *  String value (BLOSUM62 or BLOSUM50).
 */
#define STR_OPTION_SCORE_MATRIX             "-M"

/** Command Line option giving the output format.
 *  Enum value (1 for tabulated output, 2 for XML output)
 */
#define STR_OPTION_OUTPUT_FORMAT            "-outfmt"

/** Command Line option giving the maximum database size (in bytes) the algorithm can deal with.
 *  If the subject and/or the query databases are greater than this threshold, the algorithm
 *  will segment the database by blocks of this "max-database-size" size, so even huge file
 *  may be handled by the algorithm.
 *  Integer value
 */
#define STR_OPTION_MAX_DATABASE_SIZE        "-max-database-size"

/** Command Line option giving the maximum number of hits per query we want in the output.
 *  This may be useful for avoiding to have too many alignments for one query sequence (only the
 *  "best" ones are kept)
 *  Integer value
 */
#define STR_OPTION_MAX_HIT_PER_QUERY        "-max-hit-per-query"

/** Command Line option giving the maximum number of hits handled in a row for one seed.
 *  By default, for one seed, all possible matches between subject and query are handled in a row.
 *  Since the neighbourhoods for all these matches are copied into memory, it may lead to huge
 *  memory usage in case we have a lot of matches for one seed. This option gives a threshold that
 *  allows to handle all the matches (for the current seed) by blocks of "max-hit-per-iteration" size.
 *  Integer value
 */
#define STR_OPTION_MAX_HIT_PER_ITERATION    "-max-hit-per-iteration"

/** Command Line option giving the list of strands to be used when we deal with ADN databases. The value
 *  is a list of strands number (from 1 to 6) comma separated, for instance "1,2,3"
 *  String value
 */
#define STR_OPTION_STRANDS_LIST             "-strands-list"

/** Command Line option giving the size of the allowed range between the last
 *  invalid character and the next stop codon.
 *  Integer value
 */
#define STR_OPTION_CODON_STOP_OPTIM         "-optim-codon-stop"

/** Command Line option giving the factory name for creating commands dispatcher.
 *  String value, one of: SerialCommandDispatcher, ParallelCommandDispatcher
 */
#define STR_OPTION_FACTORY_DISPATCHER       "-factory-dispatcher"

/** Command Line option giving the factory name for creating indexator instances.
 *  String value, one of: BasicIndexator, BasicSortedIndexator
 */
#define STR_OPTION_FACTORY_INDEXATION       "-factory-indexation"

/** Command Line option giving the factory name for creating ungap algorithm part instances.
 *  String value, one of: UngapHitIteratorNull, UngapHitIterator, UngapHitIteratorSSE16
 */
#define STR_OPTION_FACTORY_HIT_UNGAP        "-factory-hit-ungap"

/** Command Line option giving the factory name for creating small gap algorithm part instances.
 *  String value, one of: SmallGapHitIteratorNull, SmallGapHitIteratorSSE8
 */
#define STR_OPTION_FACTORY_HIT_SMALLGAP     "-factory-hit-smallgap"

/** Command Line option giving the factory name for creating small gap algorithm part instances.
 *  String value, one of: FullGapHitIteratorNull, FullGapHitIterator
 */
#define STR_OPTION_FACTORY_HIT_FULLGAP      "-factory-hit-fullgap"

/** Command Line option giving the factory name for creating composition gap algorithm part instances.
 *  String value, one of: CompositionHitIteratorNull, CompositionHitIterator
 */
#define STR_OPTION_FACTORY_HIT_COMPOSITION  "-factory-hit-composition"

/** Command Line option giving the factory name for creating gap result instances (containing final alignments).
 *  String value, one of: BasicAlignmentResult
 */
#define STR_OPTION_FACTORY_GAP_RESULT       "-factory-gap-result"

/** Command Line option giving the factory name for creating ungap result instances (containing final ungap alignments).
 *  String value, one of: UngapAlignmentResult
 */
#define STR_OPTION_FACTORY_UNGAP_RESULT     "-factory-ungap-result"

/** Command Line option giving the boolean value for filtering out already known ungap alignments before small gap algorithm.
 *  String value (F or T)
 */
#define STR_OPTION_OPTIM_FILTER_UNGAP       "-optim-filter-ungap"

/** Command Line option telling whether one wants to have a progress bar during algorithm execution.
 *  No associated value.
 */
#define STR_OPTION_INFO_BARGRAPH            "-bargraph"

/** Command Line option giving the size (in characters) of the progress bargraph.
 *  Integer value.
 */
#define STR_OPTION_INFO_BARGRAPH_SIZE       "-bargraph-size"

/** Command Line option telling that some information have to be displayed during algorithm execution.
 *  No associated value.
 */
#define STR_OPTION_INFO_VERBOSE             "-verbose"

/** Command Line option giving the file path for statistics information about the algorithm execution.
 *  String.
 */
#define STR_OPTION_INFO_FULL_STATS          "-full-stats"

/** Command Line option giving the file path for statistics information about the hits iterators information.
 *  String.
 */
#define STR_OPTION_INFO_STATS               "-stats"

/** Format of the stats: 'raw' (default) or 'xml' */
#define STR_OPTION_INFO_STATS_FORMAT        "-stats-fmt"

/** Command Line option giving the path of the file holding the algorithm execution percentage.
 *  String.
 */
#define STR_OPTION_INFO_PROGRESSION         "-progression-file"

/** Command Line option giving the path of the file holding statistics data about alignments number.
 *  String.
 */
#define STR_OPTION_INFO_ALIGNMENT_PROGRESS  "-alignment-progress"

/** Command Line option giving the path of the configuration file.
 *  String.
 */
#define STR_OPTION_INFO_CONFIG_FILE         "-plastrc"

/** Command Line option for displaying help.
 *  No associated value.
 */
#define STR_OPTION_HELP                     "-h"

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _PLAST_STRINGS_HPP_ */
