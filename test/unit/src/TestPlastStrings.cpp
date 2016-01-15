/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

#include "cppunit.h"

#include <misc/impl/ObsfucatedString.hpp>

#include <string.h>
#include <string>

using namespace std;

using namespace misc::impl;

/********************************************************************************/

class TestPlastStrings : public TestFixture
{
private:

public:

    /********************************************************************************/
    void setUp ()
    {
    }

    void tearDown ()
    {
    }

    /********************************************************************************/
    static Test* suite()
    {
         TestSuite* result = new TestSuite ("PlastStringsTest");

         //result->addTest (new TestCaller<TestPlastStrings> ("testObsfucation", &TestPlastStrings::testObsfucation) );
         result->addTest (new TestCaller<TestPlastStrings> ("generateMethod", &TestPlastStrings::generateStrings) );

         return result;
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void testObsfucation ()
    {
        cout << ObsfucatedString::obsfucate ("PLAST %s (%ld cores available)") << endl;
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    string generateMethod (bool obsfucation, const string& name, const string& init)
    {
        char buffer[4*1024];

        if (obsfucation)
        {
            string obs = ObsfucatedString::obsfucate (init).replace (16, 1, " s ");

            snprintf (buffer, sizeof(buffer), "    static const char* m_%s () { static misc::impl::%s; return s.toString().c_str(); }",
                name.c_str(), obs.c_str()
            );
        }
        else
        {
            snprintf (buffer, sizeof(buffer), "    static const char* m_%s () { return \"%s\"; }",  name.c_str(), init.c_str()  );
        }

        return string (buffer);
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    class StringRepo
    {
    public:
        static const char* STR_OPTION_ALGO_TYPE () { static misc::impl::ObsfucatedString s (1343323437, 3223911, 0)  /* => "-p" */; return s.toString().c_str(); }
        static const char* STR_OPTION_SUBJECT_URI () { static misc::impl::ObsfucatedString s (1343323437, 3220839, 0)  /* => "-d" */; return s.toString().c_str(); }
        static const char* STR_OPTION_QUERY_URI () { static misc::impl::ObsfucatedString s (1343323437, 3221607, 0)  /* => "-i" */; return s.toString().c_str(); }
        static const char* STR_OPTION_OUTPUT_FILE () { static misc::impl::ObsfucatedString s (1343323437, 3223143, 0)  /* => "-o" */; return s.toString().c_str(); }
        static const char* STR_OPTION_EVALUE () { static misc::impl::ObsfucatedString s (1343323437, 3220583, 0)  /* => "-e" */; return s.toString().c_str(); }
        static const char* STR_OPTION_UNGAP_NEIGHBOUR_LENGTH () { static misc::impl::ObsfucatedString s (1343323437, 3223399, 0)  /* => "-n" */; return s.toString().c_str(); }
        static const char* STR_OPTION_UNGAP_SCORE_THRESHOLD () { static misc::impl::ObsfucatedString s (1343323437, 3224167, 0)  /* => "-s" */; return s.toString().c_str(); }
        static const char* STR_OPTION_SMALLGAP_THRESHOLD () { static misc::impl::ObsfucatedString s (1343323437, 3221095, 0)  /* => "-g" */; return s.toString().c_str(); }
        static const char* STR_OPTION_SMALLGAP_BAND_WITH () { static misc::impl::ObsfucatedString s (1343323437, 3220327, 0)  /* => "-b" */; return s.toString().c_str(); }
        static const char* STR_OPTION_NB_PROCESSORS () { static misc::impl::ObsfucatedString s (1343323437, 3219559, 0)  /* => "-a" */; return s.toString().c_str(); }
        static const char* STR_OPTION_OPEN_GAP_COST () { static misc::impl::ObsfucatedString s (1343323437, 3212903, 0)  /* => "-G" */; return s.toString().c_str(); }
        static const char* STR_OPTION_EXTEND_GAP_COST () { static misc::impl::ObsfucatedString s (1343323437, 3212391, 0)  /* => "-E" */; return s.toString().c_str(); }
        static const char* STR_OPTION_X_DROPOFF_GAPPED () { static misc::impl::ObsfucatedString s (1343323437, 3217767, 0)  /* => "-X" */; return s.toString().c_str(); }
        static const char* STR_OPTION_X_DROPOFF_FINAL () { static misc::impl::ObsfucatedString s (1343323437, 3218279, 0)  /* => "-Z" */; return s.toString().c_str(); }
        static const char* STR_OPTION_FILTER_QUERY () { static misc::impl::ObsfucatedString s (1343323437, 3213159, 0)  /* => "-F" */; return s.toString().c_str(); }
        static const char* STR_OPTION_SCORE_MATRIX () { static misc::impl::ObsfucatedString s (1343323437, 3214439, 0)  /* => "-M" */; return s.toString().c_str(); }
        static const char* STR_OPTION_REWARD () { static misc::impl::ObsfucatedString s (1343323437, 3224423, 0)  /* => "-r" */; return s.toString().c_str(); }
        static const char* STR_OPTION_PENALTY () { static misc::impl::ObsfucatedString s (1343323437, 3223655, 0)  /* => "-q" */; return s.toString().c_str(); }
        static const char* STR_OPTION_OUTPUT_FORMAT () { static misc::impl::ObsfucatedString s (1343323437, 32771384103153255, 0)  /* => "-outfmt" */; return s.toString().c_str(); }
        static const char* STR_OPTION_STRAND () { static misc::impl::ObsfucatedString s (1343323437, 28268862479086183, 0)  /* => "-strand" */; return s.toString().c_str(); }
        static const char* STR_OPTION_FORCE_QUERY_ORDERING () { static misc::impl::ObsfucatedString s (1343323437, 8155285978032777063, 7237969792239675702, 8251021, 0)  /* => "-force-query-order" */; return s.toString().c_str(); }
        static const char* STR_OPTION_MAX_DATABASE_SIZE () { static misc::impl::ObsfucatedString s (1343323437, 8386094127595400295, 7598466910055247394, 8253842, 0)  /* => "-max-database-size" */; return s.toString().c_str(); }
        static const char* STR_OPTION_MAX_HSP_PER_HIT () { static misc::impl::ObsfucatedString s (1343323437, 8102934699070991463, 8388350325356182638, 0)  /* => "-max-hsp-per-hit" */; return s.toString().c_str(); }
        static const char* STR_OPTION_MAX_HIT_PER_ITERATION () { static misc::impl::ObsfucatedString s (1343323437, 8388350325455596647, 7310583739042708590, 121424782685594, 0)  /* => "-max-hit-per-iteration" */; return s.toString().c_str(); }
        static const char* STR_OPTION_STRANDS_LIST () { static misc::impl::ObsfucatedString s (1343323437, 8314892176840798823, 500152868974, 0)  /* => "-strands-list" */; return s.toString().c_str(); }
        static const char* STR_OPTION_CODON_STOP_OPTIM () { static misc::impl::ObsfucatedString s (1343323437, 7146488483396267623, 8031170674471544876, 8230040, 0)  /* => "-optim-codon-stop" */; return s.toString().c_str(); }
        static const char* STR_OPTION_FACTORY_DISPATCHER () { static misc::impl::ObsfucatedString s (1343323437, 8751179571607381863, 7166460042654197870, 1044864, 0)  /* => "-factory-dispatcher" */; return s.toString().c_str(); }
        static const char* STR_OPTION_FACTORY_INDEXATION () { static misc::impl::ObsfucatedString s (1343323437, 8751179571607381863, 8386116358012151150, 1309569, 0)  /* => "-factory-indexation" */; return s.toString().c_str(); }
        static const char* STR_OPTION_FACTORY_HIT_UNGAP () { static misc::impl::ObsfucatedString s (1343323437, 8751179571607381863, 7453023271432168558, 8250505, 0)  /* => "-factory-hit-ungap" */; return s.toString().c_str(); }
        static const char* STR_OPTION_FACTORY_HIT_SMALLGAP () { static misc::impl::ObsfucatedString s (1343323437, 8751179571607381863, 7020394033204634734, 482665494660, 0)  /* => "-factory-hit-smallgap" */; return s.toString().c_str(); }
        static const char* STR_OPTION_FACTORY_HIT_FULLGAP () { static misc::impl::ObsfucatedString s (1343323437, 8751179571607381863, 7815265073784366190, 1880945540, 0)  /* => "-factory-hit-fullgap" */; return s.toString().c_str(); }
        static const char* STR_OPTION_FACTORY_HIT_COMPOSITION () { static misc::impl::ObsfucatedString s (1343323437, 8751179571607381863, 7885630519427146862, 7957695015286668184, 0)  /* => "-factory-hit-composition" */; return s.toString().c_str(); }
        static const char* STR_OPTION_FACTORY_GAP_RESULT () { static misc::impl::ObsfucatedString s (1343323437, 8751179571607381863, 8315177826494393198, 653469, 0)  /* => "-factory-gap-result" */; return s.toString().c_str(); }
        static const char* STR_OPTION_FACTORY_UNGAP_RESULT () { static misc::impl::ObsfucatedString s (1343323437, 8751179571607381863, 8227355657924786542, 500028729229, 0)  /* => "-factory-ungap-result" */; return s.toString().c_str(); }
        static const char* STR_OPTION_OPTIM_FILTER_UNGAP () { static misc::impl::ObsfucatedString s (1343323437, 7362661265510051431, 7959317885798719530, 914831, 0)  /* => "-optim-filter-ungap" */; return s.toString().c_str(); }
        static const char* STR_OPTION_INFO_BARGRAPH () { static misc::impl::ObsfucatedString s (1343323437, 8097879393613914983, 412715, 0)  /* => "-bargraph" */; return s.toString().c_str(); }
        static const char* STR_OPTION_INFO_BARGRAPH_SIZE () { static misc::impl::ObsfucatedString s (1343323437, 8097879393613914983, 111576429715755, 0)  /* => "-bargraph-size" */; return s.toString().c_str(); }
        static const char* STR_OPTION_INFO_VERBOSE () { static misc::impl::ObsfucatedString s (1343323437, 7310309088768046951, 0)  /* => "-verbose" */; return s.toString().c_str(); }
        static const char* STR_OPTION_INFO_FULL_STATS () { static misc::impl::ObsfucatedString s (1343323437, 8391100474417489767, 7682082, 0)  /* => "-full-stats" */; return s.toString().c_str(); }
        static const char* STR_OPTION_INFO_STATS () { static misc::impl::ObsfucatedString s (1343323437, 126943685325415, 0)  /* => "-stats" */; return s.toString().c_str(); }
        static const char* STR_OPTION_INFO_STATS_FORMAT () { static misc::impl::ObsfucatedString s (1343323437, 7362667909505954407, 407598, 0)  /* => "-stats-fmt" */; return s.toString().c_str(); }
        static const char* STR_OPTION_INFO_STATS_AUTO () { static misc::impl::ObsfucatedString s (1343323437, 7002379939316314727, 6895670, 0)  /* => "-stats-auto" */; return s.toString().c_str(); }
        static const char* STR_OPTION_INFO_PROGRESSION () { static misc::impl::ObsfucatedString s (1343323437, 8315178075583361383, 7811887373962782000, 8230029, 0)  /* => "-progression-file" */; return s.toString().c_str(); }
        static const char* STR_OPTION_INFO_ALIGNMENT_PROGRESS () { static misc::impl::ObsfucatedString s (1343323437, 7308619160720580711, 8243680180223817773, 976781, 0)  /* => "-alignment-progress" */; return s.toString().c_str(); }
        static const char* STR_OPTION_INFO_RESOURCES_PROGRESS () { static misc::impl::ObsfucatedString s (1343323437, 7165919078636598119, 8243680180223819558, 976781, 0)  /* => "-resources-progress" */; return s.toString().c_str(); }
        static const char* STR_OPTION_INFO_CONFIG_FILE () { static misc::impl::ObsfucatedString s (1343323437, 7165917996003438951, 0)  /* => "-plastrc" */; return s.toString().c_str(); }
        static const char* STR_OPTION_XML_FILTER_FILE () { static misc::impl::ObsfucatedString s (1343323437, 8389196094509955431, 409126, 0)  /* => "-xmlfilter" */; return s.toString().c_str(); }
        static const char* STR_OPTION_SEEDS_USE_RATIO () { static misc::impl::ObsfucatedString s (1343323437, 8443531751423488615, 8028075772640700720, 0)  /* => "-seeds-use-ratio" */; return s.toString().c_str(); }
        static const char* STR_OPTION_HELP () { static misc::impl::ObsfucatedString s (1343323437, 3221863, 0)  /* => "-h" */; return s.toString().c_str(); }
    };

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void checkStrings ()
    {
#define CHECK(n,init)  CPPUNIT_ASSERT (strcmp (init, StringRepo::n()) == 0)

        CHECK (STR_OPTION_ALGO_TYPE                , "-p");
        CHECK (STR_OPTION_SUBJECT_URI              , "-d");
        CHECK (STR_OPTION_QUERY_URI                , "-i");
        CHECK (STR_OPTION_OUTPUT_FILE              , "-o");
        CHECK (STR_OPTION_EVALUE                   , "-e");
        CHECK (STR_OPTION_UNGAP_NEIGHBOUR_LENGTH   , "-n");
        CHECK (STR_OPTION_UNGAP_SCORE_THRESHOLD    , "-s");
        CHECK (STR_OPTION_SMALLGAP_THRESHOLD       , "-g");
        CHECK (STR_OPTION_SMALLGAP_BAND_WITH       , "-b");
        CHECK (STR_OPTION_NB_PROCESSORS            , "-a");
        CHECK (STR_OPTION_OPEN_GAP_COST            , "-G");
        CHECK (STR_OPTION_EXTEND_GAP_COST          , "-E");
        CHECK (STR_OPTION_X_DROPOFF_GAPPED         , "-X");
        CHECK (STR_OPTION_X_DROPOFF_FINAL          , "-Z");
        CHECK (STR_OPTION_FILTER_QUERY             , "-F");
        CHECK (STR_OPTION_SCORE_MATRIX             , "-M");
        CHECK (STR_OPTION_REWARD ,      "-r");
        CHECK (STR_OPTION_PENALTY , "-q");
        CHECK (STR_OPTION_OUTPUT_FORMAT            , "-outfmt");
        CHECK (STR_OPTION_STRAND            , "-strand");
        CHECK (STR_OPTION_FORCE_QUERY_ORDERING  , "-force-query-order");
        CHECK (STR_OPTION_MAX_DATABASE_SIZE        , "-max-database-size");
        CHECK (STR_OPTION_MAX_HSP_PER_HIT        , "-max-hsp-per-hit");
        CHECK (STR_OPTION_MAX_HIT_PER_ITERATION    , "-max-hit-per-iteration");
        CHECK (STR_OPTION_STRANDS_LIST             , "-strands-list");
        CHECK (STR_OPTION_CODON_STOP_OPTIM         , "-optim-codon-stop");
        CHECK (STR_OPTION_FACTORY_DISPATCHER       , "-factory-dispatcher");
        CHECK (STR_OPTION_FACTORY_INDEXATION       , "-factory-indexation");
        CHECK (STR_OPTION_FACTORY_HIT_UNGAP        , "-factory-hit-ungap");
        CHECK (STR_OPTION_FACTORY_HIT_SMALLGAP     , "-factory-hit-smallgap");
        CHECK (STR_OPTION_FACTORY_HIT_FULLGAP      , "-factory-hit-fullgap");
        CHECK (STR_OPTION_FACTORY_HIT_COMPOSITION  , "-factory-hit-composition");
        CHECK (STR_OPTION_FACTORY_GAP_RESULT       , "-factory-gap-result");
        CHECK (STR_OPTION_FACTORY_UNGAP_RESULT     , "-factory-ungap-result");
        CHECK (STR_OPTION_OPTIM_FILTER_UNGAP       , "-optim-filter-ungap");
        CHECK (STR_OPTION_INFO_BARGRAPH            , "-bargraph");
        CHECK (STR_OPTION_INFO_BARGRAPH_SIZE       , "-bargraph-size");
        CHECK (STR_OPTION_INFO_VERBOSE             , "-verbose");
        CHECK (STR_OPTION_INFO_FULL_STATS          , "-full-stats");
        CHECK (STR_OPTION_INFO_STATS               , "-stats");
        CHECK (STR_OPTION_INFO_STATS_FORMAT        , "-stats-fmt");
        CHECK (STR_OPTION_INFO_STATS_AUTO          , "-stats-auto");
        CHECK (STR_OPTION_INFO_PROGRESSION         , "-progression-file");
        CHECK (STR_OPTION_INFO_ALIGNMENT_PROGRESS  , "-alignment-progress");
        CHECK (STR_OPTION_INFO_RESOURCES_PROGRESS  , "-resources-progress");
        CHECK (STR_OPTION_INFO_CONFIG_FILE         , "-plastrc");
        CHECK (STR_OPTION_XML_FILTER_FILE         , "-xmlfilter");
        CHECK (STR_OPTION_SEEDS_USE_RATIO         , "-seeds-use-ratio");
        CHECK (STR_OPTION_HELP                     , "-h");
    }

    /********************************************************************************/
    /* */
    /********************************************************************************/
    void generateStrings ()
    {

#define OBSFUCATE(n,s)   cout << generateMethod (true,#n,s) << endl;

        cout << "class StringRepository "   << endl;
        cout << "{"                         << endl;
        cout << "public:"                   << endl;

        OBSFUCATE (STR_OPTION_ALGO_TYPE                 , "-p");
        OBSFUCATE (STR_OPTION_SUBJECT_URI               , "-d");
        OBSFUCATE (STR_OPTION_QUERY_URI                 , "-i");
        OBSFUCATE (STR_OPTION_OUTPUT_FILE               , "-o");
        OBSFUCATE (STR_OPTION_EVALUE                    , "-e");
        OBSFUCATE (STR_OPTION_UNGAP_NEIGHBOUR_LENGTH    , "-n");
        OBSFUCATE (STR_OPTION_UNGAP_SCORE_THRESHOLD     , "-s");
        OBSFUCATE (STR_OPTION_SMALLGAP_THRESHOLD        , "-g");
        OBSFUCATE (STR_OPTION_SMALLGAP_BAND_WITH        , "-b");
        OBSFUCATE (STR_OPTION_NB_PROCESSORS             , "-a");
        OBSFUCATE (STR_OPTION_OPEN_GAP_COST             , "-G");
        OBSFUCATE (STR_OPTION_EXTEND_GAP_COST           , "-E");
        OBSFUCATE (STR_OPTION_X_DROPOFF_GAPPED          , "-X");
        OBSFUCATE (STR_OPTION_X_DROPOFF_FINAL           , "-Z");
        OBSFUCATE (STR_OPTION_FILTER_QUERY              , "-F");
        OBSFUCATE (STR_OPTION_SCORE_MATRIX              , "-M");
        OBSFUCATE (STR_OPTION_REWARD                    , "-r");
        OBSFUCATE (STR_OPTION_PENALTY                   , "-q");
        OBSFUCATE (STR_OPTION_OUTPUT_FORMAT             , "-outfmt");
        OBSFUCATE (STR_OPTION_STRAND                    , "-strand");
        OBSFUCATE (STR_OPTION_FORCE_QUERY_ORDERING      , "-force-query-order");
        OBSFUCATE (STR_OPTION_MAX_DATABASE_SIZE         , "-max-database-size");
        OBSFUCATE (STR_OPTION_MAX_HSP_PER_HIT           , "-max-hsp-per-hit");
        OBSFUCATE (STR_OPTION_MAX_HIT_PER_ITERATION     , "-max-hit-per-iteration");
        OBSFUCATE (STR_OPTION_STRANDS_LIST              , "-strands-list");
        OBSFUCATE (STR_OPTION_CODON_STOP_OPTIM          , "-optim-codon-stop");
        OBSFUCATE (STR_OPTION_FACTORY_DISPATCHER        , "-factory-dispatcher");
        OBSFUCATE (STR_OPTION_FACTORY_INDEXATION        , "-factory-indexation");
        OBSFUCATE (STR_OPTION_FACTORY_HIT_UNGAP         , "-factory-hit-ungap");
        OBSFUCATE (STR_OPTION_FACTORY_HIT_SMALLGAP      , "-factory-hit-smallgap");
        OBSFUCATE (STR_OPTION_FACTORY_HIT_FULLGAP       , "-factory-hit-fullgap");
        OBSFUCATE (STR_OPTION_FACTORY_HIT_COMPOSITION   , "-factory-hit-composition");
        OBSFUCATE (STR_OPTION_FACTORY_GAP_RESULT        , "-factory-gap-result");
        OBSFUCATE (STR_OPTION_FACTORY_UNGAP_RESULT      , "-factory-ungap-result");
        OBSFUCATE (STR_OPTION_OPTIM_FILTER_UNGAP        , "-optim-filter-ungap");
        OBSFUCATE (STR_OPTION_INFO_BARGRAPH             , "-bargraph");
        OBSFUCATE (STR_OPTION_INFO_BARGRAPH_SIZE        , "-bargraph-size");
        OBSFUCATE (STR_OPTION_INFO_VERBOSE              , "-verbose");
        OBSFUCATE (STR_OPTION_INFO_FULL_STATS           , "-full-stats");
        OBSFUCATE (STR_OPTION_INFO_STATS                , "-stats");
        OBSFUCATE (STR_OPTION_INFO_STATS_FORMAT         , "-stats-fmt");
        OBSFUCATE (STR_OPTION_INFO_STATS_AUTO           , "-stats-auto");
        OBSFUCATE (STR_OPTION_INFO_PROGRESSION          , "-progression-file");
        OBSFUCATE (STR_OPTION_INFO_ALIGNMENT_PROGRESS   , "-alignment-progress");
        OBSFUCATE (STR_OPTION_INFO_RESOURCES_PROGRESS   , "-resources-progress");
        OBSFUCATE (STR_OPTION_INFO_CONFIG_FILE          , "-plastrc");
        OBSFUCATE (STR_OPTION_XML_FILTER_FILE           , "-xmlfilter");
        OBSFUCATE (STR_OPTION_SEEDS_USE_RATIO           , "-seeds-use-ratio");
        OBSFUCATE (STR_OPTION_HELP                      , "-h");

        OBSFUCATE (MSG_MAIN_RC_FILE                   , "/.plastrc");
        OBSFUCATE (MSG_MAIN_HOME                      , "HOME");
        OBSFUCATE (MSG_MAIN_MSG1                      , "PLAST %s (%ld cores available)\n");
        OBSFUCATE (MSG_MAIN_MSG2                      , "ERROR BAD PARAMETERS: %s\n");
        OBSFUCATE (MSG_MAIN_MSG3                      , "ERROR: %s\n");
        OBSFUCATE (MSG_MAIN_MSG4                      , "GENERIC ERROR...\n");

        OBSFUCATE (MSG_INDEXATION_MSG1                , "ERROR!!! : BAD HASH CODE\n");

        OBSFUCATE (MSG_DATABASE_MSG1                , "ERROR IN SEQUENCE SEARCH...\n");

        OBSFUCATE (MSG_HITS_MSG1                      , "SEQUENCE NOT FOUND IN DATABASE...\n");
        OBSFUCATE (MSG_HITS_MSG2                      , "iterating HSP");
        OBSFUCATE (MSG_HITS_MSG3                      , "starting iterating seeds...");
        OBSFUCATE (MSG_HITS_MSG4                      , "iterating all possible seeds...  (%ld/%ld)");
        OBSFUCATE (MSG_HITS_MSG5                      , "finishing iterating seeds...");
        OBSFUCATE (MSG_HITS_MSG6                      , "iterating seeds codes");

        OBSFUCATE (MSG_STATS_MSG1                    , "unknown score matrix");
        OBSFUCATE (MSG_STATS_MSG2                    , "No found statistical parameters for given matrix and open/extend gap scores...");

        OBSFUCATE (MSG_OPTIONS_MSG1                    , "Option %s already seen, ignored...");
        OBSFUCATE (MSG_OPTIONS_MSG2                    , "Option %s can't be used with option %s, ignored...");
        OBSFUCATE (MSG_OPTIONS_MSG3                    , "To few arguments for the %s option...");
        OBSFUCATE (MSG_OPTIONS_MSG4                    , "Error : %s\n");
        OBSFUCATE (MSG_OPTIONS_MSG5                    , "Warning : %s\n");
        OBSFUCATE (MSG_OPTIONS_MSG6                    , "Option %s must be used with one of the following options : %s");
        OBSFUCATE (MSG_OPTIONS_MSG7                    , "Option %s is mandatory");

        OBSFUCATE (STR_HELP_ALGO_TYPE,                "Program Name [plastp, tplastn, plastx or tplastx]");
        OBSFUCATE (STR_HELP_SUBJECT_URI,              "Subject database file");
        OBSFUCATE (STR_HELP_QUERY_URI,                "Query database file");
        OBSFUCATE (STR_HELP_OUTPUT_FILE,              "PLAST report Output File");
        OBSFUCATE (STR_HELP_EVALUE,                   "Expectation value");
        OBSFUCATE (STR_HELP_UNGAP_NEIGHBOUR_LENGTH,   "Size of neighbourhood peforming ungapped extension");
        OBSFUCATE (STR_HELP_UNGAP_SCORE_THRESHOLD,    "Ungapped threshold trigger a small gapped extension");
        OBSFUCATE (STR_HELP_SMALLGAP_THRESHOLD,       "threshold for small gapped extension");
        OBSFUCATE (STR_HELP_SMALLGAP_BAND_WITH,       "bandwith for small gapped extension");
        OBSFUCATE (STR_HELP_NB_PROCESSORS,            "Number of processors to use");
        OBSFUCATE (STR_HELP_OPEN_GAP_COST,            "Cost to open a gap");
        OBSFUCATE (STR_HELP_EXTEND_GAP_COST,          "Cost to extend a gap");
        OBSFUCATE (STR_HELP_X_DROPOFF_GAPPED,         "X dropoff value for gapped alignment (in bits) (zero invokes default behavior)");
        OBSFUCATE (STR_HELP_X_DROPOFF_FINAL,          "X dropoff value for final gapped alignment in bits (0.0 invokes default behavior)");
        OBSFUCATE (STR_HELP_FILTER_QUERY,             "Filter query sequence");
        OBSFUCATE (STR_HELP_SCORE_MATRIX,             "Score matrix (BLOSUM62 or BLOSUM50)");
        OBSFUCATE (STR_HELP_STRAND,                   "strands for plastn: 'plus', 'minus' or 'both' (default)");
        OBSFUCATE (STR_HELP_REWARD,                   "reward for a nucleotide match (plastn)");
        OBSFUCATE (STR_HELP_PENALTY,                  "penalty for a nucleotide mismatch (plastn)");
        OBSFUCATE (STR_HELP_FORCE_QUERY_ORDERING,     "Force queries ordering in output file.");
        OBSFUCATE (STR_HELP_MAX_DATABASE_SIZE,        "Maximum allowed size (in bytes) for a database. If greater, database is segmented.");
        OBSFUCATE (STR_HELP_MAX_HSP_PER_HIT,          "Maximum hits per query. 0 value will dump all hits (default)");
        OBSFUCATE (STR_HELP_MAX_HIT_PER_ITERATION,    "Maximum hits per iteration (for memory usage control). 1000000 by default");
        OBSFUCATE (STR_HELP_OUTPUT_FORMAT,            "Output format: 1 for tabulated (default).");
        OBSFUCATE (STR_HELP_STRANDS_LIST,             "List of the strands (ex: \"1,2,6\") to be used when using algo using nucleotids databases.");
        OBSFUCATE (STR_HELP_CODON_STOP_OPTIM,          "size of the allowed range between the last invalid character and the next stop codon");
        OBSFUCATE (STR_HELP_FACTORY_DISPATCHER,       "Factory that creates dispatcher.");
        OBSFUCATE (STR_HELP_FACTORY_INDEXATION,       "Factory that creates indexation builder.");
        OBSFUCATE (STR_HELP_FACTORY_HIT_UNGAP,        "Factory that creates ungap hits iterator.");
        OBSFUCATE (STR_HELP_FACTORY_HIT_SMALLGAP,     "Factory that creates small gap hits iterator.");
        OBSFUCATE (STR_HELP_FACTORY_HIT_FULLGAP,      "Factory that creates full gap hits iterator.");
        OBSFUCATE (STR_HELP_FACTORY_HIT_COMPOSITION,  "Factory that creates composition hits iterator.");
        OBSFUCATE (STR_HELP_FACTORY_GAP_RESULT,       "Factory that creates gap alignments result.");
        OBSFUCATE (STR_HELP_FACTORY_UNGAP_RESULT,     "Factory that creates ungap alignments result.");
        OBSFUCATE (STR_HELP_OPTIM_FILTER_UNGAP,       "Optimization that filters out through ungap alignments.");
        OBSFUCATE (STR_HELP_XML_FILTER_FILE,          "Uri of a XML filter file.");
        OBSFUCATE (STR_HELP_SEEDS_USE_RATIO,          "Ratio of seeds to be used.");
        OBSFUCATE (STR_HELP_HELP,                     "help");
        OBSFUCATE (STR_HELP_INFO_BARGRAPH,            "Display a progress bar during execution.");
        OBSFUCATE (STR_HELP_INFO_BARGRAPH_SIZE,       "Nb of characters of the bargraph.");
        OBSFUCATE (STR_HELP_INFO_PROGRESSION,         "Dump in a file the current execution percentage.");
        OBSFUCATE (STR_HELP_INFO_VERBOSE,             "Display information during algorithm execution.");
        OBSFUCATE (STR_HELP_INFO_FULL_STATS,          "Dump algorithm statistics.");
        OBSFUCATE (STR_HELP_INFO_STATS,               "Dump generic statistics.");
        OBSFUCATE (STR_HELP_INFO_STATS_FORMAT,        "Format of statistics: 'raw' (default) or 'xml'");
        OBSFUCATE (STR_HELP_INFO_STATS_AUTO,          "Automatic stats file creation");
        OBSFUCATE (STR_HELP_INFO_ALIGNMENT_PROGRESS,  "Dump in a file the growing number of ungap/ungap alignments during algorithm.");
        OBSFUCATE (STR_HELP_INFO_RESOURCES_PROGRESS,  "Dump in a file information about resources during algorithm.");
        OBSFUCATE (STR_HELP_INFO_CONFIG_FILE,         "Pathname of the plast config file.");

        OBSFUCATE (STR_CONFIG_CLASS_SerialCommandDispatcher,      "SerialCommandDispatcher");
        OBSFUCATE (STR_CONFIG_CLASS_ParallelCommandDispatcher,    "ParallelCommandDispatcher");
        OBSFUCATE (STR_CONFIG_CLASS_BasicIndexator,               "BasicIndexator");
        OBSFUCATE (STR_CONFIG_CLASS_BasicSortedIndexator,         "BasicSortedIndexator");
        OBSFUCATE (STR_CONFIG_CLASS_UngapHitIteratorNull,         "UngapHitIteratorNull");
        OBSFUCATE (STR_CONFIG_CLASS_UngapHitIterator,             "UngapHitIterator");
        OBSFUCATE (STR_CONFIG_CLASS_UngapHitIteratorSSE16,        "UngapHitIteratorSSE16");
        OBSFUCATE (STR_CONFIG_CLASS_SmallGapHitIterator,          "SmallGapHitIterator");
        OBSFUCATE (STR_CONFIG_CLASS_SmallGapHitIteratorNull,      "SmallGapHitIteratorNull");
        OBSFUCATE (STR_CONFIG_CLASS_SmallGapHitIteratorSSE8,      "SmallGapHitIteratorSSE8");
        OBSFUCATE (STR_CONFIG_CLASS_FullGapHitIterator,           "FullGapHitIterator");
        OBSFUCATE (STR_CONFIG_CLASS_FullGapHitIteratorNull,       "FullGapHitIteratorNull");
        OBSFUCATE (STR_CONFIG_CLASS_CompositionHitIterator,       "CompositionHitIterator");
        OBSFUCATE (STR_CONFIG_CLASS_CompositionHitIteratorNull,   "CompositionHitIteratorNull");
        OBSFUCATE (STR_CONFIG_CLASS_BasicAlignmentResult,         "BasicAlignmentResult");
        OBSFUCATE (STR_CONFIG_CLASS_UngapAlignmentResult,         "UngapAlignmentResult");

        OBSFUCATE (STR_PARAM_params,                  "params");
        OBSFUCATE (STR_PARAM_seedModelKind,           "seedModelKind");
        OBSFUCATE (STR_PARAM_seedSpan,                "seedSpan");
        OBSFUCATE (STR_PARAM_subseedStrings,          "subseedStrings");
        OBSFUCATE (STR_PARAM_matrixKind,              "matrixKind");
        OBSFUCATE (STR_PARAM_subject,                 "subject");
        OBSFUCATE (STR_PARAM_uri,                     "uri");
        OBSFUCATE (STR_PARAM_range,                   "range");
        OBSFUCATE (STR_PARAM_begin,                   "begin");
        OBSFUCATE (STR_PARAM_end,                     "end");
        OBSFUCATE (STR_PARAM_filterQuery,             "filterQuery");
        OBSFUCATE (STR_PARAM_ungapNeighbourLength,    "ungapNeighbourLength");
        OBSFUCATE (STR_PARAM_ungapScoreThreshold,     "ungapScoreThreshold");
        OBSFUCATE (STR_PARAM_smallGapBandLength,      "smallGapBandLength");
        OBSFUCATE (STR_PARAM_smallGapBandWidth,       "smallGapBandWidth");
        OBSFUCATE (STR_PARAM_smallGapThreshold,       "smallGapThreshold");
        OBSFUCATE (STR_PARAM_openGapCost,             "openGapCost");
        OBSFUCATE (STR_PARAM_extendGapCost,           "extendGapCost");
        OBSFUCATE (STR_PARAM_evalue,                  "evalue");
        OBSFUCATE (STR_PARAM_XdroppofGap,             "XdroppofGap");
        OBSFUCATE (STR_PARAM_finalXdroppofGap,        "finalXdroppofGap");
        OBSFUCATE (STR_PARAM_outputfile,              "outputfile");
        OBSFUCATE (STR_PARAM_strands,                 "strands");
        OBSFUCATE (STR_PARAM_wordSize,                "wordSize");
        OBSFUCATE (STR_PLAST_REFERENCE,               "Nguyen VH, Lavenier D. (2009) PLAST: parallel local alignment search tool for database comparison. BMC Bioinformatics, vol 10, no 329.");

        cout << "};"  << endl;

        cout << endl;
return;

#define DUMP_DEFINE(n,s)   cout << "#define " << #n << "  misc::StringRepository::m_" << #n << " ()   // " << s << endl;

        DUMP_DEFINE (MSG_MAIN_RC_FILE                   , "/.plastrc");
        DUMP_DEFINE (MSG_MAIN_HOME                      , "HOME");
        DUMP_DEFINE (MSG_MAIN_MSG1                      , "PLAST %s (%ld cores available)\n");
        DUMP_DEFINE (MSG_MAIN_MSG2                      , "ERROR BAD PARAMETERS: %s\n");
        DUMP_DEFINE (MSG_MAIN_MSG3                      , "ERROR: %s\n");
        DUMP_DEFINE (MSG_MAIN_MSG4                      , "GENERIC ERROR...\n");

        DUMP_DEFINE (MSG_INDEXATION_MSG1                , "ERROR!!! : BAD HASH CODE\n");

        DUMP_DEFINE (MSG_DATABASE_MSG1                  , "ERROR IN SEQUENCE SEARCH...\n");

        DUMP_DEFINE (MSG_HITS_MSG1                      , "SEQUENCE NOT FOUND IN DATABASE...\n");
        DUMP_DEFINE (MSG_HITS_MSG2                      , "iterating HSP");
        DUMP_DEFINE (MSG_HITS_MSG3                      , "starting iterating seeds...");
        DUMP_DEFINE (MSG_HITS_MSG4                      , "iterating all possible seeds...  (%ld/%ld)");
        DUMP_DEFINE (MSG_HITS_MSG5                      , "finishing iterating seeds...");
        DUMP_DEFINE (MSG_HITS_MSG6                      , "iterating seeds codes");

        DUMP_DEFINE (MSG_STATS_MSG1                    , "unknown score matrix");
        DUMP_DEFINE (MSG_STATS_MSG2                    , "No found statistical parameters for given matrix and open/extend gap scores...");

        DUMP_DEFINE (MSG_OPTIONS_MSG1                    , "Option %s already seen, ignored...");
        DUMP_DEFINE (MSG_OPTIONS_MSG2                    , "Option %s can't be used with option %s, ignored...");
        DUMP_DEFINE (MSG_OPTIONS_MSG3                    , "To few arguments for the %s option...");
        DUMP_DEFINE (MSG_OPTIONS_MSG4                    , "Error : %s\n");
        DUMP_DEFINE (MSG_OPTIONS_MSG5                    , "Warning : %s\n");
        DUMP_DEFINE (MSG_OPTIONS_MSG6                    , "Option %s must be used with one of the following options : %s");
        DUMP_DEFINE (MSG_OPTIONS_MSG7                    , "Option %s is mandatory");

        DUMP_DEFINE (STR_HELP_ALGO_TYPE,                "Program Name [plastp, tplastn, plastx or tplastx]");
        DUMP_DEFINE (STR_HELP_SUBJECT_URI,              "Subject database file");
        DUMP_DEFINE (STR_HELP_QUERY_URI,                "Query database file");
        DUMP_DEFINE (STR_HELP_OUTPUT_FILE,              "PLAST report Output File");
        DUMP_DEFINE (STR_HELP_EVALUE,                   "Expectation value");
        DUMP_DEFINE (STR_HELP_UNGAP_NEIGHBOUR_LENGTH,   "Size of neighbourhood peforming ungapped extension");
        DUMP_DEFINE (STR_HELP_UNGAP_SCORE_THRESHOLD,    "Ungapped threshold trigger a small gapped extension");
        DUMP_DEFINE (STR_HELP_SMALLGAP_THRESHOLD,       "threshold for small gapped extension");
        DUMP_DEFINE (STR_HELP_SMALLGAP_BAND_WITH,       "bandwith for small gapped extension");
        DUMP_DEFINE (STR_HELP_NB_PROCESSORS,            "Number of processors to use");
        DUMP_DEFINE (STR_HELP_OPEN_GAP_COST,            "Cost to open a gap");
        DUMP_DEFINE (STR_HELP_EXTEND_GAP_COST,          "Cost to extend a gap");
        DUMP_DEFINE (STR_HELP_X_DROPOFF_GAPPED,         "X dropoff value for gapped alignment (in bits) (zero invokes default behavior)");
        DUMP_DEFINE (STR_HELP_X_DROPOFF_FINAL,          "X dropoff value for final gapped alignment in bits (0.0 invokes default behavior)");
        DUMP_DEFINE (STR_HELP_FILTER_QUERY,             "Filter query sequence");
        DUMP_DEFINE (STR_HELP_SCORE_MATRIX,             "Score matrix (BLOSUM62 or BLOSUM50)");
        DUMP_DEFINE (STR_HELP_STRAND,                   "strands for plastn: 'plus', 'minus' or 'both' (default)");
        DUMP_DEFINE (STR_HELP_REWARD,                   "reward for a nucleotide match (plastn)");
        DUMP_DEFINE (STR_HELP_PENALTY,                  "penalty for a nucleotide mismatch (plastn)");
        DUMP_DEFINE (STR_HELP_FORCE_QUERY_ORDERING,     "Force queries ordering in output file.");
        DUMP_DEFINE (STR_HELP_MAX_DATABASE_SIZE,        "Maximum allowed size (in bytes) for a database. If greater, database is segmented.");
        DUMP_DEFINE (STR_HELP_MAX_HSP_PER_HIT,          "Maximum hits per query. 0 value will dump all hits (default)");
        DUMP_DEFINE (STR_HELP_MAX_HIT_PER_ITERATION,    "Maximum hits per iteration (for memory usage control). 1000000 by default");
        DUMP_DEFINE (STR_HELP_OUTPUT_FORMAT,            "Output format: 1 for tabulated (default).");
        DUMP_DEFINE (STR_HELP_STRANDS_LIST,             "List of the strands (ex: \"1,2,6\") to be used when using algo using nucleotids databases.");
        DUMP_DEFINE (STR_HELP_CODON_STOP_OPTIM,          "size of the allowed range between the last invalid character and the next stop codon");
        DUMP_DEFINE (STR_HELP_FACTORY_DISPATCHER,       "Factory that creates dispatcher.");
        DUMP_DEFINE (STR_HELP_FACTORY_INDEXATION,       "Factory that creates indexation builder.");
        DUMP_DEFINE (STR_HELP_FACTORY_HIT_UNGAP,        "Factory that creates ungap hits iterator.");
        DUMP_DEFINE (STR_HELP_FACTORY_HIT_SMALLGAP,     "Factory that creates small gap hits iterator.");
        DUMP_DEFINE (STR_HELP_FACTORY_HIT_FULLGAP,      "Factory that creates full gap hits iterator.");
        DUMP_DEFINE (STR_HELP_FACTORY_HIT_COMPOSITION,  "Factory that creates composition hits iterator.");
        DUMP_DEFINE (STR_HELP_FACTORY_GAP_RESULT,       "Factory that creates gap alignments result.");
        DUMP_DEFINE (STR_HELP_FACTORY_UNGAP_RESULT,     "Factory that creates ungap alignments result.");
        DUMP_DEFINE (STR_HELP_OPTIM_FILTER_UNGAP,       "Optimization that filters out through ungap alignments.");
        DUMP_DEFINE (STR_HELP_XML_FILTER_FILE,          "Uri of a XML filter file.");
        DUMP_DEFINE (STR_HELP_SEEDS_USE_RATIO,          "Ratio of seeds to be used.");
        DUMP_DEFINE (STR_HELP_HELP,                     "help");
        DUMP_DEFINE (STR_HELP_INFO_BARGRAPH,            "Display a progress bar during execution.");
        DUMP_DEFINE (STR_HELP_INFO_BARGRAPH_SIZE,       "Nb of characters of the bargraph.");
        DUMP_DEFINE (STR_HELP_INFO_PROGRESSION,         "Dump in a file the current execution percentage.");
        DUMP_DEFINE (STR_HELP_INFO_VERBOSE,             "Display information during algorithm execution.");
        DUMP_DEFINE (STR_HELP_INFO_FULL_STATS,          "Dump algorithm statistics.");
        DUMP_DEFINE (STR_HELP_INFO_STATS,               "Dump generic statistics.");
        DUMP_DEFINE (STR_HELP_INFO_STATS_FORMAT,        "Format of statistics: 'raw' (default) or 'xml'");
        DUMP_DEFINE (STR_HELP_INFO_STATS_AUTO,          "Automatic stats file creation");
        DUMP_DEFINE (STR_HELP_INFO_ALIGNMENT_PROGRESS,  "Dump in a file the growing number of ungap/ungap alignments during algorithm.");
        DUMP_DEFINE (STR_HELP_INFO_RESOURCES_PROGRESS,  "Dump in a file information about resources during algorithm.");
        DUMP_DEFINE (STR_HELP_INFO_CONFIG_FILE,         "Pathname of the plast config file.");

        DUMP_DEFINE (STR_CONFIG_CLASS_SerialCommandDispatcher,      "SerialCommandDispatcher");
        DUMP_DEFINE (STR_CONFIG_CLASS_ParallelCommandDispatcher,    "ParallelCommandDispatcher");
        DUMP_DEFINE (STR_CONFIG_CLASS_BasicIndexator,               "BasicIndexator");
        DUMP_DEFINE (STR_CONFIG_CLASS_BasicSortedIndexator,         "BasicSortedIndexator");
        DUMP_DEFINE (STR_CONFIG_CLASS_UngapHitIteratorNull,         "UngapHitIteratorNull");
        DUMP_DEFINE (STR_CONFIG_CLASS_UngapHitIterator,             "UngapHitIterator");
        DUMP_DEFINE (STR_CONFIG_CLASS_UngapHitIteratorSSE16,        "UngapHitIteratorSSE16");
        DUMP_DEFINE (STR_CONFIG_CLASS_SmallGapHitIterator,          "SmallGapHitIterator");
        DUMP_DEFINE (STR_CONFIG_CLASS_SmallGapHitIteratorNull,      "SmallGapHitIteratorNull");
        DUMP_DEFINE (STR_CONFIG_CLASS_SmallGapHitIteratorSSE8,      "SmallGapHitIteratorSSE8");
        DUMP_DEFINE (STR_CONFIG_CLASS_FullGapHitIterator,           "FullGapHitIterator");
        DUMP_DEFINE (STR_CONFIG_CLASS_FullGapHitIteratorNull,       "FullGapHitIteratorNull");
        DUMP_DEFINE (STR_CONFIG_CLASS_CompositionHitIterator,       "CompositionHitIterator");
        DUMP_DEFINE (STR_CONFIG_CLASS_CompositionHitIteratorNull,   "CompositionHitIteratorNull");
        DUMP_DEFINE (STR_CONFIG_CLASS_BasicAlignmentResult,         "BasicAlignmentResult");
        DUMP_DEFINE (STR_CONFIG_CLASS_UngapAlignmentResult,         "UngapAlignmentResult");

        DUMP_DEFINE (STR_PARAM_params,                  "params");
        DUMP_DEFINE (STR_PARAM_seedModelKind,           "seedModelKind");
        DUMP_DEFINE (STR_PARAM_seedSpan,                "seedSpan");
        DUMP_DEFINE (STR_PARAM_subseedStrings,          "subseedStrings");
        DUMP_DEFINE (STR_PARAM_matrixKind,              "matrixKind");
        DUMP_DEFINE (STR_PARAM_subject,                 "subject");
        DUMP_DEFINE (STR_PARAM_uri,                     "uri");
        DUMP_DEFINE (STR_PARAM_range,                   "range");
        DUMP_DEFINE (STR_PARAM_begin,                   "begin");
        DUMP_DEFINE (STR_PARAM_end,                     "end");
        DUMP_DEFINE (STR_PARAM_filterQuery,             "filterQuery");
        DUMP_DEFINE (STR_PARAM_ungapNeighbourLength,    "ungapNeighbourLength");
        DUMP_DEFINE (STR_PARAM_ungapScoreThreshold,     "ungapScoreThreshold");
        DUMP_DEFINE (STR_PARAM_smallGapBandLength,      "smallGapBandLength");
        DUMP_DEFINE (STR_PARAM_smallGapBandWidth,       "smallGapBandWidth");
        DUMP_DEFINE (STR_PARAM_smallGapThreshold,       "smallGapThreshold");
        DUMP_DEFINE (STR_PARAM_openGapCost,             "openGapCost");
        DUMP_DEFINE (STR_PARAM_extendGapCost,           "extendGapCost");
        DUMP_DEFINE (STR_PARAM_evalue,                  "evalue");
        DUMP_DEFINE (STR_PARAM_XdroppofGap,             "XdroppofGap");
        DUMP_DEFINE (STR_PARAM_finalXdroppofGap,        "finalXdroppofGap");
        DUMP_DEFINE (STR_PARAM_outputfile,              "outputfile");
        DUMP_DEFINE (STR_PARAM_strands,                 "strands");
    }
};

/********************************************************************************/

extern "C" Test* TestPlastStrings_suite()  { return TestPlastStrings::suite (); }

