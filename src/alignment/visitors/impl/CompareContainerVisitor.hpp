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
class CompareContainerVisitor : public AdapterAlignmentResultVisitor
{
public:

    CompareContainerVisitor (core::IAlignmentContainer* dbComp, double dmax)
        : _dbComp(dbComp), _dmax(dmax), _commonSize(0), _specificSize(0)
    {
    }

    virtual ~CompareContainerVisitor () {}

    /** \copydoc IAlignmentResultVisitor::visitAlignmentsList */
    void visitAlignmentsList (
        const database::ISequence* qry,
        const database::ISequence* sbj,
        std::list<core::Alignment>& alignments
    );

    double getTotalSize    ()  { return _commonSize + _specificSize; }
    double getCommonSize   ()  { return _commonSize;   }
    double getSpecificSize ()  { return _specificSize; }

    double getCommonPercentage ()  {  return (double)_commonSize  /  (double) (_commonSize + _specificSize);  }

private:

    core::IAlignmentContainer* _dbComp;
    double _dmax;
    size_t _commonSize;
    size_t _specificSize;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _COMPARE_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
