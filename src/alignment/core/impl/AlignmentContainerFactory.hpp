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

/** \file AlignmentContainerFactory.hpp
 *  \brief Default implementation of AlignmentContainerFactory interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALIGNMENT_CONTAINER_FACTORY_HPP_
#define _ALIGNMENT_CONTAINER_FACTORY_HPP_

/********************************************************************************/

#include <alignment/core/api/IAlignmentContainerFactory.hpp>

/********************************************************************************/
namespace alignment {
namespace core      {
namespace impl      {
/********************************************************************************/

/** \brief Implementation of the IAlignmentContainerFactory interface
 *
 * This implementation creates BasicAlignmentContainer instances.
 *
 * The createContainerFromUri method is able to read tabular output files (like blast
 * can do with -outfmt 6). This is useful for instance for GatTool that reads result
 * files through this API; it can then compare alignments containers from two results
 * files (one from blast, one from plast for instance) and estimate the global overlap
 * between the two containers.
 *
 */
class AlignmentContainerFactory : public IAlignmentContainerFactory
{
public:

    /** Singleton. */
    static AlignmentContainerFactory singleton ()
    { static AlignmentContainerFactory instance; return instance; }

    /** Destructor. */
    virtual ~AlignmentContainerFactory()  {}

    /** \copydoc IAlignmentContainerFactory::createContainer  */
    IAlignmentContainer* createContainer ();

    /** \copydoc IAlignmentContainerFactory::createContainerFromUri(const std::string&,void*)  */
    IAlignmentContainer* createContainerFromUri (const std::string& uri, void* context=0);

    /** \copydoc IAlignmentContainerFactory::createContainerFromUri(dp::impl::FileLineIterator*,void*)  */
    IAlignmentContainer* createContainerFromUri (dp::impl::FileLineIterator* it, void* context=0);
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_CONTAINER_FACTORY_HPP_ */
