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

/** \file ReaderAlignmentContainer.hpp
 *  \brief Implementation of IAlignmentContainer
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _READER_ALIGNMENT_CONTAINER_HPP_
#define _READER_ALIGNMENT_CONTAINER_HPP_

/********************************************************************************/

#include <alignment/core/impl/BasicAlignmentContainer.hpp>

#include <map>

/********************************************************************************/
namespace alignment {
namespace core      {
namespace impl      {
/********************************************************************************/

/** \brief Default implementation of IAlignmentResult interface for gap alignments
 */
class ReaderAlignmentContainer : public BasicAlignmentContainer
{
public:

    /** Constructor. */
    ReaderAlignmentContainer ();

    /** Destructor. */
    ~ReaderAlignmentContainer ();

private:

    bool isInContainer (
        ContainerLevel3* container,
        const misc::Range32& sbjRange,
        const misc::Range32& qryRange,
        char delta = 0
    ) { return false; }

    std::vector<std::string>  _subjectComments;
    std::vector<std::string>  _queryComments;

    std::map<std::string,int> _subjectMapComments;
    std::map<std::string,int> _queryMapComments;

    void setComments ();

    friend class AlignmentContainerFactory;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _READER_ALIGNMENT_CONTAINER_HPP_ */
