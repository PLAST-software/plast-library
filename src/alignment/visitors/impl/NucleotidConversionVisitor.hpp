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

    /** \copydoc IAlignmentResultVisitor::visitAlignment */
    void visitAlignment (core::Alignment* align);

protected:

    core::Alignment::DbKind _kind;

    /** Shortcut. */
    typedef std::pair <database::ISequenceDatabase*, u_int32_t> Key;

    std::map <Key,database::ISequence> _nucleotidSequences;

    /** */
    virtual  bool getNucleotidSequence (
        database::ISequence& sequence,
        core::Alignment*     align,
        size_t&              frameShift,
        bool&                isTopFrame
    );
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _NUCLEOTID_CONVERSION_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
