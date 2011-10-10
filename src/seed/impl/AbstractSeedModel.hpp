/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

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
