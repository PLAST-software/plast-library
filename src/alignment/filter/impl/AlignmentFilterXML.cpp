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

#include <alignment/filter/impl/AlignmentFilterXML.hpp>
#include <alignment/filter/impl/AlignmentFilterOperator.hpp>

#include <designpattern/impl/XmlReader.hpp>

#include <string>
#include <iostream>
#include <fstream>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace dp::impl;

/********************************************************************************/
namespace alignment {
namespace filter    {
namespace impl      {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlignmentFilterFactoryXML::AlignmentFilterFactoryXML ()
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlignmentFilterFactoryXML::~AlignmentFilterFactoryXML ()
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlignmentFilterFactoryXML::parseXmlFile (const string& xmlFileUri, XmlFilterListener& listener)
{
    /** We open a stream on the provided file. */
    ifstream is;
    is.open (xmlFileUri.c_str(), ios::binary);

    /** We create a reader for our stream. */
    XmlReader reader (is);

    /** We attach the listener to the reader. */
    reader.addObserver (&listener);

    /** We read the XML file. */
    reader.read();

    /** We detach the listener from the reader. */
    reader.addObserver (&listener);

    /** We close the file. */
    is.close ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IAlignmentFilter* AlignmentFilterFactoryXML::createFilter (const char* name, ...)
{
    IAlignmentFilter* result = 0;

    DEBUG (("AlignmentFilterFactoryXML::createFilter : uri='%s' \n", xmlFileUri.c_str() ));

    /** We will need a XML listener for getting XML nodes information during parsing. */
    XmlFilterListener listener;

    /** We parse the XML file. */
    parseXmlFile (name, listener);

    DEBUG (("AlignmentFilterFactoryXML::createFilter : found %ld rules\n", listener.getRules().size() ));

    /** Now we got all the information from the XML file; we can create our alignment filter. */
    list<IAlignmentFilter*> filters;
    for (list<Rule>::const_iterator it = listener.getRules().begin(); it != listener.getRules().end(); it++)
    {
        IAlignmentFilter* filter = createFilterFromRule (*it);
        if (filter != 0)  {  filters.push_back (filter); }
    }

    DEBUG (("AlignmentFilterFactoryXML::createFilter :  filters.size=%ld\n", filters.size() ));

    result = getFilter (filters, listener.isExclusive());

    /** We may have to set the title. */
    if (result != 0)  { result->setTitle (listener.getTitle()); }

    DEBUG (("AlignmentFilterFactoryXML::createFilter : result=%p\n", result));

    /** We return the resulting filter. */
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
IAlignmentFilter* AlignmentFilterFactoryXML::createFilterFromRule (const Rule& rule)
{
    IAlignmentFilter* result = 0;

    std::vector<std::string> args;
    args.push_back (rule._operator);

    for (size_t i=0; i<rule._values.size(); i++)
    {
        args.push_back (rule._values[i]);
    }

    /** We try to get an entry from the filters map given a name. */
    map<string,IAlignmentFilter*>::iterator lookup = _filtersMap.find (rule._accessor);

    if (lookup != _filtersMap.end ())
    {
        /** We clone the found instance prototype. */
        result = (lookup->second)->clone (args);
    }

    DEBUG (("AlignmentFilterFactoryXML::createFilterFromRule  access='%s'  op='%s'  nbValues=%ld  => result=%p \n",
        rule._accessor.c_str(),
        rule._operator.c_str(),
        rule._values.size(),
        result
    ));

    for (size_t i=0; i<args.size(); i++)
    {
        DEBUG (("AlignmentFilterFactoryXML::createFilterFromRule        args[%ld]='%s'\n", i, args[i].c_str() ));
    }

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
IAlignmentFilter* AlignmentFilterFactoryXML::getFilter (const list<IAlignmentFilter*>& filters, bool isExclusive)
{
    if (filters.empty())  { return 0; }

    if (filters.size() == 1)  { return filters.front(); }
    else
    {
        if (isExclusive)    {  return new AlignmentFilterAnd (filters);  }
        else                {  return new AlignmentFilterOr  (filters);  }
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
void AlignmentFilterFactoryXML::XmlFilterListener::update (EventInfo* evt, ISubject* subject)
{
    XmlTagOpenEvent* open = dynamic_cast<XmlTagOpenEvent*> (evt);
    if (open)
    {
              if (open->_name.compare ("BRule") == 0)  {  _currentRule.reset ();   }
        else  if (open->_name.compare ("value") == 0)  {  _isReadingValue = true;  }
        return;
    }

    XmlTagCloseEvent* close = dynamic_cast<XmlTagCloseEvent*> (evt);
    if (close)
    {
        if (close->_name.compare ("BRule") == 0)
        {
            /** We add the rule into the list. */
            _rules.push_back (_currentRule);
        }
        else if (close->_name.compare ("name")      == 0)  {  _title                 = _lastText;  }
        else if (close->_name.compare ("accessor")  == 0)  {  _currentRule._accessor = _lastText;  }
        else if (close->_name.compare ("operator")  == 0)  {  _currentRule._operator = _lastText;  }
        else if (close->_name.compare ("exclusive") == 0)  {  _exclusive = _lastText.compare ("true") == 0;  }
        else if (close->_name.compare ("value")     == 0)
        {
            _isReadingValue = false;

            if (_currentRule._values.empty())
            {
                _currentRule._values.push_back (_lastText);
            }
        }
        else if (_isReadingValue == true)
        {
            _currentRule._values.push_back (_lastText);
        }

        return;
    }

    XmlTagTextEvent* text = dynamic_cast<XmlTagTextEvent*> (evt);
    if (text)
    {
        _lastText = text->_txt;
        return;
    }

    XmlTagAttributeEvent* attribute = dynamic_cast<XmlTagAttributeEvent*> (evt);
    if (attribute)
    {
        if (attribute->_name.compare("class") == 0)
        {
            _currentRule._type = attribute->_value;
        }
        return;
    }
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
