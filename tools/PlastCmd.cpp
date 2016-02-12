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

/** \mainpage PlastCmd Documentation
 *
 * \section intro Introduction
 *
 * The PlastCmd provides the software of the final binary that performs comparison
 * between two genomic databases with the PLAST algorithm.
 *
 * It relies on the PlastLibrary for the PLAST algorithm itself. The software specific
 * to PlastCmd concerns mainly:
 *      - management of command line options specific to PLAST
 *      - listening to some PlastLibrary notifications for some dynamic aspects (progression bargraph display for instance)
 *
 * \section design Design
 *
 * \subsection cli Command line options parsing
 *
 * The target binary is a console based executable. It can accept many command line options (such as subject and query
 * databases URIs).
 *
 * For analyzing this command line options, we will use a small set of tools, such as the OptionsParser class.
 *
 * Once the options have been parsed, they can be interpreted as a IProperties instance that can be used for configuring
 * the PLAST algorithm.
 *
 *
 * \subsection listen Listening to PLAST notifications
 *
 *  Genomic databases comparisons with PLAST may take a wide range of time execution, from one second to dozens of hours.
 *  Since this is a console based tool, we won't have any idea of how long we have to wait for the end of the tool.
 *
 *  However, the PlastLibrary provides a notification mechanism that can be used here. In particular, the PlastCmd can listen
 *  to notifications that gives an idea of the PLAST execution percentage.
 *
 *  What we need to do is to register ourself (ie. PlastCmd) as listeners of such notifications. More specifically, the subject
 *  (see [GOF94]) we need to register to is a IEnvironment instance. As a matter of fact, this instance is also the entry point
 *  for launching the PLAST algorithm. So we need to:
 *      - create a IEnvironment instance (or use one existing, like a singleton)
 *      - register ourself as listener to this IEnvironment instance
 *      - run the PLAST algorithm through the IEnvironment instance
 *      - unregister ourself as listener to this IEnvironment instance
 *
 * While the PLAST algorithm is running, we should receive some notifications.
 *
 * \subsection run Launching PLAST algorithm
 *
 * Launching the PLAST algorithm is done through IEnvironment::run() method. A Iproperties instance has to provided to run().
 * Note that this Iproperties instance is likely to be what the command line parser can provide through its getProperties()
 * method.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <misc/api/PlastStrings.hpp>
#include <misc/api/version.hpp>
#include <misc/impl/Option.hpp>
#include <misc/impl/OptionsParser.hpp>

#include <os/impl/DefaultOsFactory.hpp>

#include <designpattern/impl/Property.hpp>

#include <launcher/core/PlastOptionsParser.hpp>

#include <algo/stats/api/IStatistics.hpp>

#include <launcher/core/PlastCmd.hpp>

using namespace std;

using namespace dp;
using namespace dp::impl;

using namespace misc;
using namespace misc::impl;

using namespace launcher::core;

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
    PlastOptionsParser parser;
    try {
        /** We create a Properties instance for collecting both init properties file and user command line options. */
        IProperties* props = new Properties ();
        LOCAL (props);

        /** We parse the provided options. */
        parser.parse (argc, argv);
        props->add (0, parser.getProperties());

        if (parser.saw(STR_OPTION_HELP))
        {
			/** We display program name, version, compiler, etc.*/
            fprintf (stdout, "%s %s\n  - Build date: %s\n  - OS: %s\n  - Compiler: %s\n  - Host CPU: %ld cores available\n\n",
                PLAST_NAME,
                PLAST_VERSION,
                PLAST_BUILD_DATE,
                PLAST_BUILD_OS,
                PLAST_COMPILER,
                os::impl::DefaultFactory::thread().getNbCores()
            );
            parser.displayHelp   (stdout);
            fprintf (stdout, "Citing PLAST: %s\n", STR_PLAST_REFERENCE);
        }/** otherwise we start PLAST */
        else{

            /** We try to see if we have a provided rc file. */
            const Option* fileOpt = parser.getSeenOption (STR_OPTION_INFO_CONFIG_FILE);
            string plastrc = getenv (MSG_MAIN_HOME) ? string (getenv(MSG_MAIN_HOME)) + string(MSG_MAIN_RC_FILE) : "";

            IProperties* initProps = new Properties (fileOpt ? fileOpt->getParam().c_str() : plastrc.c_str());
            LOCAL (initProps);

            /** We read properties from the init file (if any). */
            props->add (0, initProps);

            /** We launch plast with the aggregated properties. */
            PlastCmd cmd (props);
            cmd.execute();
        }
    }
    catch (OptionFailure& e)
    {
        if (parser.saw(STR_OPTION_HELP))    {   parser.displayHelp   (stdout);   }
        else                                {   parser.displayErrors (stdout);   parser.displayHelpShort();}
    }
    catch (statistics::GlobalParametersFailure& e)
    {
        fprintf (stderr, MSG_MAIN_MSG2, e.getMessage());
    }
    catch (const char* e)
    {
        fprintf (stderr, MSG_MAIN_MSG3, e);
    }
    catch (...)
    {
        fprintf (stderr, "%s", MSG_MAIN_MSG4);
    }

    return 0;
}
