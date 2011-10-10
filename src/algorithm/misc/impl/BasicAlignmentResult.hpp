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

#ifndef _BASIC_ALIGNMENT_RESULT_HPP_
#define _BASIC_ALIGNMENT_RESULT_HPP_

/********************************************************************************/

#include "AbstractAlignmentResult.hpp"
#include "UngapAlignmentResult.hpp"
#include "IThread.hpp"

#include <set>
#include <list>
#include <vector>
#include <map>

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class BasicAlignmentResult : public AbstractAlignmentResult
{
public:

    BasicAlignmentResult (database::ISequenceDatabase* subjectDb, database::ISequenceDatabase* queryDb);
    virtual ~BasicAlignmentResult ();

    bool doesExist (const Alignment& align);

    bool doesExist (
        const indexation::ISeedOccurrence* subjectOccur,
        const indexation::ISeedOccurrence* queryOccur);

    bool insert (Alignment& align, void* context);

    u_int32_t getSize ()  {  return _nbAlignments; }

    void shrink (void);

    void accept (AlignmentResultVisitor* visitor);

private:

    database::ISequenceDatabase* _subjectDb;
    void setSubjectDb (database::ISequenceDatabase* subjectDb)  { SP_SETATTR(subjectDb); }

    database::ISequenceDatabase* _queryDb;
    void setQueryDb (database::ISequenceDatabase* queryDb)  { SP_SETATTR(queryDb); }

    typedef std::vector<Alignment> AlignmentsContainer;

    typedef std::map<u_int32_t,AlignmentsContainer> SubjectEntries;

    std::vector <SubjectEntries>  _queryEntries;

    bool insertInContainer (Alignment& align, void* context, AlignmentsContainer& container);

    os::ISynchronizer* _synchro;

    u_int32_t _nbAlignments;

    size_t _shiftDivisor;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_ALIGNMENT_RESULT_HPP_ */
