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

#include <os/impl/CommonOsImpl.hpp>
#include <designpattern/impl/Property.hpp>
#include <designpattern/impl/FileLineIterator.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>
#include <designpattern/impl/ProductIterator.hpp>
#include <designpattern/impl/XmlReader.hpp>

#include <stdarg.h>

#include <iostream>
#include <fstream>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace os::impl;

/********************************************************************************/
namespace dp { namespace impl {
/********************************************************************************/

class InsertionVisitor : public IPropertiesVisitor
{
public:
    InsertionVisitor (size_t depth, IProperties* ref, set<string> keys) : _depth(depth), _ref(ref), _keys (keys) {}
    virtual ~InsertionVisitor() {}

    void visitBegin () {}
    void visitEnd   () {}

    void visitProperty (IProperty* prop)
    {
        if (_ref &&  prop)
        {
            /** We add the prop only if there is not already an existing identical prop. */
            if (_keys.find (prop->key) == _keys.end())
            {
                _ref->add ( prop->depth + _depth, prop->key, prop->value);
            }
        }
    }
private:
    size_t       _depth;
    IProperties* _ref;
    set<string>  _keys;
};

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
    if ((initfile != 0)&&(CommonFile::isFileExist(string(initfile))))  {   readFile (initfile);  }
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
IProperties* Properties::clone ()
{
    IProperties* result = new Properties ();

    for (std::list<IProperty*>::iterator it = _properties.begin(); it != _properties.end(); it++)
    {
        result->add ((*it)->depth, (*it)->key, (*it)->value);
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

        /** We accept a visitor. */
        set<string>  nokeys;
        InsertionVisitor v (depth, this, nokeys);
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
void Properties::merge (IProperties* properties)
{
    if (properties)
    {
        LOCAL (properties);

        /** We accept a visitor. */
        set<string>  nokeys;
        InsertionVisitor v (0, this, this->getKeys());
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
    FileLineIterator it (filename);

    for (it.first(); !it.isDone(); it.next())
    {
        char* buffer = it.currentItem();

        if (buffer && *buffer)
        {
            char* key   = strtok (buffer, " \t");

            if (key != 0)
            {
                char* value = key + strlen (key) + 1;

                for ( ;  value; ++value)  {  if (*value != ' '  &&  *value != '\t')  { break; }  }

                add (0, key, string(value ? value : ""));
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
void Properties::readXML (std::istream& stream)
{
    /** We create an XML reader. */
    XmlReader reader (stream);

    /** We create some specific observer class. */
    class XmlObserver : public IObserver
    {
    public:
        XmlObserver (Properties* ref) : _ref(ref), _depth(-1), _currentProperty(0)  {}

        void update (EventInfo* evt, ISubject* subject)
        {
            XmlTagOpenEvent* e1 = dynamic_cast<XmlTagOpenEvent*> (evt);
            if (e1)
            {
                /** We open a tag => increase the depth. */
                _depth ++;

                /** We add a property and keep a reference on it. */
                _currentProperty = _ref->add (_depth, e1->_name.c_str(), _txt.c_str());

                DEBUG (("XmlTagOpenEvent (%d): name=%s\n", _depth, e1->_name.c_str()));
                return;
            }

            XmlTagCloseEvent* e2 = dynamic_cast<XmlTagCloseEvent*> (evt);
            if (e2)
            {
                /** We close a tag => decrease the depth. */
                _depth --;

                DEBUG (("XmlTagCloseEvent(%d) : name=%s\n", _depth, e2->_name.c_str()));
                return;
            }

            XmlTagTextEvent* e3 = dynamic_cast<XmlTagTextEvent*> (evt);
            if (e3)
            {
                /** We find a text => set it to the current property.  */
                if (_currentProperty)  { _currentProperty->value = e3->_txt; }

                DEBUG (("XmlTagTextEvent (%d) : txt=%s\n", _depth, e3->_txt.c_str()));
                return;
            }
        }

    private:
        Properties* _ref;
        string      _name;
        string      _txt;
        int         _depth;
        IProperty*  _currentProperty;
    };

    /** We attach this kind of observer to the reader. */
    XmlObserver observer (this);
    reader.addObserver (&observer);

    /** We read the stream. */
    reader.read ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
std::list<IProperties*> Properties::map (const char* separator)
{
    list<IProperties*> result;

    list <Iterator<char*>* > itList;

    for (list<IProperty*>::iterator it = _properties.begin(); it != _properties.end(); it++)
    {
        itList.push_back (new TokenizerIterator ((*it)->getString(), separator));
    }
    DEBUG (("Properties::map : nbIt=%ld\n", itList.size() ));

    CartesianIterator <char*> p (itList);

    for (p.first(); ! p.isDone(); p.next())
    {
        list<char*>& current = p.currentItem();

        IProperties* dup = this->clone();
        result.push_back (dup);

        list<char*>::iterator      itStr;
        list<IProperty*>::iterator itProps;

        for (itStr=current.begin(), itProps=_properties.begin();
             itStr!=current.end() && itProps!=_properties.end();
             itStr++, itProps++
        )
        {
            if (*itStr != 0 && (*itProps)->value.compare(*itStr) != 0)
            {
                DEBUG (("key='%s'  current='%s'  new='%s'\n", (*itProps)->key.c_str(), (*itProps)->value.c_str(), (*itStr)));
                dup->getProperty ((*itProps)->key)->value = *itStr;
            }
        }
    }

    /** Some cleanup. */
    for (list<Iterator<char*>*>::iterator it = itList.begin(); it != itList.end(); it++)
    {
        delete *it;
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
set<string> Properties::getKeys ()
{
    set<string> result;

    for (list<IProperty*>::iterator it = _properties.begin(); it != _properties.end(); it++)
    {
        result.insert (result.end(), (*it)->key);
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
void Properties::setToFront (const std::string& key)
{
    for (list<IProperty*>::iterator it = _properties.begin(); it != _properties.end(); it++)
    {
        if (key.compare ((*it)->key)==0)
        {
            /** We move the found key to the beginning of the container. */
            _properties.splice (_properties.begin(), _properties, it);
            break;
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
AbstractOutputPropertiesVisitor::AbstractOutputPropertiesVisitor (std::ostream& aStream)
    : _stream(0)
{
    /** A stream is provided, we keep a reference on it. */
    _stream = &aStream;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractOutputPropertiesVisitor::AbstractOutputPropertiesVisitor (const std::string& filename)
    : _stream(0), _filename(filename)
{
    if (_filename.empty() == false)
    {
        /** We create a file. */
        _stream = new fstream (_filename.c_str(), ios::out);
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
AbstractOutputPropertiesVisitor::~AbstractOutputPropertiesVisitor ()
{
    if (_filename.empty() == false)
    {
        delete _stream;
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
XmlDumpPropertiesVisitor::XmlDumpPropertiesVisitor (
    const std::string& filename,
    bool propertiesAsRoot,
    bool shouldIndent
)
    : AbstractOutputPropertiesVisitor (filename),
      _name (propertiesAsRoot ? "properties" : ""), _deltaDepth(0), _firstIndent(true), _shouldIndent(shouldIndent)
{
    /** We add the initial tag. */
    if (_name.empty() == false)
    {
        indent (0);
        safeprintf ("<%s>", _name.c_str());
    }

    _deltaDepth = _name.empty() == false ? 0 : 1;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
XmlDumpPropertiesVisitor::XmlDumpPropertiesVisitor (
    std::ostream& aStream,
    bool propertiesAsRoot,
    bool shouldIndent
)
    : AbstractOutputPropertiesVisitor (aStream),
      _name (propertiesAsRoot ? "properties" : ""), _deltaDepth(0), _firstIndent(true), _shouldIndent(shouldIndent)
{
    /** We add the initial tag. */
    if (_name.empty() == false)
    {
        indent (0);
        safeprintf ("<%s>", _name.c_str());
    }

    _deltaDepth = _name.empty() == false ? 0 : 1;
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
    if (_name.empty() == false)
    {
        indent (0);
        safeprintf ("</%s>", _name.c_str());
    }

    safeprintf ("\n");
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
        size_t actualDepth = prop->depth + 1;

        DEBUG (("XmlDumpPropertiesVisitor::visitProperty:  actualDepth=%ld stack.size=%ld '%s' \n",
            actualDepth, _stack.size(), prop->toString().c_str()
        ));

        if (actualDepth > _stack.size())
        {
            indent (actualDepth);
            safeprintf ("<%s>%s", prop->key.c_str(), prop->value.c_str());
            _stack.push (prop->key);
        }

        else if (actualDepth == _stack.size())
        {
            safeprintf ("</%s>", _stack.top().c_str());
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
    safeprintf ("</%s>", _stack.top().c_str());
    _stack.pop();

    while (_stack.size() >= depth && !_stack.empty())
    {
        indent (_stack.size());
        safeprintf ("</%s>", _stack.top().c_str());
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
    if (!_shouldIndent)  { return; }

    if (!_firstIndent)  {  safeprintf ("\n");  }

    for (size_t i=1; i<=(n-_deltaDepth); i++)  {  safeprintf ("   ");  }

    _firstIndent = false;
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
    if (_stream != 0)
    {
        char buffer[4*1024];

        va_list ap;
        va_start (ap, format);
        vsnprintf (buffer, sizeof(buffer), format, ap);
        va_end (ap);

        (*_stream) << buffer;
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
RawDumpPropertiesVisitor::RawDumpPropertiesVisitor (const std::string& filename)
    : _file(0), _fileToClose(true)
{
    /** We open the file. */
    _file = fopen (filename.c_str(), "w");
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
RawDumpPropertiesVisitor::~RawDumpPropertiesVisitor ()
{
    if (_fileToClose  &&  _file)  { fclose (_file); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void RawDumpPropertiesVisitor::visitProperty (IProperty* prop)
{
    if (_file != 0)
    {
        int width = 40;

        string indent;
        for (size_t i=0; i<prop->depth; i++)  { indent += "    "; }

        if (prop->getValue().empty() == false)
        {
            fprintf (_file, "%s%-*s : %s\n", indent.c_str(), width, prop->key.c_str(), prop->value.c_str());
        }
        else
        {
            fprintf (_file, "%s%-*s\n", indent.c_str(), width, prop->key.c_str());
        }
    }
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
