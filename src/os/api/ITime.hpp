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

/*****************************************************************************
 *   Operating System abstraction of time retrieval.
 *****************************************************************************/

#ifndef ITIME_HPP_
#define ITIME_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"
#include "Property.hpp"

#include <stdio.h>
#include "types.hpp"
#include <string>
#include <list>
#include <map>

/********************************************************************************/
namespace os {
/********************************************************************************/

/** */
class ITime
{
public:
    /** Returns (in msec) the time. */
    virtual u_int32_t gettime () = 0;

    /** Returns the clock. */
    virtual u_int32_t getclock() = 0;
};

/********************************************************************************/

/** */
class TimeStats
{
public:
    TimeStats (ITime& aTime) : _time(aTime)  {  _t0 = _time.gettime();  }

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

/** */
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
        dp::Properties* props = new dp::Properties();

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

/** */
class TimeSum : public dp::SmartPointer
{
public:

    TimeSum (ITime& atime, u_int32_t& sum) : _time(atime), _sum(sum)
    {
        _t0 = _time.getclock();
    }

    virtual ~TimeSum ()
    {
        _sum += _time.getclock() - _t0;
    }

private:
    ITime&     _time;
    u_int32_t& _sum;
    u_int32_t  _t0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* ITIME_HPP_ */
