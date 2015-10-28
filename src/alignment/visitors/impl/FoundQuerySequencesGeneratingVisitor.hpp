#ifndef _FOUND_QUERY_SEQUENCES_GENERATING_VISITOR_HPP_
#define _FOUND_QUERY_SEQUENCES_GENERATING_VISITOR_HPP_

/********************************************************************************/

#include <alignment/visitors/impl/HierarchyAlignmentVisitor.hpp>
#include <alignment/core/api/Alignment.hpp>

#include <set>
#include <iostream>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/********************************************************************************/
/** \brief Extract the indexes of the queries, for which alignments are found
 *
 * Except the extraction, this visitor wraps another visitor that normally
 * produces the output.
 */
class FoundQuerySequencesGeneratingVisitor : public HierarchyAlignmentResultVisitor
{
public:

    /** */
    FoundQuerySequencesGeneratingVisitor (std::set<u_int64_t>* founQueryIds, IAlignmentContainerVisitor* resultVisitor)
        : _foundQueryIds(founQueryIds), _resultVisitor(resultVisitor)
    {
        resultVisitor->use();
    }

    /** Desctructor. */
    virtual ~FoundQuerySequencesGeneratingVisitor ()
    {
        _resultVisitor->forget();
    }

    /** \copydoc AbstractAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence   (const database::ISequence* seq, const misc::ProgressInfo& progress)
    {
        processQueryId(seq->offsetInDb);

        _resultVisitor->visitQuerySequence(seq, progress);
    }

    /** \copydoc AbstractAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq, const misc::ProgressInfo& progress)
    {
        _resultVisitor->visitSubjectSequence(seq, progress);
    }

    /** \copydoc AbstractAlignmentResultVisitor::visitAlignment */
    void visitAlignment (core::Alignment* align, const misc::ProgressInfo& progress)
    {
        processQueryId(align->getSequence(core::Alignment::QUERY)->index);

        _resultVisitor->visitAlignment(align, progress);
    }

    /** \copydoc IAlignmentResultVisitor::finish */
    void postVisit (core::IAlignmentContainer* result)
    {
        _resultVisitor->postVisit(result);
    }

    /** \copydoc IAlignmentResultVisitor::getPosition */
    u_int64_t getPosition ()
    {
        return _resultVisitor->getPosition();
    }

private:

    std::set<u_int64_t>* _foundQueryIds;

    IAlignmentContainerVisitor* _resultVisitor;

    void processQueryId(u_int64_t queryId)
    {
        _foundQueryIds->insert(queryId);
    }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _FOUND_QUERY_SEQUENCES_GENERATING_VISITOR_HPP_ */
