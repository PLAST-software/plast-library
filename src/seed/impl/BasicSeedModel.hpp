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

/** \file BasicSeedModel.hpp
 *  \brief Basic implementation of ISeedModel interface
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  Implementation of a basic seed model where all possible seeds (of S characters
 *  in an alphabet of N letter) are taken into account.
 *
 *  Such a model can lead to many seeds. As a matter of fact, the PLAST algorithm
 *  relies on another kind of seed models (see SubSeedModel class).
 */

#ifndef _BASIC_SEED_MODEL_HPP_
#define _BASIC_SEED_MODEL_HPP_

/********************************************************************************/

#include <seed/impl/AbstractSeedModel.hpp>
#include <seed/impl/AbstractSeedIterator.hpp>

#include <database/impl/SequenceTokenizer.hpp>

#include <string>

/********************************************************************************/
namespace seed {
/** \brief Implementation of seed based concepts. */
namespace impl {
/********************************************************************************/

/** \brief Basic implementation of ISeedModel interface
 *
 *  Seed model that knows every combination of 'span' letters of the provided alphabet.
 *  For instance, it can be a 3 span model of 20 amino acids => 20*20*20 possible seeds.
 */
class BasicSeedModel : public AbstractSeedModel
{
public:

    /** Constructor.
     * \param[in] encoding : encoding scheme of the seeds
     * \param[in] span : number of characters of one seed.
     */
    BasicSeedModel (database::Encoding encoding, size_t span, size_t extraspan = 0);

    /** Destructor. */
    virtual ~BasicSeedModel ();

    /** \copydoc AbstractSeedModel::getSeedsMaxNumber */
    size_t getSeedsMaxNumber ()  { return _seedsMaxNumber; }

    /** \copydoc AbstractSeedModel::createSeedsIterator */
    ISeedIterator* createSeedsIterator (const database::IWord& data);

    /** \copydoc AbstractSeedModel::createAllSeedsIterator */
    ISeedIterator* createAllSeedsIterator ();

    /** \copydoc AbstractSeedModel::getAllSeedsTable */
    const database::LETTER* getAllSeedsTable() { return 0; }

    /** \copydoc AbstractSeedModel::getSeedByString
     * WARNING ! implementation not optimized at all...
     */
    bool getSeedByString (const std::string& seedAscii, ISeed& seed);

private:

    /** Total number of seeds for this seed model. */
    size_t  _seedsMaxNumber;

    /************************************************************/

    /** \brief Data seed iteration for the basic seed model.
     */
    class DataSeedIterator  : public AbstractSeedIterator
    {
    public:
        DataSeedIterator (BasicSeedModel* model, const database::IWord& data);

    protected:
        BasicSeedModel* _specificModel;

        /** Find the next valid item. May add some increment to the current position. */
        virtual bool findNextValidItem (void);

        /** Update (if needed) of the current item object. */
        void updateItem (void) { /* done in findNextValidItem */ }
    };

    /************************************************************/

    /** \brief All seeds iteration for the basic seed model.
     */
    class AllSeedsIterator : public AbstractSeedIterator
    {
    public:
        AllSeedsIterator (BasicSeedModel* model, size_t firstIdx=1, size_t lastIdx=0);
        AllSeedsIterator (BasicSeedModel* model, const std::vector<size_t>& seedsIdx);
        virtual ~AllSeedsIterator ();

        /** Create a new iterator that is a subset of the current one. */
        ISeedIterator* extract (size_t firstIdx, size_t lastIdx);

        u_int64_t getNbTotal ()  { return _totalNumber; }

        /** \copydoc ISeedIterator::createFilteredIterator */
        ISeedIterator* createFilteredIterator (const std::vector<size_t>& seedsIdx);

    private:
        BasicSeedModel* _specificModel;
        bool findNextValidItem (void) { return true; }
        void updateItem (void);

        u_int64_t _totalNumber;

        /** We need a vector that keep indexes of seeds sorted in some way.
         *  By default, they are sorted as 0,1,2,... but it may be useful to reorder them. */
        std::vector<size_t> _seedsIdx;
    };
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_SEED_MODEL_HPP_  */
