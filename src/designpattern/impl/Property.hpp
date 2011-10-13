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

#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include "IProperty.hpp"

#include <stack>
#include <string>

/********************************************************************************/
namespace dp {
/********************************************************************************/

class Properties : public IProperties
{
public:

    Properties (const char* initfile = 0);

    /** */
    virtual ~Properties ();

    /** */
    void accept (IPropertiesVisitor* visitor);

    /** */
    IProperty* add (size_t depth, const std::string& aKey, const char* format=0, ...);

    /** */
    IProperty* add (size_t depth, const std::string& aKey, const std::string& aValue);

    /** */
    void add (size_t depth, IProperties* prop);

    /** */
    IProperty* getProperty (const std::string& key);

private:
    std::list<IProperty*> _properties;

    void readFile (const char* file);
};

/********************************************************************************/

class XmlDumpPropertiesVisitor : public IPropertiesVisitor
{
public:
    XmlDumpPropertiesVisitor (const std::string& filename);
    virtual ~XmlDumpPropertiesVisitor ();

    void visitBegin ();
    void visitEnd   ();

    void visitProperty (IProperty* prop);

private:
    FILE*                   _file;
    std::string             _name;
    std::stack<std::string> _stack;

    void pop    (size_t depth);
    void indent (size_t n);

    void safeprintf (const char* format, ...);
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IPROPERTY_H_ */
