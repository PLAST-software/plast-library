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

/** \file TimeTools.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Tools for time statistics.
 */

#ifndef TIME_TOOLS_HPP_
#define TIME_TOOLS_HPP_

/********************************************************************************/

#include <misc/api/types.hpp>

#include <os/api/IResource.hpp>
#include <os/api/ITime.hpp>

#include <designpattern/impl/Property.hpp>

#include <map>

/********************************************************************************/
/** \brief Operating System abstraction layer */
namespace os {
/** \brief Implementation of Operating System abstraction layer */
namespace impl {
/********************************************************************************/

/** \brief tool for time statistics.
 */
class TimeStats : public dp::SmartPointer
{
public:
    TimeStats (ITime& aTime) : _time(aTime), _t0(0), _t1(0)   {  _t0 = _time.gettime();  }

    void tick (const char* message, const char* funcname=__PRETTY_FUNCTION__, size_t lineno=__LINE__)
    {
        _t1 = _time.gettime();
        Entry e = { (_t1-_t0), message, funcname, lineno };
        _entries.push_back (e);
        _t0 = _t1;
    }

    void dump (int (*trace) (const char*, ...) = printf)
    {
        u_int32_t total = 0;
        for (std::list <Entry>::iterator it=_entries.begin(); it!=_entries.end(); it++)
        {
            Entry& e = *it;
            trace ("[STATS %30s:%ld] %4ld msec '%s'\n", e.function, e.lineno, e.duration, e.message.c_str());
            total += e.duration;
        }
        trace ("[STATS TOTAL %ld msec]\n", total);
    }

private:

    struct Entry
    {
        u_int32_t   duration;
        std::string message;

        const char* function;
        size_t      lineno;
    };

    ITime& _time;

    u_int32_t _t0;
    u_int32_t _t1;

    std::list <Entry> _entries;
};

/** Define a macro for having the function name and the line. */
#if 0
    #define TIME_TICK(t,msg)  t.tick (msg,__FILE__, __LINE__)
    #define TIME_DUMP(t)      t.dump ()
#else
    #define TIME_TICK(t,msg)
    #define TIME_DUMP(t)
#endif

/********************************************************************************/

/** \brief tool for time statistics.
 */
class TimeInfo : public dp::SmartPointer
{
public:

    TimeInfo (ITime& aTime) : _time(aTime)  { }

    virtual void addEntry (const char* name)
    {
        _entries [name] = _time.gettime();
    }

    virtual void stopEntry (const char* name)
    {
        _entries [name] = _time.gettime() - _entries [name];
    }

    const std::map <std::string, u_int32_t>& getEntries ()  { return _entries; }

    u_int32_t getEntryByKey (const std::string& key)
    {
        u_int32_t result = 0;
        std::map <std::string, u_int32_t>::iterator  it = _entries.find (key);
        if (it != _entries.end())  { result = it->second; }
        return result;
    }

    /** Return properties about the instance. */
    virtual dp::IProperties* getProperties (const std::string& root)
    {
        dp::impl::Properties* props = new dp::impl::Properties();

        props->add (0, root,   "");

        std::map <std::string, u_int32_t>::const_iterator  it;
        for (it = getEntries().begin(); it != getEntries().end();  it++)
        {
            props->add (1, it->first.c_str(), "%ld", it->second);
        }

        return props;
    }

private:

    ITime&  _time;
    std::map <std::string, u_int32_t>  _entries;
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* TIME_TOOLS_HPP_ */
