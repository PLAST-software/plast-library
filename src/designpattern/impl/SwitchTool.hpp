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

/** \file SwitchTool.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief
 */

#ifndef _SWITCH_TOOL_HPP_
#define _SWITCH_TOOL_HPP_

#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/impl/Property.hpp>

#include <os/impl/DefaultOsFactory.hpp>

#include <string>
#include <iostream>
#include <sstream>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Tool that measures some observables.
 */
template <typename Action, typename VALUE> class SwitchTool : public dp::SmartPointer
{
public:

    /** */
    typedef int ID;
    static const ID BAD_ID = -1;

    /** Constructor.
     * \param[in] iterator : the referenced iterator.
     */
    SwitchTool (const Action& action) : _action(action), _name(""), _currentId(0)
    {
        for (ID i=0; i<MaxId; i++)
        {
            _tableStart[i] = 0;
            _tableTotal[i] = 0;
        }
    }

    /** Destructor. */
    virtual ~SwitchTool ()  {}

    /** */
    void setName (const std::string& name)  { _name = name; }

    /** */
    ID retrieveId (const char* name)
    {
        ID result = BAD_ID;

        if (_currentId < MaxId)
        {
            result = _currentId++;
            _names [result] = name;
        }

        return result;
    }

    /** */
    void start (ID id)  {  _tableStart [id]  = _action();                   }
    void stop  (ID id)  {  _tableTotal [id] += _action() - _tableStart[id]; }

    /** */
    VALUE get (ID id)  {  return _tableTotal [id];  }

    /** */
    void accept (dp::IPropertiesVisitor* visitor)
    {
        LOCAL (visitor);

        dp::IProperties* props = getProperties (_name.empty() ? "measure" : _name);
        LOCAL (props);

        props->accept (visitor);
    }

    /** Creates and return as a IProperties instance the whole timing information.
     * \param[in] root : root name of the properties to be returned.
     * \return the created IProperties instance.
     */
    virtual dp::IProperties* getProperties (const std::string& root)
    {
        double K = 1.0/2.66  / 1000000000.0;

        dp::impl::Properties* props = new dp::impl::Properties();

        props->add (0, root,   "");

        for (ID i=0; i<_currentId; i++)
        {
            std::stringstream ss;   ss << ((double)_tableTotal[i] * K);
            props->add (1, _names[i].c_str(), "%s", ss.str().c_str());
        }

        return props;
    }

private:

    /** */
    Action _action;

    /** */
    std::string _name;

    /** */
    static const ID MaxId = 50;

    /** */
    ID _currentId;

    /** */
    std::string _names[MaxId];

    /** */
    VALUE _tableStart[MaxId];
    VALUE _tableTotal[MaxId];
};

/********************************************************************************/

extern "C" u_int64_t ReadTSC ();

/** \brief Sample of actions to be used with SwitchTool
 */
class TSCReadAction   {  public:  u_int64_t operator ()  ()  { return ReadTSC(); }  };
class TimeReadAction  {  public:  u_int32_t operator ()  ()  { return os::impl::DefaultFactory::time().gettime(); }  };

class TSCReader  : public SwitchTool<TSCReadAction,u_int64_t>   {
    public: TSCReader  () : SwitchTool (TSCReadAction())  {}
};

class TimeReader : public SwitchTool<TimeReadAction,u_int32_t>  {
    public: TimeReader () : SwitchTool (TimeReadAction()) {}
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _SWITCH_TOOL_HPP_ */
