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

#include <designpattern/api/ICommand.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>
#include <designpattern/impl/CommandDispatcher.hpp>

#include <misc/api/PlastStrings.hpp>

#include <database/impl/FastaDatabaseQuickReader.hpp>
#include <database/impl/AminoAcidDatabaseQuickReader.hpp>
#include <database/impl/ReverseStrandSequenceIterator.hpp>

#include <algo/core/api/IAlgoEvents.hpp>
#include <algo/core/impl/DefaultAlgoEnvironment.hpp>
#include <algo/core/impl/DefaultAlgoConfig.hpp>
#include <algo/core/impl/PlastnAlgoConfig.hpp>
#include <algo/core/impl/AbstractAlgorithm.hpp>
#include <algo/core/impl/AlgorithmPlastn.hpp>
#include <algo/core/impl/DatabasesProvider.hpp>

#include <alignment/filter/api/IAlignmentFilter.hpp>
#include <alignment/filter/impl/AlignmentFilterXML.hpp>

#include <alignment/visitors/impl/XmlOutputVisitor.hpp>
#include <alignment/visitors/impl/NucleotidConversionVisitor.hpp>
#include <alignment/visitors/impl/ReverseStrandVisitor.hpp>

using namespace std;
using namespace os;
using namespace os::impl;
using namespace dp;
using namespace dp::impl;
using namespace misc;
using namespace database;
using namespace database::impl;

using namespace statistics;

using namespace alignment::core;
using namespace alignment::filter;
using namespace alignment::filter::impl;
using namespace alignment::visitors::impl;

using namespace algo::core;

