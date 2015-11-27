/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
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

#include <designpattern/impl/Observer.hpp>

#include <algo/core/api/IAlgoEnvironment.hpp>

#include <launcher/observers/AbstractObserver.hpp>

/********************************************************************************/
/** \brief PLAST command line. */
namespace launcher {
namespace core     {
/********************************************************************************/

/** \brief  High level ICommand implementation for launching PLAST algorithm
 *
 *  This class is mainly a ICommand that runs PLAST for a given set of arguments (provided
 *  as a IProperties instance).
 *
 *  So, running PLAST should be as simple as running such an instance.
 *
 *  Moreover, the PlastCmd is both an Observer and a Subject.
 *
 *  It is an Observer by receiving notifications from the inner parts of the algorithm. On such
 *  receptions, the PlastCmd instance only forward the notification (as a Subject). This mechanism
 *  is useful for externalizing some inner notifications to the world outside PLAST (see JNI implementation
 *  for instance).
 *
 *  It is a also a Subject for some specific observers (see method configureObservers) that are
 *  systematically attached to it for gathering information about the execution of the algorithm itself.
 *  For instance, the BargraphObserver is an observer attached to PlastCmd in order to receive information
 *  about the percentage of execution, which allows to display some bargraph to the end user for having
 *  an idea of the remaining execution time. Other observers gather other kind of information for dumping
 *  statistical information about the found alignments.
 *
 *  We can see there the power of the Observer Design Pattern. The algorithm sends notifications about its
 *  inner state, but doesn't know at all for instance that it will be used for displaying a bargraph.
 *
 *  Note that a PlastCmd can be stopped by calling cancel. As a result, the inner parts of the algorithm
 *  should stop as soon as possible (and possibly in a clean fashion).
 *
 *  \code
 *  // we create a IProperties instance holding the subject and query databases URIs
 *  IProperties* props = new Properties ();
 *  props->add (0, "-d",     "/databases/HalobacteriumSalinarum.fa");
 *  props->add (0, "-i",     "/databases/query.fa");
 *  props->add (0, "-o",     "/tmp/plast.out");
 *  props->add (0, "-a",     "1");
 *
 *  // We create the PLAST command with the arguments above
 *  ICommand* cmd = new PlastCmd (props);
 *
 *  // We launch the request through some dispatcher.
 *  SerialCommandDispatcher().dispatchCommand (cmd);
 *  \endcode
 *
 *  \see observers::BargraphObserver
 *  \see observers::AlgoVerboseObserver
 *  \see observers::AlgoPropertiesObserver
 *  \see observers::AlgoHitsPropertiesObserver
 *  \see observers::FileProgressionObserver
 *  \see observers::AlignmentProgressionObserver
 *  \see observers::ResourcesObserver
 */
class PlastCmd : public dp::ICommand, public dp::impl::Subject, public dp::IObserver
{
public:

    /** Constructor.
     * \param[in] properties : configuration for the PLAST command
     */
    PlastCmd (dp::IProperties* properties);

    /** Constructor. */
    virtual ~PlastCmd ();

    /** \copydoc dp::ICommand::execute */
    void execute ();

    /** Cancel the command. */
    void cancel ();

    /** \copydoc dp::IObserver::update */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

    /** Tells whether the command is finished or not.
     * \return true if the command is finished, false otherwise.
     */
    bool isRunning ()  { return _isFinished == false; }

    /** Returns the properties of the command
     * \return the command properties.
     */
    dp::IProperties* getProperties ()  { return _properties; }

private:

    algo::core::IEnvironment* _env;
    void setEnv (algo::core::IEnvironment* env)  { SP_SETATTR(env); }

    dp::IProperties* _properties;
    void setProperties (dp::IProperties* properties)  { SP_SETATTR(properties); }

    /** */
    bool _isRunning;

    /** */
    bool _isFinished;

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
