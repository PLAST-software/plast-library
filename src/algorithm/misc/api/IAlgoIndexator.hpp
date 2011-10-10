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

#ifndef _IALGO_INDEXATOR_HPP_
#define _IALGO_INDEXATOR_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"

#include "ISequenceDatabase.hpp"
#include "ICommand.hpp"
#include "IHitIterator.hpp"
#include "IDatabaseIndex.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** Interface that takes care about subject and query databases, indexes them and
 *  then can create IHitIterator instances.
 */
class IIndexator : public dp::SmartPointer
{
public:

    /** */
    virtual database::ISequenceDatabase* getSubjectDatabase () = 0;
    virtual database::ISequenceDatabase* getQueryDatabase   () = 0;

    /** */
    virtual void setSubjectDatabase (database::ISequenceDatabase* db) = 0;
    virtual void setQueryDatabase   (database::ISequenceDatabase* db) = 0;

    /** */
    virtual void build (dp::ICommandDispatcher* dispatcher) = 0;

    /** */
    virtual indexation::IDatabaseIndex* getSubjectIndex () = 0;
    virtual indexation::IDatabaseIndex* getQueryIndex   () = 0;

    /** */
    virtual indexation::IHitIterator* createHitIterator () = 0;

    /** Return properties about the instance. */
    virtual dp::IProperties* getProperties () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_INDEXATOR_HPP_ */
