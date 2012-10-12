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

#include <os/impl/DefaultOsFactory.hpp>

#include <designpattern/impl/Property.hpp>

#include <map>

/********************************************************************************/
/** \brief Operating System abstraction layer */
namespace os {
/** \brief Implementation of Operating System abstraction layer */
namespace impl {
/********************************************************************************/

/** \brief Tool for time statistics.
 *
 * This class provides methods for getting time information between two execution points.
 *
 * One can use a label for getting a specific time duration; it is possible later to get
 * this duration by giving the label.
 *
 * Example of use:
 * \code
 void foo ()
 {
     TimeInfo t;

     t.addEntry ("part1");
     // do something here
     t.stopEntry ("part1");

     t.addEntry ("part2");
     // do something here
     t.stopEntry ("part2");

     // now, we dump the duration of part1 and part2:
     cout << "part1: " << t.getEntryByKey("part1") << "  "
          << "part2: " << t.getEntryByKey("part2") << endl;
 }
 * \endcode
  */
class TimeInfo : public dp::SmartPointer
{
public:

    /** Default constructor. */
    TimeInfo () : _time(DefaultFactory::time())  { }

    /** Constructor taking a time factory.
     * \param[in] aTime : the time factory to be used.
     */
    TimeInfo (ITime& aTime) : _time(aTime)  { }

    /** Get the start time for a given label.
     * \param[in] name : the label
     */
    virtual void addEntry (const char* name)
    {
        _entriesT0 [name] = _time.gettime();
    }

    /** Get the stop time for a given label.
     * \param[in] name : the label
     */
    virtual void stopEntry (const char* name)
    {
        _entries [name] += _time.gettime() - _entriesT0 [name];
    }

    /** Provides (as a map) all got durations for each known label/
     * \return a map holding all retrieved timing information.
     */
    const std::map <std::string, u_int32_t>& getEntries ()  { return _entries; }

    /** Retrieve the duration for a given label.
     * \param[in] key : the label we want the duration for.
     * \return the duration.
     */
    u_int32_t getEntryByKey (const std::string& key)
    {
        u_int32_t result = 0;
        std::map <std::string, u_int32_t>::iterator  it = _entries.find (key);
        if (it != _entries.end())  { result = it->second; }
        return result;
    }

    /** Creates and return as a IProperties instance the whole timing information.
     * \param[in] root : root name of the properties to be returned.
     * \return the created IProperties instance.
     */
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
    std::map <std::string, u_int32_t>  _entriesT0;
    std::map <std::string, u_int32_t>  _entries;
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* TIME_TOOLS_HPP_ */
