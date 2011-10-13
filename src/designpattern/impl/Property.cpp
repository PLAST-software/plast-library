/*******************************************************************************
 * 
 * @file /vobs/th_comp/util_design_pattern/src/dp_observer.cpp
 *
 * @brief this file contains the definition of the EventInfoLocator class
 *
 * Copyright (C) 2009-2010 Thomson
 * All Rights Reserved
 *
 * This program contains proprietary information which is a trade
 * secret of THOMSON and/or its affiliates and also is protected as
 * an unpublished work under applicable Copyright laws. Recipient is
 * to retain this program in confidence and is not permitted to use or
 * make copies thereof other than as permitted in a written agreement
 * with THOMSON, unless otherwise expressly allowed by applicable laws
 *
 ******************************************************************************/

#include "Property.hpp"
#include "FileLineIterator.hpp"
#include <stdarg.h>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;

/********************************************************************************/
namespace dp {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
Properties::Properties (const char* initfile)
{
    if (initfile != 0)  {   readFile (initfile);  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
Properties::~Properties ()
{
    for (std::list<IProperty*>::iterator it = _properties.begin(); it != _properties.end(); it++)
    {
        delete *it;
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
void Properties::accept (IPropertiesVisitor* visitor)
{
    visitor->visitBegin ();

    for (std::list<IProperty*>::iterator it = _properties.begin(); it != _properties.end(); it++)
    {
        visitor->visitProperty (*it);
    }

    visitor->visitEnd ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IProperty* Properties::add (size_t depth, const std::string& aKey, const char* format, ...)
{
    IProperty* result = 0;

    if (format != 0)
    {
        char buffer[256];
        va_list ap;
        va_start (ap, format);
        vsnprintf (buffer, sizeof(buffer), format, ap);
        va_end (ap);

        result = new IProperty (depth, aKey, buffer);
        _properties.push_back (result);
    }
    else
    {
        result = new IProperty (depth, aKey, "");
        _properties.push_back (result);
    }

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
IProperty* Properties::add (size_t depth, const std::string& aKey, const std::string& aValue)
{
    IProperty* result = new IProperty (depth, aKey, aValue);
    _properties.push_back (result);
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
void Properties::add (size_t depth, IProperties* properties)
{
    if (properties)
    {
        LOCAL (properties);

        /** We define a local visitor for adding items of provided 'prop' to the current list. */
        class LocalVisitor : public IPropertiesVisitor
        {
        public:
            LocalVisitor (size_t depth, IProperties* ref) : _depth(depth), _ref(ref) {}
            virtual ~LocalVisitor() {}

            void visitBegin () {}
            void visitEnd   () {}

            void visitProperty (IProperty* prop)
            {
                if (_ref &&  prop)  {  _ref->add ( prop->depth + _depth, prop->key, prop->value);  }
            }
        private:
            size_t       _depth;
            IProperties* _ref;
        };

        /** We accept such a visitor. */
        LocalVisitor v (depth, this);
        properties->accept (&v);
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
IProperty* Properties::getProperty (const std::string& key)
{
    IProperty* result = 0;

    for (list<IProperty*>::iterator it = _properties.begin(); !result  &&  it != _properties.end(); it++)
    {
        if (key.compare ((*it)->key)==0)    { result = *it; }
    }

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
void Properties::readFile (const char* filename)
{
    FileLineIterator it (filename, 1024);

    for (it.first(); !it.isDone(); it.next())
    {
        char* buffer = it.currentItem();

        if (buffer && *buffer)
        {
            char* key   = strtok (buffer, " \t");
            char* value = strtok (NULL,   " \t");

            add (0, key, (value ? value : ""));
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
XmlDumpPropertiesVisitor::XmlDumpPropertiesVisitor (const std::string& filename)
    : _file (0), _name ("properties")
{
    /** We open the file. */
    _file = fopen (filename.c_str(), "w");

    /** We add the initial tag. */
    safeprintf ("<%s>", _name.c_str());
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
XmlDumpPropertiesVisitor::~XmlDumpPropertiesVisitor ()
{
    /** We add the final tag. */
    safeprintf ("</%s>", _name.c_str());

    /** Some clean up. */
    if (_file != 0)    {  fclose (_file);  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void XmlDumpPropertiesVisitor::visitBegin ()
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
void XmlDumpPropertiesVisitor::visitEnd ()
{
    /** We dump the remaining tags. */
    pop (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void XmlDumpPropertiesVisitor::visitProperty (IProperty* prop)
{
    if (prop != 0)
    {
        size_t actualDepth = prop->depth + 2;

        if (actualDepth > _stack.size())
        {
            safeprintf ("\n");
            indent (actualDepth);
            safeprintf ("<%s>%s", prop->key.c_str(), prop->value.c_str());
            _stack.push (prop->key);
        }

        else if (actualDepth == _stack.size())
        {
            safeprintf ("</%s>\n", _stack.top().c_str());
            _stack.pop();

            indent (actualDepth);
            safeprintf ("<%s>%s",  prop->key.c_str(),  prop->value.c_str() );
            _stack.push (prop->key);
        }

        else
        {
            pop (actualDepth);

            indent (actualDepth);
            safeprintf ("<%s>%s",  prop->key.c_str(),  prop->value.c_str() );
            _stack.push (prop->key);
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
void XmlDumpPropertiesVisitor::pop (size_t depth)
{
    safeprintf ("</%s>\n", _stack.top().c_str());
    _stack.pop();

    while (_stack.size() >= depth && !_stack.empty())
    {
        indent (_stack.size());
        safeprintf ("</%s>\n", _stack.top().c_str());
        _stack.pop();
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
void XmlDumpPropertiesVisitor::indent (size_t n)
{
    for (size_t i=1; i<=n; i++)  {  safeprintf ("   ");  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void XmlDumpPropertiesVisitor::safeprintf (const char* format, ...)
{
    /** A safe printf method that check that the output file is ok. */
    if (_file != 0)
    {
        va_list ap;
        va_start (ap, format);
        vfprintf (_file, format, ap);
        va_end (ap);
    }
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
