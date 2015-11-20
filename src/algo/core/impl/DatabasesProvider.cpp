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

#include <algo/core/api/IAlgoConfig.hpp>
#include <algo/core/api/IAlgoParameters.hpp>
#include <algo/core/impl/DatabasesProvider.hpp>

#include <designpattern/api/ICommand.hpp>
#include <designpattern/impl/CommandDispatcher.hpp>

#include <database/impl/ReadingFrameSequenceDatabase.hpp>
#include <database/impl/CompositeSequenceDatabase.hpp>
#include <database/impl/BufferedCachedSequenceDatabase.hpp>
#include <database/impl/CachedSubDatabase.hpp>


#include <iostream>
#define DEBUG(a)  //a

using namespace std;
using namespace misc;

using namespace dp;
using namespace dp::impl;

using namespace database;
using namespace database::impl;

/********************************************************************************/
namespace algo {
namespace core {
namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DatabasesProvider::DatabasesProvider (algo::core::IConfiguration* config)
    : _config (0), _currentParams(0), _sbjFactory(0), _qryFactory(0)
{
    setConfig (config);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DatabasesProvider::~DatabasesProvider ()
{
    setConfig        (0);
    setCurrentParams (0);

    setSbjFactory (0);
    setQryFactory (0);

    clearDatabaseList (_sbjDbList);
    clearDatabaseList (_qryDbList);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabasesProvider::createDatabases (
    IParameters* params,
    const std::vector<misc::ReadingFrame_e>&    sbjFrames,
    const std::vector<misc::ReadingFrame_e>&    qryFrames,
    database::ISequenceIteratorFactory*         sbjFactory,
    database::ISequenceIteratorFactory*         qryFactory
)
{
    /** We may create the subject database (more than one for tplastn) in case parameters
     * have change from previous call.  */
	bool newSubject = areNewSubjectParameters (params, sbjFactory);
    if (newSubject == true)
    {
        /** We first release potential resources. */
        clearDatabaseList (_sbjDbList);

        createDatabaseList (
            params->subjectUri,
            params->subjectRange,
            false,
            sbjFrames,
            _sbjDbList,
            sbjFactory
        );
    }

    /** We may create the query database (more than one for plastx) in case parameters
     * have change from previous call.  */
	bool newQuery = areNewQueryParameters (params, qryFactory);
    if (newQuery == true)
    {
        /** We first release potential resources. */
        clearDatabaseList (_qryDbList);

        /** We create the query database (more than one for plastx) */
        createDatabaseList (
            params->queryUri,
            params->queryRange,
            params->filterQuery,
            qryFrames,
            _qryDbList,
            qryFactory,
            params->querySequencesBlacklist
        );
    }

    DEBUG (cout << "DatabasesProvider::createDatabases  "
		<< "subject " <<  (newSubject ? "NEW" : "OLD") << "   "
		<< "query "   <<  (newQuery   ? "NEW" : "OLD") << "   "
		<< endl
	);

    /** We keep the provided params. */
    setCurrentParams (params);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabasesProvider::createDatabaseList (
    const std::string&   uri,
    const misc::Range64& range,
    int                 filtering,
    const std::vector<ReadingFrame_e>& frames,
    std::list<database::ISequenceDatabase*>& dbList,
    database::ISequenceIteratorFactory* seqIterFactory,
    std::set<u_int64_t>* blacklist
)
{
    int shouldFilter = !frames.empty() ? 0 : filtering;

    /** We create the source database. */
    ISequenceDatabase* db = createDatabase (uri, range, shouldFilter, seqIterFactory, blacklist);

    if (frames.empty() == false)
    {
        /** We create the 6 reading frame databases. Note we use an auxiliary method (parallelization possibility). */
        list<ISequenceDatabase*> framedList;
        readReadingFrameDatabases (frames, db, filtering, framedList);

        /** We could improve this by reading only once the nucleotid databases and generating 6 reading frames
         *  from this single reading. */
        dbList.push_back (new CompositeSequenceDatabase (framedList));
    }
    else
    {
        dbList.push_back (db);
    }

    /** We loop each entry in the list and use it. */
    for (list<ISequenceDatabase*>::iterator it = dbList.begin();  it != dbList.end(); it++)
    {
        /** We forget the db. */
        (*it)->use();
    }

    DEBUG (cout << "DatabasesProvider::createDatabaseList: "
        << " uri="       << uri
        << " range="     << range
        << " framesNb="    << frames.size()
        << " filter="    << filtering
        << " shouldFilter=" << shouldFilter
        << " nbSeq="     << db->getSequencesNumber()
        << " list.size=" << dbList.size()
        << endl
    );
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabasesProvider::clearDatabaseList (list<ISequenceDatabase*>& dbList)
{
    for (list<ISequenceDatabase*>::iterator it = dbList.begin();  it != dbList.end(); it++)
    {
        /** We forget the db. */
        (*it)->forget ();
    }

    dbList.clear ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabasesProvider::readReadingFrameDatabases (
    const vector<ReadingFrame_e>& frames,
    ISequenceDatabase* db,
    int filtering,
    list<ISequenceDatabase*>& framedList
)
{
    /** We first clear the list to be filled. */
    framedList.clear ();

    /** IMPORTANT !!! The provided nucleotid database may have lazy accessors, so some
     * internals may have not been yet initialized. Since we are going to use it in
     * different threads, we should be sure that the internals are initialized first.
     * We can do it by calling some accessor.
     */
    db->getSize();

    /** We create a list of commands. */
    list<ICommand*> commands;
    for (size_t i=0; i<frames.size(); i++)
    {
        /** We create and use a command. */
        ICommand* cmd = new ReadingFrameSequenceCommand (db, frames[i], filtering);
        cmd->use ();

        /** We add the command to the list to be dispatched. */
        commands.push_back (cmd);
    }

    /** We dispatch the databases reading in a parallel way. */
    ParallelCommandDispatcher dispatcher;
    dispatcher.dispatchCommands (commands, 0);

    /** We retrieve the created databases. */
    for (list<ICommand*>::iterator it = commands.begin(); it != commands.end(); it++)
    {
        /** Shortcut. */
        ReadingFrameSequenceCommand* current = dynamic_cast<ReadingFrameSequenceCommand*> (*it);

        /** We add the database to the resulting list. */
        if (current != 0)  {  framedList.push_back (current->getResult());  }

        /** We forget the command. */
        (*it)->forget();
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool DatabasesProvider::areNewSubjectParameters (algo::core::IParameters* params, database::ISequenceIteratorFactory* factory)
{
    bool result = true;

    if (_currentParams && params)
    {
        result =
            (params->subjectUri   != _currentParams->subjectUri)   ||
            (params->subjectRange != _currentParams->subjectRange) ||
            _sbjFactory != factory;
    }

    DEBUG (cout << "DatabasesProvider::areNewSubjectParameters: result=" << result << endl);
    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool DatabasesProvider::areNewQueryParameters (algo::core::IParameters* params, database::ISequenceIteratorFactory* factory)
{
    bool result = true;

    if (_currentParams && params)
    {
        result =
            (params->queryUri    != _currentParams->queryUri)    ||
            (params->queryRange  != _currentParams->queryRange)  ||
            _qryFactory != factory                               ||
            (params->filterQuery != _currentParams->filterQuery);
    }

    DEBUG (cout << "DatabasesProvider::areNewQueryParameters: result=" << result << endl);
    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabasesProviderReverse::createDatabaseList (
    const std::string&   uri,
    const misc::Range64& range,
    int                 filtering,
    const std::vector<misc::ReadingFrame_e>& frames,
    std::list<database::ISequenceDatabase*>& dbList,
    database::ISequenceIteratorFactory* seqIterFactory,
    std::set<u_int64_t>* blacklist
)
{
    int shouldFilter = !frames.empty() ? 0 : filtering;

    dbList.clear ();

    /** We create the source database. */
    ISequenceDatabase* db = createDatabase (uri, range, shouldFilter, seqIterFactory, blacklist);

    /** We set at least one strand. */
    dbList.push_back (db);

    /** We may add a second strand. Note that the db is the same, but it will be interpreted in a different way
     * by clients according to the frame content (PLUS or MINUS). */
    if (frames.size() >= 2)  {  dbList.push_back (db); }

    /** We loop each entry in the list and use it. */
    for (list<ISequenceDatabase*>::iterator it = dbList.begin();  it != dbList.end(); it++)  {  (*it)->use();  }

    DEBUG (cout << "DatabasesProviderReverse::createDatabaseList: "
        << " uri="       << uri
        << " range="     << range
        << " framesNb="    << frames.size()
        << " filter="    << filtering
        << " shouldFilter=" << shouldFilter
        << " nbSeq="     << db->getSequencesNumber()
        << " list.size=" << dbList.size()
        << endl
    );
}

ISequenceIterator* DatabasesProvider::getSequenceIterator(
    const string& uri,
    const misc::Range64& range,
    database::ISequenceIteratorFactory* sequenceIteratorFactory)
{
    LOCAL (sequenceIteratorFactory);
    ISequenceIteratorFactory* tempFactory = createSequenceIteratorFactory(uri);
    LOCAL (tempFactory);

    /** We create the sequence iterator. */
    ISequenceIterator* seqIterator =  sequenceIteratorFactory ?
        sequenceIteratorFactory->createSequenceIterator (uri, range) :
        tempFactory->createSequenceIterator (uri, range);

    return seqIterator;
}

ISequenceDatabase*  DatabasesProvider::createDatabase (
    const std::string& uri,
    const misc::Range64& range,
    int filtering,
    database::ISequenceIteratorFactory* sequenceIteratorFactory,
    std::set<u_int64_t>* blacklist
)
{
    ISequenceIterator* seqIterator = getSequenceIterator(uri, range, sequenceIteratorFactory);

    if (true && blacklist != NULL) {
        return new CachedSubDatabase(seqIterator, blacklist);
    }

    return new BufferedCachedSequenceDatabase (seqIterator, filtering);
}

database::ISequenceIteratorFactory* DatabasesProvider::createSequenceIteratorFactory (const std::string& uri)
{
       DatabaseLookupType::QuickReaderType_e databaseType = DatabaseLookupType::ENUM_TYPE_UNKNOWN;

       databaseType = DatabaseLookupType::quickReaderType(uri);
       if ((databaseType==DatabaseLookupType::ENUM_BLAST_PIN)||(databaseType==DatabaseLookupType::ENUM_BLAST_NIN)
               ||(databaseType==DatabaseLookupType::ENUM_BLAST_PAL)||(databaseType==DatabaseLookupType::ENUM_BLAST_NAL))
       {
               return new BlastdbSequenceIteratorFactory ();
       }
       else
               return new FastaSequenceIteratorFactory();
}

void DatabasesProviderReverse::createDatabases (
        algo::core::IParameters* params,
        const std::vector<misc::ReadingFrame_e>&    sbjFrames,
        const std::vector<misc::ReadingFrame_e>&    qryFrames,
        database::ISequenceIteratorFactory*         sbjFactory,
        database::ISequenceIteratorFactory*         qryFactory)
{
    bool isQryFactoryCreatedLocally = false;

    if (qryFactory == NULL && params->querySequencesBlacklist != NULL) {
        qryFactory = new FastaSequenceConditionalPureIteratorFactory(params);
        qryFactory->use();
        isQryFactoryCreatedLocally = true;
    }

    DatabasesProvider::createDatabases(params, sbjFrames, qryFrames, sbjFactory, qryFactory);

    if (isQryFactoryCreatedLocally) {
        qryFactory->forget();
    }
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

