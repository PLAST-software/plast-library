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

#ifndef _IPROPERTY_H_
#define _IPROPERTY_H_

#include "SmartPointer.hpp"
#include <string>
#include <list>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/********************************************************************************/
namespace dp {
/********************************************************************************/

/** Define what a property can be. This could be generalized to other components. */
struct IProperty
{
    IProperty (size_t aDepth, const std::string& aKey, const std::string& aValue)
        : depth(aDepth), key(aKey), value(aValue)  {}

    size_t      depth;
    std::string key;
    std::string value;

    const std::string&  getValue  ()  { return value;                }
    long                getInt    ()  { return atol (value.c_str()); }
    const char*         getString ()  { return value.c_str();        }
};

/********************************************************************************/

class IPropertiesVisitor : public SmartPointer
{
public:
    virtual void visitBegin    () = 0;
    virtual void visitProperty (IProperty* prop) = 0;
    virtual void visitEnd      () = 0;
};

/********************************************************************************/

class IProperties : public SmartPointer
{
public:

    /** */
    virtual void accept (IPropertiesVisitor* visitor) = 0;

    /** */
    virtual IProperty* add (size_t depth, const std::string& aKey, const char* format=0, ...) = 0;

    /** */
    virtual IProperty* add (size_t depth, const std::string& aKey, const std::string& aValue) = 0;

    /** */
    virtual void       add (size_t depth, IProperties* prop) = 0;

    virtual IProperty* getProperty (const std::string& key) = 0;
};


/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IPROPERTY_H_ */
