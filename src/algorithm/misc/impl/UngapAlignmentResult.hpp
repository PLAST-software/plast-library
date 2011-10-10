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

#include "SmartPointer.hpp"

#include "AbstractAlignmentResult.hpp"
#include "IScoreMatrix.hpp"
#include "IAlgoParameters.hpp"
#include "IThread.hpp"

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

    os::ISynchronizer* _synchro;

    u_int32_t _nbAlignments;

    int _DIVDIAG;

    int  _diagGlobal;

    // information about ungapped alignment list
    struct LISTGAP {  // *** ungapped alignments structure list
      int diag;               // no de diag max ou est situ�  l'alignement
      int start;              // start position of alignment in the bank
      int stop;               // stop position of alignment in the bank
      struct LISTGAP *next;   // pointer to a structure identical (to manage the list)
    };

    LISTGAP** _listGaplessAlign;
    size_t    _listGaplessAlignSize;

    bool addDiag (int q_start, int q_stop, int s_start, int s_stop, int seqIdx);

    friend class BasicAlignmentIterator;
};

/********************************************************************************/

class AlignmentSplitter : public dp::SmartPointer
{
public:
    AlignmentSplitter (IScoreMatrix* scoreMatrix, int openGapCost, int extendGapCost);
    virtual ~AlignmentSplitter ();

    size_t splitAlign (
        const database::LETTER* subjectSeq,
        const database::LETTER* querySeq,
        u_int32_t subjectStartInSeq,
        u_int32_t subjectEndInSeq,
        u_int32_t queryStartInSeq,
        u_int32_t queryEndInSeq,
        int* splittab,
        size_t& identity,
        size_t& nbGap,
        size_t& nbMis,
        size_t& alignSize,
        database::LETTER* subjectAlign = 0,
        database::LETTER* queryAlign   = 0
    );

    size_t splitAlign (Alignment& align,  int* splittab);

    void computeInfo (Alignment& align);

private:

    IScoreMatrix* _scoreMatrix;
    void setScoreMatrix (IScoreMatrix* scoreMatrix)  { SP_SETATTR (scoreMatrix); }

    int _openGapCost;
    int _extendGapCost;

    short** _matrix_H;
    short** _matrix_E;
    short** _matrix_F;

    int _DefaultAlignSize;
    int _MaxAlignSize;

    short** newMatrix  (int nrows, int ncols);
    void    freeMatrix (short*** mat);
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _UNGAP_ALIGNMENT_RESULT_HPP_ */
