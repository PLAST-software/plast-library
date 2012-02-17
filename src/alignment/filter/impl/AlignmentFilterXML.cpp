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
    /** We create some filters. */
    _filtersList.push_back (new AlignmentFilter_HitFrom());
    _filtersList.push_back (new AlignmentFilter_HitTo());
    _filtersList.push_back (new AlignmentFilter_QueryFrom());
    _filtersList.push_back (new AlignmentFilter_QueryTo());
    _filtersList.push_back (new AlignmentFilter_Length());
    _filtersList.push_back (new AlignmentFilter_Evalue());
    _filtersList.push_back (new AlignmentFilter_Bitscore());
    _filtersList.push_back (new AlignmentFilter_NbIdentities());
    _filtersList.push_back (new AlignmentFilter_PercentIdentities());
    _filtersList.push_back (new AlignmentFilter_QueryCoverage());
    _filtersList.push_back (new AlignmentFilter_SubjectCoverage());

    /** We build the map of [name,filter] couples from the list. */
    for (list<IAlignmentFilter*>::iterator it = _filtersList.begin(); it != _filtersList.end(); it++)
    {
        _filtersMap [(*it)->getName()] = *it;
        DEBUG (("AlignmentFilterFactoryXML::AlignmentFilterFactoryXML  add filter '%s'\n",
            (*it)->getName().c_str ()
        ));
    }

    DEBUG (("AlignmentFilterFactoryXML::AlignmentFilterFactoryXML  map.size=%ld \n", _filtersMap.size ()));
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
    /** We delete all prototype instances from the filters map. */
    for (map<string,IAlignmentFilter*>::iterator it = _filtersMap.begin(); it != _filtersMap.end(); it++)
    {
        delete it->second;
    }

    _filtersMap.clear ();
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
IAlignmentFilter* AlignmentFilterFactoryXML::createFilter (const std::string& xmlFileUri)
{
    IAlignmentFilter* result = 0;

    DEBUG (("AlignmentFilterFactoryXML::createFilter : uri='%s' \n", xmlFileUri.c_str() ));

    /** We will need a XML listener for getting XML nodes information during parsing. */
    XmlFilterListener listener;

    /** We parse the XML file. */
    parseXmlFile (xmlFileUri, listener);

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
    args.push_back (rule._value);

    /** We try to get an entry from the filters map given a name. */
    map<string,IAlignmentFilter*>::iterator lookup = _filtersMap.find (rule._accessor);

    if (lookup != _filtersMap.end ())
    {
        /** We clone the found instance prototype. */
        result = (lookup->second)->clone (args);
    }

    DEBUG (("AlignmentFilterFactoryXML::createFilterFromRule  access='%s'  op='%s'  val='%s'  => result=%p \n",
        rule._accessor.c_str(),
        rule._operator.c_str(),
        rule._value.c_str(),
        result
    ));

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
IAlignmentFilter* AlignmentFilterFactoryXML::getFilter (list<IAlignmentFilter*> filters, bool isExclusive)
{
    if (filters.empty())  { return 0; }

    IAlignmentFilter* first = filters.front();

    if (filters.size() == 1)  { return first; }
    else
    {
        filters.pop_front();

        if (isExclusive)
        {
            return new AlignmentFilterAnd (first, getFilter (filters, isExclusive));
        }
        else
        {
            return new AlignmentFilterOr (first, getFilter (filters, isExclusive));
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
void AlignmentFilterFactoryXML::XmlFilterListener::update (EventInfo* evt, ISubject* subject)
{
    XmlTagOpenEvent* open = dynamic_cast<XmlTagOpenEvent*> (evt);
    if (open)
    {
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
        else if (close->_name.compare ("accessor")  == 0)  {  _currentRule._accessor = _lastText;  }
        else if (close->_name.compare ("operator")  == 0)  {  _currentRule._operator = _lastText;  }
        else if (close->_name.compare ("value")     == 0)  {  _currentRule._value    = _lastText;  }
        else if (close->_name.compare ("exclusive") == 0)
        {
            _exclusive = _lastText.compare ("true") == 0;
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
