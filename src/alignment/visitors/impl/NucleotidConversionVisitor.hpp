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

/** \file NucleotidConversionVisitor.hpp
 *  \brief A few implementations of IAlignmentResultVisitor interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _NUCLEOTID_CONVERSION_ALIGNMENT_CONTAINER_VISITOR_HPP_
#define _NUCLEOTID_CONVERSION_ALIGNMENT_CONTAINER_VISITOR_HPP_

/********************************************************************************/

#include <alignment/visitors/impl/ProxyVisitor.hpp>

#include <map>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/** \brief Visitor that fills a list holding all Alignment instances
 *
 * This visitor dumps alignments into a list
 */
class NucleotidConversionVisitor : public AlignmentsProxyVisitor
{
public:

    /** */
    NucleotidConversionVisitor (core::IAlignmentContainerVisitor* ref, core::Alignment::DbKind kind);

    /** \copydoc IAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence (const database::ISequence* seq, const misc::ProgressInfo& progress)
    {
        const database::ISequence* actualSeq =  (_kind == core::Alignment::QUERY ?
            retrieveNucleotidSequence (seq) :  seq);

        _ref->visitQuerySequence (actualSeq, progress);
    }

    /** \copydoc IAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq, const misc::ProgressInfo& progress)
    {
        const database::ISequence* actualSeq =  (_kind == core::Alignment::SUBJECT ?
            retrieveNucleotidSequence (seq) :  seq);

        _ref->visitSubjectSequence (actualSeq, progress);
    }

    /** \copydoc IAlignmentResultVisitor::visitAlignment */
    void visitAlignment (core::Alignment* align, const misc::ProgressInfo& progress);

protected:

    core::Alignment::DbKind _kind;

    database::ISequenceDatabase* _nucleotidDb;
    database::ISequence          _nucleotidSequence;
    int8_t                       _frameShift;
    bool                         _isTopFrame;

    /** Shortcut. */
    typedef std::pair <database::ISequenceDatabase*, u_int32_t> Key;

    std::map <Key,database::ISequence> _nucleotidSequences;

    /** */
    virtual  const database::ISequence* retrieveNucleotidSequence (const database::ISequence*  proteinSequence);
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _NUCLEOTID_CONVERSION_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
