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

/** \brief Default implementation of IAlignmentResult interface for gap alignments
 */
class BasicAlignmentContainer : public AbstractAlignmentContainer
{
public:

    /** Constructor. */
    BasicAlignmentContainer ();

    /** Desctructor. */
    ~BasicAlignmentContainer ();

    /** \copydoc AbstractAlignmentResult::doesExist */
    bool doesExist (const Alignment& align);

    /** \copydoc AbstractAlignmentResult::doesExist */
    bool doesExist (
        const indexation::ISeedOccurrence* subjectOccur,
        const indexation::ISeedOccurrence* queryOccur,
        u_int32_t bandSize
    );

    /** \copydoc AbstractAlignmentResult::insert */
    bool insert (Alignment& align, void* context);

    /** \copydoc AbstractAlignmentResult::accept */
    void accept (IAlignmentContainerVisitor* visitor);

    /** \copydoc AbstractAlignmentResult::getContainer */
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

    typedef std::map <Key, std::pair<database::ISequence*,ContainerLevel3> >  ContainerLevel2;
    typedef std::map <Key, std::pair<database::ISequence*,ContainerLevel2> >  ContainerLevel1;

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
                found = (*it).getSbjRange().includes (sbjRange)  &&  (*it).getQryRange().includes (qryRange);
            }
        }
        return found;
    }

    u_int32_t _nbSeqLevel1;
    u_int32_t _nbSeqLevel2;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_ALIGNMENT_CONTAINER_HPP_ */
