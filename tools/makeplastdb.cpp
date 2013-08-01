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

/********************************************************************************/

#include <misc/api/PlastStrings.hpp>
#include <misc/impl/OptionsParser.hpp>

#include <designpattern/impl/Property.hpp>

#include <database/impl/FastaDatabaseQuickReader.hpp>

#define DEBUG(a)  printf a
#define INFO(a)   printf a

/********************************************************************************/

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace misc;
using namespace misc::impl;
using namespace database;
using namespace database::impl;

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
int main (int argc, char* argv[])
{
    /** We need a command line options parser. */
    OptionsParser parser;
    parser.add (new OptionOneParam (STR_OPTION_SUBJECT_URI,         "name of the bank", true));
    parser.add (new OptionOneParam (STR_OPTION_MAX_DATABASE_SIZE,   "block size (default 20000000)", false));
    parser.add (new OptionOneParam (STR_OPTION_OUTPUT_FILE,         "output filename (default bankname + .info)",  false));
    parser.add (new OptionNoParam  (STR_OPTION_INFO_VERBOSE,        "verbose",  false));

    try {
        /** We parse the provided options. */
        parser.parse (argc, argv);

        /** We create a Properties instance for collecting user command line options. */
        Properties props;
        props.add (0, parser.getProperties());

        /** We may want to display some help. */
        if (parser.saw(STR_OPTION_HELP))   {   parser.displayHelp   (stdout);  }

        /** We retrieve the URI of the bank. */
        string filename = props.getProperty (STR_OPTION_SUBJECT_URI)->getValue();

        /** We retrieve the max database size. */
        u_int64_t maxdatabasesize = props.getProperty (STR_OPTION_MAX_DATABASE_SIZE) ?
            atoll (props.getProperty (STR_OPTION_MAX_DATABASE_SIZE)->getString())
            : 20000000;

        /** We retrieve the name of the output file. */
        string output = props.getProperty (STR_OPTION_OUTPUT_FILE) ?  props.getProperty (STR_OPTION_OUTPUT_FILE)->getValue() : (filename + ".info");

        /** We create a fast reader for the bank. */
        FastaDatabaseQuickReader reader (filename, true);

        /** We read the bank. */
        reader.read (maxdatabasesize);

        /** We save as an info file. */
        reader.save (output);

        Properties info;

        info.add (0, "input");
        info.add (1, "filename",  filename);
        info.add (1, "output",     output);
        info.add (1, "maxdbsize",  "%ld", maxdatabasesize);

        info.add (0, reader.getProperties());

        if (props.getProperty (STR_OPTION_INFO_VERBOSE))
        {
            RawDumpPropertiesVisitor v;
            info.accept (&v);
        }

    }
    catch (OptionFailure& e)
    {
        parser.displayErrors (stdout);
        parser.displayHelp   (stdout);
    }
    catch (...)
    {
        fprintf (stderr, MSG_MAIN_MSG4);
    }

    return 0;
}
