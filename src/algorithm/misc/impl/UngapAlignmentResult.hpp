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

#ifndef _UNGAP_ALIGNMENT_RESULT_HPP_
#define _UNGAP_ALIGNMENT_RESULT_HPP_

/********************************************************************************/

#include "AbstractAlignmentResult.hpp"

#include <set>
#include <list>
#include <vector>
#include <map>

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class UngapAlignmentResult : public AbstractAlignmentResult
{
public:

    UngapAlignmentResult (size_t nbQuerySequences);

    virtual ~UngapAlignmentResult ();

    bool doesExist (
        const indexation::ISeedOccurrence* subjectOccur,
        const indexation::ISeedOccurrence* queryOccur
    );

    bool insert (Alignment& align, void* context);

    u_int32_t getSize ()  {  return _listGaplessAlignSize; }

    void shrink (void) {}

private:

    typedef std::list<Alignment> AlignmentsList;

    typedef std::map<u_int32_t,AlignmentsList> SubjectEntries;

    std::vector <SubjectEntries>  _queryEntries;

    u_int32_t _nbAlignments;

    int _DIVDIAG;

    int  _diagGlobal;

    // information about ungapped alignment list
    struct LISTGAP {  // *** ungapped alignments structure list
      u_int32_t diag;               // no de diag max ou est situ�  l'alignement
      u_int64_t start;              // start position of alignment in the bank
      u_int64_t stop;               // stop position of alignment in the bank
      struct LISTGAP *next;   // pointer to a structure identical (to manage the list)
    };

    LISTGAP** _listGaplessAlign;
    size_t    _listGaplessAlignSize;

    bool addDiag (int q_start, int q_stop, int s_start, int s_stop, int seqIdx);

    /** */
    LISTGAP* getItem (int q_start, int s_start, int seqIdx, u_int32_t& d)
    {
        if (q_start == s_start)  {  d = ((q_start - s_start) & _diagGlobal)  + (seqIdx % _DIVDIAG);  }
        else                     {  d = (q_start - s_start) & _diagGlobal;  }

        return _listGaplessAlign [d/_DIVDIAG];
    }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _UNGAP_ALIGNMENT_RESULT_HPP_ */
