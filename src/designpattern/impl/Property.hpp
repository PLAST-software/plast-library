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

/** \file Property.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementation of the IProperties interface.
 */

#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include <designpattern/api/IProperty.hpp>

#include <stack>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Implementation of IProperties interface.
 *
 *   This class provides a constructor that can read a file holding properties as [key,value] entries
 *   (one per line). This is useful for instance for managing a configuration file.
 */
class Properties : public IProperties
{
public:

    /** Constructor. If a file path is provided, it tries to read [key,value] entries from this file.
     * \param initfile : the file (if any) to be read
     */
    Properties (const char* initfile = 0);

    /** Destructor. */
    virtual ~Properties ();

    /** \copydoc IProperties::accept */
    void accept (IPropertiesVisitor* visitor);

    /** \copydoc IProperties::add */
    IProperty* add (size_t depth, const std::string& aKey, const char* format=0, ...);

    /** \copydoc IProperties::add(size_t,const std::string&,const std::string&)  */
    IProperty* add (size_t depth, const std::string& aKey, const std::string& aValue);

    /** \copydoc IProperties::add(size_t,IProperties*)  */
    void add (size_t depth, IProperties* prop);

    /** \copydoc IProperties::getProperty  */
    IProperty* getProperty (const std::string& key);

    /** \copydoc IProperties::clone  */
    IProperties* clone ()   {  return new Properties (*this);  }

private:

    /** List of IProperty instances. */
    std::list<IProperty*> _properties;

    /** Read a file holding [key,value] entries and add them through 'add' method.
     * \param[in] file : the file to be read
     */
    void readFile (const char* file);
};

/********************************************************************************/

/** \brief XML serialization of a IProperties instance.
 *
 *  This kind of visitor serializes into a file the content of a IProperties instance.
 *
 *  The output format is XML; the 'depth' attribute of each IProperty instance is used
 *  as a basis for building the XML tree.
 */
class XmlDumpPropertiesVisitor : public IPropertiesVisitor
{
public:

    /** Constructor.
     * \param filename : uri of the file where to serialiaze the instance. */
    XmlDumpPropertiesVisitor (const std::string& filename);

    /** Desctructor. */
    virtual ~XmlDumpPropertiesVisitor ();

    /** \copydoc IPropertiesVisitor::visitBegin */
    void visitBegin ();

    /** \copydoc IPropertiesVisitor::visitEnd */
    void visitEnd   ();

    /** \copydoc IPropertiesVisitor::visitProperty */
    void visitProperty (IProperty* prop);

private:

    /** The file where to serialiaze the instance. */
    FILE*                   _file;

    /** The name of the serialization file. */
    std::string             _name;

    /** Some stack for XML production. */
    std::stack<std::string> _stack;

    /** Internals. */
    void pop    (size_t depth);

    /** Indentation of the XML file. */
    void indent (size_t n);

    /** Method for writing into the file. */
    void safeprintf (const char* format, ...);
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _IPROPERTY_H_ */
