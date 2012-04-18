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

/** \file CompareContainerVisitor.hpp
 *  \brief A few implementations of IAlignmentResultVisitor interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _COMPARE_ALIGNMENT_CONTAINER_VISITOR_HPP_
#define _COMPARE_ALIGNMENT_CONTAINER_VISITOR_HPP_

/********************************************************************************/

#include <alignment/core/api/IAlignmentContainer.hpp>
#include <alignment/visitors/impl/AdapterAlignmentVisitor.hpp>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/** \brief Visitor that fills a list holding all Alignment instances
 *
 * This visitor dumps alignments into a list
 */
class CompareContainerVisitor : public core::IAlignmentContainerVisitor
{
public:

    CompareContainerVisitor (
        core::IAlignmentContainer* dbComp,
        misc::Range<double> overlapRange,
        core::IAlignmentContainerVisitor* visitorCommon   = 0,
        core::IAlignmentContainerVisitor* visitorDistinct = 0
    )
        : _dbComp(dbComp), _overlapRange(overlapRange), _commonSize(0), _specificSize(0),
          _visitorCommon(0), _visitorDistinct(0)
    {
        setVisitorCommon   (visitorCommon);
        setVisitorDistinct (visitorDistinct);
    }

    /** */
    virtual ~CompareContainerVisitor ()
    {
        setVisitorCommon   (0);
        setVisitorDistinct (0);
    }

    /** */
    void visitQuerySequence   (const database::ISequence* seq,   const misc::ProgressInfo& progress)
    {
        if (_visitorCommon)     {  _visitorCommon->visitQuerySequence   (seq, progress);    }
        if (_visitorDistinct)   {  _visitorDistinct->visitQuerySequence (seq, progress);    }
    }

    /** */
    void visitSubjectSequence (const database::ISequence* seq,   const misc::ProgressInfo& progress)
    {
        if (_visitorCommon)     {  _visitorCommon->visitSubjectSequence   (seq, progress);    }
        if (_visitorDistinct)   {  _visitorDistinct->visitSubjectSequence (seq, progress);    }
    }

    /** */
    void visitAlignment (core::Alignment* align, const misc::ProgressInfo& progress)  {}

    /** */
    void postVisit  (core::IAlignmentContainer* result)
    {
        if (_visitorCommon)     {  _visitorCommon->postVisit   (result);    }
        if (_visitorDistinct)   {  _visitorDistinct->postVisit (result);    }
    }

    /** \copydoc IAlignmentResultVisitor::visitAlignmentsList */
    void visitAlignmentsList (
        const database::ISequence* qry,
        const database::ISequence* sbj,
        std::list<core::Alignment>& alignments
    );

    /** \copydoc IAlignmentResultVisitor::finalize */
    void finalize (void)  { }

    size_t getTotalSize    ()  { return _commonSize + _specificSize; }
    size_t getCommonSize   ()  { return _commonSize;   }
    size_t getSpecificSize ()  { return _specificSize; }

    double getCommonPercentage ()  {  return (double)_commonSize  /  (double) (_commonSize + _specificSize);  }

private:

    core::IAlignmentContainer* _dbComp;
    misc::Range<double> _overlapRange;
    size_t _commonSize;
    size_t _specificSize;

    core::IAlignmentContainerVisitor* _visitorCommon;
    void setVisitorCommon (core::IAlignmentContainerVisitor* visitorCommon)  { SP_SETATTR(visitorCommon); }

    core::IAlignmentContainerVisitor* _visitorDistinct;
    void setVisitorDistinct (core::IAlignmentContainerVisitor* visitorDistinct)  { SP_SETATTR(visitorDistinct); }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _COMPARE_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
