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

#include <alignment/filter/impl/AlignmentFilterOperator.hpp>
#include <designpattern/impl/Property.hpp>

#include <stdio.h>
#define DEBUG(a)    //printf a
#define VERBOSE(a)

using namespace std;
using namespace dp;
using namespace dp::impl;

/********************************************************************************/
namespace alignment {
namespace filter    {
namespace impl      {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlignmentFilterBinaryOperator::AlignmentFilterBinaryOperator (list<IAlignmentFilter*>& filters)
    : _filters (filters)
{
    /** We have to 'use' each item of the list. */
    for (list<IAlignmentFilter*>::iterator it = _filters.begin(); it != _filters.end(); ++it)
    {
        (*it)->use();
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
AlignmentFilterBinaryOperator::~AlignmentFilterBinaryOperator ()
{
    /** We have to 'forget' each item of the list. */
    for (list<IAlignmentFilter*>::iterator it = _filters.begin(); it != _filters.end(); ++it)
    {
        (*it)->forget();
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
bool AlignmentFilterBinaryOperator::isOk (const core::Alignment& align) const
{
    bool result = false;

    VERBOSE (("AlignmentFilterBinaryOperator::isOk: size=%ld\n", _filters.size() ));

    /** A little check. */
    if (_filters.empty())  { return false; }

    /** We need to iterate the list of filters. */
    std::list<IAlignmentFilter*>::const_iterator it = _filters.begin();

    if (it != _filters.end())
    {
        /** We initialize the result with the result for the first alignment. */
        result = (*it)->isOk(align);

        for (it++  ; result==true && it != _filters.end(); it++)  {  result = getResult (result, (*it), align);  }
    }

    VERBOSE (("AlignmentFilterBinaryOperator::isOk: result=%d\n", result ));

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
std::string AlignmentFilterBinaryOperator::toString ()
{
    std::stringstream ss;

    DEBUG (("AlignmentFilterBinaryOperator::toString: nbFilters=%ld \n", _filters.size() ));

    for (list<IAlignmentFilter*>::iterator it = _filters.begin();  it != _filters.end(); it++)
    {
        ss << "(" << (*it)->toString() << ") ";
    }

    DEBUG (("AlignmentFilterBinaryOperator::toString: '%s'\n", ss.str().c_str() ));

    return ss.str();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
dp::IProperties* AlignmentFilterBinaryOperator::getProperties ()
{
    DEBUG (("AlignmentFilterBinaryOperator::getProperties: size=%ld\n", _filters.size() ));

    IProperties* result = new Properties ();

    result->add (0, "mode", getName());

    for (list<IAlignmentFilter*>::iterator it = _filters.begin();  it != _filters.end(); it++)
    {
        result->add (0, (*it)->getProperties());
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
AlignmentFilterRegexOperator::AlignmentFilterRegexOperator (const std::vector<std::string>& args)
    : AlignmentFilterUnaryOperator<std::string>(args), _reg(0), _regExtra(0)
{
    const char* pcreErrorStr = 0;
    int pcreErrorOffset = 0;

    /** We compile the regexp. */
    _reg = pcre_compile (_value.c_str(), 0, &pcreErrorStr, &pcreErrorOffset, NULL);

    /** We optimize the regex. */
    if (_reg != 0)  {  _regExtra = pcre_study (_reg, 0, &pcreErrorStr);  }

    DEBUG (("AlignmentFilterRegexOperator::AlignmentFilterRegexOperator  _value='%s'  _reg=%p  _regExtra=%p\n",
        _value.c_str(), _reg, _regExtra
    ));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlignmentFilterRegexOperator::~AlignmentFilterRegexOperator ()
{
    if (_reg)       {  pcre_free (_reg);        }
    if (_regExtra)  {  pcre_free (_regExtra);   }

    DEBUG (("AlignmentFilterRegexOperator::~AlignmentFilterRegexOperator\n"));
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
