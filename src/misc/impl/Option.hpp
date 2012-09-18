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

/** \file Option.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Define an abstraction for command line option
 */

#ifndef _OPTION_HPP
#define _OPTION_HPP

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>

#include <string>
#include <list>

/********************************************************************************/
/** \brief PLAST command line. */
namespace misc {
namespace impl {
/********************************************************************************/

/** \brief Interface for command line option
 *
 * Define an interface for what is and what contains an option.
 *
 * It can't be instantiated since the constructor is protected and so, has to be derived.
 *
 * \see OptionsParser
 */
class Option : public dp::SmartPointer
{
public:

    /** Constructor.
     * \param[in] name : name of the option
     * \param[in] nbArgs : number of arguments for this option
     * \param[in] mandatory : tells whether this option is mandatory or not
     * \param[in] help : textual help for this option
     * \param[in] multiple : tells whether this option may be used more than once
     * \param[in] include : list of names of options that must be used with the current one
     * \param[in] exclude : list of names of options that must not be used with the current one
     */
    Option (
        const std::string& name,
        int nbArgs,
        bool mandatory,
        const std::string& help,
        int multiple,
        const std::string& include,
        const std::string& exclude
    )
        : _name(name), _nbArgs(nbArgs), _mandatory(mandatory), _help(help), _multiple(multiple), _include(include), _exclude(exclude)
    {
    }

    /** Desctructor. */
    virtual ~Option() {}

    /** Label of the option (example "-log").
     * \return the label
     */
    const std::string& getLabel () const   { return _name; }

    /** Parameter string
     * \return the parameter.
     */
    const std::string& getParam ()  const { return _param; }

protected:

    /** Gives the number of arguments that must follow the option.
     * \return the arguments number.
     */
    size_t getNbArgs () const   { return _nbArgs; }

    /** Tells whether the option is mandatory or not.
     * \return the mandatory status.
     */
    bool isMandatory () const { return _mandatory; }

    /** Help text about this option.
     * \return help string
     */
    const std::string& getHelp () const { return _help; }

    /** Tell if the option can be used more than once.
     * \return true if can be multiple, false otherwise
     */
    short canBeMultiple () const    { return _multiple; }

    /** List of options that should be used with the current one.
     * The format of this list is for example "-C,-x,-X")
     * \return list of options
     */
    const std::string& getInclude () const  { return _include; }

    /** List of options that must not be used with the current one.
     * The format of this list is for example "-F,-x,-X")
     * \return list of options
     */
    const std::string& getExclude ()  const { return _exclude; }

    /** When an option is recognized in the argumenst list, we look the number of waited args and put
     * them in a list of string objects. This is this list that is given as argument of the proceed() method
     * that mainly will affect the given args to the variable given to the instanciation of the
     * (derived class) Option.
     */
    virtual int proceed (const std::list<std::string>& args) = 0;

    std::string     _name;
    size_t          _nbArgs;
    bool            _mandatory;
    std::string     _help;
    short           _multiple;
    std::string     _include;
    std::string     _exclude;
    std::string     _param;

    /* Since the CheckOption class is responsable to the full job, we let it access to the internal informations
     of one Option. */
    friend class OptionsParser;
};

/********************************************************************************/

/** \brief Option that has no argument.
 *
 * This is a special option (with no name) that memorize the arguments that are not
 * involved with a known option.
 */
class OptionNoParam : public Option
{
public:

    /** Constructor.
     * \param[in] name : name of the option
     * \param[in] help : textual help for this option
     * \param[in] mandatory : tells whether this option is mandatory or not
     * \param[in] multiple : tells whether this option may be used more than once
     * \param[in] include : list of names of options that must be used with the current one
     * \param[in] exclude : list of names of options that must not be used with the current one
     */
    OptionNoParam (
        const std::string& name,
        const std::string& help,
        bool mandatory = false,
        int multiple = 0,
        const char* include = "",
        const char* exclude = ""
    )
        : Option (name, 0, mandatory, help, multiple, include, exclude)
    {
    }

    /** \copydoc Option::proceed */
    int proceed (const std::list<std::string>& args)  {  return 1;  }
};

/********************************************************************************/

/** \brief Option that has one argument.
 *
 * This is a special option with only one argument.
 */
class OptionOneParam : public Option
{
public:

    /** Constructor.
     * \param[in] name : name of the option
     * \param[in] help : textual help for this option
     * \param[in] mandatory : tells whether this option is mandatory or not
     * \param[in] multiple : tells whether this option may be used more than once
     * \param[in] include : list of names of options that must be used with the current one
     * \param[in] exclude : list of names of options that must not be used with the current one
     */
    OptionOneParam (
        const std::string& name,
        const std::string& help,
        bool mandatory = false,
        int multiple = 0,
        const char* include = "",
        const char* exclude = ""
    )
        : Option (name, 1, mandatory, help, multiple, include, exclude)
    {
    }

    /** \copydoc Option::proceed */
    int proceed (const std::list<std::string>& args)
    {
        _param = *(args.begin());
        return 1;
    }
};

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/

#endif /* _OPTION_HPP */
