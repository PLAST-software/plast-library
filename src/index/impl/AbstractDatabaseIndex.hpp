/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

#ifndef _ABSTRACT_DATABASE_INDEX_HPP_
#define _ABSTRACT_DATABASE_INDEX_HPP_

/********************************************************************************/

#include "ISequenceDatabase.hpp"
#include "IDatabaseIndex.hpp"
#include <list>

/********************************************************************************/
namespace indexation {
/********************************************************************************/

class AbstractDatabaseIndex : public IDatabaseIndex
{
public:

    /** */
    AbstractDatabaseIndex (database::ISequenceDatabase* database, seed::ISeedModel* model);
    virtual ~AbstractDatabaseIndex ();

    /** Returns the sequences database. */
    database::ISequenceDatabase* getDatabase () { return _database; }

    /** Returns the seed model used for the indexation. */
    seed::ISeedModel* getModel () { return _model; }

    /** Add/Remove a child index (Design Pattern Composite). */
    void addChildIndex    (IDatabaseIndex* child);
    void removeChildIndex (IDatabaseIndex* child);

    /** */
    dp::IProperties* getProperties (const std::string& root);

protected:

    database::ISequenceDatabase* _database;
    void setDatabase (database::ISequenceDatabase* database);

    seed::ISeedModel* _model;
    void setModel (seed::ISeedModel* model);

    /** A little shortcut. */
    size_t _maxSeedsNumber;

    /** The container of children indexes. */
    std::list<IDatabaseIndex*> _children;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_DATABASE_INDEX_HPP_  */
