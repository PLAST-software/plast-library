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
class RawOutputVisitor : public FileVisitor
{
public:

    /** */
    RawOutputVisitor (const std::string& uri) : FileVisitor(uri) {}

    /** \copydoc AbstractAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence   (const database::ISequence* seq, const misc::ProgressInfo& progress)
    {
        getFile()->print ("Q %d %s\n",  seq->getLength(), seq->comment);
    }

    /** \copydoc AbstractAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq, const misc::ProgressInfo& progress)
    {
    	getFile()->print ("S %d %s\n",  seq->getLength(), seq->comment);
    }

    /** \copydoc AbstractAlignmentResultVisitor::visitAlignment */
    void visitAlignment  (core::Alignment* a, const misc::ProgressInfo& progress)
    {
    	getFile()->print (
			"%c %d %d %d %d %lg %d %d %d %d %lg %lg %lg %d %d %d %d %d\n",
			'H',
            (int)a->getRange(alignment::core::Alignment::QUERY).begin + 1,
            (int)a->getRange(alignment::core::Alignment::QUERY).end   + 1,
            (int)a->getNbGaps(alignment::core::Alignment::QUERY),
            (int)a->getFrame(alignment::core::Alignment::QUERY),
            a->getCoverage(alignment::core::Alignment::QUERY),

            (int)a->getRange(alignment::core::Alignment::SUBJECT).begin  + 1,
            (int)a->getRange(alignment::core::Alignment::SUBJECT).end    + 1,
            (int)a->getNbGaps(alignment::core::Alignment::SUBJECT),
            (int)a->getFrame(alignment::core::Alignment::SUBJECT),
            a->getCoverage(alignment::core::Alignment::SUBJECT),

            a->getEvalue(),
            a->getBitScore(),
            a->getScore(),

            (int)a->getLength(),

            (int)a->getNbIdentities(),
            (int)a->getNbPositives(),
            (int)a->getNbMisses()
		);
    }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _RAW_OUTPUT_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
