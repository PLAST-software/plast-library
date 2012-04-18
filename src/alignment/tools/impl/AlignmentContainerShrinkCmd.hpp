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

/** \file AlignmentContainerShrinkCmd.hpp
 *  \brief Shrink redundant alignments
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALIGNMENT_CONTAINER_SHRINK_CMD_HPP_
#define _ALIGNMENT_CONTAINER_SHRINK_CMD_HPP_

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
class AlignmentContainerShrinkCmd : public dp::ICommand
{
public:

    AlignmentContainerShrinkCmd (
        std::list<core::Alignment>& alignments,
        bool (*sort_cbk) (const core::Alignment& i, const core::Alignment& j),
        size_t nbAlignToKeep
    );

    void execute ();

    size_t getNbRemoved ()  { return _nbRemoved; }

private:
    std::list<core::Alignment>& _alignments;
    bool (*_sort_cbk) (const core::Alignment& i, const core::Alignment& j);
    int _shiftDivisor;
    size_t _nbRemoved;
    static bool mysortfunction (const core::Alignment& i, const core::Alignment& j);
    size_t _nbAlignToKeep;
};

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_CONTAINER_SHRINK_CMD_HPP_ */
