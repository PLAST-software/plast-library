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

#ifndef _BASIC_ALIGNMENT_RESULT3_HPP_
#define _BASIC_ALIGNMENT_RESULT3_HPP_

/********************************************************************************/

#include "AbstractAlignmentResult.hpp"

#include <set>
#include <list>
#include <vector>
#include <map>

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class BasicAlignmentResult3 : public AbstractAlignmentResult
{
public:

    BasicAlignmentResult3 (size_t nbQuerySequences);
    virtual ~BasicAlignmentResult3 ();

    bool doesExist (
        const indexation::ISeedOccurrence* subjectOccur,
        const indexation::ISeedOccurrence* queryOccur);

    bool insert (Alignment& align, void* context);

    u_int32_t getSize ()  {  return _nbAlignments; }

    void shrink (void);

    void accept (AlignmentResultVisitor* visitor);

private:

    u_int32_t _nbAlignments;

    typedef std::pair <u_int32_t,u_int32_t> QuerySubjectIndexes;

    typedef std::multimap<float,Alignment> AlignmentsContainer;

    typedef std::map <QuerySubjectIndexes, AlignmentsContainer> Entries;

    Entries _entries;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_ALIGNMENT_RESULT2_HPP_ */
