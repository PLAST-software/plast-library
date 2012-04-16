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

/** \file AlignmentOverlap.hpp
 *  \brief
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALIGNMENT_OVERLAP_HPP_
#define _ALIGNMENT_OVERLAP_HPP_

/********************************************************************************/

#include <designpattern/api/ICommand.hpp>
#include <alignment/core/api/Alignment.hpp>
#include <alignment/core/api/IAlignmentContainerVisitor.hpp>

/********************************************************************************/
namespace alignment {
namespace tools     {
namespace impl      {
/********************************************************************************/

/** \brief Definition of an alignment filtering
 */
class AbstractAlignmentOverlapCmd : public dp::ICommand
{
public:

    AbstractAlignmentOverlapCmd (
        const std::list<core::Alignment>* ref,
        const std::list<core::Alignment>* comp,
        misc::Range<double> overlapRange
    ) : _ref(ref), _comp(comp), _overlapRange(overlapRange) {}

    void execute ();

protected:

    virtual void handleCommonAlignment   (const core::Alignment& a) = 0;
    virtual void handleDistinctAlignment (const core::Alignment& a) = 0;

    const std::list<core::Alignment>* _ref;
    const std::list<core::Alignment>* _comp;

    misc::Range<double> _overlapRange;
};

/********************************************************************************/

/** \brief Definition of an alignment filtering
 */
class AlignmentOverlapCmd : public AbstractAlignmentOverlapCmd
{
public:

    AlignmentOverlapCmd (
        const std::list<core::Alignment>* ref,
        const std::list<core::Alignment>* comp,
        misc::Range<double> overlapRange,
        core::IAlignmentContainerVisitor* visitorCommon   = 0,
        core::IAlignmentContainerVisitor* visitorDistinct = 0
    ) : AbstractAlignmentOverlapCmd (ref,comp,overlapRange),
        _commonSize(0), _specificSize(0), _visitorCommon(0), _visitorDistinct(0)
    {
        //if (_ref && !_comp)  { _specificSize = _ref->size();  }
        setVisitorCommon   (visitorCommon);
        setVisitorDistinct (visitorDistinct);
    }

    ~AlignmentOverlapCmd ()
    {
        setVisitorCommon   (0);
        setVisitorDistinct (0);
    }

    size_t getTotalSize    () { return getCommonSize() + getSpecificSize();   }
    size_t getCommonSize   () { return _commonSize;     }
    size_t getSpecificSize () { return _specificSize;   }

protected:

    void handleCommonAlignment   (const core::Alignment& a)
    {
        _commonSize++;
        _count++;
        if (_visitorCommon)  { _visitorCommon->visitAlignment ((core::Alignment*)&a, _dummyProgress); }
    }

    void handleDistinctAlignment (const core::Alignment& a)
    {
        _specificSize++;
        _count++;
        if (_visitorDistinct)  { _visitorDistinct->visitAlignment ((core::Alignment*)&a, _dummyProgress); }
    }

private:

    size_t _commonSize;
    size_t _specificSize;

    core::IAlignmentContainerVisitor* _visitorCommon;
    void setVisitorCommon (core::IAlignmentContainerVisitor* visitorCommon)  { SP_SETATTR(visitorCommon); }

    core::IAlignmentContainerVisitor* _visitorDistinct;
    void setVisitorDistinct (core::IAlignmentContainerVisitor* visitorDistinct)  { SP_SETATTR(visitorDistinct); }

    misc::ProgressInfo _dummyProgress;

    static int _count;
};

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_OVERLAP_HPP_ */