#include <stdio.h>
#define DEBUG(a)  //printf a

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
DefaultEnvironment::DefaultEnvironment (IProperties* properties, bool& isRunning)
    : _properties(0), _isRunning(isRunning),
      _config(0), _filter(0), _quickSubjectDbReader(0), _quickQueryDbReader(0),
      _resultVisitor(0), _dbProvider(0), _timeInfo(0), _timeInfoAlgo(0)
{
    setProperties (properties);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DefaultEnvironment::~DefaultEnvironment ()
{
    setProperties           (0);
    setConfig               (0);
    setFilter               (0);
    setQuickSubjectDbReader (0);
    setQuickQueryDbReader   (0);
    setResultVisitor        (0);
    setDatabasesProvider    (0);
    setTimeInfo             (0);
    setTimeInfoAlgo         (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DefaultEnvironment::configure ()
{
    const char* keyConfig = "config";

    /** We create a configuration object for the provided program name (plastp, tplasn...) */
    setConfig (createConfiguration (_properties));

    /** We create a TimeInfo instance for the current instance. */
    setTimeInfo (_config->createTimeInfo());

    /** We create a TimeInfo instance for the algorithm instances. */
    setTimeInfoAlgo (_config->createTimeInfo());

    _timeInfo->addEntry (keyConfig);

    /** We create a subject/query databases provider. */
    setDatabasesProvider (_config->createDatabaseProvider());

    /** We retrieve the type of algorithm (may be not set). */
    IProperty* algoProp = _properties->getProperty (STR_OPTION_ALGO_TYPE);
    bool inferType = true; //(algoProp == 0);

    /** We may have a defined XML file URI. */
    IProperty* filterProp = _properties->getProperty (STR_OPTION_XML_FILTER_FILE);
    if (filterProp != 0)
    {
        setFilter (AlignmentFilterFactoryXML().createFilter (filterProp->getString()));
    }

    u_int64_t  maxblocksize = 20*1000*1000;
    IProperty* maxBlockProp = _properties->getProperty(STR_OPTION_MAX_DATABASE_SIZE);
    if (maxBlockProp != 0)  {  maxblocksize = maxBlockProp->getInt();  }
    else  { _properties->add (0, STR_OPTION_MAX_DATABASE_SIZE, "%lld", maxblocksize); }

    /** We need to read the subject database to get its data size and the number of sequences.
     *  This information will be used for computing cutoffs for the query sequences. */
    IProperty* subjectProp = _properties->getProperty (STR_OPTION_SUBJECT_URI);
    if (subjectProp == 0)  {  subjectProp = _properties->add (0, STR_OPTION_SUBJECT_URI, "foo"); }

    setSubjectBank (_properties, maxblocksize);

    /** We need to read the subject database to get its data size and the number of sequences. */
    IProperty* queryProp = _properties->getProperty (STR_OPTION_QUERY_URI);
    if (queryProp == 0)  {  queryProp = _properties->add (0, STR_OPTION_QUERY_URI, "foo"); }
    if (queryProp != 0)
    {
        setQuickQueryDbReader (new FastaDatabaseQuickReader (queryProp->value, inferType));
        _quickQueryDbReader->read (maxblocksize);
    }

    /** We may have to infer the kind of algorithm (plastp, plastx...) if no one is provided. */
    if (algoProp == 0)
    {
        /** Shortcuts. */
        IDatabaseQuickReader::DatabaseKind_e subjectKind = _quickSubjectDbReader->getKind();
        IDatabaseQuickReader::DatabaseKind_e queryKind   = _quickQueryDbReader->getKind();

        if (subjectKind == IDatabaseQuickReader::ENUM_AMINO_ACID &&  queryKind == IDatabaseQuickReader::ENUM_AMINO_ACID)
        {
            _properties->add (0, STR_OPTION_ALGO_TYPE, "plastp");
        }
        else if (subjectKind == IDatabaseQuickReader::ENUM_AMINO_ACID &&  queryKind == IDatabaseQuickReader::ENUM_NUCLOTID)
        {
            _properties->add (0, STR_OPTION_ALGO_TYPE, "plastx");
        }
        else if (subjectKind == IDatabaseQuickReader::ENUM_NUCLOTID &&  queryKind == IDatabaseQuickReader::ENUM_AMINO_ACID)
        {
            _properties->add (0, STR_OPTION_ALGO_TYPE, "tplastn");
        }
        //else if (subjectKind == IDatabaseQuickReader::ENUM_NUCLOTID &&  queryKind == IDatabaseQuickReader::ENUM_NUCLOTID)
        //{
        //    _properties->add (0, STR_OPTION_ALGO_TYPE, "tplastx");
        //}
        else
        {
            /** Note that for ADN/ADN, we have 2 choices (tplastx or plastn), so we can't guess what the user wants.
             * Should throw an exception ?*/
        }
    }

    /** We create a visitor for visiting the resulting alignments. Note that we use only one visitor even if
     *  we have to run several algorithm; in such a case, the results are 'concatenated' by the same visitor.
     *  IMPORTANT: we should create this instance after creating quick readers since these ones may depend
     *  on this result visitor. */
    setResultVisitor (_config->createResultVisitor ());

    /** We register as listener to the result visitor. */
    _resultVisitor->addObserver (this);

    /** We build a list of uri for subject/query databases. */
    vector<pair<Range64,Range64> > uriList = buildUri (_quickSubjectDbReader, _quickQueryDbReader);

    /** We build a list of Parameters. We will launch the algorithm for each item of this list. */
    _parametersList = createParametersList (_config, _properties, uriList);

    _timeInfo->stopEntry (keyConfig);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IConfiguration* DefaultEnvironment::createConfiguration (dp::IProperties* properties)
{
    IConfiguration* result = 0;

    /** We retrieve the type of algorithm (may be not set). */
    IProperty* algoProp = _properties->getProperty (STR_OPTION_ALGO_TYPE);

    DEBUG (("DefaultEnvironment::createConfiguration: algoProp=%p (val='%s') \n", algoProp, (algoProp ? algoProp->getString() : "?")));

    if (algoProp != 0  &&  algoProp->getValue().compare ("plastn")==0)
    {
        result = new PlastnConfiguration (this, properties);
    }
    else
    {
        result = new DefaultConfiguration (this, properties);
    }

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
void DefaultEnvironment::run ()
{
    const char* keyTotal  = "total";
    const char* keyFinal  = "finalization";

    /** We check that we got at least one paramter. */
    if (_parametersList.empty())  { return; }

    _timeInfo->addEntry (keyTotal);

    EncodingManager::singleton().setKind (_parametersList[0]->algoKind == ENUM_PLASTN ?
        EncodingManager::ALPHABET_NUCLEOTID : EncodingManager::ALPHABET_AMINO_ACID
    );

    /** We may launch an event with information about the two databases. */
    if (_quickSubjectDbReader != 0  &&  _quickQueryDbReader != 0)
    {
        this->notify (new DatabasesInformationEvent (_quickSubjectDbReader, _quickQueryDbReader) );
    }

    /** We create the seeds model. */
    seed::ISeedModel* seedsModel = _config->createSeedModel (
        _parametersList[0]->seedModelKind,
        _parametersList[0]->seedSpan,
        _parametersList[0]->subseedStrings
    );
    LOCAL (seedsModel);

    /** We create an object for indexing subject and query databases. This object will be in
     * charge to feed the algorithm with the source Hit Iterator, ie the one that provides for
     * a given seed all the occurrences in subject and query databases.
     */
    IIndexator* indexator = _config->createIndexator (seedsModel, _parametersList[0], _isRunning);
    LOCAL (indexator);

    /** We iterate each parameters. */
    for (size_t i=0; _isRunning && i<_parametersList.size(); i++)
    {
        list<ICommand*> algosCmd;

        /** We send a notification to potential listeners. */
        this->notify (new AlgorithmConfigurationEvent (_properties, i, _parametersList.size()));

        /** We create an Algorithm instance. */
        list<IAlgorithm*> algos = this->createAlgorithm (
            _config,
            _quickSubjectDbReader,
            _parametersList[i],
            _filter,
            _resultVisitor,
            seedsModel,
            _dbProvider,
            indexator,
            _config->createGlobalParameters (_parametersList[i],_quickSubjectDbReader->getDataSize()),
            _timeInfoAlgo,
            _isRunning
        );
        if (algos.empty())  { continue; }

        /** We loop over each created algorithm. */
        for (list<IAlgorithm*>::iterator it = algos.begin(); it != algos.end(); ++it)
        {
        	/** We can register ourself to be notified by execution events. */
        	(*it)->addObserver (this);

        	/** We add this instance to the algorithms list. */
        	algosCmd.push_back (*it);
        }

        /** We create a commands dispatcher. */
        ICommandDispatcher* dispatcher = new SerialCommandDispatcher ();
        LOCAL (dispatcher);

        /** We execute the algorithms through the dispatcher. */
        dispatcher->dispatchCommands (algosCmd, 0);
    }

    /** We may check whether we are actually done or the request has been canceled. */
    if (_isRunning == true)
    {
        /** We finalize the output result visitor. */
        _timeInfo->addEntry (keyFinal);

        _resultVisitor->finalize ();

        _timeInfo->stopEntry (keyFinal);
    }

    _timeInfo->stopEntry (keyTotal);

    /** We send a notification telling we are done (ie. current==total). */
    this->notify (new AlgorithmConfigurationEvent (_properties, _parametersList.size(), _parametersList.size()));

    /** We also send a notification that provides time information. */
    this->notify (new TimeInfoEvent ("environment", _timeInfo));
    this->notify (new TimeInfoEvent ("algorithm",   _timeInfoAlgo));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
list<IAlgorithm*> DefaultEnvironment::createAlgorithm (
    IConfiguration*             config,
    IDatabaseQuickReader*       reader,
    IParameters*                params,
    IAlignmentFilter*           filter,
    IAlignmentContainerVisitor* resultVisitor,
    seed::ISeedModel*           seedModel,
    IDatabasesProvider*         dbProvider,
    algo::core::IIndexator*     indexator,
    IGlobalParameters*          globalStats,
    TimeInfo*                   timeInfo,
    bool&                       isRunning
)
{
	list<IAlgorithm*> result;

    switch (params->algoKind)
    {
        case ENUM_PLASTP:
            result.push_back (new AlgorithmPlastp (
                config,
                reader,
                params,
                filter,
                resultVisitor,
                seedModel,
                dbProvider,
                indexator,
                globalStats,
                timeInfo,
                isRunning
            ));
            break;

        case ENUM_TPLASTN:
        	result.push_back (new AlgorithmTplastn (
                config,
                new AminoAcidDatabaseQuickReader (reader),
                params,
                filter,
                resultVisitor, //new NucleotidConversionVisitor (resultVisitor, Alignment::SUBJECT),
                seedModel,
                dbProvider,
                indexator,
                globalStats,
                timeInfo,
                isRunning
            ));
            break;

        case ENUM_PLASTX:
        	result.push_back (new AlgorithmPlastx (
                config,
                reader,
                params,
                filter,
                resultVisitor, //new NucleotidConversionVisitor (resultVisitor, Alignment::QUERY),
                seedModel,
                dbProvider,
                indexator,
                globalStats,
                timeInfo,
                isRunning
            ));
            break;

        case ENUM_TPLASTX:
        	result.push_back (new AlgorithmTplastx (
                config,
                new AminoAcidDatabaseQuickReader (reader),
                params,
                filter,
                resultVisitor,
                //new NucleotidConversionVisitor (new NucleotidConversionVisitor (resultVisitor, Alignment::SUBJECT),
                //    Alignment::QUERY
                //),
                seedModel,
                dbProvider,
                indexator,
                globalStats,
                timeInfo,
                isRunning
            ));
            break;

        case ENUM_PLASTN:
        {
            DEBUG (("DefaultEnvironment::createAlgorithm: strand=%d\n", params->strand));

            /** Note: in a preliminary version, we used to compute the "minus" strand first. Now, we prefer
             *  to do the "plus" strand first in order to optimize perfect match for a sequence vs itself.
             *  For instance, if we compare a database with itself, we expect to get the best hit as an alignment
             *  on the plus strand with qry and sbj length being equal to the sequence length.
             */

            std::vector<misc::ReadingFrame_e>  subjectFrames;
            if (params->strand == 0  ||  params->strand== 1)  {  subjectFrames.push_back (FRAME_1);  }
            if (params->strand == 0  ||  params->strand==-1)  {  subjectFrames.push_back (FRAME_2);  }

            result.push_back (new AlgorithmPlastn (
                config,
                reader,
                params,
                filter,
                new ReverseStrandVisitor (resultVisitor, Alignment::SUBJECT, ReverseStrandVisitor::PLUS),
                seedModel,
                dbProvider,
                indexator,
                globalStats,
                timeInfo,
                isRunning,
                subjectFrames
            ));

            break;
        }

        default:
            break;
    }

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
void DefaultEnvironment::update (dp::EventInfo* evt, dp::ISubject* subject)
{
    /** We just forward the event. */
    if (this != subject) {  this->notify (evt);  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
vector<pair<Range64,Range64> > DefaultEnvironment::buildUri (
    IDatabaseQuickReader* subjectReader,
    IDatabaseQuickReader* queryReader
)
{
    vector<pair<Range64,Range64> > result;

    /** Shortcuts. */
    vector<u_int64_t>& subjectOffsets = subjectReader->getOffsets();
    vector<u_int64_t>& queryOffsets   = queryReader->getOffsets();

    for (size_t j=0; j<queryOffsets.size()-1; j++)
    {
        for (size_t i=0; i<subjectOffsets.size()-1; i++)
        {
            Range64 s (subjectOffsets[i], subjectOffsets[i+1]-1);
            Range64 q (queryOffsets[j],   queryOffsets  [j+1]-1);

            result.push_back (pair<Range64,Range64> (s,q));
        }
    }

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
vector<IParameters*> DefaultEnvironment::createParametersList (
    IConfiguration* config,
    dp::IProperties* properties,
    vector <pair <Range64,Range64> >& uri
)
{
    vector<IParameters*> result;

    /** We look for the program type property. */
    IProperty* progType = properties->getProperty (STR_OPTION_ALGO_TYPE);

    if (progType != 0)
    {
        DEBUG (("DefaultEnvironment::createParametersList: algotype='%s'\n", progType->getString()));

        IProperty* prop = 0;

        for (size_t i=0; i<uri.size(); i++)
        {
            /** We create default parameters. */
            IParameters* params = config->createDefaultParameters (progType->value);

            if ( (prop = properties->getProperty (STR_OPTION_SUBJECT_URI)) != 0)            {  params->subjectUri           = prop->value; }
            if ( (prop = properties->getProperty (STR_OPTION_QUERY_URI)) != 0)              {  params->queryUri             = prop->value; }

            if ( (prop = properties->getProperty (STR_OPTION_OUTPUT_FILE)) != 0)            {  params->outputfile           = prop->value; }

            if ( (prop = properties->getProperty (STR_OPTION_FILTER_QUERY)) != 0)
            {
                     if (prop->value == "T")   {  params->filterQuery = params->filterQueryThreshold;   }
                else if (prop->value == "F")   {  params->filterQuery = 0;  }
                else                           {  params->filterQuery = misc::atoi(prop->value.c_str()); }
            }

            if ( (prop = properties->getProperty (STR_OPTION_UNGAP_NEIGHBOUR_LENGTH)) != 0) {  params->ungapNeighbourLength = misc::atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_UNGAP_SCORE_THRESHOLD)) != 0)  {  params->ungapScoreThreshold  = misc::atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_SMALLGAP_BAND_WITH)) != 0)     {  params->smallGapBandWidth    = misc::atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_SMALLGAP_THRESHOLD)) != 0)     {  params->smallGapThreshold    = misc::atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_OPEN_GAP_COST)) != 0)          {  params->openGapCost          = misc::atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_EXTEND_GAP_COST)) != 0)        {  params->extendGapCost        = misc::atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_EVALUE)) != 0)                 {  params->evalue               = misc::atof (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_X_DROPOFF_UNGAPPED)) != 0)     {  params->XdroppofUnGap        = misc::atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_X_DROPOFF_GAPPED)) != 0)       {  params->XdroppofGap          = misc::atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_X_DROPOFF_FINAL)) != 0)        {  params->finalXdroppofGap     = misc::atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_INDEX_NEIGHBOUR_THRESHOLD)) != 0)     {  params->index_neighbor_threshold      = misc::atoi (prop->value.c_str()); }

            if ( (prop = properties->getProperty (STR_OPTION_REWARD))  != 0)                {  params->reward  = misc::atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_PENALTY)) != 0)                {  params->penalty = misc::atoi (prop->value.c_str()); }

            if ( (prop = properties->getProperty (STR_OPTION_WORD_SIZE)) != 0)              {  params->seedSpan = misc::atoi (prop->value.c_str()); }

            if ( (prop = properties->getProperty (STR_OPTION_SCORE_MATRIX)) != 0)
            {
                     if (prop->value.compare ("BLOSUM62")==0)   {  params->matrixKind = ENUM_BLOSUM62;  }
                else if (prop->value.compare ("BLOSUM50")==0)   {  params->matrixKind = ENUM_BLOSUM50;  }
                else                                            {  params->matrixKind = ENUM_BLOSUM62;  }
            }

            if ( (prop = properties->getProperty (STR_OPTION_STRANDS_LIST)) != 0)
            {
                TokenizerIterator it (prop->getString(), ",");
                for (it.first(); !it.isDone(); it.next())
                {
                    ReadingFrame_e val = (ReadingFrame_e) (misc::atoi(it.currentItem()) - 1);
                    params->strands.push_back (val);
                }
            }

            /** We set databases ranges. */
            params->subjectRange = uri[i].first;
            params->queryRange   = uri[i].second;

            result.push_back (params);

			if (i==0) { this->notify (new EnvironmentParameterEvent (params)); }

        }
    }

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
void DefaultEnvironment::setSubjectBank (dp::IProperties* properties, u_int64_t  maxblocksize)
{
    /** We get the subject URI. */
    IProperty* subjectUriProp = properties->getProperty (STR_OPTION_SUBJECT_URI);

    /** We create the quick reader instance for the subject bank. */
    setQuickSubjectDbReader (_config->createDefaultQuickReader(subjectUriProp->value, true));
    subjectUriProp->value = _quickSubjectDbReader->getUri();

    /** We check whether it is an 'info' file or a fasta file. */
    bool isInfoFile = FastaDatabaseQuickReader::isQuickReaderFile (subjectUriProp->value);

    DEBUG (("DefaultEnvironment::setSubjectBank  subjectUriProp=%s  maxblocksize=%d  isInfoFile=%d \n", subjectUriProp->getString(), maxblocksize, isInfoFile));

    if (isInfoFile)
    {
        /** We load the info file. */
        _quickSubjectDbReader->load (subjectUriProp->getValue());

        /** We check that its maxblocksize is coherent to the provided one. */
        if (_quickSubjectDbReader->getMaxBlockSize() != maxblocksize)
        {
            /** We have to read the subject database to extract some information. */
            _quickSubjectDbReader->read (maxblocksize);
        }

        /** We set the actual bank URI. */
        subjectUriProp->value = _quickSubjectDbReader->getUri();
    }
    else
    {
        /** We have to read the subject database to extract some information. */
        _quickSubjectDbReader->read (maxblocksize);
    }
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
