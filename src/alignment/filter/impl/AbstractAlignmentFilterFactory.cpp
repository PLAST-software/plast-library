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

#include <alignment/filter/impl/AbstractAlignmentFilterFactory.hpp>
#include <alignment/filter/impl/AlignmentFilterOperator.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;

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
AbstractAlignmentFilterFactory::AbstractAlignmentFilterFactory ()
{
    /** We create some filters. */
    _filtersList.push_back (new AlignmentFilter_HitFrom());
    _filtersList.push_back (new AlignmentFilter_HitTo());

    _filtersList.push_back (new AlignmentFilter_QueryFrom());
    _filtersList.push_back (new AlignmentFilter_QueryTo());

    _filtersList.push_back (new AlignmentFilter_Length());

    _filtersList.push_back (new AlignmentFilter_Evalue());
    _filtersList.push_back (new AlignmentFilter_Bitscore());
    _filtersList.push_back (new AlignmentFilter_Score());

    _filtersList.push_back (new AlignmentFilter_NbIdentities());
    _filtersList.push_back (new AlignmentFilter_PercentIdentities());

    _filtersList.push_back (new AlignmentFilter_NbPositives());
    _filtersList.push_back (new AlignmentFilter_PercentPositives());

    _filtersList.push_back (new AlignmentFilter_QueryCoverage());
    _filtersList.push_back (new AlignmentFilter_SubjectCoverage());

    _filtersList.push_back (new AlignmentFilter_NbGaps());
    _filtersList.push_back (new AlignmentFilter_PercentGaps());

    _filtersList.push_back (new AlignmentFilter_NbMissses());
    _filtersList.push_back (new AlignmentFilter_PercentMisses());

    _filtersList.push_back (new AlignmentFilter_HitLength());

    _filtersList.push_back (new AlignmentFilter_HitsGaps());
    _filtersList.push_back (new AlignmentFilter_QueryGaps());

    _filtersList.push_back (new AlignmentFilter_HitFrame());
    _filtersList.push_back (new AlignmentFilter_QueryFrame());

    _filtersList.push_back (new AlignmentFilter_HSPNumber());
    _filtersList.push_back (new AlignmentFilter_HSPRank());

    _filtersList.push_back (new AlignmentFilter_HitsNumber());
    _filtersList.push_back (new AlignmentFilter_HitRank());

    _filtersList.push_back (new AlignmentFilter_QueryNumber());
    _filtersList.push_back (new AlignmentFilter_QueryRank());

    _filtersList.push_back (new AlignmentFilter_QueryDefinition());
    _filtersList.push_back (new AlignmentFilter_HitDefinition());

    /** We build the map of [name,filter] couples from the list. */
    for (list<IAlignmentFilter*>::iterator it = _filtersList.begin(); it != _filtersList.end(); it++)
    {
        _filtersMap [(*it)->getName()] = *it;
        DEBUG (("AbstractAlignmentFilterFactory::AbstractAlignmentFilterFactory  add filter '%s'\n",
            (*it)->getName().c_str ()
        ));
    }

    DEBUG (("AbstractAlignmentFilterFactory::AbstractAlignmentFilterFactory  map.size=%ld \n", _filtersMap.size ()));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractAlignmentFilterFactory::~AbstractAlignmentFilterFactory ()
{
    /** We delete all prototype instances from the filters map. */
    for (map<string,IAlignmentFilter*>::iterator it = _filtersMap.begin(); it != _filtersMap.end(); it++)
    {
        delete it->second;
    }

    _filtersMap.clear ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IAlignmentFilter* AbstractAlignmentFilterFactory::clone (const string& name, const vector<string>& args)
{
    IAlignmentFilter* result = 0;

    /** We try to get an entry from the filters map given a name. */
    map<string,IAlignmentFilter*>::iterator lookup = _filtersMap.find (name);

    if (lookup != _filtersMap.end ())
    {
        /** We clone the found instance prototype. */
        result = (lookup->second)->clone (args);
    }

    return result;
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
