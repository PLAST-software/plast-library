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

/** \file AlignmentContainerFactory.hpp
 *  \brief Abstract implementation of IAlignmentContainer interface.
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

/** \brief TO DO
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

    /** \copydoc IAlignmentContainerFactory::createFromFile  */
    IAlignmentContainer* createContainerFromUri (const std::string& uri, void* context=0);
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_CONTAINER_FACTORY_HPP_ */
