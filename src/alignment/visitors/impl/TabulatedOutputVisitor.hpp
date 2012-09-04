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

/** \file AlignmentResultVisitors.hpp
 *  \brief A few implementations of IAlignmentResultVisitor interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _TABULATED_OUTPUT_ALIGNMENT_CONTAINER_VISITOR_HPP_
#define _TABULATED_OUTPUT_ALIGNMENT_CONTAINER_VISITOR_HPP_

/********************************************************************************/

#include <alignment/visitors/impl/HierarchyAlignmentVisitor.hpp>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/********************************************************************************/
/** \brief Alignments file dump in tabulated format
 *
 * This visitor dumps alignments into a file in a tabulated format: each line in the
 * file is an alignment found in the subject and query databases.
 *
 * Each alignment line has the following columns:
 *      - query sequence short comment
 *      - subject sequence short comment
 *      - percentage of identity (nb of identical residues in both database / nb of characters of the alignment)
 *      - length of the alignment (ie. nb of characters)
 *      - number of non identical residues
 *      - number of gaps in the alignment (0 means that this is an ungap alignment)
 *      - start offset in the query sequence
 *      - stop offset in the query sequence
 *      - start offset in the subject sequence
 *      - stop offset in the subject sequence
 *      - expected value
 *      - bit score
 *
 * Example of tabulated output:
 * \code
 * ENSTTRP00000007202  sp|P48347|14310_ARATH   59.04   249  102   3   1     245   1     249   2e-78   292.4
 * ENSTTRP00000007202  sp|P93207|14310_SOLLC   60.08   238  95    4   3     235   9     246   7e-75   280.4
 * ENSTTRP00000001033  sp|P29673|APTE_DROME    28.40   81   58    0   313   393   369   449   0.018   40.8
 * \endcode
 */
class TabulatedOutputVisitor : public HierarchyAlignmentResultVisitor
{
public:

    /** */
    TabulatedOutputVisitor (const std::string& uri);

    /** Desctructor. */
    virtual ~TabulatedOutputVisitor ();

    /** \copydoc AbstractAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence   (const database::ISequence* seq, const misc::ProgressInfo& progress)
    { _currentQuery   = seq;  }

    /** \copydoc AbstractAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq, const misc::ProgressInfo& progress)
    { _currentSubject = seq;  }

    /** \copydoc AbstractAlignmentResultVisitor::visitAlignment */
    void visitAlignment (core::Alignment* align, const misc::ProgressInfo& progress);

    /** \copydoc IAlignmentResultVisitor::finish */
    void postVisit (core::IAlignmentContainer* result)
    {
        /** We should flush the stream. */
        fflush (_file);
    }

    /** \copydoc IAlignmentResultVisitor::getPosition */
    u_int64_t getPosition ()  { return ftell (_file); }

protected:

    FILE* _file;

    const database::ISequence* _currentQuery;
    const database::ISequence* _currentSubject;
    char _sep;

    virtual void dumpLine (core::Alignment* align);
};

/********************************************************************************/
/** \brief Alignments file dump in tabulated format
 *  Extends the AlignmentResultOutputTabulatedVisitor implementation by adding two columns:
 *      - query coverage
 *      - subject coverage
 */
class TabulatedOutputExtendedVisitor : public TabulatedOutputVisitor
{
public:

    /** */
    TabulatedOutputExtendedVisitor (const std::string& uri) : TabulatedOutputVisitor(uri)  {}

protected:

    virtual void dumpLine (core::Alignment* align);
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _TABULATED_OUTPUT_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
