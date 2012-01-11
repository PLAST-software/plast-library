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

/** \file BasicAlignmentResult.hpp
 *  \brief Implementation of IAlignmentResult interface for gap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _BASIC_ALIGNMENT_RESULT_HPP_
#define _BASIC_ALIGNMENT_RESULT_HPP_

/********************************************************************************/

#include <algo/align/impl/AbstractAlignmentResult.hpp>

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
class BasicAlignmentResult : public AbstractAlignmentResult
{
public:

    /** Constructor.
     * \param[in] subjectDb : the subject database.
     * \param[in] queryDb   : the query database.
     */
    BasicAlignmentResult (database::ISequenceDatabase* subjectDb, database::ISequenceDatabase* queryDb);

    /** Destructor. */
    virtual ~BasicAlignmentResult ();

    /** \copydoc AbstractAlignmentResult::doesExist(const Alignment&) */
    bool doesExist (const Alignment& align);

    /** \copydoc IAlignmentResult::doesExist */
    bool doesExist (
        const indexation::ISeedOccurrence* subjectOccur,
        const indexation::ISeedOccurrence* queryOccur,
        size_t bandLength
    );

    /** \copydoc AbstractAlignmentResult::insert */
    bool insert (Alignment& align, void* context);

    /** \copydoc AbstractAlignmentResult::getSize */
    u_int32_t getSize ()  {  return _nbAlignments; }

    /** \copydoc AbstractAlignmentResult::shrink */
    void shrink (void);

    /** \copydoc AbstractAlignmentResult::accept */
    void accept (IAlignmentResultVisitor* visitor);

protected:

    database::ISequenceDatabase* _subjectDb;
    void setSubjectDb (database::ISequenceDatabase* subjectDb)  { SP_SETATTR(subjectDb); }

    database::ISequenceDatabase* _queryDb;
    void setQueryDb (database::ISequenceDatabase* queryDb)  { SP_SETATTR(queryDb); }

    typedef std::vector<Alignment> AlignmentsContainer;

    typedef std::map<u_int32_t,AlignmentsContainer> SubjectEntries;

    std::vector <SubjectEntries>  _queryEntries;

    virtual bool insertInContainer (Alignment& align, void* context, AlignmentsContainer& container);

    static bool mysortfunction (const Alignment& i, const Alignment& j);

    u_int32_t _nbAlignments;

    size_t _shiftDivisor;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_ALIGNMENT_RESULT_HPP_ */
