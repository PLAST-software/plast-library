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

/** \file BasicAlignmentContainer.hpp
 *  \brief Implementation of IAlignmentContainer
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _BASIC_ALIGNMENT_CONTAINER_HPP_
#define _BASIC_ALIGNMENT_CONTAINER_HPP_

/********************************************************************************/

#include <alignment/core/impl/AbstractAlignmentContainer.hpp>

#include <map>

/********************************************************************************/
namespace alignment {
namespace core      {
namespace impl      {
/********************************************************************************/

/** \brief Default implementation of IAlignmentContainer interface for gap alignments
 *
 * This implementation stores information in three hierarchical levels:
 *  - 1) query level
 *  - 2) subject level
 *  - 3) alignment level
 *
 * The purpose here is to store the information in an efficient way both in terms of
 *    - memory
 *    - speed for inserting/accessing information.
 *
 * The insertion of a new item (ie. an Alignment instance) is done as follow:
 *   - 1) we look for the query node matching the query of the alignment to be inserted.
 *     If no query sequence node is found, a new one is added.
 *   - 2) we look for the subject node matching the subject of the alignment to be inserted.
 *     If no subject sequence node is found, a new one is added.
 *   - 3) having found (or created) a couple [query node, subject node], we check that the
 *     alignment to be inserted doesn't already exist (through isInContainer method).
 *
 *  This scheme means that we have 3 searches before inserting an alignment:
 *   - search at query level (level 1)
 *   - search at subject level (level 2)
 *   - search at alignment level (level 3)
 *
 *  The concern is then to minimize the searching time. We use therefore for levels 1 and 2
 *  std::map structures whose 'find' method has a logarithmic complexity in size.
 *
 *  The level 3 is a simple std::list which is surely not the best choice: indeed, our
 *  'isInContainer' method is the corresponding 'find' method with a specific sort criteria,
 *  so the complexity is roughly linear. It is mainly due to historical design concerns about
 *  the visitor interface of IAlignmentContainer instances had to provide a list of Alignments
 *  (see IAlignmentContainerVisitor::visitAlignment method).
 *
 *  This point can be an issue when many alignments can be found for one sequences couple
 *  [query,subject], which can be the case when using option -max-database-size with a huge
 *  value on huge databases. An improvement would be to modify the visitor API by providing
 *  an iterator instead of the list itself, and so we could also use search-optimized structure.
 *  Note however that it would be tricky because of some visitors implementations that are allowed
 *  to modify the alignments list they receive (like shrinker visitors).
 */
class BasicAlignmentContainer : public AbstractAlignmentContainer
{
public:

    /** Constructor. */
    BasicAlignmentContainer (size_t nbHitPerQuery=0, size_t nbHspPerHit=0);

    /** Desctructor. */
    ~BasicAlignmentContainer ();

    /** \copydoc AbstractAlignmentContainer::doesExist(const Alignment&) */
    bool doesExist (const Alignment& align);

    /** \copydoc AbstractAlignmentContainer::doesExist */
    bool doesExist (
        const indexation::ISeedOccurrence* subjectOccur,
        const indexation::ISeedOccurrence* queryOccur,
        u_int32_t bandSize
    );

    /** \copydoc AbstractAlignmentContainer::insertFirstLevel */
    bool insertFirstLevel (const database::ISequence* sequence);

    /** \copydoc IAlignmentResult::doesFirstLevelExists */
    bool doesFirstLevelExists (const database::ISequence* sequence);

    /** \copydoc AbstractAlignmentContainer::getFirstLevelNumber */
    u_int32_t getFirstLevelNumber ()  { return _containerLevel1.size(); }

    /** \copydoc IAlignmentContainer::getSecondLevelNumber */
    u_int32_t getSecondLevelNumber ();

    /** \copydoc AbstractAlignmentContainer::insert(Alignment&,void*) */
    bool insert (Alignment& align, void* context);

    /** \copydoc AbstractAlignmentContainer::merge */
    void merge (const std::vector<IAlignmentContainer*> containers);

    /** \copydoc AbstractAlignmentContainer::accept */
    void accept (IAlignmentContainerVisitor* visitor);

    /** \copydoc AbstractAlignmentContainer::shrink */
    void shrink ();

    /** \copydoc AbstractAlignmentContainer::getContainer */
    std::list<Alignment>* getContainer (
        const database::ISequence* seqLevel1,
        const database::ISequence* seqLevel2
    );

protected:

    typedef std::list<Alignment>  ContainerLevel3;

    /** Define a key for the maps. Note that a sequence is identified both by its id and its
     *  containing database. It is important for composite databases (like 6 reading frames
     *  databases) where one can have several distinct sequences with the same id but belonging
     *  to distinct databases. */
    typedef std::pair <database::ISequenceDatabase*, u_int32_t> Key;

    typedef std::pair<database::ISequence*,ContainerLevel3*> ValueLevel2;
    typedef std::map <Key, ValueLevel2>  ContainerLevel2;

    typedef std::pair<database::ISequence*,ContainerLevel2*> ValueLevel1;
    typedef std::map <Key, ValueLevel1>  ContainerLevel1;

    ContainerLevel1 _containerLevel1;

    /** */
    virtual bool isInContainer (
        ContainerLevel3* container,
        const misc::Range32& sbjRange,
        const misc::Range32& qryRange,
        char delta = 0
    )
    {
        bool found = false;
        if (container != 0)
        {
            for (ContainerLevel3::iterator it = container->begin(); !found  &&  it != container->end(); it++)
            {
                found =
                    (*it).getRange(Alignment::SUBJECT).includes (sbjRange,delta)  &&
                    (*it).getRange(Alignment::QUERY).includes (qryRange,delta);
            }
        }
        return found;
    }

    u_int32_t _nbSeqLevel1;
    u_int32_t _nbSeqLevel2;

    /** */
    size_t _nbHitPerQuery;
    size_t _nbHspPerHit;

    /** */
    friend struct SortHitsFunctor;
};

/********************************************************************************/

/** \brief Default implementation of IAlignmentContainer interface for gap alignments
 *
 * It inherits from BasicAlignmentContainer.
 *
 * There are two differences between these classes:
 *    - we don't need any synchronization in this class (we do it by deleting the synchronizer in the constructor)
 *    - the isInContainer always returns false here (ie. we accept all alignments).
 *
 * About the second point, this should be improved by removing redundant alignments having same anchoring bounds
 * (see what blast does in this case).
 */
class BasicAlignmentContainerBis : public BasicAlignmentContainer
{
public:

    /** Constructor. */
	BasicAlignmentContainerBis (size_t nbHitPerQuery, size_t nbHspPerHit)
        : BasicAlignmentContainer (nbHitPerQuery, nbHspPerHit)
            {  if (_synchro != 0)  { delete _synchro;  _synchro=0;}  }

protected:

    /** */
    virtual bool isInContainer (
        ContainerLevel3* container,
        const misc::Range32& sbjRange,
        const misc::Range32& qryRange,
        char delta = 0
    )
    {
        return false;
    }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_ALIGNMENT_CONTAINER_HPP_ */
