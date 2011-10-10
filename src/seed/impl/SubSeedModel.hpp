/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

#ifndef _SUB_SEED_MODEL_HPP_
#define _SUB_SEED_MODEL_HPP_

/********************************************************************************/

#include "AbstractSeedModel.hpp"
#include "AbstractSeedIterator.hpp"

#include <stdarg.h>

#include <string>
#include <vector>
#include <list>

/********************************************************************************/
namespace seed {
/********************************************************************************/

/** Seed model that uses subseed set concept.
 */
class SubSeedModel : public AbstractSeedModel
{
public:

    SubSeedModel ();
    SubSeedModel (const char* filename);
    SubSeedModel (size_t nbEntries, ...);
    SubSeedModel (size_t nbEntries, va_list va);
    SubSeedModel (const std::vector<std::string>& subseedStrings);

    virtual ~SubSeedModel ();

    /** */
    size_t getSeedsMaxNumber ()  { return _seedsMaxNumber; }

    /** Returns an iterator that loops over seeds in a word (a sequence for instance).  */
    ISeedIterator* createSeedsIterator (const database::IWord& data);

    /** Returns an iterator that loops over all possible seeds for the model.  */
    ISeedIterator* createAllSeedsIterator ();

    /** */
    const database::LETTER* getAllSeedsTable();

    /** Debug purpose. */
    void dump (void);

protected:

    void initialize (size_t span);

    database::LETTER* _allSeedsTable;

    size_t  _seedsMaxNumber;

    const database::LETTER* _convertTable;

    char**  _equivalenceTable;

    std::vector< std::vector<database::LETTER> > _equivalentsTable;

    size_t _currentNumber;

    void addEntry (const char* sequence);

    /************************************************************/
    class DataSeedIterator  : public AbstractSeedIterator
    {
    public:
        DataSeedIterator (SubSeedModel* model, const database::IWord& data);
    private:
        SubSeedModel* _specificModel;
        char** _equivalenceTable;

        /** Find the next valid item. May add some increment to the current position. */
        bool findNextValidItem (void);

        /** Update (if needed) of the current item object. */
        void updateItem (void) { /* done in findNextValidItem */ }
    };

    /************************************************************/
    class AllSeedsIterator : public AbstractSeedIterator
    {
    public:
        AllSeedsIterator (SubSeedModel* model, size_t firstIdx=1, size_t lastIdx=0);
        AllSeedsIterator (SubSeedModel* model, const std::vector<size_t>& seedsIdx);
        virtual ~AllSeedsIterator ();

        /** Create a new iterator that is a subset of the current one. */
        ISeedIterator* extract (size_t firstIdx, size_t lastIdx);

        u_int64_t getNbTotal ()  { return _totalNumber; }

        ISeedIterator* createFilteredIterator (const std::vector<size_t>& seedsIdx);

    private:
        SubSeedModel*           _specificModel;
        const database::LETTER* _allSeedsTable;
        bool findNextValidItem (void) { return true; }
        void updateItem ();

        u_int64_t _totalNumber;

        /** We need a vector that keep indexes of seeds sorted in some way.
         *  By default, they are sorted as 0,1,2,... but it may be useful to reorder them. */
        std::vector<size_t> _seedsIdx;
    };
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _SUB_SEED_MODEL_HPP_  */
