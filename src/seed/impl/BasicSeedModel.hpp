/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

#ifndef _BASIC_SEED_MODEL_HPP_
#define _BASIC_SEED_MODEL_HPP_

/********************************************************************************/

#include "AbstractSeedModel.hpp"
#include "AbstractSeedIterator.hpp"

#include <string>

/********************************************************************************/
namespace seed {
/********************************************************************************/

/** Seed model that knows every combination of 'span' letters of the provided alphabet.
 *  For instance, it can be a 3 span model of 20 amino acids => 20*20*20 possible seeds.
 */
class BasicSeedModel : public AbstractSeedModel
{
public:

    BasicSeedModel (database::Encoding encoding, size_t span);

    virtual ~BasicSeedModel ();

    /** */
    size_t getSeedsMaxNumber ()  { return _seedsMaxNumber; }

    /** Returns an iterator that loops over seeds in a word (a sequence for instance).  */
    ISeedIterator* createSeedsIterator (const database::IWord& data);

    /** Returns an iterator that loops over all possible seeds for the model.  */
    ISeedIterator* createAllSeedsIterator ();

    const database::LETTER* getAllSeedsTable() { return 0; }

private:

    size_t  _seedsMaxNumber;

    /************************************************************/
    class DataSeedIterator  : public AbstractSeedIterator
    {
    public:
        DataSeedIterator (BasicSeedModel* model, const database::IWord& data);
    private:
        BasicSeedModel* _specificModel;

        /** Find the next valid item. May add some increment to the current position. */
        virtual bool findNextValidItem (void);

        /** Update (if needed) of the current item object. */
        void updateItem (void) { /* done in findNextValidItem */ }
    };

    /************************************************************/
    class AllSeedsIterator : public AbstractSeedIterator
    {
    public:
        AllSeedsIterator (BasicSeedModel* model, size_t firstIdx=1, size_t lastIdx=0);
        virtual ~AllSeedsIterator ();

        /** Create a new iterator that is a subset of the current one. */
        ISeedIterator* extract (size_t firstIdx, size_t lastIdx);

        u_int64_t getNbTotal ()  { return _totalNumber; }

    private:
        BasicSeedModel* _specificModel;
        bool findNextValidItem (void) { return true; }
        void updateItem (void);

        u_int64_t _totalNumber;
    };
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_SEED_MODEL_HPP_  */
