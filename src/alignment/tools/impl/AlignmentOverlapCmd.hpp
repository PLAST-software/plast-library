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

/********************************************************************************/
namespace alignment {
namespace tools     {
namespace impl      {
/********************************************************************************/

/** \brief Definition of an alignment filtering
 */
class AlignmentOverlapCmd : public dp::ICommand
{
public:

    AlignmentOverlapCmd (
        const std::list<core::Alignment>* ref,
        const std::list<core::Alignment>* comp,
        double overlapThreshold
    ) : _ref(ref), _comp(comp), _threshold(overlapThreshold), _commonSize(0), _specificSize(0) {}

    void execute ();

    size_t getTotalSize    () { return _commonSize + _specificSize;   }
    size_t getCommonSize   () { return _commonSize;     }
    size_t getSpecificSize () { return _specificSize;   }

private:

    const std::list<core::Alignment>* _ref;
    const std::list<core::Alignment>* _comp;
    double _threshold;

    size_t _commonSize;
    size_t _specificSize;
};

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_OVERLAP_HPP_ */
