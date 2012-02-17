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

/** \file XmlReader.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementation of a XML reader
 */

#ifndef _XML_READER_H_
#define _XML_READER_H_

#include <designpattern/impl/Observer.hpp>
#include <list>
#include <iostream>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** */
class XmlEvent : public dp::EventInfo
{
public:
    XmlEvent () : dp::EventInfo(0) {}
};

/** */
class XmlTagOpenEvent : public XmlEvent
{
public:
    XmlTagOpenEvent (const std::string& name) : _name(name) {}
    std::string _name;
};

/** */
class XmlTagCloseEvent : public XmlEvent
{
public:
    XmlTagCloseEvent (const std::string& name) : _name(name) {}
    std::string _name;
};

/** */
class XmlTagTextEvent : public XmlEvent
{
public:
    XmlTagTextEvent (const std::string& txt) : _txt(txt) {}
    std::string _txt;
};

/** */
class XmlTagAttributeEvent : public XmlEvent
{
public:
    XmlTagAttributeEvent (const std::string& name, const std::string& value)
        : _name(name), _value(value) {}
    std::string _name;
    std::string _value;
};

/********************************************************************************/

/**
 */
class XmlReader : public Subject
{
public:

    /** Constructor. */
    XmlReader (std::istream& is);

    /** Destructor. */
    virtual ~XmlReader();

    /** */
    void read ();

private:

    /** */
    std::istream& _is;

    /** */
    void processMainState (std::istream& is);
    void processTagState  (std::istream& is);

    /** */
    void normalizeText (std::string& s);

    /** */
    void replace (std::string& str, const std::string& oldStr, const std::string& newStr);
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _XML_READER_H_ */
