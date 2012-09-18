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
 *
 *  We provide here an XML parser in the SAX manner. The idea is that an XML reader
 *  reads some input stream and launches specific notifications when some events occurs
 *  (tag open, tag close...)
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

/** Root class for all XML events. Inherits from dp::EventInfo.
 */
class XmlEvent : public dp::EventInfo
{
public:
    /** Constructor. */
    XmlEvent () : dp::EventInfo(0) {}
};

/** XML event corresponding to a tag opening.
 */
class XmlTagOpenEvent : public XmlEvent
{
public:
    /** Constructor.
     * \param[in] name : name of the tag. */
    XmlTagOpenEvent (const std::string& name) : _name(name) {}

    /** Tag name*/
    std::string _name;
};

/** XML event corresponding to a tag closing.
 */
class XmlTagCloseEvent : public XmlEvent
{
public:
    /** Constructor.
     * \param[in] name : name of the tag. */
    XmlTagCloseEvent (const std::string& name) : _name(name) {}

    /** Tag name*/
    std::string _name;
};

/** XML event corresponding to a text
 */
class XmlTagTextEvent : public XmlEvent
{
public:
    /** Constructor.
     * \param[in] txt : found text. */
    XmlTagTextEvent (const std::string& txt) : _txt(txt) {}

    /** The found text. */
    std::string _txt;
};

/** XML event corresponding to an attribute
 */
class XmlTagAttributeEvent : public XmlEvent
{
public:
    /** Constructor.
     * \param[in] name : name of the attribute
     * \param[in] value : value of the attribute
     */
    XmlTagAttributeEvent (const std::string& name, const std::string& value)
        : _name(name), _value(value) {}

    /** Name of the attribute. */
    std::string _name;

    /** Value of the attribute. */
    std::string _value;
};

/********************************************************************************/

/** \brief Simple implementation of an XML (SAX) parser
 *
 * This implementation considers that the reader is mainly a Subject, and therefore
 * sends notification to potential listeners as its parsing goes on.
 *
 * Any attached observer is likely to receive instances of subclasses of XMLEvent, and
 * has to do something in reaction.
 *
 * \code
 * // We create some XML listener class
 * class XmlListener : public IObserver
 * {
 * public:
 *      void update (EventInfo* evt, ISubject* subject)
 *      {
 *          XmlTagOpenEvent* open = dynamic_cast<XmlTagOpenEvent*> (evt);
 *          if (open)  {  cout << "open '" << open->_name << "'" << endl;  return;  }
 *
 *          XmlTagCloseEvent* close = dynamic_cast<XmlTagCloseEvent*> (evt);
 *          if (close)  {  cout << "close '" << close->_name << "'" << endl;  return;  }
 *
 *          XmlTagTextEvent* text = dynamic_cast<XmlTagTextEvent*> (evt);
 *          if (text)  {  cout << "text '" << text->_txt << "'" << endl;  return;  }
 *
 *          XmlTagAttributeEvent* attribute = dynamic_cast<XmlTagAttributeEvent*> (evt);
 *          if (attribute)  {  cout << "attribute: name='" << attribute->_name << "'  value='" << attribute->_value << "'" << endl;  return;  }
 *     }
 * };
 *
 * void foo ()
 * {
 *      // We define some input stream holding an XML string
 *      stringstream is (stringstream::in | stringstream::out);
 *      is << "<properties><progression><exec_percentage>100</exec_percentage><nb_alignments>4257</nb_alignments></progression>properties>";
 *
 *      // We create a reader with our input stream.
 *      XmlReader reader (is);
 *
 *      // We instantiate one observer
 *       IObserver* listener = new XmlListener();
 *
 *      // We attach the listener to the reader
 *      reader.addObserver (listener);
 *
 *      // We read the XML stream
 *      reader.read();
 *
 *      // We detach the listener from the reader.
 *      reader.removeObserver (listener);
 *  }
 * \endcode
 *
 * \see XmlTagOpenEvent
 * \see XmlTagCloseEvent
 * \see XmlTagTextEvent
 * \see XmlTagAttributeEvent
 */
class XmlReader : public Subject
{
public:

    /** Constructor.
     * \param[in] is : the input stream containing the XML stream. */
    XmlReader (std::istream& is);

    /** Destructor. */
    virtual ~XmlReader();

    /** Parse the input stream and possibly sends notifications to potential observers.
     */
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
