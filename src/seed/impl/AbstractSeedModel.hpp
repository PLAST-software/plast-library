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

/** \file AbstractSeedModel.hpp
 *  \brief Abstract implementation of ISeedModel interface
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  Abstract class that factorizes some common parts between the ISeedModel
 *  implementors.
 */

#ifndef _ABSTRACT_SEED_MODEL_HPP_
#define _ABSTRACT_SEED_MODEL_HPP_

/********************************************************************************/

#include <seed/api/ISeedModel.hpp>
#include <database/api/IAlphabet.hpp>
#include <seed/impl/AbstractSeedIterator.hpp>

#include <string>

/********************************************************************************/
namespace seed {
/** \brief Implementation of seed based concepts. */
namespace impl {
/********************************************************************************/

/** \brief Abstract class factorizing common parts between the ISeedModel implementors
 *
 *  This class is mainly defined for factorization purpose.
 *
 *  Note however that we introduce the getHashCode method intended to compute a perfect
 *  hash code for a given word. We provide a default implementation here that could be
 *  refined in subclasses. According to the kind of seed model, the hash function proposed
 *  here may be minimal or not.
 */
class AbstractSeedModel : public ISeedModel
{
public:

    /** Constructor.
     * \param[in] span : span of the seeds.
     */
    AbstractSeedModel (size_t span);

    /** Destructor. */
    virtual ~AbstractSeedModel ();

    /** \copydoc ISeedModel::getAlphabet */
    database::IAlphabet* getAlphabet () { return _alphabet; }

    /** \copydoc ISeedModel::getSpan */
    size_t getSpan ()  { return _span; }

    /** \copydoc ISeedModel::compare */
    bool compare (const ISeed& s1, const ISeed& s2);

    /** \copydoc ISeedModel::getProperties */
    dp::IProperties* getProperties ();

    /** \copydoc ISeedModel::getSeedByString */
    bool getSeedByString (const std::string& seedAscii, ISeed& seed)  { return false; }

protected:

    /** Compute a perfect hash code for a given word.
     * \param word : word we want to compute a hash code for.
     * \return the computed hash code.
     */
    virtual SeedHashCode getHashCode (const database::IWord& word);

    /** Alphabet. */
    database::IAlphabet* _alphabet;

    /** Span of the seed model.*/
    size_t               _span;

    /** Extra span of the seed model.*/
    size_t               _extraSpan;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_SEED_MODEL_HPP_  */
