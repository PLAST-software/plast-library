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

/** \file BasicAlignmentResult3.hpp
 *  \brief Implementation of IAlignmentResult interface for gap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _BASIC_ALIGNMENT_RESULT3_HPP_
#define _BASIC_ALIGNMENT_RESULT3_HPP_

/********************************************************************************/

#include <algo/align/impl/BasicAlignmentResult.hpp>

#include <set>
#include <list>
#include <vector>
#include <map>

/********************************************************************************/
namespace algo  {
namespace align {
/** \brief Implementation of concepts about alignments with gaps. */
namespace impl  {
/********************************************************************************/

/** \brief Default implementation of IAlignmentResult interface for gap alignments
 */
class BasicAlignmentResult3 : public BasicAlignmentResult
{
public:

    /** Constructor.
     * \param[in] subjectDb : the subject database.
     * \param[in] queryDb   : the query database.
     */
    BasicAlignmentResult3 (database::ISequenceDatabase* subjectDb, database::ISequenceDatabase* queryDb)
        : BasicAlignmentResult (subjectDb, queryDb) {}

    /** */
    bool doesExist (const Alignment& align);

    /** */
    bool insertInContainer (Alignment& a1, void* context, AlignmentsContainer& container);

    /** \copydoc BasicAlignmentResult::shrink */
    void shrink (void);
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_ALIGNMENT_RESULT_HPP_ */
