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

/** \file DatabasesProvider.hpp
 *  \brief Some kind of cache for subject and query databases
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _DATABASES_PROVIDER_HPP_
#define _DATABASES_PROVIDER_HPP_

/********************************************************************************/

#include <designpattern/impl/ListIterator.hpp>
#include <algo/core/api/IDatabasesProvider.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

/** \brief Provides means for getting the subject and query databases.
 */
class DatabasesProvider : public IDatabasesProvider
{
public:

    /** Constructor. */
    DatabasesProvider (algo::core::IConfiguration* config);

    /** Destructor. */
    virtual ~DatabasesProvider ();

    /** */
    void createDatabases (
        algo::core::IParameters* params,
        const std::vector<misc::ReadingFrame_e>&    sbjFrames,
        const std::vector<misc::ReadingFrame_e>&    qryFrames,
        database::ISequenceIteratorFactory*         sbjFactory,
        database::ISequenceIteratorFactory*         qryFactory
    );

    /** */
    dp::impl::ListIterator<database::ISequenceDatabase*> getSubjectDbIterator ()
    {
        return dp::impl::ListIterator<database::ISequenceDatabase*> (_sbjDbList);
    }

    dp::impl::ListIterator<database::ISequenceDatabase*> getQueryDbIterator ()
    {
        return dp::impl::ListIterator<database::ISequenceDatabase*> (_qryDbList);
    }

private:

    algo::core::IConfiguration* _config;
    void setConfig (algo::core::IConfiguration* config) { SP_SETATTR(config); }

    std::list<database::ISequenceDatabase*> _sbjDbList;
    std::list<database::ISequenceDatabase*> _qryDbList;

    algo::core::IParameters* _currentParams;
    void setCurrentParams (algo::core::IParameters* currentParams)  { SP_SETATTR(currentParams); }

    /** Provides the list of databases to be used as source databases for the algorithm.
     *  It should be called twice, one for the subject databases configuration, and once
     *  for the query.
     *
     *  If the frames attribute is empty, one will get only one database in the resulting
     *  list (we will read the file normally).
     *
     *  If the frames attribute is not empty, we will read the provided uri and interpret it
     *  as a nucleotid database that will be translated in amino acid database for each frame
     *  of the frames attribute; in such a case, the resulting list will have more than one
     *  item.
     */
    void createDatabaseList (
        const std::string&   uri,
        const misc::Range64& range,
        bool                 filtering,
        const std::vector<misc::ReadingFrame_e>& frames,
        std::list<database::ISequenceDatabase*>& dbList,
        database::ISequenceIteratorFactory* seqIterFactory
    );

    /** */
    void clearDatabaseList (std::list<database::ISequenceDatabase*>& dbList);

    /** */
    void readReadingFrameDatabases (
        const std::vector<misc::ReadingFrame_e>& frames,
        database::ISequenceDatabase* db,
        bool filtering,
        std::list<database::ISequenceDatabase*>& framedList
    );

    /** */
    bool areNewSubjectParameters (algo::core::IParameters* params);
    bool areNewQueryParameters   (algo::core::IParameters* params);
};

/********************************************************************************/

/**
 */
class DatabasesProviderProxy : public DatabasesProvider
{
public:

    /** Constructor. */
    DatabasesProviderProxy (
        IDatabasesProvider*                 ref,
        algo::core::IConfiguration*         config,
        database::ISequenceIteratorFactory* sbjFactory,
        database::ISequenceIteratorFactory* qryFactory
    )
        : DatabasesProvider(config), _ref(0), _sbjFactory(0), _qryFactory(0)
    {
        setRef        (ref);
        setSbjFactory (sbjFactory);
        setQryFactory (qryFactory);
    }

    /** Destructor. */
    virtual ~DatabasesProviderProxy ()
    {
        setRef (0);
        setSbjFactory (0);
        setQryFactory (0);
    }

    /** */
    void createDatabases (
        algo::core::IParameters* params,
        const std::vector<misc::ReadingFrame_e>&    sbjFrames,
        const std::vector<misc::ReadingFrame_e>&    qryFrames,
        database::ISequenceIteratorFactory*         sbjFactory,
        database::ISequenceIteratorFactory*         qryFactory
    )
    {
        _ref->createDatabases (params, sbjFrames, qryFrames, _sbjFactory, _qryFactory);
    }

    /** */
    dp::impl::ListIterator<database::ISequenceDatabase*> getSubjectDbIterator ()
    {
        return _ref->getSubjectDbIterator();
    }

    dp::impl::ListIterator<database::ISequenceDatabase*> getQueryDbIterator ()
    {
        return _ref->getQueryDbIterator();
    }

private:

    IDatabasesProvider* _ref;
    void setRef (IDatabasesProvider* ref)  { SP_SETATTR(ref); }

    database::ISequenceIteratorFactory* _sbjFactory;
    void setSbjFactory (database::ISequenceIteratorFactory* sbjFactory)  { SP_SETATTR(sbjFactory); }

    database::ISequenceIteratorFactory* _qryFactory;
    void setQryFactory (database::ISequenceIteratorFactory* qryFactory)  { SP_SETATTR(qryFactory); }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _DATABASES_PROVIDER_HPP_ */
