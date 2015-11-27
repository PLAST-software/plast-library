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

/** \file AlignmentFilterXML.hpp
 *  \brief Concepts about alignments filtering.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALIGNMENT_FILTER_XML_HPP_
#define _ALIGNMENT_FILTER_XML_HPP_

/********************************************************************************/

#include <alignment/filter/impl/AbstractAlignmentFilterFactory.hpp>

#include <map>
#include <string>

/********************************************************************************/
namespace alignment {
namespace filter    {
namespace impl      {
/********************************************************************************/

/** \brief Definition of an alignment filtering manager
 */
class AlignmentFilterFactoryXML : public AbstractAlignmentFilterFactory
{
public:

    /** */
    AlignmentFilterFactoryXML ();

    /** */
    virtual ~AlignmentFilterFactoryXML ();

    /** */
    IAlignmentFilter* createFilter (const char* name, ...);

private:

    /** We define a Rule structure that keeps information read from a XML filter file. */
    struct Rule
    {
        std::string _accessor;
        std::string _operator;
        std::string _type;
        std::vector<std::string> _values;

        void reset ()  {
            _accessor.clear();
            _operator.clear ();
            _type.clear ();
            _values.clear();
        }
    };

    /** We will use a listener for catching information of interest during a XML file reading. */
    class XmlFilterListener : public dp::IObserver
    {
    public:

        XmlFilterListener () : _exclusive(true), _isReadingValue(false) {}

        /** */
        bool isExclusive () { return _exclusive; }

        /** */
        const std::list<Rule>& getRules ()  { return _rules; }

        /** */
        void update (dp::EventInfo* evt, dp::ISubject* subject);

        /** */
        const std::string& getTitle () { return _title; }

    private:

        bool _exclusive;
        Rule _currentRule;
        bool _isReadingValue;
        std::list<Rule> _rules;
        std::string _title;
        std::string _lastText;
        std::string _lastAttributeName;
        std::string _lastAttributeValue;
    };

    /** */
    IAlignmentFilter* createFilterFromRule (const Rule& rule);

    /** */
    IAlignmentFilter* getFilter (const std::list<IAlignmentFilter*>& filters, bool isExclusive);

    /** Parse the XML filter file and extract useful information. */
    void parseXmlFile (const std::string& xmlFileUri, XmlFilterListener& listener);
};

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_FILTER_XML_HPP_ */
