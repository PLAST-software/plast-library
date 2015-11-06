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
 * We define here several sets of strings constants:
 *    - command line options strings
 *    - command line help strings
 *    - exception messages strings
 *    - Miscellaneous strings.
 *
 * Note that these macros are just shortcuts for constant strings located in StringRepository class.
 *
 */

#ifndef _PLAST_STRINGS_HPP_
#define _PLAST_STRINGS_HPP_

/** \namespace misc
 * \brief Strings constants definitions.
 *
 */

#include <misc/api/PlastStringsRepository.hpp>

/********************************************************************************/
namespace misc {
/********************************************************************************/

/** "-p"    Command Line option telling which kind of comparison we are interested in.
 *  String value (one of: plastp, tplastn, plastx, tplastx, plastn)
 *
 *  Note: user can omit this option in case of protein/protein, protein/ADN, ADN/protein
 *  searches. For ADN/ADN searches, there would be an ambiguity between tplastx and plastn,
 *  so the option has to be set in such a case.
 */
#define STR_OPTION_ALGO_TYPE                misc::StringRepository::m_STR_OPTION_ALGO_TYPE ()

/** "-d"    Command Line option giving the file path of the subject database.
 *  String value.
 */
#define STR_OPTION_SUBJECT_URI              misc::StringRepository::m_STR_OPTION_SUBJECT_URI ()

/** "-i"    Command Line option giving the file path of the query database.
 *  String value.
 */
#define STR_OPTION_QUERY_URI                misc::StringRepository::m_STR_OPTION_QUERY_URI ()

/** "-o"    Command Line option giving the file path of the output file.
 *  String value.
 */
#define STR_OPTION_OUTPUT_FILE              misc::StringRepository::m_STR_OPTION_OUTPUT_FILE ()

/** "-e"    Command Line option giving the maximum evalue to be used by PLAST.
 *  Float value.
 */
#define STR_OPTION_EVALUE                   misc::StringRepository::m_STR_OPTION_EVALUE ()

/** "-n"    Command Line option giving the neighborhood size (ie. number of amino acids or nucleotides)
 *  used for computing left and right scores during the ungap algorithm.
 *  Integer value.
 */
#define STR_OPTION_UNGAP_NEIGHBOUR_LENGTH   misc::StringRepository::m_STR_OPTION_UNGAP_NEIGHBOUR_LENGTH ()

/** "-s"    Command Line option giving the threshold score used for computing score
 *  during the ungap algorithm.
 *  Integer value.
 */
#define STR_OPTION_UNGAP_SCORE_THRESHOLD    misc::StringRepository::m_STR_OPTION_UNGAP_SCORE_THRESHOLD ()

/** "-g"    Command Line option giving the threshold score used for computing score
 *  during the small gap algorithm.
 *  Integer value.
 */
#define STR_OPTION_SMALLGAP_THRESHOLD       misc::StringRepository::m_STR_OPTION_SMALLGAP_THRESHOLD ()

/** "-b"    Command Line option giving the bandwith size used during the small gap algorithm.
 *  Integer value.
 */
#define STR_OPTION_SMALLGAP_BAND_WITH       misc::StringRepository::m_STR_OPTION_SMALLGAP_BAND_WITH ()

/** "-a"    Command Line option giving the number of cores to be used for computation.
 *
 * If not set, PLAST will take all available cores. Note that the number of available cores is displayed
 * when launching PLAST with -h option.
 *  Integer value.
 */
#define STR_OPTION_NB_PROCESSORS            misc::StringRepository::m_STR_OPTION_NB_PROCESSORS ()

/** "-G"    Command Line option giving the cost for opening a gap.
 *  Integer value.
 */
#define STR_OPTION_OPEN_GAP_COST            misc::StringRepository::m_STR_OPTION_OPEN_GAP_COST ()

/** "-E"    Command Line option giving the cost for extending a gap.
 *  Integer value.
 */
#define STR_OPTION_EXTEND_GAP_COST          misc::StringRepository::m_STR_OPTION_EXTEND_GAP_COST ()

/** "-xdrop_ungap"    Command Line option giving the dropoff value for ungap algorithm.
 *  Integer value.
 */
#define STR_OPTION_X_DROPOFF_UNGAPPED       misc::StringRepository::m_STR_OPTION_X_DROPOFF_UNGAPPED ()

/** "-X"    Command Line option giving the dropoff value for full gap algorithm.
 *  Integer value.
 */
#define STR_OPTION_X_DROPOFF_GAPPED         misc::StringRepository::m_STR_OPTION_X_DROPOFF_GAPPED ()

/** "-Z"    Command Line option giving the dropoff value for composition gap algorithm.
 *  Integer value.
 */
#define STR_OPTION_X_DROPOFF_FINAL          misc::StringRepository::m_STR_OPTION_X_DROPOFF_FINAL ()

/** "-index-threshold"    Command Line option giving the threshold for the indexation neighbourg calculation.
 *  Integer value.
 */
#define STR_OPTION_INDEX_NEIGHBOUR_THRESHOLD    misc::StringRepository::m_STR_OPTION_INDEX_NEIGHBOUR_THRESHOLD ()

/** "-F"    Command Line option telling if the algorithm should filter out low information parts
 *  in the query database.
 *  String value (F or T).
 */
#define STR_OPTION_FILTER_QUERY             misc::StringRepository::m_STR_OPTION_FILTER_QUERY ()

/** "-M"    Command Line option telling which score matrix is wanted. BLOSUM62 is used by default.
 *  String value (BLOSUM62 or BLOSUM50).
 */
#define STR_OPTION_SCORE_MATRIX             misc::StringRepository::m_STR_OPTION_SCORE_MATRIX ()

/** "-r"    Option giving the reward for a nucleotide match (plastn)
 *  Integer value */
#define STR_OPTION_REWARD                   misc::StringRepository::m_STR_OPTION_REWARD ()

/** "-q"    Option giving the penalty for a nucleotide mismatch (plastn)
 *  Integer value */
#define STR_OPTION_PENALTY                  misc::StringRepository::m_STR_OPTION_PENALTY ()

/** "-outfmt"   Command Line option giving the output format. Tabulated output by default.
 *  Enum value (1 for tabulated output, 2 for XML output)
 */
#define STR_OPTION_OUTPUT_FORMAT            misc::StringRepository::m_STR_OPTION_OUTPUT_FORMAT ()

/** "-strand"   Command Line option giving the wanted strands (in particular for plastn).
 *  string: "plus", "minus" or "both" (default)
 */
#define STR_OPTION_STRAND                   misc::StringRepository::m_STR_OPTION_STRAND ()

/** "-force-query-order"    Command Line option forcing the ordering of queries identifiers in the final output file.
 *  Integer: number of query per notification
 */
#define STR_OPTION_FORCE_QUERY_ORDERING     misc::StringRepository::m_STR_OPTION_FORCE_QUERY_ORDERING ()

/** "-max-database-size"    Command Line option giving the maximum database size (in bytes) the algorithm can deal with.
 *  If the subject and/or the query databases are greater than this threshold, the algorithm
 *  will segment the databases by blocks of this "max-database-size" size, so even huge file
 *  may be handled by the algorithm.
 *  Integer value (default 20000000)
 */
#define STR_OPTION_MAX_DATABASE_SIZE        misc::StringRepository::m_STR_OPTION_MAX_DATABASE_SIZE ()

/** "-max-hit_per-query"  Command Line option giving the maximum number of hits per query we want in the output.
 *  This may be useful for avoiding to have too many alignments for one query sequence (only the
 *  "best" ones are kept)
 *  Integer value
 */
#define STR_OPTION_MAX_HIT_PER_QUERY       misc::StringRepository::m_STR_OPTION_MAX_HIT_PER_QUERY ()

/** "-max-hsp-per-hit"  Command Line option giving the maximum number of hits per query we want in the output.
 *  This may be useful for avoiding to have too many alignments for one query sequence (only the
 *  "best" ones are kept)
 *  Integer value
 */
#define STR_OPTION_MAX_HSP_PER_HIT          misc::StringRepository::m_STR_OPTION_MAX_HSP_PER_HIT ()

/** "-max-hit-per-iteration"    Command Line option giving the maximum number of hits handled in a row for one seed.
 *  By default, for one seed, all possible matches between subject and query are handled in a row.
 *  Since the neighborhoods for all these matches are copied into memory, it may lead to huge
 *  memory usage in case we have a lot of matches for one seed. This option gives a threshold that
 *  allows to handle all the matches (for the current seed) by blocks of "max-hit-per-iteration" size.
 *  Integer value
 */
#define STR_OPTION_MAX_HIT_PER_ITERATION    misc::StringRepository::m_STR_OPTION_MAX_HIT_PER_ITERATION ()

/** "-strands-list" Command Line option giving the list of strands to be used when we deal with ADN databases. The value
 *  is a list of strands number (from 1 to 6) comma separated, for instance "1,2,3"
 *  String value
 */
#define STR_OPTION_STRANDS_LIST             misc::StringRepository::m_STR_OPTION_STRANDS_LIST ()

/** "-optim-codon-stop" Command Line option giving the size of the allowed range between the last
 *  invalid character and the next stop codon.
 *  Integer value
 */
#define STR_OPTION_CODON_STOP_OPTIM         misc::StringRepository::m_STR_OPTION_CODON_STOP_OPTIM ()

/** "-factory-dispatcher"   Command Line option giving the factory name for creating commands dispatcher.
 *  String value, one of: SerialCommandDispatcher, ParallelCommandDispatcher (default)
 */
#define STR_OPTION_FACTORY_DISPATCHER       misc::StringRepository::m_STR_OPTION_FACTORY_DISPATCHER ()

/** "-factory-statistics"   Command Line option giving the factory name for creating statistics.
 *  String value, one of: Karlin (default), Spouge
 */
#define STR_OPTION_FACTORY_STATISTICS       misc::StringRepository::m_STR_OPTION_FACTORY_STATISTICS ()

/** "-factory-indexation"   Command Line option giving the factory name for creating indexator instances.
 *  String value, one of: BasicIndexator, BasicSortedIndexator (default)
 */
#define STR_OPTION_FACTORY_INDEXATION       misc::StringRepository::m_STR_OPTION_FACTORY_INDEXATION ()

/** "-factory-hit-ungap"    Command Line option giving the factory name for creating ungap algorithm part instances.
 *  String value, one of: UngapHitIteratorNull, UngapHitIterator, UngapHitIteratorSSE16 (default)
 */
#define STR_OPTION_FACTORY_HIT_UNGAP        misc::StringRepository::m_STR_OPTION_FACTORY_HIT_UNGAP ()

/** "-factory-hit-smallgap" Command Line option giving the factory name for creating small gap algorithm part instances.
 *  String value, one of: SmallGapHitIteratorNull, SmallGapHitIteratorSSE8 (default)
 */
#define STR_OPTION_FACTORY_HIT_SMALLGAP     misc::StringRepository::m_STR_OPTION_FACTORY_HIT_SMALLGAP ()

/** "-factory-hit-fullgap"  Command Line option giving the factory name for creating small gap algorithm part instances.
 *  String value, one of: FullGapHitIteratorNull, FullGapHitIterator (default)
 */
#define STR_OPTION_FACTORY_HIT_FULLGAP      misc::StringRepository::m_STR_OPTION_FACTORY_HIT_FULLGAP ()

/** "-factory-hit-composition"  Command Line option giving the factory name for creating composition gap algorithm part instances.
 *  String value, one of: CompositionHitIteratorNull, CompositionHitIterator (default)
 */
#define STR_OPTION_FACTORY_HIT_COMPOSITION  misc::StringRepository::m_STR_OPTION_FACTORY_HIT_COMPOSITION ()

/** "-factory-gap-result" Command Line option giving the factory name for creating gap result instances (containing final alignments).
 *  String value, one of: BasicAlignmentResult
 */
#define STR_OPTION_FACTORY_GAP_RESULT       misc::StringRepository::m_STR_OPTION_FACTORY_GAP_RESULT ()

/** "-factory-ungap-result" Command Line option giving the factory name for creating ungap result instances (containing final ungap alignments).
 *  String value, one of: UngapAlignmentResult
 */
#define STR_OPTION_FACTORY_UNGAP_RESULT     misc::StringRepository::m_STR_OPTION_FACTORY_UNGAP_RESULT ()

/** "-splitter" Command Line option giving the factory name for creating alignment splitter
 *  String value, one of: "normal" or "banded" (default)
 */
#define STR_OPTION_FACTORY_SPLITTER     misc::StringRepository::m_STR_OPTION_FACTORY_SPLITTER ()

/** "-optim-filter-ungap" Command Line option giving the boolean value for filtering out already known ungap alignments before small gap algorithm.
 * This option may lead to great time optimization.
 *  String value (F or T). Default to true
 */
#define STR_OPTION_OPTIM_FILTER_UNGAP       misc::StringRepository::m_STR_OPTION_OPTIM_FILTER_UNGAP ()

/** "-bargraph" Command Line option telling whether one wants to have a progress bar during algorithm execution.
 *  No associated value.
 */
#define STR_OPTION_INFO_BARGRAPH            misc::StringRepository::m_STR_OPTION_INFO_BARGRAPH ()

/** "-bargraph-size" Command Line option giving the size (in characters) of the progress bargraph.
 *  Integer value.
 */
#define STR_OPTION_INFO_BARGRAPH_SIZE       misc::StringRepository::m_STR_OPTION_INFO_BARGRAPH_SIZE ()

/** "-verbose"  Command Line option telling that some information have to be displayed during algorithm execution.
 *  No associated value.
 */
#define STR_OPTION_INFO_VERBOSE             misc::StringRepository::m_STR_OPTION_INFO_VERBOSE ()

/** "-full-stats"   Command Line option giving the file path for statistics information about the algorithm execution.
 *  String.
 */
#define STR_OPTION_INFO_FULL_STATS          misc::StringRepository::m_STR_OPTION_INFO_FULL_STATS ()

/** "-stats"    Command Line option giving the file path for statistics information about the hits iterators information.
 *  String.
 */
#define STR_OPTION_INFO_STATS               misc::StringRepository::m_STR_OPTION_INFO_STATS ()

/** "-stats-fmt"    Format of the stats: 'raw' (default) or 'xml' */
#define STR_OPTION_INFO_STATS_FORMAT        misc::StringRepository::m_STR_OPTION_INFO_STATS_FORMAT ()

/** "-stats-auto"   Command Line option for automatic statistics creation. When set, a statistics file will be created
 * at each PLAST execution, the file being created in the current directory with a name holding the current date.
 */
#define STR_OPTION_INFO_STATS_AUTO          misc::StringRepository::m_STR_OPTION_INFO_STATS_AUTO ()

/** "-progression-file" Command Line option giving the path of the file holding the algorithm execution percentage.
 *  String.
 */
#define STR_OPTION_INFO_PROGRESSION         misc::StringRepository::m_STR_OPTION_INFO_PROGRESSION ()

/** "-alignment-progress"   Command Line option giving the path of the file holding statistics data about alignments number.
 *  String.
 */
#define STR_OPTION_INFO_ALIGNMENT_PROGRESS  misc::StringRepository::m_STR_OPTION_INFO_ALIGNMENT_PROGRESS ()

/** "-resources-progress"   Command Line option giving the path of the file holding statistics data about resources.
 *  String.
 */
#define STR_OPTION_INFO_RESOURCES_PROGRESS  misc::StringRepository::m_STR_OPTION_INFO_RESOURCES_PROGRESS ()

/** "-plastrc"  Command Line option giving the path of the configuration file.
 *  String.
 */
#define STR_OPTION_INFO_CONFIG_FILE         misc::StringRepository::m_STR_OPTION_INFO_CONFIG_FILE ()

/** "-xmlfilter"    Command Line option giving the uri of a XML filter file. Such a filter may be useful to reduce the number
 * of final alignments. The syntax of such filters is provided by Korilog.
 *  String.
 */
#define STR_OPTION_XML_FILTER_FILE         misc::StringRepository::m_STR_OPTION_XML_FILTER_FILE ()

/** "-seeds-index-filter" command line option giving the length of the seed (X) that we want to simulate
 * We will only read in the subject database the seed with the size W each X size
 */
#define STR_OPTION_INDEX_FILTER_SEED         misc::StringRepository::m_STR_OPTION_INDEX_FILTER_SEED ()

/** "-seeds-use-ratio"  Command Line option giving the ratio of seeds to be used. Setting a value less than 1.0 will speed up
 * the algorithm with the drawback to get fewer alignments. In some cases, setting the value to 0.05 with a evalue to 1e-30 will
 * lead to x2 speed up with a loss of about 1% of alignements.
 *  float (1.0 by default).
 */
#define STR_OPTION_SEEDS_USE_RATIO         misc::StringRepository::m_STR_OPTION_SEEDS_USE_RATIO ()

/** "-h"    Command Line option for displaying help.
 *  No associated value.
 */
#define STR_OPTION_HELP                     misc::StringRepository::m_STR_OPTION_HELP ()

/** Word size
 */
#define STR_OPTION_WORD_SIZE   				misc::StringRepository::m_STR_OPTION_WORD_SIZE ()

/** "-complete-subject-database-stats-file"  Command Line option giving the path of the complete subject database stats file.
 *  String.
 */
#define STR_OPTION_COMPLETE_SUBJECT_DB_STATS_FILE       misc::StringRepository::m_STR_OPTION_COMPLETE_SUBJECT_DB_STATS_FILE ()

/** "-complete-subject-database-size"  Option giving the size of the complete subject database file.
 *  size_t.
 */
#define STR_OPTION_COMPLETE_SUBJECT_DB_SIZE             misc::StringRepository::m_STR_OPTION_COMPLETE_SUBJECT_DB_SIZE ()

/** "-complete-subject-database-sequences-number"  Option giving the number of sequences of the complete subject database file.
 *  int.
 */
#define STR_OPTION_COMPLETE_SUBJECT_DB_SEQUENCE_NUMBER  misc::StringRepository::m_STR_OPTION_COMPLETE_SUBJECT_DB_SEQUENCE_NUMBER ()


/** option very useful for ARN16S comparison.
 * This option will tell plastn to use only some of the available kmers in the
 * query and the database. The kmers that will be used are those marked in the
 * bitset file.
 */
#define STR_OPTION_KMERS_BITSET_PATH misc::StringRepository::m_STR_OPTION_KMERS_BITSET_PATH ()

#define STR_OPTION_ITERATIONS_STEPS misc::StringRepository::m_STR_OPTION_ITERATIONS_STEPS ()

#define STR_OPTION_KMERS_TO_SELECT misc::StringRepository::m_STR_OPTION_KMERS_TO_SELECT ()

/********************************************************************************/

/** Strings occurring in messages, exceptions... */

#define MSG_MAIN_RC_FILE  misc::StringRepository::m_MSG_MAIN_RC_FILE ()   // /.plastrc
#define MSG_MAIN_HOME  misc::StringRepository::m_MSG_MAIN_HOME ()   // HOME
#define MSG_MAIN_MSG1  misc::StringRepository::m_MSG_MAIN_MSG1 ()   // PLAST %s (%ld cores available)
#define MSG_MAIN_MSG2  misc::StringRepository::m_MSG_MAIN_MSG2 ()   // ERROR BAD PARAMETERS: %s
#define MSG_MAIN_MSG3  misc::StringRepository::m_MSG_MAIN_MSG3 ()   // ERROR: %s
#define MSG_MAIN_MSG4  misc::StringRepository::m_MSG_MAIN_MSG4 ()   // GENERIC ERROR...

#define MSG_INDEXATION_MSG1  misc::StringRepository::m_MSG_INDEXATION_MSG1 ()   // ERROR!!! : BAD HASH CODE

#define MSG_DATABASE_MSG1  misc::StringRepository::m_MSG_DATABASE_MSG1 ()   // ERROR IN SEQUENCE SEARCH...

#define MSG_FILE_BLAST_MSG1  misc::StringRepository::m_MSG_FILE_BLAST_MSG1 ()   // ERROR DURING BLAST READING...
#define MSG_FILE_BLAST_MSG2  misc::StringRepository::m_MSG_FILE_BLAST_MSG2 ()   // ERROR DURING BLAST PARSING...
#define MSG_FILE_BLAST_MSG3  misc::StringRepository::m_MSG_FILE_BLAST_MSG3 ()   // ERROR DURING ASN1 PARSING...

#define MSG_HITS_MSG1  misc::StringRepository::m_MSG_HITS_MSG1 ()   // SEQUENCE NOT FOUND IN DATABASE...
#define MSG_HITS_MSG2  misc::StringRepository::m_MSG_HITS_MSG2 ()   // iterating HSP
#define MSG_HITS_MSG3  misc::StringRepository::m_MSG_HITS_MSG3 ()   // starting iterating seeds...
#define MSG_HITS_MSG4  misc::StringRepository::m_MSG_HITS_MSG4 ()   // iterating all possible seeds...  (%ld/%ld)
#define MSG_HITS_MSG5  misc::StringRepository::m_MSG_HITS_MSG5 ()   // finishing iterating seeds...
#define MSG_HITS_MSG6  misc::StringRepository::m_MSG_HITS_MSG6 ()   // iterating seeds codes

#define MSG_STATS_MSG1  misc::StringRepository::m_MSG_STATS_MSG1 ()   // unknown score matrix
#define MSG_STATS_MSG2  misc::StringRepository::m_MSG_STATS_MSG2 ()   // No found statistical parameters for given matrix and open/extend gap scores...

#define MSG_OPTIONS_MSG1  misc::StringRepository::m_MSG_OPTIONS_MSG1 ()   // Option %s already seen, ignored...
#define MSG_OPTIONS_MSG2  misc::StringRepository::m_MSG_OPTIONS_MSG2 ()   // Option %s can't be used with option %s, ignored...
#define MSG_OPTIONS_MSG3  misc::StringRepository::m_MSG_OPTIONS_MSG3 ()   // To few arguments for the %s option...
#define MSG_OPTIONS_MSG4  misc::StringRepository::m_MSG_OPTIONS_MSG4 ()   // Error : %s
#define MSG_OPTIONS_MSG5  misc::StringRepository::m_MSG_OPTIONS_MSG5 ()   // Warning : %s
#define MSG_OPTIONS_MSG6  misc::StringRepository::m_MSG_OPTIONS_MSG6 ()   // Option %s must be used with one of the following options : %s
#define MSG_OPTIONS_MSG7  misc::StringRepository::m_MSG_OPTIONS_MSG7 ()   // Option %s is mandatory
#define MSG_OPTIONS_MSG8  misc::StringRepository::m_MSG_OPTIONS_MSG8 ()   // use option -h for full help

#define STR_HELP_ALGO_TYPE                  misc::StringRepository::m_STR_HELP_ALGO_TYPE ()   // Program Name [plastp, tplastn, plastx or tplastx]
#define STR_HELP_SUBJECT_URI                misc::StringRepository::m_STR_HELP_SUBJECT_URI ()   // Subject database file
#define STR_HELP_QUERY_URI                  misc::StringRepository::m_STR_HELP_QUERY_URI ()   // Query database file
#define STR_HELP_OUTPUT_FILE                misc::StringRepository::m_STR_HELP_OUTPUT_FILE ()   // PLAST report Output File
#define STR_HELP_EVALUE                     misc::StringRepository::m_STR_HELP_EVALUE ()   // Expectation value
#define STR_HELP_UNGAP_NEIGHBOUR_LENGTH     misc::StringRepository::m_STR_HELP_UNGAP_NEIGHBOUR_LENGTH ()   // Size of neighbourhood peforming ungapped extension
#define STR_HELP_UNGAP_SCORE_THRESHOLD      misc::StringRepository::m_STR_HELP_UNGAP_SCORE_THRESHOLD ()   // Ungapped threshold trigger a small gapped extension
#define STR_HELP_SMALLGAP_THRESHOLD         misc::StringRepository::m_STR_HELP_SMALLGAP_THRESHOLD ()   // threshold for small gapped extension
#define STR_HELP_SMALLGAP_BAND_WITH         misc::StringRepository::m_STR_HELP_SMALLGAP_BAND_WITH ()   // bandwith for small gapped extension
#define STR_HELP_NB_PROCESSORS              misc::StringRepository::m_STR_HELP_NB_PROCESSORS ()   // Number of processors to use
#define STR_HELP_OPEN_GAP_COST              misc::StringRepository::m_STR_HELP_OPEN_GAP_COST ()   // Cost to open a gap
#define STR_HELP_EXTEND_GAP_COST            misc::StringRepository::m_STR_HELP_EXTEND_GAP_COST ()   // Cost to extend a gap
#define STR_HELP_X_DROPOFF_UNGAPPED         misc::StringRepository::m_STR_HELP_X_DROPOFF_UNGAPPED ()   // X dropoff value for ungapped alignment (in bits) (zero invokes default behavior 20 bits)
#define STR_HELP_X_DROPOFF_GAPPED           misc::StringRepository::m_STR_HELP_X_DROPOFF_GAPPED ()   // X dropoff value for gapped alignment (in bits) (zero invokes default behavior)
#define STR_HELP_X_DROPOFF_FINAL            misc::StringRepository::m_STR_HELP_X_DROPOFF_FINAL ()   // X dropoff value for final gapped alignment in bits (0.0 invokes default behavior)
#define STR_HELP_INDEX_NEIGHBOUR_THRESHOLD            misc::StringRepository::m_STR_HELP_INDEX_NEIGHBOUR_THRESHOLD ()   // Index threshold to calculate the similarity between neighbour
#define STR_HELP_FILTER_QUERY               misc::StringRepository::m_STR_HELP_FILTER_QUERY ()   // Filter query sequence
#define STR_HELP_SCORE_MATRIX               misc::StringRepository::m_STR_HELP_SCORE_MATRIX ()   // Score matrix (BLOSUM62 or BLOSUM50)
#define STR_HELP_STRAND                     misc::StringRepository::m_STR_HELP_STRAND ()   // strands for plastn: 'plus', 'minus' or 'both' (default)
#define STR_HELP_REWARD                     misc::StringRepository::m_STR_HELP_REWARD ()   // reward for a nucleotide match (plastn)
#define STR_HELP_PENALTY                    misc::StringRepository::m_STR_HELP_PENALTY ()   // penalty for a nucleotide mismatch (plastn)
#define STR_HELP_FORCE_QUERY_ORDERING       misc::StringRepository::m_STR_HELP_FORCE_QUERY_ORDERING ()   // Force queries ordering in output file.
#define STR_HELP_MAX_DATABASE_SIZE          misc::StringRepository::m_STR_HELP_MAX_DATABASE_SIZE ()   // Maximum allowed size (in bytes) for a database. If greater, database is segmented.
#define STR_HELP_MAX_HIT_PER_QUERY			misc::StringRepository::m_STR_HELP_MAX_HIT_PER_QUERY ()
#define STR_HELP_MAX_HSP_PER_HIT            misc::StringRepository::m_STR_HELP_MAX_HSP_PER_HIT ()   // Maximum hits per query. 0 value will dump all hits (default)
#define STR_HELP_MAX_HIT_PER_ITERATION      misc::StringRepository::m_STR_HELP_MAX_HIT_PER_ITERATION ()   // Maximum hits per iteration (for memory usage control). 1000000 by default
#define STR_HELP_OUTPUT_FORMAT              misc::StringRepository::m_STR_HELP_OUTPUT_FORMAT ()   // Output format: 1 for tabulated (default).
#define STR_HELP_STRANDS_LIST               misc::StringRepository::m_STR_HELP_STRANDS_LIST ()   // List of the strands (ex: "1,2,6") to be used when using algo using nucleotids databases.
#define STR_HELP_CODON_STOP_OPTIM           misc::StringRepository::m_STR_HELP_CODON_STOP_OPTIM ()   // size of the allowed range between the last invalid character and the next stop codon
#define STR_HELP_FACTORY_DISPATCHER         misc::StringRepository::m_STR_HELP_FACTORY_DISPATCHER ()   // Factory that creates dispatcher.
#define STR_HELP_FACTORY_STATISTICS         misc::StringRepository::m_STR_HELP_FACTORY_STATISTICS ()   // Factory that creates statistics builder.
#define STR_HELP_FACTORY_INDEXATION         misc::StringRepository::m_STR_HELP_FACTORY_INDEXATION ()   // Factory that creates indexation builder.
#define STR_HELP_FACTORY_HIT_UNGAP          misc::StringRepository::m_STR_HELP_FACTORY_HIT_UNGAP ()   // Factory that creates ungap hits iterator.
#define STR_HELP_FACTORY_HIT_SMALLGAP       misc::StringRepository::m_STR_HELP_FACTORY_HIT_SMALLGAP ()   // Factory that creates small gap hits iterator.
#define STR_HELP_FACTORY_HIT_FULLGAP        misc::StringRepository::m_STR_HELP_FACTORY_HIT_FULLGAP ()   // Factory that creates full gap hits iterator.
#define STR_HELP_FACTORY_HIT_COMPOSITION    misc::StringRepository::m_STR_HELP_FACTORY_HIT_COMPOSITION ()   // Factory that creates composition hits iterator.
#define STR_HELP_FACTORY_GAP_RESULT         misc::StringRepository::m_STR_HELP_FACTORY_GAP_RESULT ()   // Factory that creates gap alignments result.
#define STR_HELP_FACTORY_UNGAP_RESULT       misc::StringRepository::m_STR_HELP_FACTORY_UNGAP_RESULT ()   // Factory that creates ungap alignments result.
#define STR_HELP_FACTORY_SPLITTER           misc::StringRepository::m_STR_HELP_FACTORY_SPLITTER ()   // Factory that creates an alignment splitter
#define STR_HELP_OPTIM_FILTER_UNGAP         misc::StringRepository::m_STR_HELP_OPTIM_FILTER_UNGAP ()   // Optimization that filters out through ungap alignments.
#define STR_HELP_XML_FILTER_FILE            misc::StringRepository::m_STR_HELP_XML_FILTER_FILE ()   // Uri of a XML filter file.
#define STR_HELP_SEEDS_USE_RATIO            misc::StringRepository::m_STR_HELP_SEEDS_USE_RATIO ()   // Ratio of seeds to be used.
#define STR_HELP_INDEX_FILTER_SEED          misc::StringRepository::m_STR_HELP_INDEX_FILTER_SEED ()   // size of the seeds index filter.
#define STR_HELP_HELP                       misc::StringRepository::m_STR_HELP_HELP ()   // help
#define STR_HELP_INFO_BARGRAPH              misc::StringRepository::m_STR_HELP_INFO_BARGRAPH ()   // Display a progress bar during execution.
#define STR_HELP_INFO_BARGRAPH_SIZE         misc::StringRepository::m_STR_HELP_INFO_BARGRAPH_SIZE ()   // Nb of characters of the bargraph.
#define STR_HELP_INFO_PROGRESSION           misc::StringRepository::m_STR_HELP_INFO_PROGRESSION ()   // Dump in a file the current execution percentage.
#define STR_HELP_INFO_VERBOSE               misc::StringRepository::m_STR_HELP_INFO_VERBOSE ()   // Display information during algorithm execution.
#define STR_HELP_INFO_FULL_STATS            misc::StringRepository::m_STR_HELP_INFO_FULL_STATS ()   // Dump algorithm statistics.
#define STR_HELP_INFO_STATS                 misc::StringRepository::m_STR_HELP_INFO_STATS ()   // Dump generic statistics.
#define STR_HELP_INFO_STATS_FORMAT          misc::StringRepository::m_STR_HELP_INFO_STATS_FORMAT ()   // Format of statistics: 'raw' (default) or 'xml'
#define STR_HELP_INFO_STATS_AUTO            misc::StringRepository::m_STR_HELP_INFO_STATS_AUTO ()   // Automatic stats file creation
#define STR_HELP_INFO_ALIGNMENT_PROGRESS    misc::StringRepository::m_STR_HELP_INFO_ALIGNMENT_PROGRESS ()   // Dump in a file the growing number of ungap/ungap alignments during algorithm.
#define STR_HELP_INFO_RESOURCES_PROGRESS    misc::StringRepository::m_STR_HELP_INFO_RESOURCES_PROGRESS ()   // Dump in a file information about resources during algorithm.
#define STR_HELP_INFO_CONFIG_FILE           misc::StringRepository::m_STR_HELP_INFO_CONFIG_FILE ()   // Pathname of the plast config file.
#define STR_HELP_WORD_SIZE                  misc::StringRepository::m_STR_HELP_WORD_SIZE ()   // Pathname of the plast config file.
#define STR_HELP_COMPLETE_SUBJECT_DB_STATS_FILE   		misc::StringRepository::m_STR_HELP_COMPLETE_SUBJECT_DB_STATS_FILE ()   // File path to the stats of the complete subject db
#define STR_HELP_KMERS_BITSET_PATH                      misc::StringRepository::m_STR_HELP_KMERS_BITSET_PATH () // Option to read a bitset to filter kmers.
#define STR_HELP_ITERATIONS_STEPS                      misc::StringRepository::m_STR_HELP_ITERATIONS_STEPS () // Option to run multiple iterations

#define STR_CONFIG_CLASS_KarlinStats			        misc::StringRepository::m_STR_CONFIG_CLASS_KarlinStats ()   // KarlinStats
#define STR_CONFIG_CLASS_SpougeStats				    misc::StringRepository::m_STR_CONFIG_CLASS_SpougeStats ()   // SpougeStats
#define STR_CONFIG_CLASS_SerialCommandDispatcher        misc::StringRepository::m_STR_CONFIG_CLASS_SerialCommandDispatcher ()   // SerialCommandDispatcher
#define STR_CONFIG_CLASS_ParallelCommandDispatcher      misc::StringRepository::m_STR_CONFIG_CLASS_ParallelCommandDispatcher ()   // ParallelCommandDispatcher
#define STR_CONFIG_CLASS_BasicIndexator                 misc::StringRepository::m_STR_CONFIG_CLASS_BasicIndexator ()   // BasicIndexator
#define STR_CONFIG_CLASS_BasicSortedIndexator           misc::StringRepository::m_STR_CONFIG_CLASS_BasicSortedIndexator ()   // BasicSortedIndexator
#define STR_CONFIG_CLASS_BasicIndexatorOptim            misc::StringRepository::m_STR_CONFIG_CLASS_BasicIndexatorOptim ()   // BasicIndexatorOptim
#define STR_CONFIG_CLASS_UngapHitIteratorNull           misc::StringRepository::m_STR_CONFIG_CLASS_UngapHitIteratorNull ()   // UngapHitIteratorNull
#define STR_CONFIG_CLASS_UngapHitIterator               misc::StringRepository::m_STR_CONFIG_CLASS_UngapHitIterator ()   // UngapHitIterator
#define STR_CONFIG_CLASS_UngapHitIteratorSSE16          misc::StringRepository::m_STR_CONFIG_CLASS_UngapHitIteratorSSE16 ()   // UngapHitIteratorSSE16
#define STR_CONFIG_CLASS_SmallGapHitIterator            misc::StringRepository::m_STR_CONFIG_CLASS_SmallGapHitIterator ()   // SmallGapHitIterator
#define STR_CONFIG_CLASS_SmallGapHitIteratorNull        misc::StringRepository::m_STR_CONFIG_CLASS_SmallGapHitIteratorNull ()   // SmallGapHitIteratorNull
#define STR_CONFIG_CLASS_SmallGapHitIteratorSSE8        misc::StringRepository::m_STR_CONFIG_CLASS_SmallGapHitIteratorSSE8 ()   // SmallGapHitIteratorSSE8
#define STR_CONFIG_CLASS_FullGapHitIterator             misc::StringRepository::m_STR_CONFIG_CLASS_FullGapHitIterator ()   // FullGapHitIterator
#define STR_CONFIG_CLASS_FullGapHitIteratorNull         misc::StringRepository::m_STR_CONFIG_CLASS_FullGapHitIteratorNull ()   // FullGapHitIteratorNull
#define STR_CONFIG_CLASS_CompositionHitIterator         misc::StringRepository::m_STR_CONFIG_CLASS_CompositionHitIterator ()   // CompositionHitIterator
#define STR_CONFIG_CLASS_CompositionHitIteratorNull     misc::StringRepository::m_STR_CONFIG_CLASS_CompositionHitIteratorNull ()   // CompositionHitIteratorNull
#define STR_CONFIG_CLASS_BasicAlignmentResult           misc::StringRepository::m_STR_CONFIG_CLASS_BasicAlignmentResult ()   // BasicAlignmentResult
#define STR_CONFIG_CLASS_UngapAlignmentResult           misc::StringRepository::m_STR_CONFIG_CLASS_UngapAlignmentResult ()   // UngapAlignmentResult

#define STR_PARAM_params                misc::StringRepository::m_STR_PARAM_params ()   // params
#define STR_PARAM_seedModelKind         misc::StringRepository::m_STR_PARAM_seedModelKind ()   // seedModelKind
#define STR_PARAM_seedSpan              misc::StringRepository::m_STR_PARAM_seedSpan ()   // seedSpan
#define STR_PARAM_subseedStrings        misc::StringRepository::m_STR_PARAM_subseedStrings ()   // subseedStrings
#define STR_PARAM_matrixKind            misc::StringRepository::m_STR_PARAM_matrixKind ()   // matrixKind
#define STR_PARAM_subject               misc::StringRepository::m_STR_PARAM_subject ()   // subject
#define STR_PARAM_uri                   misc::StringRepository::m_STR_PARAM_uri ()   // uri
#define STR_PARAM_range                 misc::StringRepository::m_STR_PARAM_range ()   // range
#define STR_PARAM_begin                 misc::StringRepository::m_STR_PARAM_begin ()   // begin
#define STR_PARAM_end                   misc::StringRepository::m_STR_PARAM_end ()   // end
#define STR_PARAM_filterQuery           misc::StringRepository::m_STR_PARAM_filterQuery ()   // filterQuery
#define STR_PARAM_ungapNeighbourLength  misc::StringRepository::m_STR_PARAM_ungapNeighbourLength ()   // ungapNeighbourLength
#define STR_PARAM_ungapScoreThreshold   misc::StringRepository::m_STR_PARAM_ungapScoreThreshold ()   // ungapScoreThreshold
#define STR_PARAM_smallGapBandLength    misc::StringRepository::m_STR_PARAM_smallGapBandLength ()   // smallGapBandLength
#define STR_PARAM_smallGapBandWidth     misc::StringRepository::m_STR_PARAM_smallGapBandWidth ()   // smallGapBandWidth
#define STR_PARAM_smallGapThreshold     misc::StringRepository::m_STR_PARAM_smallGapThreshold ()   // smallGapThreshold
#define STR_PARAM_openGapCost           misc::StringRepository::m_STR_PARAM_openGapCost ()   // openGapCost
#define STR_PARAM_extendGapCost         misc::StringRepository::m_STR_PARAM_extendGapCost ()   // extendGapCost
#define STR_PARAM_evalue                misc::StringRepository::m_STR_PARAM_evalue ()   // evalue
#define STR_PARAM_XdroppofUngap         misc::StringRepository::m_STR_PARAM_XdroppofUngap ()   // XdroppofUngap
#define STR_PARAM_XdroppofGap           misc::StringRepository::m_STR_PARAM_XdroppofGap ()   // XdroppofGap
#define STR_PARAM_finalXdroppofGap      misc::StringRepository::m_STR_PARAM_finalXdroppofGap ()   // finalXdroppofGap
#define STR_PARAM_outputfile            misc::StringRepository::m_STR_PARAM_outputfile ()   // outputfile
#define STR_PARAM_strands               misc::StringRepository::m_STR_PARAM_strands ()   // strands
#define STR_PARAM_wordSize              misc::StringRepository::m_STR_PARAM_wordSize ()   // word size
#define STR_PARAM_kmersBitsetPath  misc::StringRepository::m_STR_PARAM_kmersBitsetPath () // bitset for kmers to be used

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _PLAST_STRINGS_HPP_ */
