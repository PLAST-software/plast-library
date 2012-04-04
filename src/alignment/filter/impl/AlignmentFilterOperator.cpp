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
#define DEBUG(a)  //printf a

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
bool AlignmentFilterBinaryOperator::isOk (const core::Alignment& align) const
{
    bool result = false;

    DEBUG (("AlignmentFilterBinaryOperator::isOk: size=%ld\n", _filters.size() ));

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

    DEBUG (("AlignmentFilterBinaryOperator::isOk: result=%d\n", result ));

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
    : AlignmentFilterUnaryOperator<std::string>(args), _err(0)
{
    //TO BE DONE _err = regcomp (&_preg, _value.c_str(), REG_NOSUB | REG_EXTENDED | REG_ICASE);

    DEBUG (("AlignmentFilterRegexOperator::AlignmentFilterRegexOperator   _value='%s'   _err=%d \n", _value.c_str(), _err ));
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
    //TO BE DONE regfree (&_preg);
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
