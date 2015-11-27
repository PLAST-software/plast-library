/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

/** \file UngapAlignmentResult.hpp
 *  \brief Implementation of IAlignmentResult interface for ungap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _UNGAP_ALIGNMENT_RESULT_HPP_
#define _UNGAP_ALIGNMENT_RESULT_HPP_

/********************************************************************************/

#include <alignment/core/impl/AbstractAlignmentContainer.hpp>

#include <set>
#include <list>
#include <vector>
#include <map>

/********************************************************************************/
namespace alignment {
namespace core      {
namespace impl      {
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
class UngapAlignmentResult : public AbstractAlignmentContainer
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
        const indexation::ISeedOccurrence* queryOccur,
        u_int32_t bandSize
    );

    /** \copydoc IAlignmentResult::doesExist */
    bool doesExist (const Alignment& align) { return false; }

    /** \copydoc AbstractAlignmentResult::insert */
    bool insert (Alignment& align, void* context);

    /** \copydoc AbstractAlignmentResult::insert */
    bool insert (const misc::Range64& qry, const misc::Range64& sbj, u_int32_t qryIndex);

    /** \copydoc AbstractAlignmentResult::getSize */
    u_int32_t getAlignmentsNumber ()  {  return _listGaplessAlignSize; }

    /** \copydoc AbstractAlignmentResult::getSize */
    std::list<Alignment>* getContainer (
        const database::ISequence* seqLevel1,
        const database::ISequence* seqLevel2
    ) { return 0; }

private:

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

    bool addDiag (
        u_int64_t q_start,
        u_int64_t q_stop,
        u_int64_t s_start,
        u_int64_t s_stop,
        u_int32_t seqIdx
    );

    /** */
    LISTGAP* getItem (u_int64_t q_start, u_int64_t s_start, u_int32_t seqIdx, u_int32_t& d)
    {
        if (q_start == s_start)  {  d = ((q_start - s_start) & _diagGlobal)  + (seqIdx % _DIVDIAG);  }
        else                     {  d =  (q_start - s_start) & _diagGlobal;  }

        return _listGaplessAlign [d/_DIVDIAG];
    }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _UNGAP_ALIGNMENT_RESULT_HPP_ */
