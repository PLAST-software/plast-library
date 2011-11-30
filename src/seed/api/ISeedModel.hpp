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

/** \file ISeedModel.hpp
 *  \brief Definition of the concept of seeds model
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  A seeds model defines a set of seeds. This set can depend on:
 *    - the size of the seeds
 *    - the alphabet of the seeds
 */

#ifndef _ISEED_MODEL_HPP_
#define _ISEED_MODEL_HPP_

/********************************************************************************/

#include <seed/api/ISeed.hpp>
#include <seed/api/ISeedIterator.hpp>
#include <database/api/IAlphabet.hpp>
#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/api/IProperty.hpp>
#include <misc/api/types.hpp>

/********************************************************************************/
/** \brief Seed based concepts. */
namespace seed {
/********************************************************************************/

/** \brief Definition of a seed model.
 *
 *  A seed model defines a set of seeds.
 *
 *  For instance, a seed model for the amino acids that defines seeds of 4 letters
 *  may hold 20^4=16000 seeds, ie. all the possible words of 4 letters in an alphabet
 *  of 20 letters.
 *
 *  Note that some implementation may use some kind of equivalence relation between seeds,
 *  for instance "PFRA" being equivalent to "PFRG"; in this case, one keeps only one
 *  item for all equivalent seeds, which leads to a seeds cardinal much smaller than the
 *  all-possible-seeds model.
 *
 *  This interface provides:
 *   - a way to iterate all the seeds of this set
 *   - a way to iterate all the seeds in a word
 *
 *  These two kinds of iterations are achieved by creating a specific subclass of Iterator,
 *  named ISeedIterator.
 */
class ISeedModel : public dp::SmartPointer
{
public:

    /** Get the alphabet associated to the seed model.
     * \return the alphabet.
     */
    virtual  database::IAlphabet* getAlphabet () = 0;

    /** Get the span (ie. the number of letters of a seed).
     * \return the span
     */
    virtual size_t getSpan () = 0;

    /** Returns the cardinal of the seeds set for the seed model.
     * \return seeds number.
     */
    virtual size_t getSeedsMaxNumber () = 0;

    /** Returns an iterator that loops over seeds in a word (a sequence for instance).
     * \param[in] data : the data
     * \return the seed iterator.
     */
    virtual ISeedIterator* createSeedsIterator (const database::IWord& data) = 0;

    /** Returns an iterator that loops over all possible seeds for the model.
     * \return a seed iterator.
     */
    virtual ISeedIterator* createAllSeedsIterator () = 0;

    /** Tells whether two seeds are equivalent.
     * \param[in] s1 : first seed
     * \param[in] s2 : second seed
     * \return true if the two seeds are equivalent, false otherwise.
     */
    virtual bool compare (const ISeed& s1, const ISeed& s2) = 0;

    /** Returns a table holding all the concatenated seeds.
     * \return the table.
     */
    virtual const database::LETTER* getAllSeedsTable() = 0;

    /** Retrieve a seed given its ASCII representation (like "PQRS")
     * param[in]  seedAscii : the ASCII representation of the seed
     * param[out] seed : the ISeed instance to be retrieved
     * \return true if the seed has been found, false otherwise.
     */
    virtual bool getSeedByString (const std::string& seedAscii, ISeed& seed) = 0;

    /** Return properties about the instance.
     * \return properties
     */
    virtual dp::IProperties* getProperties () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEED_MODEL_HPP_  */
