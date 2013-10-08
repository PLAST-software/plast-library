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

/** \file AbstractDatabaseIndex.hpp
 *  \brief Implemtation of interfaces for genomic database indexation.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ABSTRACT_DATABASE_INDEX_HPP_
#define _ABSTRACT_DATABASE_INDEX_HPP_

/********************************************************************************/

#include <database/api/ISequenceDatabase.hpp>
#include <index/api/IDatabaseIndex.hpp>
#include <list>

/********************************************************************************/
namespace indexation {
/** \brief Implementation of genomic database indexation concepts. */
namespace impl {
/********************************************************************************/

/** \brief Partial implementation of IDatabaseIndex interface
 *
 *  This class factorizes some common behaviours between implementors of the
 *  IDatabaseIndex interface, like getters and add/remove children.
 *
 *  It is still abstract and can't be instantiated.
 */
class AbstractDatabaseIndex : public IDatabaseIndex
{
public:

    /** Constructor.
     * \param[in] database : the database to be indexed
     * \param[in] model : the seed model whose seeds are index keys
     * */
    AbstractDatabaseIndex (database::ISequenceDatabase* database, seed::ISeedModel* model);

    /** Destructor. */
    virtual ~AbstractDatabaseIndex ();

    /** \copydoc IDatabaseIndex::getDatabase */
    database::ISequenceDatabase* getDatabase () { return _database; }

    /** \copydoc IDatabaseIndex::setDatabase */
    void setDatabase (database::ISequenceDatabase* database)  { SP_SETATTR(database); }

    /** \copydoc IDatabaseIndex::getModel */
    seed::ISeedModel* getModel () { return _model; }

    u_int8_t* getMask () { return 0; }

    /** \copydoc IDatabaseIndex::addChildIndex */
    void addChildIndex    (IDatabaseIndex* child);

    /** \copydoc IDatabaseIndex::removeChildIndex */
    void removeChildIndex (IDatabaseIndex* child);

    /** \copydoc IDatabaseIndex::getProperties */
    dp::IProperties* getProperties (const std::string& root);

protected:

    /** Database to be indexed. */
    database::ISequenceDatabase* _database;

    /** Seed model to be used for the indexation. */
    seed::ISeedModel* _model;
    void setModel (seed::ISeedModel* model)  { SP_SETATTR(model); }

    /** A little shortcut. */
    size_t _maxSeedsNumber;

    /** The container of children instances. */
    std::list<IDatabaseIndex*> _children;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_DATABASE_INDEX_HPP_  */
