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

/** \file ModelFactoryVisitors.hpp
 *  \brief A few implementations of IAlignmentResultVisitor interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _MODEL_BUILDER_VISITOR_HPP_
#define _MODEL_BUILDER_VISITOR_HPP_

/********************************************************************************/

#include <alignment/visitors/impl/HierarchyAlignmentVisitor.hpp>
#include <alignment/core/api/IAlignmentContainerModel.hpp>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/********************************************************************************/
/** \brief
 */
class ModelBuilderVisitor : public HierarchyAlignmentResultVisitor
{
public:

    /** */
    ModelBuilderVisitor ();

    /** */
    ~ModelBuilderVisitor ();

    /** */
    void visitQuerySequence   (
        const database::ISequence*  seq,
        const misc::ProgressInfo&   progress
    );

    /** */
    void visitSubjectSequence (
        const database::ISequence*  seq,
        const misc::ProgressInfo&   progress
    );

    /** */
    void visitAlignment (
        core::Alignment*            align,
        const misc::ProgressInfo&   progress
    );

    /** */
    void postVisit (core::IAlignmentContainer* result)  {}

    /** */
    core::IRootLevel* getModel ()  { return _model; }

private:

    core::IRootLevel* _model;
    void setModel (core::IRootLevel* model)  { SP_SETATTR(model); }

    core::IQueryLevel*   _currentQuery;
    core::ISubjectLevel* _currentSubject;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _MODEL_BUILDER_VISITOR_HPP_ */
