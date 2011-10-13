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

#ifndef _BASIC_ALIGNMENT_RESULT2_HPP_
#define _BASIC_ALIGNMENT_RESULT2_HPP_

/********************************************************************************/

#include "AbstractAlignmentResult.hpp"

#include <set>
#include <list>
#include <vector>
#include <map>

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class BasicAlignmentResult2 : public AbstractAlignmentResult
{
public:

    BasicAlignmentResult2 (size_t nbQuerySequences=0);
    BasicAlignmentResult2 (std::vector<std::string>& subjectComments,  std::vector<std::string>& queryComments);

    virtual ~BasicAlignmentResult2 ();

    bool doesExist (
        const indexation::ISeedOccurrence* subjectOccur,
        const indexation::ISeedOccurrence* queryOccur);

    bool insert (Alignment& align, void* context);

    u_int32_t getSize ()  {  return _nbAlignments; }

    void shrink (void);

    void accept (AlignmentResultVisitor* visitor);

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
} /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_ALIGNMENT_RESULT2_HPP_ */
