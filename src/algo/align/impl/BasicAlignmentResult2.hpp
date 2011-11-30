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

/** \file BasicAlignmentResult2.hpp
 *  \brief Implementation of IAlignmentResult interface for gap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _BASIC_ALIGNMENT_RESULT2_HPP_
#define _BASIC_ALIGNMENT_RESULT2_HPP_

/********************************************************************************/

#include <algo/align/impl/AbstractAlignmentResult.hpp>

#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>

/********************************************************************************/
namespace algo   {
namespace align {
/** \brief Implementation of concepts about alignments with gaps. */
namespace impl   {
/********************************************************************************/

/** \brief Alternative implementation of IAlignmentResult interface for gap alignments
 */
class BasicAlignmentResult2 : public AbstractAlignmentResult
{
public:

    /** Constructor.
     * \param[in] nbQuerySequences : number of query sequences.
     */
    BasicAlignmentResult2 (size_t nbQuerySequences=0);

    /** Constructor.
     * \param[out] subjectComments
     * \param[out] queryComments
     */
    BasicAlignmentResult2 (std::vector<std::string>& subjectComments,  std::vector<std::string>& queryComments);

    /** Destructor. */
    virtual ~BasicAlignmentResult2 ();

    /** \copydoc IAlignmentResult::doesExist */
    bool doesExist (
        const indexation::ISeedOccurrence* subjectOccur,
        const indexation::ISeedOccurrence* queryOccur);

    /** \copydoc IAlignmentResult::insert */
    bool insert (Alignment& align, void* context);

    /** \copydoc IAlignmentResult::getSize */
    u_int32_t getSize ()  {  return _nbAlignments; }

    /** \copydoc IAlignmentResult::shrink */
    void shrink (void);

    /** \copydoc IAlignmentResult::accept */
    void accept (IAlignmentResultVisitor* visitor);

    /** \copydoc IAlignmentResult::getAlignments */
    std::list<Alignment> getAlignments (const database::ISequence* querySeq, const database::ISequence* subjectSeq);

    typedef std::pair <u_int32_t,u_int32_t> QuerySubjectIndexes;
    typedef std::vector<Alignment> AlignmentsContainer;
    typedef std::map <QuerySubjectIndexes, AlignmentsContainer> Entries;

    Entries& getEntries () { return  _entries;  }

private:

    u_int32_t _nbAlignments;

    Entries _entries;

    std::vector<std::string>  _subjectComments;
    std::vector<std::string>  _queryComments;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_ALIGNMENT_RESULT2_HPP_ */
