/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

/** \file SubSeedModel.hpp
 *  \brief Subseed implementation of ISeedModel interface
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  Implementation of subseeds sets concepts.
 */

#ifndef _SUB_SEED_MODEL_HPP_
#define _SUB_SEED_MODEL_HPP_

/********************************************************************************/

#include <seed/impl/AbstractSeedModel.hpp>
#include <seed/impl/AbstractSeedIterator.hpp>

#include <database/impl/SequenceTokenizer.hpp>

#include <stdarg.h>

#include <string>
#include <vector>
#include <list>

/********************************************************************************/
namespace seed {
/** \brief Implementation of seed based concepts. */
namespace impl {
/********************************************************************************/

/** \brief Seed model that uses subseed set concept.
 *
 *  The SubSeedModel class implements the ISeedModel interface.
 *
 *  It follows the concept of subseeds where some seeds are equivalent to each other.
 *
 *  Such a model can be defined by several strings, one string per character in a seed.
 *
 *  For instance, consider a 4 span subseed model (so each seed has 4 characters) for the
 *  amino acid alphabet. Now consider the 4 defining strings:
 *   - [0]  "H,FY,W,IV,LM,C,RK,Q,E,N,D,A,S,T,G,P"
 *   - [1]  "HFYWIVLMC,RKQENDASTGP"
 *   - [2]  "H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P"
 *   - [3]  "H,FY,W,IV,LM,C,R,K,Q,E,N,D,A,S,T,G,P"
 *
 *  Here, the string [0] defines some subsets of equivalent letters for the character 0 of
 *  a seed. For instance, letter F and Y belong to the same subset; by convention, we
 *  take the first letter of each subset as the representant of the subset. So, a seed whose
 *  first character is Y will be considered as a seed whose first character is F.
 *  Some subsets have only one representant.
 *
 *  In this sample, the seeds "FRHW", "YKHW" and "YPHW" are equivalent.
 *
 *  It is used by the PLAST algorithm and is one of the major difference between PLAST
 *  and BLAST. It is required by PLAST for easing parallization schemes.
 *
 *  Code sample:
 *  \code
 *  // This sample shows how to read a genomic database and how to iterate
 *  // each seed in each sequence of the database. This simple code gives an
 *  // idea of the skeleton needed for indexing a database.
 *
 *  void sample ()
 *  {
 *      // we create a subseed model.
 *      ISeedModel* model = new SubSeedModel (3,
            "H,FY,W,IV,LM,C,RK,Q,E,N,D,A,S,T,G,P",
            "HFYWIVLMC,RKQENDASTGP",
            "H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P"
        );
 *
 *      // we create a memory cached database from a FASTA file
 *      ISequenceDatabase* database = new BufferedSequenceDatabase (
 *          new FastaSequenceIterator ("tursiops.fa", 100),
 *          false
 *      );
 *
 *      // we create a sequence iterator from the database
 *      ISequenceIterator* itDb = database->createSequenceIterator();
 *
 *      // we iterate the database
 *      for (itDb->first(); !itDb->isDone(); itDb->next())
 *      {
 *          // we retrieve the current sequence.
 *          ISequence* sequence = itDb->currentItem();
 *
 *          // we create a seed iterator from the sequence data
 *          ISeedIterator* itSeed = model->createSeedsIterator (sequence->data);
 *
 *          // we loop each seed contained in the sequence.
 *          for (itSeed->first(); !itSeed->isDone(); itSeed->next())
 *          {
 *              // we retrieve the current seed
 *              const ISeed* seed = itSeed->currentItem();
 *          }
 *      }
 *  }
 *  \endcode
 */
class SubSeedModel : public AbstractSeedModel
{
public:

    /** Constructor. */
    SubSeedModel ();

    /** Constructor. Read the subseeds set definition from a file.
     * \param[in] filename : path of the file to be read.
     */
    SubSeedModel (const char* filename);

    /** Constructor that takes defining strings as input.
     * \param[in] nbEntries : number of defining strings
     * \param[in] ... : varargs holding the defining strings.
     */
    SubSeedModel (size_t nbEntries, ...);

    /** Constructor that takes defining strings as input.
     * \param[in] subseedStrings : defining strings.
     */
    SubSeedModel (const std::vector<std::string>& subseedStrings);

    /** Destructor. */
    virtual ~SubSeedModel ();

    /** \copydoc AbstractSeedModel::getSeedsMaxNumber
     * Note that for a subseed model, we should have
     * a reasonable number of possible seeds due to the equivalence subsets
     *  (which is not the case for a basic seed model).
     */
    size_t getSeedsMaxNumber ()  { return _seedsMaxNumber; }

    /** \copydoc AbstractSeedModel::createSeedsIterator */
    ISeedIterator* createSeedsIterator (const database::IWord& data);

    /** \copydoc AbstractSeedModel::createAllSeedsIterator */
    ISeedIterator* createAllSeedsIterator ();

    /** \copydoc AbstractSeedModel::getAllSeedsTable */
    const database::LETTER* getAllSeedsTable();

    /** Debug purpose. */
    void dump (void);

protected:

    /** Initialization.
     * \param[in] span: span of the seeds.
     */
    void initialize (size_t span);

    /** Table holding the concatenation of all possible seeds. */
    database::LETTER* _allSeedsTable;

    /** Number of possible seeds for the model. */
    size_t  _seedsMaxNumber;

    /** Conversion table. */
    const database::LETTER* _convertTable;

    /** Equivalence table. */
    char**  _equivalenceTable;

    /** Equivalents vector. */
    std::vector< std::vector<database::LETTER> > _equivalentsTable;

    size_t _currentNumber;

    /** */
    void addEntry (const char* sequence);

    /************************************************************/
    /** \brief Data seed iteration for the sub seed model.
     */
    class DataSeedIterator  : public AbstractSeedIterator
    {
    public:
        DataSeedIterator (SubSeedModel* model, const database::IWord& data);

    protected:
        SubSeedModel* _specificModel;
        char** _equivalenceTable;

        /** Find the next valid item. May add some increment to the current position. */
        bool findNextValidItem (void);

        /** Update (if needed) of the current item object. */
        void updateItem (void) { /* done in findNextValidItem */ }
    };

    /************************************************************/
    /** \brief Data seed iteration for the sub seed model.
     */
    class DataSeedIteratorWithTokenizer  : public DataSeedIterator
    {
    public:
        DataSeedIteratorWithTokenizer (SubSeedModel* model, const database::IWord& data);

        /** \copydoc DataSeedIterator::first */
        void first()
        {
            _delta = 20;
            _currentToken = 0;

            updateBound ();
            DataSeedIterator::first();
        }

    protected:

        /** Find the next valid item. May add some increment to the current position. */
        virtual bool findNextValidItem (void);

        /** */
        bool updateBound ()
        {
            bool result = _currentToken < _tokenizer.getItems().size();

            if (result)
            {
                _begin = _tokenizer.getItems()[_currentToken].first  + 0*_delta;
                _end   = _tokenizer.getItems()[_currentToken].second;

                if (_end >= _delta)  { _end -= _delta; }

                if (_end >= _begin + _span - 1)  {  _end -= _span-1;      }
                else                             {  _end  = 0;  _begin=1; }
            }
            return result;
        }

        size_t _delta;

        database::impl::SequenceTokenizer _tokenizer;
        size_t _currentToken;
        size_t _begin;
        size_t _end;
    };

    /************************************************************/
    /** \brief All seeds iteration for the sub seed model.
     */
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
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _SUB_SEED_MODEL_HPP_  */
