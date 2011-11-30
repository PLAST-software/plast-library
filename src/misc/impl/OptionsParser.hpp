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

/** \file OptionsParser.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Define an abstraction for command line option
 */

#ifndef _CHECK_OPTION_HPP
#define _CHECK_OPTION_HPP

/********************************************************************************/

#include <misc/impl/Option.hpp>
#include <designpattern/api/IProperty.hpp>

#include <list>
#include <string>
#include <stdio.h>

/********************************************************************************/
/** \brief PLAST command line. */
namespace misc {
/** \brief Tools (command line options management) */
namespace impl {
/********************************************************************************/

/** \brief Parser that analyzes command line options.
 *
 * Client can use this class for registering command line options specifications
 * and then can use it for parsing some command line options, typically given
 * as arguments of the 'main' function.
 *
 * Code sample:
 * \code
 * int main (int argc, char* argv[])
 * {
 *      // we create a parser
 *      OptionsParser parser;
 *
 *      // we register some options to it
 *      parser.add (new OptionOneParam ("-p", "Program Name [plastp, tplastn, plastx or tplastx]") );
 *      parser.add (new OptionOneParam ("-d", "Subject database file") );
 *      parser.add (new OptionOneParam ("-i", "Query database file") );
 *      parser.add (new OptionOneParam ("-h", "Help") );
 *
 *      // we parse the provided options
 *      int nbErrors = parser.parse (argc, argv);
 *
 *      // we may want to display help
 *      if (parser.saw("-h"))    {   parser.displayHelp  (stdout);   }
 *
 *      // we retrieve options information as properties
 *      dp::IProperties* props = parser.getProperties ();
 * }
 * \endcode
 */
class OptionsParser
{
public:

    /** Constructor. */
    OptionsParser ();

    /** Destructor. */
    ~OptionsParser ();

    /** Add an option to the pool of recognized options.
     * \param[in] opt : option to be registered to the parser.
     * \return the number of known options
     */
    int add (Option* opt);

    /** Perform the analyze of the arguments.
     * \param[in] argc : number of command line arguments.
     * \param[in] argv : table of arguments
     * \return number of parsing errors.
     */
    int parse (int argc, char* argv[]);

    /** Display errors (if there are some).
     * \param[in] fp : the file descriptor where to dump the errors
     */
    void displayErrors (FILE* fp = stdout);

    /** Display warnings (if there are some).
     * \param[in] fp : the file descriptor where to dump the warnings
     */
    void displayWarnings (FILE* fp = stdout);

    /** Display the help of each options recorded.
     * \param[in] fp : the file descriptor where to dump the help
     */
    void displayHelp (FILE* fp = stdout);

    /** Tells (after Proceed) if one option whose name is given has been seen or not.
     * \param[in] txt : the option name to be checked
     * \return true if option was seen, false otherwise.
     */
    bool saw (const std::string& txt);

    /** Return the list of seen options during the parsing.
     * \return the list of seen options.
     */
    const std::list<Option*>& getSeenOptions ()  { return _seenOptions; }

    /** Tells whether an option has been seen or not, given its label.
     * \return true if seed, false otherwise.
     */
    const Option* getSeenOption (const std::string& label);

    /** Return a IProperties instance holding parsed options information.
     * \return the IProperties instance.
     */
    dp::IProperties* getProperties ()  { return _properties; }

private:

    /** */
    dp::IProperties* _properties;
    void setProperties (dp::IProperties* properties)  { SP_SETATTR(properties); }
    void buildProperties ();

    /** List of Options*. */
    std::list<Option*> _options;

    /** List of errors. */
    std::list<std::string> _errors;

    /** List of Text* of warnings. */
    std::list<std::string> _warnings;

    /** List of seen options. */
    std::list<Option*> _seenOptions;

    /** */
    char _proceed;

    /** */
    int _argc;
    /** */
    char** _argv;
    /** */
    int _currentArg;

    /** */
    Option* lookForOption (char* txt);

    /** */
    char* nextArg ();

    /** */
    void getOptionArgs (const Option* option, std::list<std::string>& args);

    /** */
    void giveToNoOption (char* txt);

    /** */
    char* checkExcludingOptions (const Option* option);

    /** */
    void checkIncludingOptions ();

    /** */
    void checkMandatoryOptions ();
};

/********************************************************************************/

/** \brief Exception class to be used for option management error.
 *
 * This class should be thrown when something went wrong during options parsing.
 */
class OptionFailure
{
public:

    /** Constructor.
     * \param[in] parser : the parser that threw the exception.
     */
    OptionFailure (OptionsParser& parser) : _parser(parser) {}

    /** Getter on the parser.
     * \return the parser.
     */
    OptionsParser& getParser ()  { return _parser; }

private:
    OptionsParser& _parser;
};

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/

#endif /* _CHECK_OPTION_HPP */
