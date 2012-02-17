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

/** \file RawOutputVisitor.hpp
 *  \brief A few implementations of IAlignmentResultVisitor interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _RAW_OUTPUT_ALIGNMENT_CONTAINER_VISITOR_HPP_
#define _RAW_OUTPUT_ALIGNMENT_CONTAINER_VISITOR_HPP_

/********************************************************************************/

#include <alignment/visitors/impl/OstreamVisitor.hpp>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/** \brief Alignments file dump in raw format
 *
 * This visitor dumps alignments into a file in a raw format.
 *
 * Example of raw output:
 * \code
 * QUERY SEQUENCE ENSTTRP00000007202 pep:novel scaffold:turTru1:scaffold_113855:32105:37173:1 gene:ENSTTRG00000007615
 *    SUBJECT SEQUENCE ENSTTRP00000007202 pep:novel scaffold:turTru1:scaffold_113855:32105:37173:1 gene:ENSTTRG00000007615
 *          ALIGNMENT [ALIGNMENT Q(0) 1 246   S(0) 1 246  len=246 nbgap=0 identity=246 nbMiss=0]
 *    SUBJECT SEQUENCE ENSTTRP00000003468 pep:novel genescaffold:turTru1:GeneScaffold_1110:25324:562263:-1 gene:ENSTTRG0000
 *          ALIGNMENT [ALIGNMENT Q(0) 27 81   S(982) 2627 2681  len=55 nbgap=0 identity=18 nbMiss=37]
 * \endcode
*/
class RawOutputVisitor : public OstreamVisitor
{
public:

    /** \copydoc AbstractAlignmentResultVisitor::AbstractAlignmentResultVisitor */
    RawOutputVisitor (std::ostream* ostream)  : OstreamVisitor(ostream) {}

    /** */
    RawOutputVisitor (const std::string& uri) : OstreamVisitor(uri) {}

    /** \copydoc AbstractAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence   (const database::ISequence* seq)
    {
        getStream() << std::endl << "QUERY SEQUENCE " << seq->comment << std::endl;
    }

    /** \copydoc AbstractAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq)
    {
        getStream() << "   SUBJECT SEQUENCE " << seq->comment << std::endl;
    }

    /** \copydoc AbstractAlignmentResultVisitor::visitAlignment */
    void visitAlignment  (core::Alignment* align)
    {
        getStream() << "         ALIGNMENT " << align->toString() << std::endl;
    }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _RAW_OUTPUT_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
