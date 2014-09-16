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

#include <seed/impl/AbstractSeedModel.hpp>
#include <designpattern/impl/Property.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace database;

/********************************************************************************/
namespace seed { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractSeedModel::AbstractSeedModel (size_t span, size_t extraspan)
    : _alphabet(0), _span(span), _extraSpan(extraspan)
{
    /** We get a reference on the alphabet given the encoding scheme. */
    _alphabet = EncodingManager::singleton().getAlphabet (SUBSEED);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractSeedModel::~AbstractSeedModel ()
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool AbstractSeedModel::compare (const ISeed& s1, const ISeed& s2)
{
    return getHashCode(s1.kmer) == getHashCode (s2.kmer);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SeedHashCode AbstractSeedModel::getHashCode (const IWord& word)
{
    /** Optimization: avoid to access several time to a structure attribute. */
    char size = _alphabet->size;

    /** Shorcuts. */
    const LETTER* buffer = word.letters.data;

    /** Optimization: direct computation according to the span size, full loop otherwise. */
         if (_span == 4)  { return  buffer[0] + size*(buffer[1] + size*(buffer[2] + size*buffer[3])); }
    else if (_span == 3)  { return  buffer[0] + size*(buffer[1] + size*buffer[2]); }
    else if (_span == 2)  { return  buffer[0] + size*buffer[1]; }
    else
    {
        SeedHashCode result = buffer[_span-1];
        for (int i=_span-2; i>=0; i--)
        {
            result = size * result + buffer[i];
        }
        return result;
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
IProperties* AbstractSeedModel::getProperties ()
{
    IProperties* props = new Properties();

    props->add (0, "seeds_model");
    props->add (1, "span",          "%ld", getSpan());
    props->add (1, "seeds_number",  "%ld", getSeedsMaxNumber());

    return props;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
