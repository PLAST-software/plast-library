/*
 * ICommand.hpp
 *
 *  Created on: Jun 20, 2011
 *      Author: edrezen
 */

#include "AbstractSeedModel.hpp"
#include "Property.hpp"

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace database;

/********************************************************************************/
namespace seed {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractSeedModel::AbstractSeedModel (size_t span)
    : _alphabet(0), _span(span)
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
dp::IProperties* AbstractSeedModel::getProperties ()
{
    dp::IProperties* props = new dp::Properties();

    props->add (0, "seeds_model");
    props->add (1, "span",          "%ld", getSpan());
    props->add (1, "seeds_number",  "%ld", getSeedsMaxNumber());

    return props;
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
