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

/** \file PlastStringsRepository.hpp
 *  \brief Strings definition for PLAST.
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  Defines the StringRepository class. Two implementations are provided (according to a compilation flag), one with obfuscated strings,
 *  the other with readable strings.
 */

#ifndef _PLAST_STRINGS_REPOSITORY_HPP_
#define _PLAST_STRINGS_REPOSITORY_HPP_

/** \namespace misc
 * \brief Miscellanous definitions
 */

#include <misc/impl/ObsfucatedString.hpp>

/********************************************************************************/
namespace misc {
/********************************************************************************/

/** AUTOMATIC GENERATION (see unit tests and TestPlastStrings in particular).  */

#ifdef OBSFUCATION_TMP

class StringRepository
{
public:
    static const char* m_STR_OPTION_ALGO_TYPE () { static misc::impl::ObsfucatedString s (1343381595, 4715149, 0)  /* => "-p" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_SUBJECT_URI () { static misc::impl::ObsfucatedString s (1343381595, 4712077, 0)  /* => "-d" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_QUERY_URI () { static misc::impl::ObsfucatedString s (1343381595, 4713357, 0)  /* => "-i" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_OUTPUT_FILE () { static misc::impl::ObsfucatedString s (1343381595, 4713869, 0)  /* => "-o" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_EVALUE () { static misc::impl::ObsfucatedString s (1343381595, 4712333, 0)  /* => "-e" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_UNGAP_NEIGHBOUR_LENGTH () { static misc::impl::ObsfucatedString s (1343381595, 4713613, 0)  /* => "-n" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_UNGAP_SCORE_THRESHOLD () { static misc::impl::ObsfucatedString s (1343381595, 4714893, 0)  /* => "-s" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_SMALLGAP_THRESHOLD () { static misc::impl::ObsfucatedString s (1343381595, 4711821, 0)  /* => "-g" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_SMALLGAP_BAND_WITH () { static misc::impl::ObsfucatedString s (1343381595, 4710541, 0)  /* => "-b" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_NB_PROCESSORS () { static misc::impl::ObsfucatedString s (1343381595, 4711309, 0)  /* => "-a" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_OPEN_GAP_COST () { static misc::impl::ObsfucatedString s (1343381595, 4703629, 0)  /* => "-G" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_EXTEND_GAP_COST () { static misc::impl::ObsfucatedString s (1343381595, 4704141, 0)  /* => "-E" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_X_DROPOFF_GAPPED () { static misc::impl::ObsfucatedString s (1343381595, 4709005, 0)  /* => "-X" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_X_DROPOFF_FINAL () { static misc::impl::ObsfucatedString s (1343381595, 4708493, 0)  /* => "-Z" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_FILTER_QUERY () { static misc::impl::ObsfucatedString s (1343381595, 4703373, 0)  /* => "-F" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_SCORE_MATRIX () { static misc::impl::ObsfucatedString s (1343381595, 4706189, 0)  /* => "-M" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_REWARD () { static misc::impl::ObsfucatedString s (1343381595, 4714637, 0)  /* => "-r" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_PENALTY () { static misc::impl::ObsfucatedString s (1343381595, 4715405, 0)  /* => "-q" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_OUTPUT_FORMAT () { static misc::impl::ObsfucatedString s (1343381595, 32771384102022541, 0)  /* => "-outfmt" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_STRAND () { static misc::impl::ObsfucatedString s (1343381595, 28268862477955469, 0)  /* => "-strand" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_FORCE_QUERY_ORDERING () { static misc::impl::ObsfucatedString s (1343381595, 8155285978029286541, 7237969792237589972, 916171, 0)  /* => "-force-query-order" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_MAX_DATABASE_SIZE () { static misc::impl::ObsfucatedString s (1343381595, 8386094127592697741, 7598466910053159616, 912852, 0)  /* => "-max-database-size" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_MAX_HIT_PER_QUERY () { return "-max-hit-per-query"; }
    static const char* m_STR_OPTION_MAX_HSP_PER_HIT () { static misc::impl::ObsfucatedString s (1343381595, 8102934699068288909, 8388350325354086540, 0)  /* => "-max-hsp-per-hit" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_MAX_HIT_PER_ITERATION () { static misc::impl::ObsfucatedString s (1343381595, 8388350325452894093, 7310583739040612492, 121424790022620, 0)  /* => "-max-hit-per-iteration" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_STRANDS_LIST () { static misc::impl::ObsfucatedString s (1343381595, 8314892176839668109, 500150787212, 0)  /* => "-strands-list" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_CODON_STOP_OPTIM () { static misc::impl::ObsfucatedString s (1343381595, 7146488483395661197, 8031170674469459150, 887006, 0)  /* => "-optim-codon-stop" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_FACTORY_DISPATCHER () { static misc::impl::ObsfucatedString s (1343381595, 8751179571604677773, 7166460042652112012, 8383942, 0)  /* => "-factory-dispatcher" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_FACTORY_INDEXATION () { static misc::impl::ObsfucatedString s (1343381595, 8751179571604677773, 8386116358010067340, 6547399, 0)  /* => "-factory-indexation" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_FACTORY_HIT_UNGAP () { static misc::impl::ObsfucatedString s (1343381595, 8751179571604677773, 7453023271430084748, 915663, 0)  /* => "-factory-hit-ungap" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_FACTORY_HIT_SMALLGAP () { static misc::impl::ObsfucatedString s (1343381595, 8751179571604677773, 7020394033202550924, 482670732482, 0)  /* => "-factory-hit-smallgap" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_FACTORY_HIT_FULLGAP () { static misc::impl::ObsfucatedString s (1343381595, 8751179571604677773, 7815265073782282380, 1886187458, 0)  /* => "-factory-hit-fullgap" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_FACTORY_HIT_COMPOSITION () { static misc::impl::ObsfucatedString s (1343381595, 8751179571604677773, 7885630519425063052, 7957695015294003166, 0)  /* => "-factory-hit-composition" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_FACTORY_GAP_RESULT () { static misc::impl::ObsfucatedString s (1343381595, 8751179571604677773, 8315177826492307340, 7988443, 0)  /* => "-factory-gap-result" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_FACTORY_UNGAP_RESULT () { static misc::impl::ObsfucatedString s (1343381595, 8751179571604677773, 8227355657922692492, 500036074443, 0)  /* => "-factory-ungap-result" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_FACTORY_SPLITTER () { static misc::impl::ObsfucatedString (1347439858, 7310596091278083686, 4933154, 0)  /* => "-splitter" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_OPTIM_FILTER_UNGAP () { static misc::impl::ObsfucatedString s (1343381595, 7362661265509445005, 7959317885796637896, 8251849, 0)  /* => "-optim-filter-ungap" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_INFO_BARGRAPH () { static misc::impl::ObsfucatedString s (1343381595, 8097879393611210893, 2502857, 0)  /* => "-bargraph" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_INFO_BARGRAPH_SIZE () { static misc::impl::ObsfucatedString s (1343381595, 8097879393611210893, 111576427601353, 0)  /* => "-bargraph-size" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_INFO_VERBOSE () { static misc::impl::ObsfucatedString s (1343381595, 7310309088764818573, 0)  /* => "-verbose" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_INFO_FULL_STATS () { static misc::impl::ObsfucatedString s (1343381595, 8391100474416358541, 5588160, 0)  /* => "-full-stats" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_INFO_STATS () { static misc::impl::ObsfucatedString s (1343381595, 126943684194701, 0)  /* => "-stats" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_INFO_STATS_FORMAT () { static misc::impl::ObsfucatedString s (1343381595, 7362667909504823693, 2507980, 0)  /* => "-stats-fmt" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_INFO_STATS_AUTO () { static misc::impl::ObsfucatedString s (1343381595, 7002379939315184013, 4801748, 0)  /* => "-stats-auto" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_INFO_PROGRESSION () { static misc::impl::ObsfucatedString s (1343381595, 8315178075582493325, 7811887373960698322, 886987, 0)  /* => "-progression-file" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_INFO_ALIGNMENT_PROGRESS () { static misc::impl::ObsfucatedString s (1343381595, 7308619160717353869, 8243680180221723855, 8321995, 0)  /* => "-alignment-progress" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_INFO_RESOURCES_PROGRESS () { static misc::impl::ObsfucatedString s (1343381595, 7165919078633369741, 8243680180221723588, 8321995, 0)  /* => "-resources-progress" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_INFO_CONFIG_FILE () { static misc::impl::ObsfucatedString s (1343381595, 7165917996000211597, 0)  /* => "-plastrc" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_XML_FILTER_FILE () { static misc::impl::ObsfucatedString s (1343381595, 8389196094506728077, 2507460, 0)  /* => "-xmlfilter" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_SEEDS_USE_RATIO () { static misc::impl::ObsfucatedString s (1343381595, 8443531751420260749, 8028075772638614994, 0)  /* => "-seeds-use-ratio" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_HELP () { static misc::impl::ObsfucatedString s (1343381595, 4713101, 0)  /* => "-h" */; return s.toString().c_str(); }
    static const char* m_STR_OPTION_WORD_SIZE () { return "-W"; }
    static const char* m_MSG_MAIN_RC_FILE () { static misc::impl::ObsfucatedString s (1343381595, 8247343679886830735, 2502850, 0)  /* => "/.plastrc" */; return s.toString().c_str(); }
    static const char* m_MSG_MAIN_HOME () { static misc::impl::ObsfucatedString s (1343381595, 1158335976, 0)  /* => "HOME" */; return s.toString().c_str(); }
    static const char* m_MSG_MAIN_MSG1 () { static misc::impl::ObsfucatedString s (1343381595, 8297073435042303728, 8026294576585316481, 7593480606252461532, 2860265347866683, 0)  /* => "PLAST %s (%ld cores available)" */; return s.toString().c_str(); }
    static const char* m_MSG_MAIN_MSG2 () { static misc::impl::ObsfucatedString s (1343381595, 4702356498814456037, 4993719383365128421, 8297073323378986490, 6783325, 0)  /* => "ERROR BAD PARAMETERS: %s" */; return s.toString().c_str(); }
    static const char* m_MSG_MAIN_MSG3 () { static misc::impl::ObsfucatedString s (1343381595, 2675202303846633701, 2505426, 0)  /* => "ERROR: %s" */; return s.toString().c_str(); }
    static const char* m_MSG_MAIN_MSG4 () { static misc::impl::ObsfucatedString s (1343381595, 2324782450347722727, 3327648105728598756, 886948, 0)  /* => "GENERIC ERROR..." */; return s.toString().c_str(); }
    static const char* m_MSG_INDEXATION_MSG1 () { static misc::impl::ObsfucatedString s (1343389443, 2387225914884044909, 5197229017068056745, 4991201436765245879, 4010997, 0)  /* => "ERROR!!! : BAD HASH CODE */; return s.toString().c_str(); }
    static const char* m_MSG_DATABASE_MSG1 () { static misc::impl::ObsfucatedString s (1343390264, 5641075546145763228, 4849890081459613990, 5207096034462472714, 170775466, 0)  /* => "ERROR IN SEQUENCE SEARCH...*/; return s.toString().c_str(); }
    static const char* m_MSG_HITS_MSG1 () { static misc::impl::ObsfucatedString s (1343395975, 4990918871756489743, 6147209121046379661, 4702919431497603716, 3327673398551885079, 4619206, 0)  /* => "SEQUENCE NOT FOUND IN DATABASE..." */; return s.toString().c_str(); }
    static const char* m_MSG_HITS_MSG2 () { static misc::impl::ObsfucatedString s (1343395975, 7956018178614818101, 344990274250, 0)  /* => "iterating HSP" */; return s.toString().c_str(); }
    static const char* m_MSG_HITS_MSG3 () { static misc::impl::ObsfucatedString s (1343395975, 7453010382209611055, 7598805615233309581, 8314882298439956900, 6281837, 0)  /* => "starting iterating seeds..." */; return s.toString().c_str(); }
    static const char* m_MSG_HITS_MSG4 () { static misc::impl::ObsfucatedString s (1343395975, 7956018178614818101, 8029953785645028042, 8295742029807971769, 2318841635176550694, 7792686838596196808, 3655853, 0)  /* => "iterating all possible seeds...  (%ld/%ld)" */; return s.toString().c_str(); }
    static const char* m_MSG_HITS_MSG5 () { static misc::impl::ObsfucatedString s (1343395975, 7956005061634156602, 8386109761206407882, 7234299921589170339, 778033712, 0)  /* => "finishing iterating seeds..." */; return s.toString().c_str(); }
    static const char* m_MSG_HITS_MSG6 () { static misc::impl::ObsfucatedString s (1343395975, 7956018178614818101, 2338323018188509898, 495631888809, 0)  /* => "iterating seeds codes" */; return s.toString().c_str(); }
    static const char* m_MSG_STATS_MSG1 () { static misc::impl::ObsfucatedString s (1343392514, 2336936577128291182, 7020303014221476755, 2013320981, 0)  /* => "unknown score matrix" */; return s.toString().c_str(); }
    static const char* m_MSG_STATS_MSG2 () { static misc::impl::ObsfucatedString s (1343392611, 7236850772544347590, 8391166479288371401, 8241992077625696319, 2338338411605609534, 7311146936738057394, 8676592001289827243, 7309464380133902754, 7236833202137211765, 8026385544075647680, 50775880001811, 0)  /* => "No found statistical parameters for given matrix and open/extend gap scores..." */; return s.toString().c_str(); }
    static const char* m_MSG_OPTIONS_MSG1 () { static misc::impl::ObsfucatedString s (1343393015, 2675259603455960259, 7233174018651882222, 2318349289970450179, 3342908365586778202, 5764406, 0)  /* => "Option %s already seen, ignored..." */; return s.toString().c_str(); }
    static const char* m_MSG_OPTIONS_MSG2 () { static misc::impl::ObsfucatedString s (1343393015, 2675259603455960259, 2338537461545075438, 2334102053454150168, 8390328290772076100, 2318354511480599665, 3342908365578905168, 3533640, 0)  /* => "Option %s can't be used with option %s, ignored..." */; return s.toString().c_str(); }
    static const char* m_MSG_OPTIONS_MSG3 () { static misc::impl::ObsfucatedString s (1343393015, 6998725098331306968, 8319395793563862511, 7307218078124472666, 7598822093868108307, 198348223863, 0)  /* => "To few arguments for the %s option..." */; return s.toString().c_str(); }
    static const char* m_MSG_OPTIONS_MSG4 () { static misc::impl::ObsfucatedString s (1343393015, 2322204233739790025, 4182456, 0)  /* => "Error : %s*/; return s.toString().c_str(); }
    static const char* m_MSG_OPTIONS_MSG5 () { static misc::impl::ObsfucatedString s (1343393015, 2334956330927257051, 44880134823, 0)  /* => "Warning : %s*/; return s.toString().c_str(); }
    static const char* m_MSG_OPTIONS_MSG6 () { static misc::impl::ObsfucatedString s (1343393015, 2675259603455960259, 7070779454210474734, 8583971276667862815, 2334393405426981722, 7358993290428398967, 7453010395029122390, 8317708060512856646, 494547802895, 0)  /* => "Option %s must be used with one of the following options : %s" */; return s.toString().c_str(); }
    static const char* m_MSG_OPTIONS_MSG7 () { static misc::impl::ObsfucatedString s (1343393015, 2675259603455960259, 7953758403054830318, 133532411297310, 0)  /* => "Option %s is mandatory" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_ALGO_TYPE () { static misc::impl::ObsfucatedString s (1343394373, 2336630925660878162, 7813845548187402837, 8103137004780393217, 8079506584413298189, 8245845144455516674, 8679689244116465975, 2511089, 0)  /* => "Program Name [plastp, tplastn, plastx or tplastx]" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_SUBJECT_URI () { static misc::impl::ObsfucatedString s (1343394373, 2338603393737060945, 7310293695321031295, 435609863744, 0)  /* => "Subject database file" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_QUERY_URI () { static misc::impl::ObsfucatedString s (1343394373, 7017769825329967699, 7358993337354109551, 5442569, 0)  /* => "Query database file" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_OUTPUT_FILE () { static misc::impl::ObsfucatedString s (1343394373, 7309940691719375698, 8391700481485130859, 7308332045225301264, 0)  /* => "PLAST report Output File" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_EVALUE () { static misc::impl::ObsfucatedString s (1343394373, 8386111951388442439, 8461244959901174898, 3563525, 0)  /* => "Expectation value" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_UNGAP_NEIGHBOUR_LENGTH () { static misc::impl::ObsfucatedString s (1343394373, 2334675641764441681, 8462090425404900981, 7309377810959239186, 2334956330900738055, 7234312004409924891, 7598538378327304247, 2506435, 0)  /* => "Size of neighbourhood peforming ungapped extension" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_UNGAP_SCORE_THRESHOLD () { static misc::impl::ObsfucatedString s (1343394373, 7234312004410632535, 8027793215337748283, 7451040048802563084, 7020393977047239940, 7309465758872215298, 8317697112708115827, 4734917, 0)  /* => "Ungapped threshold trigger a small gapped extension" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_SMALLGAP_THRESHOLD () { static misc::impl::ObsfucatedString s (1343394373, 7813578723338776438, 7886683048323908479, 8102082825191820289, 7954892377262480132, 1849334301, 0)  /* => "threshold for small gapped extension" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_SMALLGAP_BAND_WITH () { static misc::impl::ObsfucatedString s (1343394373, 7526756838746121824, 7020393977337103675, 7309465758865951756, 8317697112711372549, 3906567, 0)  /* => "bandwith for small gapped extension" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_NB_PROCESSORS () { static misc::impl::ObsfucatedString s (1343394373, 8007525917229806156, 8315161617321362301, 2337214414350192403, 7621652, 0)  /* => "Number of processors to use" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_OPEN_GAP_COST () { static misc::impl::ObsfucatedString s (1343394373, 2337214414365222977, 7431046177081152372, 3543041, 0)  /* => "Cost to open a gap" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_EXTEND_GAP_COST () { static misc::impl::ObsfucatedString s (1343394373, 2337214414365222977, 6998704246243941246, 1884751680, 0)  /* => "Cost to extend a gap" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_X_DROPOFF_GAPPED () { static misc::impl::ObsfucatedString s (1343394373, 7381241938243888986, 2334401090212135805, 8102082825190379270, 7955443180981279492, 7955934336580818435, 2891356536621117239, 8533873931941328342, 7378413655050304850, 7522526644123385723, 45571471275570, 0)  /* => "X dropoff value for gapped alignment (in bits) (zero invokes default behavior)" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_X_DROPOFF_FINAL () { static misc::impl::ObsfucatedString s (1343394373, 7381241938243888986, 2334401090212135805, 7020664756466093830, 7234312004414349069, 7308619160718250574, 7593667580280650105, 2319404592460604376, 2338324143637346388, 2338613357913354110, 8245925399899039025, 6466705, 0)  /* => "X dropoff value for final gapped alignment in bits (0.0 invokes default behavior)" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_FILTER_QUERY () { static misc::impl::ObsfucatedString s (1343394373, 8151641105607585348, 8171063682246461038, 435458409749, 0)  /* => "Filter query sequence" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_SCORE_MATRIX () { static misc::impl::ObsfucatedString s (1343394373, 7020303014220358737, 5494998615271035247, 8007455346272711471, 5572451713988894483, 8176475, 0)  /* => "Score matrix (BLOSUM62 or BLOSUM50)" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_STRAND () { static misc::impl::ObsfucatedString s (1343394373, 2338323056774640497, 8314045615938292861, 8461261113266015764, 7956010231538718738, 2819379188345241627, 7217053756487311989, 11668483218028233, 0)  /* => "strands for plastn: 'plus', 'minus' or 'both' (default)" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_REWARD () { static misc::impl::ObsfucatedString s (1343394373, 7358992233542804080, 7166755467379036532, 2334382385947804940, 8081745164032630284, 45554371826178, 0)  /* => "reward for a nucleotide match (plastn)" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_PENALTY () { static misc::impl::ObsfucatedString s (1343394373, 2340029490231796338, 8461736351403166845, 7306080444306623491, 7166460029769896513, 8391157649093022470, 2581625, 0)  /* => "penalty for a nucleotide mismatch (plastn)" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_FORCE_QUERY_ORDERING () { static misc::impl::ObsfucatedString s (1343394373, 8462580794633250884, 8245845122734772606, 7575168297147303172, 8391737126164504335, 51013142484302, 0)  /* => "Force queries ordering in output file." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_MAX_DATABASE_SIZE () { static misc::impl::ObsfucatedString s (1343394373, 2336652894450902607, 2334102070530625402, 7955934336330238227, 2317510362840718657, 7017769720873458696, 2318902404369951843, 8386095523169711845, 7022344801645438040, 2338328219398024056, 7310589477034222880, 6456028, 0)  /* => "Maximum allowed size (in bytes) for a database. If greater, database is segmented." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_MAX_HIT_PER_QUERY () { return "Maximum hits per query. 0 value will dump all hits (default)"; }
    static const char* m_STR_HELP_MAX_HSP_PER_HIT () { return "Maximum alignments per hit. 0 value will dump all hits (default)"; }
    static const char* m_STR_HELP_MAX_HIT_PER_ITERATION () { static misc::impl::ObsfucatedString s (1343394373, 2336652894450902607, 8243117977648712307, 7598805615233861952, 2338053641110376718, 8439879234091521539, 7957688057375763556, 3539880062558479064, 7070704398501897741, 7815259820787427939, 3276839, 0)  /* => "Maximum hits per iteration (for memory usage control). 1000000 by default" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_OUTPUT_FORMAT () { static misc::impl::ObsfucatedString s (1343394373, 7359009838865478221, 3539893578639233396, 7089074971164083776, 2891421347679851284, 2987131704251225610, 2575673, 0)  /* => "Output format: 1 for tabulated (default)." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_STRANDS_LIST () { static misc::impl::ObsfucatedString s (1343394373, 2334675642016689742, 7021802806838446959, 4213228674419590158, 2465206440032214337, 2334379873304433479, 7307221376413351522, 2334956331000361154, 7598545778468469340, 7308325651326348660, 7017769799327118396, 3347130494289497548, 0)  /* => "List of the strands (ex: "1,2,6") to be used when using algo using nucleotids databases." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_CODON_STOP_OPTIM () { static misc::impl::ObsfucatedString s (1343394373, 2334675641764441713, 8028911400106740591, 7453001576141161751, 7306377312280452868, 7020021539074593536, 7809653445417711972, 7021781869989868741, 7236828443213177438, 8675461342618369594, 7142832633003712551, 1846397143, 0)  /* => "size of the allowed range between the last invalid character and the next stop codon" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_FACTORY_DISPATCHER () { static misc::impl::ObsfucatedString s (1343394373, 2340027304412538436, 7310014136219749231, 8316288277452887041, 3346848972108291601, 0)  /* => "Factory that creates dispatcher." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_FACTORY_INDEXATION () { static misc::impl::ObsfucatedString s (1343394373, 2340027304412538436, 7310014136219749231, 7236837239325660161, 2336927755367630596, 3346848955076740620, 0)  /* => "Factory that creates indexation builder." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_FACTORY_HIT_UNGAP () { static misc::impl::ObsfucatedString s (1343394373, 2340027304412538436, 7310014136219749231, 7453023215578977281, 2338340627603869440, 8245937404615100167, 2575673, 0)  /* => "Factory that creates ungap hits iterator." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_FACTORY_HIT_SMALLGAP () { static misc::impl::ObsfucatedString s (1343394373, 2340027304412538436, 7310014136219749231, 7020393977351443457, 7503120542844670733, 8243122687831344903, 199485831542, 0)  /* => "Factory that creates small gap hits iterator." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_FACTORY_HIT_FULLGAP () { static misc::impl::ObsfucatedString s (1343394373, 2340027304412538436, 7310014136219749231, 7815265017931174913, 7595356438603898637, 7021786319683944474, 777331299, 0)  /* => "Factory that creates full gap hits iterator." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_FACTORY_HIT_COMPOSITION () { static misc::impl::ObsfucatedString s (1343394373, 2340027304412538436, 7310014136219749231, 7885630463573955585, 7957695015292129297, 8388271461197716302, 13073671946387314, 0)  /* => "Factory that creates composition hits iterator." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_FACTORY_GAP_RESULT () { static misc::impl::ObsfucatedString s (1343394373, 2340027304412538436, 7310014136219749231, 8097866994060301313, 7308619160722693697, 8463219665599366912, 604539, 0)  /* => "Factory that creates gap alignments result." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_FACTORY_UNGAP_RESULT () { static misc::impl::ObsfucatedString s (1343394373, 2340027304412538436, 7310014136219749231, 7453023215578977281, 7955443180981276416, 7309940825419126275, 199519582308, 0)  /* => "Factory that creates ungap alignments result." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_FACTORY_SPLITTER () { static misc::impl::ObsfucatedString (1347439764, 2340027304408860355, 7310014136228024502, 2336912048655163834, 7954884637765327937, 8391447920160585045, 4744267, 0)  /* => "Factory that creates an alignment splitter." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_OPTIM_FILTER_UNGAP () { static misc::impl::ObsfucatedString s (1343394373, 7024042487474714445, 7018987701898070639, 8243122701943717908, 7526676561802436370, 7959303549226097692, 7451606222871407728, 13074792831991234, 0)  /* => "Optimization that filters out through ungap alignments." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_XML_FILTER_FILE () { static misc::impl::ObsfucatedString s (1343394373, 6998706448401397079, 7811887317556820795, 7308332182664381716, 1130319, 0)  /* => "Uri of a XML filter file." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_SEEDS_USE_RATIO () { static misc::impl::ObsfucatedString s (1343394373, 7381153977161639504, 8367814982690295867, 7310315402151870479, 1118469, 0)  /* => "Ratio of seeds to be used." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_HELP () { static misc::impl::ObsfucatedString s (1343394373, 1882612330, 0)  /* => "help" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_INFO_BARGRAPH () { static misc::impl::ObsfucatedString s (1343395004, 2340008599775699785, 7310018873543462987, 7214892365631414352, 8675375885802338877, 3345734071894036172, 0)  /* => "Display a progress bar during execution." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_INFO_BARGRAPH_SIZE () { static misc::impl::ObsfucatedString s (1343395004, 7521891426971314243, 8318823012273275467, 2334386829830487043, 7525622121320933674, 6237575, 0)  /* => "Nb of characters of the bargraph." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_INFO_PROGRESSION () { static misc::impl::ObsfucatedString s (1343395004, 2336920844705693513, 8367799623961776203, 7310030993675744843, 8458716092904635430, 8243117977561482461, 3343191966649828725, 0)  /* => "Dump in a file the current execution percentage." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_INFO_VERBOSE () { static misc::impl::ObsfucatedString s (1343395004, 2340008599775699785, 8386104319411478083, 7598264594228673610, 8245923157927915302, 7311705184981703104, 13069273710744949, 0)  /* => "Display information during algorithm execution." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_INFO_FULL_STATS () { static misc::impl::ObsfucatedString s (1343395004, 7452438275352133449, 8295750809022153285, 7163384721348981335, 5613115, 0)  /* => "Dump algorithm statistics." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_INFO_STATS () { static misc::impl::ObsfucatedString s (1343395004, 7954877705850421065, 7022364301930054223, 3347128252642654807, 0)  /* => "Dump generic statistics." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_INFO_STATS_FORMAT () { static misc::impl::ObsfucatedString s (1343395004, 8007528099270649163, 8316306148839297100, 8225578679831394903, 7378413688022302505, 8245844805153937608, 43346639456054, 0)  /* => "Format of statistics: 'raw' (default) or 'xml'" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_INFO_STATS_AUTO () { static misc::impl::ObsfucatedString s (1343395004, 7598805593938886476, 2338340593458384969, 7310014135967644229, 474312670249, 0)  /* => "Automatic stats file creation" */; return s.toString().c_str(); }
    static const char* m_STR_HELP_INFO_ALIGNMENT_PROGRESS () { static misc::impl::ObsfucatedString s (1343395004, 2336920844705693513, 8367799623961776203, 7599665433358333515, 7305521896680189734, 7453023215358578139, 8097867329101844599, 7308619160716100927, 7598264594228681722, 8245923157923053163, 199413002667, 0)  /* => "Dump in a file the growing number of ungap/ungap alignments during algorithm." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_INFO_RESOURCES_PROGRESS () { static misc::impl::ObsfucatedString s (1343395004, 2336920844705693513, 7575166089544568907, 7598805593979909453, 8391735949007382055, 7165919078634905481, 7956016065246679923, 7598258011313112184, 779323360, 0)  /* => "Dump in a file information about resources during algorithm." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_INFO_CONFIG_FILE () { static misc::impl::ObsfucatedString s (1343395004, 7308604897127721821, 2334386829838769930, 8026294645428485971, 7811887318007868966, 6226892, 0)  /* => "Pathname of the plast config file." */; return s.toString().c_str(); }
    static const char* m_STR_HELP_WORD_SIZE  () { return "size of the seeds"; }
    static const char* m_STR_CONFIG_CLASS_SerialCommandDispatcher () { static misc::impl::ObsfucatedString s (1343396678, 8017370927284409692, 8316253385052224169, 32199646361063685, 0)  /* => "SerialCommandDispatcher" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_ParallelCommandDispatcher () { static misc::impl::ObsfucatedString s (1343396678, 7810768341489209695, 4928185256971958407, 7307199746907210524, 5036928, 0)  /* => "ParallelCommandDispatcher" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_BasicIndexator () { static misc::impl::ObsfucatedString s (1343396678, 7236802342548787533, 125823016071073, 0)  /* => "BasicIndexator" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_BasicSortedIndexator () { static misc::impl::ObsfucatedString s (1343396678, 8245901129172767053, 8675450682035696304, 1915061268, 0)  /* => "BasicSortedIndexator" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_UngapHitIteratorNull () { static misc::impl::ObsfucatedString s (1343396678, 8388315428459503194, 8245937404614632333, 1814463803, 0)  /* => "UngapHitIteratorNull" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_UngapHitIterator () { static misc::impl::ObsfucatedString s (1343396678, 8388315428459503194, 8245937404614632333, 0)  /* => "UngapHitIterator" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_UngapHitIteratorSSE16 () { static misc::impl::ObsfucatedString s (1343396678, 8388315428459503194, 8245937404614632333, 232751342374, 0)  /* => "UngapHitIteratorSSE16" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_SmallGapHitIterator () { static misc::impl::ObsfucatedString s (1343396678, 8097832135987551580, 7021786319143075468, 3710721, 0)  /* => "SmallGapHitIterator" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_SmallGapHitIteratorNull () { static misc::impl::ObsfucatedString s (1343396678, 8097832135987551580, 7021786319143075468, 30518548564057857, 0)  /* => "SmallGapHitIteratorNull" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_SmallGapHitIteratorSSE8 () { static misc::impl::ObsfucatedString s (1343396678, 8097832135987551580, 7021786319143075468, 15838822876618497, 0)  /* => "SmallGapHitIteratorSSE8" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_FullGapHitIterator () { static misc::impl::ObsfucatedString s (1343396678, 5219778927509816649, 8386109761204347821, 4882970, 0)  /* => "FullGapHitIterator" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_FullGapHitIteratorNull () { static misc::impl::ObsfucatedString s (1343396678, 5219778927509816649, 8386109761204347821, 119213075497498, 0)  /* => "FullGapHitIteratorNull" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_CompositionHitIterator () { static misc::impl::ObsfucatedString s (1343396678, 8388362703416844108, 8379356577230125229, 125823015944720, 0)  /* => "CompositionHitIterator" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_CompositionHitIteratorNull () { static misc::impl::ObsfucatedString s (1343396678, 8388362703416844108, 8379356577230125229, 8452819373636944400, 5027742, 0)  /* => "CompositionHitIteratorNull" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_BasicAlignmentResult () { static misc::impl::ObsfucatedString s (1343396678, 7596518566691983693, 7301025963412912547, 1948681478, 0)  /* => "BasicAlignmentResult" */; return s.toString().c_str(); }
    static const char* m_STR_CONFIG_CLASS_UngapAlignmentResult () { static misc::impl::ObsfucatedString s (1343396678, 7596518622391033434, 7301025963412912547, 1948681478, 0)  /* => "UngapAlignmentResult" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_params () { static misc::impl::ObsfucatedString s (1343397334, 126913619857199, 0)  /* => "params" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_seedModelKind () { static misc::impl::ObsfucatedString s (1343397334, 7306086873685658412, 431347073464, 0)  /* => "seedModelKind" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_seedSpan () { static misc::impl::ObsfucatedString s (1343397334, 7953761920378309420, 0)  /* => "seedSpan" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_subseedStrings () { static misc::impl::ObsfucatedString s (1343397334, 6009039289217358636, 126888069183648, 0)  /* => "subseedStrings" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_matrixKind () { static misc::impl::ObsfucatedString s (1343397334, 7587290391519203122, 2305722, 0)  /* => "matrixKind" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_subject () { static misc::impl::ObsfucatedString s (1343397334, 32760384522459948, 0)  /* => "subject" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_uri () { static misc::impl::ObsfucatedString s (1343397334, 2202666, 0)  /* => "uri" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_range () { static misc::impl::ObsfucatedString s (1343397334, 435522277165, 0)  /* => "range" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_begin () { static misc::impl::ObsfucatedString s (1343397334, 474211126077, 0)  /* => "begin" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_end () { static misc::impl::ObsfucatedString s (1343397334, 2916410, 0)  /* => "end" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_filterQuery () { static misc::impl::ObsfucatedString s (1343397334, 8453663755616945977, 5912753, 0)  /* => "filterQuery" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_ungapNeighbourLength () { static misc::impl::ObsfucatedString s (1343397334, 7594562591204016170, 7299334944764011187, 1745248171, 0)  /* => "ungapNeighbourLength" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_ungapScoreThreshold () { static misc::impl::ObsfucatedString s (1343397334, 8026350703036301354, 7526470944319549350, 1463466, 0)  /* => "ungapScoreThreshold" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_smallGapBandLength () { static misc::impl::ObsfucatedString s (1343397334, 8097832135985562412, 7453005812119448470, 7491761, 0)  /* => "smallGapBandLength" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_smallGapBandWidth () { static misc::impl::ObsfucatedString s (1343397334, 8097832135985562412, 8386944230136556438, 7485613, 0)  /* => "smallGapBandWidth" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_smallGapThreshold () { static misc::impl::ObsfucatedString s (1343397334, 8097832135985562412, 7813578723341443712, 7485601, 0)  /* => "smallGapThreshold" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_openGapCost () { static misc::impl::ObsfucatedString s (1343397334, 4859490957351820848, 5716411, 0)  /* => "openGapCost" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_extendGapCost () { static misc::impl::ObsfucatedString s (1343397334, 7009681770333115962, 500150569380, 0)  /* => "extendGapCost" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_evalue () { static misc::impl::ObsfucatedString s (1343397334, 111555000244282, 0)  /* => "evalue" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_XdroppofGap () { static misc::impl::ObsfucatedString s (1343397334, 7381241942488287751, 5450643, 0)  /* => "XdroppofGap" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_finalXdroppofGap () { static misc::impl::ObsfucatedString s (1343397334, 8242810440504477497, 8097832110268824251, 0)  /* => "finalXdroppofGap" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_outputfile () { static misc::impl::ObsfucatedString s (1343397334, 7594885869349935920, 2305976, 0)  /* => "outputfile" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_strands () { static misc::impl::ObsfucatedString s (1343397751, 32480047562085503, 0)  /* => "strands" */; return s.toString().c_str(); }
    static const char* m_STR_PARAM_wordSize () { return "wordSize"; }
};

#else

class StringRepository
{
public:
    static const char* m_STR_OPTION_ALGO_TYPE () { return "-p"; }
    static const char* m_STR_OPTION_SUBJECT_URI () { return "-d"; }
    static const char* m_STR_OPTION_QUERY_URI () { return "-i"; }
    static const char* m_STR_OPTION_OUTPUT_FILE () { return "-o"; }
    static const char* m_STR_OPTION_EVALUE () { return "-e"; }
    static const char* m_STR_OPTION_UNGAP_NEIGHBOUR_LENGTH () { return "-n"; }
    static const char* m_STR_OPTION_UNGAP_SCORE_THRESHOLD () { return "-s"; }
    static const char* m_STR_OPTION_SMALLGAP_THRESHOLD () { return "-g"; }
    static const char* m_STR_OPTION_SMALLGAP_BAND_WITH () { return "-b"; }
    static const char* m_STR_OPTION_NB_PROCESSORS () { return "-a"; }
    static const char* m_STR_OPTION_OPEN_GAP_COST () { return "-G"; }
    static const char* m_STR_OPTION_EXTEND_GAP_COST () { return "-E"; }
    static const char* m_STR_OPTION_X_DROPOFF_UNGAPPED () { return "-xdrop-ungap"; }
    static const char* m_STR_OPTION_X_DROPOFF_GAPPED () { return "-X"; }
    static const char* m_STR_OPTION_X_DROPOFF_FINAL () { return "-Z"; }
    static const char* m_STR_OPTION_INDEX_NEIGHBOUR_THRESHOLD () { return "-index-threshold"; }
    static const char* m_STR_OPTION_FILTER_QUERY () { return "-F"; }
    static const char* m_STR_OPTION_SCORE_MATRIX () { return "-M"; }
    static const char* m_STR_OPTION_REWARD () { return "-r"; }
    static const char* m_STR_OPTION_PENALTY () { return "-q"; }
    static const char* m_STR_OPTION_OUTPUT_FORMAT () { return "-outfmt"; }
    static const char* m_STR_OPTION_STRAND () { return "-strand"; }
    static const char* m_STR_OPTION_FORCE_QUERY_ORDERING () { return "-force-query-order"; }
    static const char* m_STR_OPTION_MAX_DATABASE_SIZE () { return "-max-database-size"; }
    static const char* m_STR_OPTION_MAX_HIT_PER_QUERY () { return "-max-hit-per-query"; }
    static const char* m_STR_OPTION_MAX_HSP_PER_HIT () { return "-max-hsp-per-hit"; }
    static const char* m_STR_OPTION_MAX_HIT_PER_ITERATION () { return "-max-hit-per-iteration"; }
    static const char* m_STR_OPTION_STRANDS_LIST () { return "-strands-list"; }
    static const char* m_STR_OPTION_CODON_STOP_OPTIM () { return "-optim-codon-stop"; }
    static const char* m_STR_OPTION_FACTORY_DISPATCHER () { return "-factory-dispatcher"; }
    static const char* m_STR_OPTION_FACTORY_STATISTICS () { return "-factory-statistics"; }
    static const char* m_STR_OPTION_FACTORY_INDEXATION () { return "-factory-indexation"; }
    static const char* m_STR_OPTION_FACTORY_HIT_UNGAP () { return "-factory-hit-ungap"; }
    static const char* m_STR_OPTION_FACTORY_HIT_SMALLGAP () { return "-factory-hit-smallgap"; }
    static const char* m_STR_OPTION_FACTORY_HIT_FULLGAP () { return "-factory-hit-fullgap"; }
    static const char* m_STR_OPTION_FACTORY_HIT_COMPOSITION () { return "-factory-hit-composition"; }
    static const char* m_STR_OPTION_FACTORY_GAP_RESULT () { return "-factory-gap-result"; }
    static const char* m_STR_OPTION_FACTORY_UNGAP_RESULT () { return "-factory-ungap-result"; }
    static const char* m_STR_OPTION_FACTORY_SPLITTER () { return "-splitter"; }
    static const char* m_STR_OPTION_OPTIM_FILTER_UNGAP () { return "-optim-filter-ungap"; }
    static const char* m_STR_OPTION_INFO_BARGRAPH () { return "-bargraph"; }
    static const char* m_STR_OPTION_INFO_BARGRAPH_SIZE () { return "-bargraph-size"; }
    static const char* m_STR_OPTION_INFO_VERBOSE () { return "-verbose"; }
    static const char* m_STR_OPTION_INFO_FULL_STATS () { return "-full-stats"; }
    static const char* m_STR_OPTION_INFO_STATS () { return "-stats"; }
    static const char* m_STR_OPTION_INFO_STATS_FORMAT () { return "-stats-fmt"; }
    static const char* m_STR_OPTION_INFO_STATS_AUTO () { return "-stats-auto"; }
    static const char* m_STR_OPTION_INFO_PROGRESSION () { return "-progression-file"; }
    static const char* m_STR_OPTION_INFO_ALIGNMENT_PROGRESS () { return "-alignment-progress"; }
    static const char* m_STR_OPTION_INFO_RESOURCES_PROGRESS () { return "-resources-progress"; }
    static const char* m_STR_OPTION_INFO_CONFIG_FILE () { return "-plastrc"; }
    static const char* m_STR_OPTION_XML_FILTER_FILE () { return "-xmlfilter"; }
    static const char* m_STR_OPTION_SEEDS_USE_RATIO () { return "-seeds-use-ratio"; }
    static const char* m_STR_OPTION_INDEX_FILTER_SEED () { return "-seeds-index-filter"; }
    static const char* m_STR_OPTION_HELP () { return "-h"; }
    static const char* m_STR_OPTION_WORD_SIZE () { return "-W"; }
    static const char* m_MSG_MAIN_RC_FILE () { return "/.plastrc"; }
    static const char* m_MSG_MAIN_HOME () { return "HOME"; }
    static const char* m_MSG_MAIN_MSG1 () { return "PLAST %s (%ld cores available)\n"; }
    static const char* m_MSG_MAIN_MSG2 () { return "ERROR BAD PARAMETERS: %s\n"; }
    static const char* m_MSG_MAIN_MSG3 () { return "ERROR: %s\n"; }
    static const char* m_MSG_MAIN_MSG4 () { return "GENERIC ERROR...\n"; }
    static const char* m_MSG_INDEXATION_MSG1 () { return "ERROR!!! : BAD HASH CODE\n"; }
    static const char* m_MSG_FILE_BLAST_MSG1 () { return "ERROR DURING BLAST FORMAT READING...\n"; }
    static const char* m_MSG_FILE_BLAST_MSG2 () { return "ERROR DURING BLAST FORMAT PARSING...\n"; }
    static const char* m_MSG_FILE_BLAST_MSG3 () { return "ERROR DURING ASN1 PARSING...\n"; }
    static const char* m_MSG_DATABASE_MSG1 () { return "ERROR IN SEQUENCE SEARCH...\n"; }
    static const char* m_MSG_HITS_MSG1 () { return "SEQUENCE NOT FOUND IN DATABASE...\n"; }
    static const char* m_MSG_HITS_MSG2 () { return "iterating HSP"; }
    static const char* m_MSG_HITS_MSG3 () { return "starting iterating seeds..."; }
    static const char* m_MSG_HITS_MSG4 () { return "iterating all possible seeds...  (%ld/%ld)"; }
    static const char* m_MSG_HITS_MSG5 () { return "finishing iterating seeds..."; }
    static const char* m_MSG_HITS_MSG6 () { return "iterating seeds codes"; }
    static const char* m_MSG_STATS_MSG1 () { return "unknown score matrix"; }
    static const char* m_MSG_STATS_MSG2 () { return "No found statistical parameters for given matrix and open/extend gap scores..."; }
    static const char* m_MSG_OPTIONS_MSG1 () { return "Option %s already seen, ignored..."; }
    static const char* m_MSG_OPTIONS_MSG2 () { return "Option %s can't be used with option %s, ignored..."; }
    static const char* m_MSG_OPTIONS_MSG3 () { return "To few arguments for the %s option..."; }
    static const char* m_MSG_OPTIONS_MSG4 () { return "Error : %s\n"; }
    static const char* m_MSG_OPTIONS_MSG5 () { return "Warning : %s\n"; }
    static const char* m_MSG_OPTIONS_MSG6 () { return "Option %s must be used with one of the following options : %s"; }
    static const char* m_MSG_OPTIONS_MSG7 () { return "Option %s is mandatory"; }
    static const char* m_STR_HELP_ALGO_TYPE () { return "Program Name [plastp, tplastn, plastx, tplastx or plastn]"; }
    static const char* m_STR_HELP_SUBJECT_URI () { return "Subject database file"; }
    static const char* m_STR_HELP_QUERY_URI () { return "Query database file"; }
    static const char* m_STR_HELP_OUTPUT_FILE () { return "PLAST report Output File"; }
    static const char* m_STR_HELP_EVALUE () { return "Expectation value"; }
    static const char* m_STR_HELP_UNGAP_NEIGHBOUR_LENGTH () { return "Size of neighbourhood peforming ungapped extension"; }
    static const char* m_STR_HELP_UNGAP_SCORE_THRESHOLD () { return "Ungapped threshold trigger a small gapped extension"; }
    static const char* m_STR_HELP_SMALLGAP_THRESHOLD () { return "threshold for small gapped extension"; }
    static const char* m_STR_HELP_SMALLGAP_BAND_WITH () { return "bandwith for small gapped extension"; }
    static const char* m_STR_HELP_NB_PROCESSORS () { return "Number of processors to use"; }
    static const char* m_STR_HELP_OPEN_GAP_COST () { return "Cost to open a gap"; }
    static const char* m_STR_HELP_EXTEND_GAP_COST () { return "Cost to extend a gap"; }
    static const char* m_STR_HELP_X_DROPOFF_UNGAPPED () { return "X dropoff value for Ungapped alignment (in bits) (zero invokes default behavior 20 bits)"; }
    static const char* m_STR_HELP_X_DROPOFF_GAPPED () { return "X dropoff value for gapped alignment (in bits) (zero invokes default behavior)"; }
    static const char* m_STR_HELP_X_DROPOFF_FINAL () { return "X dropoff value for final gapped alignment in bits (0.0 invokes default behavior)"; }
    static const char* m_STR_HELP_INDEX_NEIGHBOUR_THRESHOLD () { return "Index threshold to calculate the similarity between neighbour"; }
    static const char* m_STR_HELP_FILTER_QUERY () { return "Filter query sequence"; }
    static const char* m_STR_HELP_SCORE_MATRIX () { return "Score matrix (BLOSUM62 or BLOSUM50)"; }
    static const char* m_STR_HELP_STRAND () { return "strands for plastn: 'plus', 'minus' or 'both' (default)"; }
    static const char* m_STR_HELP_REWARD () { return "reward for a nucleotide match (plastn)"; }
    static const char* m_STR_HELP_PENALTY () { return "penalty for a nucleotide mismatch (plastn)"; }
    static const char* m_STR_HELP_FORCE_QUERY_ORDERING () { return "Force queries ordering in output file."; }
    static const char* m_STR_HELP_MAX_DATABASE_SIZE () { return "Maximum allowed size (in bytes) for a database. If greater, database is segmented."; }
    static const char* m_STR_HELP_MAX_HIT_PER_QUERY () { return "Maximum hits per query. 0 value will dump all hits (default)"; }
    static const char* m_STR_HELP_MAX_HSP_PER_HIT () { return "Maximum alignments per hit. 0 value will dump all hits (default)"; }
    static const char* m_STR_HELP_MAX_HIT_PER_ITERATION () { return "Maximum hits per iteration (for memory usage control). 1000000 by default"; }
    static const char* m_STR_HELP_OUTPUT_FORMAT () { return "Output format: 1 for tabulated (default)."; }
    static const char* m_STR_HELP_STRANDS_LIST () { return "List of the strands (ex: \"1,2,6\") to be used when using algo using nucleotids databases."; }
    static const char* m_STR_HELP_CODON_STOP_OPTIM () { return "size of the allowed range between the last invalid character and the next stop codon"; }
    static const char* m_STR_HELP_FACTORY_DISPATCHER () { return "Factory that creates dispatcher."; }
    static const char* m_STR_HELP_FACTORY_STATISTICS () { return "Factory that creates statistics builder."; }
    static const char* m_STR_HELP_FACTORY_INDEXATION () { return "Factory that creates indexation builder."; }
    static const char* m_STR_HELP_FACTORY_HIT_UNGAP () { return "Factory that creates ungap hits iterator."; }
    static const char* m_STR_HELP_FACTORY_HIT_SMALLGAP () { return "Factory that creates small gap hits iterator."; }
    static const char* m_STR_HELP_FACTORY_HIT_FULLGAP () { return "Factory that creates full gap hits iterator."; }
    static const char* m_STR_HELP_FACTORY_HIT_COMPOSITION () { return "Factory that creates composition hits iterator."; }
    static const char* m_STR_HELP_FACTORY_GAP_RESULT () { return "Factory that creates gap alignments result."; }
    static const char* m_STR_HELP_FACTORY_UNGAP_RESULT () { return "Factory that creates ungap alignments result."; }
    static const char* m_STR_HELP_FACTORY_SPLITTER () { return "Factory that creates an alignment splitter. String: 'normal' or 'banded' (default)"; }
    static const char* m_STR_HELP_OPTIM_FILTER_UNGAP () { return "Optimization that filters out through ungap alignments."; }
    static const char* m_STR_HELP_XML_FILTER_FILE () { return "Uri of a XML filter file."; }
    static const char* m_STR_HELP_SEEDS_USE_RATIO () { return "Ratio of seeds to be used."; }
    static const char* m_STR_HELP_INDEX_FILTER_SEED () { return "seeds length to be used for the indexation filter."; }
    static const char* m_STR_HELP_HELP () { return "help"; }
    static const char* m_STR_HELP_INFO_BARGRAPH () { return "Display a progress bar during execution."; }
    static const char* m_STR_HELP_INFO_BARGRAPH_SIZE () { return "Nb of characters of the bargraph."; }
    static const char* m_STR_HELP_INFO_PROGRESSION () { return "Dump in a file the current execution percentage."; }
    static const char* m_STR_HELP_INFO_VERBOSE () { return "Display information during algorithm execution."; }
    static const char* m_STR_HELP_INFO_FULL_STATS () { return "Dump algorithm statistics."; }
    static const char* m_STR_HELP_INFO_STATS () { return "Dump generic statistics."; }
    static const char* m_STR_HELP_INFO_STATS_FORMAT () { return "Format of statistics: 'raw' (default) or 'xml'"; }
    static const char* m_STR_HELP_INFO_STATS_AUTO () { return "Automatic stats file creation"; }
    static const char* m_STR_HELP_INFO_ALIGNMENT_PROGRESS () { return "Dump in a file the growing number of ungap/ungap alignments during algorithm."; }
    static const char* m_STR_HELP_INFO_RESOURCES_PROGRESS () { return "Dump in a file information about resources during algorithm."; }
    static const char* m_STR_HELP_INFO_CONFIG_FILE () { return "Pathname of the plast config file."; }
    static const char* m_STR_HELP_WORD_SIZE  () { return "size of the seeds"; }
    static const char* m_STR_CONFIG_CLASS_KarlinStats () { return "KarlinStats"; }
    static const char* m_STR_CONFIG_CLASS_SpougeStats () { return "SpougeStats"; }
    static const char* m_STR_CONFIG_CLASS_SerialCommandDispatcher () { return "SerialCommandDispatcher"; }
    static const char* m_STR_CONFIG_CLASS_ParallelCommandDispatcher () { return "ParallelCommandDispatcher"; }
    static const char* m_STR_CONFIG_CLASS_BasicIndexator () { return "BasicIndexator"; }
    static const char* m_STR_CONFIG_CLASS_BasicSortedIndexator () { return "BasicSortedIndexator"; }
    static const char* m_STR_CONFIG_CLASS_BasicIndexatorOptim () { return "BasicIndexatorOptim"; }
    static const char* m_STR_CONFIG_CLASS_UngapHitIteratorNull () { return "UngapHitIteratorNull"; }
    static const char* m_STR_CONFIG_CLASS_UngapHitIterator () { return "UngapHitIterator"; }
    static const char* m_STR_CONFIG_CLASS_UngapHitIteratorSSE16 () { return "UngapHitIteratorSSE16"; }
    static const char* m_STR_CONFIG_CLASS_SmallGapHitIterator () { return "SmallGapHitIterator"; }
    static const char* m_STR_CONFIG_CLASS_SmallGapHitIteratorNull () { return "SmallGapHitIteratorNull"; }
    static const char* m_STR_CONFIG_CLASS_SmallGapHitIteratorSSE8 () { return "SmallGapHitIteratorSSE8"; }
    static const char* m_STR_CONFIG_CLASS_FullGapHitIterator () { return "FullGapHitIterator"; }
    static const char* m_STR_CONFIG_CLASS_FullGapHitIteratorNull () { return "FullGapHitIteratorNull"; }
    static const char* m_STR_CONFIG_CLASS_CompositionHitIterator () { return "CompositionHitIterator"; }
    static const char* m_STR_CONFIG_CLASS_CompositionHitIteratorNull () { return "CompositionHitIteratorNull"; }
    static const char* m_STR_CONFIG_CLASS_BasicAlignmentResult () { return "BasicAlignmentResult"; }
    static const char* m_STR_CONFIG_CLASS_UngapAlignmentResult () { return "UngapAlignmentResult"; }
    static const char* m_STR_PARAM_params () { return "params"; }
    static const char* m_STR_PARAM_seedModelKind () { return "seedModelKind"; }
    static const char* m_STR_PARAM_seedSpan () { return "seedSpan"; }
    static const char* m_STR_PARAM_subseedStrings () { return "subseedStrings"; }
    static const char* m_STR_PARAM_matrixKind () { return "matrixKind"; }
    static const char* m_STR_PARAM_subject () { return "subject"; }
    static const char* m_STR_PARAM_uri () { return "uri"; }
    static const char* m_STR_PARAM_range () { return "range"; }
    static const char* m_STR_PARAM_begin () { return "begin"; }
    static const char* m_STR_PARAM_end () { return "end"; }
    static const char* m_STR_PARAM_filterQuery () { return "filterQuery"; }
    static const char* m_STR_PARAM_ungapNeighbourLength () { return "ungapNeighbourLength"; }
    static const char* m_STR_PARAM_ungapScoreThreshold () { return "ungapScoreThreshold"; }
    static const char* m_STR_PARAM_smallGapBandLength () { return "smallGapBandLength"; }
    static const char* m_STR_PARAM_smallGapBandWidth () { return "smallGapBandWidth"; }
    static const char* m_STR_PARAM_smallGapThreshold () { return "smallGapThreshold"; }
    static const char* m_STR_PARAM_openGapCost () { return "openGapCost"; }
    static const char* m_STR_PARAM_extendGapCost () { return "extendGapCost"; }
    static const char* m_STR_PARAM_evalue () { return "evalue"; }
    static const char* m_STR_PARAM_XdroppofUngap () { return "XdroppofUnGap"; }
    static const char* m_STR_PARAM_XdroppofGap () { return "XdroppofGap"; }
    static const char* m_STR_PARAM_finalXdroppofGap () { return "finalXdroppofGap"; }
    static const char* m_STR_PARAM_outputfile () { return "outputfile"; }
    static const char* m_STR_PARAM_strands () { return "strands"; }
    static const char* m_STR_PARAM_wordSize () { return "wordSize"; }
};

#endif

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _PLAST_STRINGS_REPOSITORY_HPP_ */
