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

/** \file UngapAlignmentResult.hpp
 *  \brief Implementation of IAlignmentResult interface for ungap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _UNGAP_ALIGNMENT_RESULT_HPP_
#define _UNGAP_ALIGNMENT_RESULT_HPP_

/********************************************************************************/

#include <algo/align/impl/AbstractAlignmentResult.hpp>

#include <set>
#include <list>
#include <vector>
#include <map>

/********************************************************************************/
namespace algo   {
namespace align {
/** \brief Implementation of concepts about alignments with gaps. */
namespace impl   {
/********************************************************************************/

/** \brief Implementation of IAlignmentResult for ungap alignments
 *
 * This implementation stores only ungap alignments.
 *
 * It uses a structure that stores the alignments according to their distance to
 * the diagonal (ie. query start offset in database minus subject start offset in database).
 *
 * During its execution, the PLAST algorithm fills a UngapAlignmentResult instance; each time
 * a gap alignment is found by the dynamic programming, it is split into ungap parts and each
 * ungap part is inserted into the UngapAlignmentResult instance of the PLAST algorithm.
 *
 * This UngapAlignmentResult instance may be used for PLAST algorithm optimization.
 * For instance, before processing some hit by a given algorithm piece, one could check that
 * this hit is not contained in an ungap alignment of the UngapAlignmentResult instance.
 * In such a case, further processing for this hit may be unecessary because already known by
 * previous computations.
 */
class UngapAlignmentResult : public AbstractAlignmentResult
{
public:

    /** Constructor.
     * \param[in] nbQuerySequences : of query sequences.
     */
    UngapAlignmentResult (size_t nbQuerySequences);

    /** Destructor. */
    virtual ~UngapAlignmentResult ();

    /** \copydoc IAlignmentResult::doesExist */
    bool doesExist (
        const indexation::ISeedOccurrence* subjectOccur,
        const indexation::ISeedOccurrence* queryOccur
    );

    /** \copydoc AbstractAlignmentResult::insert */
    bool insert (Alignment& align, void* context);

    /** \copydoc AbstractAlignmentResult::getSize */
    u_int32_t getSize ()  {  return _listGaplessAlignSize; }

    /** \copydoc AbstractAlignmentResult::shrink
     * Nothing is done for this implementation.
     */
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
      u_int32_t diag;       // no de diag max ou est situe  l'alignement
      u_int64_t start;      // start position of alignment in the bank
      u_int64_t stop;       // stop position of alignment in the bank
      struct LISTGAP *next; // pointer to a structure identical (to manage the list)
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
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _UNGAP_ALIGNMENT_RESULT_HPP_ */
