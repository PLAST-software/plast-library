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

/** \file QueryReorderVisitor.hpp
 *  \brief Visitor that reorders alignments by queries identifiers
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _QUERY_REORDER_VISITOR_HPP_
#define _QUERY_REORDER_VISITOR_HPP_

/********************************************************************************/

#include <algo/core/api/IAlgoEnvironment.hpp>
#include <alignment/visitors/impl/ProxyVisitor.hpp>

#include <iostream>
#include <fstream>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/********************************************************************************/
/** \brief
 *
 */
class QueryReorderVisitor : public AlignmentsProxyVisitor
{
public:

    /** Constructor. */
    QueryReorderVisitor  (
        algo::core::IConfiguration*         config,
        const std::string&                  uri,
        core::IAlignmentContainerVisitor*   realVisitor,
        core::IAlignmentContainerVisitor*   finalVisitor,
        database::IDatabaseQuickReader*     qryReader,
        u_int32_t                           nbAlignmentsThreshold
    );

    /** Destructor. */
    virtual ~QueryReorderVisitor();

    /** \copydoc AbstractAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence (const database::ISequence* seq, const misc::ProgressInfo& progress);

    /** \copydoc IAlignmentResultVisitor::visitAlignment */
    void visitAlignment (core::Alignment* align, const misc::ProgressInfo& progress)
    {
        // nothing to do here: our delegate '_realVisitor' will be called by parent class AlignmentsProxyVisitor.
    }

    /** \copydoc AbstractAlignmentResultVisitor::finalize */
    void finalize (void);

    /** */
    core::IAlignmentContainerVisitor* getFinalVisitor ()  { return _finalVisitor; }

protected:

    std::string getOutputFileUri  ()  { return _outputUri;                       }
    std::string getTmpFileUri     ()  { return _outputUri + std::string(".tmp"); }
    std::string getIndexesFileUri ()  { return _outputUri + std::string(".idx"); }

private:

    /** */
    algo::core::IConfiguration* _config;

    /** */
    std::string _outputUri;

    /** */
    core::IAlignmentContainerVisitor* _finalVisitor;
    void setFinalVisitor (core::IAlignmentContainerVisitor* finalVisitor) { SP_SETATTR(finalVisitor); }

    /** */
    database::IDatabaseQuickReader* _qryReader;
    void setQryReader (database::IDatabaseQuickReader* qryReader)  { SP_SETATTR(qryReader); }

    /** We need a file for storing queries indexes. */
    std::fstream _indexesFile;

    /** */
    char _queryId[1024];

    /** */
    int64_t _prevPos;
    int64_t _newPos;

    /** */
    u_int32_t _nbAlignmentsThreshold;

    /** */
    void dumpIndex (void);

    friend class AlignmentContainerBuilderStream;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _QUERY_REORDER_VISITOR_HPP_ */
