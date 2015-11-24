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


#include <stdio.h>
#include <string.h>

#include <misc/api/PlastStrings.hpp>
#include <misc/impl/OptionsParser.hpp>
#include <designpattern/impl/Property.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>
#include <os/impl/DefaultOsFactory.hpp>

using namespace std;
using namespace dp::impl;
using namespace os::impl;

#define DEBUG(a)  //printf a

/********************************************************************************/
namespace misc {
namespace impl {
/********************************************************************************/

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
OptionsParser::OptionsParser ()  : _properties(0)
{
    _proceed=0;
    setProperties (new Properties());
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
OptionsParser::~OptionsParser ()
{
    setProperties (0);

    for (std::list<Option*>::iterator it = _options.begin(); it != _options.end(); it++)
    {
        (*it)->forget();
    }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
int OptionsParser::add (Option* option)
{
    if (option != 0)
    {
        option->use ();

        /* We add the option in the list. */
        _options.push_back (option);
    }

    return _options.size();
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
int OptionsParser::parse (int argc, char* argv[])
{
    /* Some initializations. */
    _argc=argc; _argv=argv; _currentArg=1;
    _proceed=1;
    _errors.clear();
    _warnings.clear();
    _seenOptions.clear();
    char* exclude;

    char* txt=0;
    while ( (txt = nextArg ()) != 0)
    {
        DEBUG (("CheckOption::proceed:  _currentArg=%d  txt=%p \n", _currentArg, txt));

        /* We look if is matches one of the recognized options. */
        Option* optionMatch = lookForOption (txt);
        DEBUG (("CheckOption::proceed:  txt='%s'  optionMatch=%p \n", txt, optionMatch));

        if (optionMatch != 0)
        {
            /* This is a recognized option. So we try to retrieve the args that should follow. */
            std::list<std::string> optionArgs;
            getOptionArgs (optionMatch, optionArgs);

            DEBUG (("CheckOption::proceed:  optionMatch.size=%ld  optionArgs.size=%ld\n",
                optionMatch->getNbArgs(),
                optionArgs.size()
            ));

            if (optionArgs.size() == optionMatch->getNbArgs())
            {
                /* We first look if this option has not already been met. */
                if (!optionMatch->canBeMultiple () && saw (optionMatch->getLabel()))
                {
                    char buffer[128];
                    snprintf (buffer, sizeof(buffer), MSG_OPTIONS_MSG1, optionMatch->getLabel().c_str());
                    _warnings.push_back (buffer);
                }
                else if ( (exclude = checkExcludingOptions (optionMatch)) != 0)
                {
                    char buffer[128];
                    snprintf (buffer, sizeof(buffer), MSG_OPTIONS_MSG2,
                        optionMatch->getLabel().c_str(),
                        exclude
                    );
                    _errors.push_back (buffer);
                }
                else
                {
                    int res = optionMatch->proceed (optionArgs);
                    _seenOptions.push_back (optionMatch);

                    DEBUG (("CheckOption::proceed:  proceed the option => res=%ld  seenOptions=%ld  _currentArg=%d\n",
                        res, _seenOptions.size(), _currentArg
                    ));

                    res=0;  // reduce warning
                }
            }
        }
        else
        {
#if 0
            /* This is an NOT a recognized option. We try to find an No_Option. */
            giveToNoOption (txt);
#else
            /** Unknown option => add it to the error list. */
            _errors.push_back (txt);
#endif
        }
    }

    if(saw(STR_OPTION_HELP)==false){
        /** We check mandatory options. */
        checkMandatoryOptions ();

        /* Now, we check that the accepted options are used with with include options. */
        checkIncludingOptions ();
    }

    /** We may launch an exception if needed. */
    if (!_errors.empty())   {  throw OptionFailure (*this);  }

    /** We fill the properties. */
    buildProperties ();

    /* And we return the errors number. */
    return _errors.size();
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
int OptionsParser::parse (const std::string& s)
{
    int    result = 0;
    int    argc   = 0;
    size_t idx    = 0;

    /** We tokenize the string and count the number of tokens. */
    TokenizerIterator it1 (s.c_str(), " ");
    for (it1.first(); !it1.isDone(); it1.next())  { argc++; }

    argc++;

    /** We allocate a table of char* */
    char** argv = (char**) DefaultFactory::memory().calloc (argc, sizeof(char*));

    argv[idx++] = DefaultFactory::memory().strdup ("foo");

    /** We fill the table with the tokens. */
    TokenizerIterator it2 (s.c_str(), " ");
    for (it2.first(); !it2.isDone(); it2.next())
    {
        argv[idx++] = DefaultFactory::memory().strdup (it2.currentItem());
    }

    for (int i=0; i<argc; i++)  {  DEBUG (("   item='%s' \n", argv[i]));  }

    /** We parse the arguments. */
    result = parse (argc, argv);

    DEBUG (("OptionsParser::parse  argc=%d => idx=%ld  result=%d  seenOptions=%ld\n", argc, idx, result, _seenOptions.size() ));

    /** Some clean up. */
    for (int i=0; i<argc; i++)  {  DefaultFactory::memory().free (argv[i]);  }
    DefaultFactory::memory().free (argv);

    /** We return the result. */
    return result;
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void OptionsParser::buildProperties ()
{
    for (std::list<Option*>::iterator it = _seenOptions.begin();  it != _seenOptions.end();  it++)
    {
        _properties->add (0, (*it)->getLabel(), (*it)->getParam());
    }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
char* OptionsParser::nextArg ()
{
    DEBUG (("\nCheckOption::nextArg:  _currentArg=%d  _argc=%d \n", _currentArg, _argc));

    return (_currentArg >= _argc ? (char*)0 : _argv[_currentArg++]);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
Option* OptionsParser::lookForOption (char* txt)
{
    Option* result = 0;

    DEBUG (("CheckOption::lookForOption:  txt='%s'  _options.size=%ld  \n", txt, _options.size()));

    for (list<Option*>::iterator it = _options.begin(); !result &&  it != _options.end(); it++)
    {
        if ((*it)->getLabel().compare (txt) == 0)  {  result = *it; }
    }

    DEBUG (("CheckOption::lookForOption:  _options.size=%ld  => found=%d \n", _options.size(), result!=0));

    return result;
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void OptionsParser::getOptionArgs (const Option* option, std::list<std::string>& result)
{
    char* txt;
    int i=1;
    int n = option->getNbArgs();
    while ( (i<=n) && (txt=nextArg()) )
    {
        result.push_back (txt);
        i++;
    }

    if (i<=n)
    {
        char buffer [128];
        snprintf (buffer, sizeof(buffer), MSG_OPTIONS_MSG3, option->getLabel().c_str());
        _errors.push_back (buffer);
    }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void OptionsParser::displayErrors (FILE* fp)
{
    for (list<string>::iterator it = _errors.begin();  it != _errors.end();  it++)
    {
        fprintf (fp, MSG_OPTIONS_MSG4, it->c_str());
    }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void OptionsParser::displayWarnings (FILE* fp)
{
    for (list<string>::iterator it = _warnings.begin();  it != _warnings.end();  it++)
    {
        fprintf (fp, MSG_OPTIONS_MSG5, it->c_str());
    }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void OptionsParser::displayHelpShort (FILE* fp)
{
    fprintf (fp, "%s\n", MSG_OPTIONS_MSG8);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void OptionsParser::displayHelp (FILE* fp)
{
    fprintf (fp,"[*] denotes mandatory argument.\n");
    for (list<Option*>::iterator it = _options.begin();  it != _options.end();  it++)
    {
        if (!(*it)->getLabel().empty())
        {
            fprintf (fp, "\t%s %s:\t %s\n",
                     (*it)->getLabel().c_str(),
                     (*it)->isMandatory()?"[*]":"",
                     (*it)->getHelp().c_str());
        }
    }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void OptionsParser::giveToNoOption (char* txt)
{
    Option* option = 0;

    for (list<Option*>::iterator it = _options.begin();  option==0  &&  it != _options.end();  it++)
    {
        if ((*it)->getLabel().empty())  {  option = (*it);  }
    }

    if (option != 0)
    {
        list<string> args;
        args.push_back (txt);
        option->proceed (args);
    }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
bool OptionsParser::saw (const std::string& txt)
{
    bool found = false;

    for (std::list<Option*>::iterator it = _seenOptions.begin();  !found && it != _seenOptions.end();  it++)
    {
        found = (*it)->getLabel().compare(txt) == 0;
    }

    DEBUG (("CheckOption::saw: txt='%s'  _seenOptions.size=%ld  => found=%d \n",
        txt.c_str(), _seenOptions.size(), found
    ));

    return found;
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
char* OptionsParser::checkExcludingOptions (const Option* option)
{
    if (option->getExclude().empty())
    {
        return (char*)0;
    }

    short found=0;

    for (list<Option*>::iterator it = _seenOptions.begin(); it != _seenOptions.end(); it++)
    {
        if (strstr ((*it)->getExclude().c_str(), option->getLabel().c_str()))
        {
            found=1;
            break;
        }
    }
    return (found ? (char*) option->getLabel().c_str() : (char*)0);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void OptionsParser::checkIncludingOptions ()
{
    for (list<Option*>::iterator it = _seenOptions.begin(); it != _seenOptions.end(); it++)
    {
        string include = (*it)->getInclude ();
        if (! include.empty ())
        {
            short inner_found=0;

            for (list<Option*>::iterator itInner = _seenOptions.begin(); itInner != _seenOptions.end(); itInner++)
            {
                if (strstr (include.c_str(), (*itInner)->getLabel().c_str()))
                {
                    inner_found=1;
                    break;
                }
            }
            if (!inner_found)
            {
                char buffer[128];
                snprintf (buffer, sizeof(buffer),
                        MSG_OPTIONS_MSG6,
                    (*it)->getLabel().c_str(),
                    include.c_str()
                );

                _errors.push_back (buffer);
            }
        }
    }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void OptionsParser::checkMandatoryOptions ()
{
    for (list<Option*>::iterator it = _options.begin(); it != _options.end(); it++)
    {
        bool found = false;

        for (list<Option*>::iterator itInner = _seenOptions.begin(); !found &&  itInner != _seenOptions.end(); itInner++)
        {
            found = (*it)->getLabel() == (*itInner)->getLabel();
        }

        if ((*it)->isMandatory() && !found)
        {
            char buffer[128];
            snprintf (buffer, sizeof(buffer), MSG_OPTIONS_MSG7, (*it)->getLabel().c_str());
            _errors.push_back (buffer);
        }
    }
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
const Option* OptionsParser::getSeenOption (const std::string& label)
{
    const Option* result = 0;

    for (list<Option*>::iterator it = _seenOptions.begin();  !result && it != _seenOptions.end(); it++)
    {
        if ((*it)->getLabel().compare (label) == 0)   { result = (*it); }
    }

    return result;
}

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/
