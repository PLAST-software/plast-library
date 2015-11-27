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

/** \file IAlignmentContainerFactory.hpp
 *  \brief Interface for some alignments container.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALIGNMENT_CONTAINER_FACTORY_HPP_
#define _IALIGNMENT_CONTAINER_FACTORY_HPP_

/********************************************************************************/

#include <alignment/core/api/IAlignmentContainer.hpp>
#include <designpattern/impl/FileLineIterator.hpp>

#include <string>
#include <vector>

/********************************************************************************/
namespace alignment {
namespace core      {
/********************************************************************************/

/** \brief Factory that builds IAlignmentContainer instances.
 *
 * This interface allows to create IAlignmentContainer instances. It may return
 * empty instances (without any alignments) or instances filled by alignments found
 * in some file.
 */
class IAlignmentContainerFactory : public dp::SmartPointer
{
public:

    /** Create a new IAlignmentContainer instance.
     * \return the created container
     */
    virtual IAlignmentContainer* createContainer () = 0;

    /** Read alignments from some uri resource. Different implementations could read from different file format.
     * \param[in]  uri : uri of the resource to be used
     * \param[in] context : some context for customizing the uri reading
     */
    virtual IAlignmentContainer* createContainerFromUri (const std::string& uri, void* context) = 0;

    /** Read alignments from some uri resource. Different implementations could read from different file format.
     * \param[in]  it : file line iterator
     * \param[in] context : some context for customizing the uri reading
     */
    virtual IAlignmentContainer* createContainerFromUri (dp::impl::FileLineIterator* it, void* context) = 0;
};

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _IALIGNMENT_CONTAINER_FACTORY_HPP_ */
