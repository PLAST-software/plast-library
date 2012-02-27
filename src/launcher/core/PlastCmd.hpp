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

/** \file PlastCmd.hpp
 *  \brief Define a command that executes the plast algorithm
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _PLAST_CMD_HPP
#define _PLAST_CMD_HPP

/********************************************************************************/

#include <designpattern/api/ICommand.hpp>
#include <designpattern/api/IProperty.hpp>

#include <launcher/observers/AbstractObserver.hpp>

/********************************************************************************/
/** \brief PLAST command line. */
namespace launcher {
namespace core     {
/********************************************************************************/

/** \brief  Define common class for target observer implementations
 *
 * We just assemble the dp::IObserver and the dp::SmartPointer interfaces as skeleton
 * for the concrete observers we want to implement.
 */
class PlastCmd : public dp::ICommand
{
public:

    /** Constructor. */
    PlastCmd (dp::IProperties* properties);

    /** Constructor. */
    virtual ~PlastCmd ();

    /** \copydoc dp::ICommand::update*/
    void execute ();

private:

    dp::IProperties* _properties;
    void setProperties (dp::IProperties* properties)  { SP_SETATTR(properties); }

    /** */
    void configureObservers (
        dp::IProperties* properties,
        std::list<observers::AbstractObserver*>& observers
    );

    /** */
    dp::IPropertiesVisitor* getPropertiesVisitor (
        dp::IProperties* props,
        const std::string& filename
    );
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _PLAST_CMD_HPP */
