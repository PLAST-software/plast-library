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

#ifndef _ABSTRACT_SEED_MODEL_HPP_
#define _ABSTRACT_SEED_MODEL_HPP_

/********************************************************************************/

#include "ISeedModel.hpp"
#include "IAlphabet.hpp"
#include "AbstractSeedIterator.hpp"

#include <string>

/********************************************************************************/
namespace seed {
/********************************************************************************/

class AbstractSeedModel : public ISeedModel
{
public:

    AbstractSeedModel (size_t span);

    virtual ~AbstractSeedModel ();

    /** Get alphabet. */
    database::IAlphabet* getAlphabet () { return _alphabet; }

    /** Get the span (ie. the number of letters of a seed). */
    size_t getSpan ()  { return _span; }

    /** Compares two seeds. */
    bool compare (const ISeed& s1, const ISeed& s2);

    dp::IProperties* getProperties ();

protected:

    virtual SeedHashCode getHashCode (const database::IWord& word);

    database::IAlphabet* _alphabet;
    size_t               _span;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_SEED_MODEL_HPP_  */
