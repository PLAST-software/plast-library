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

#include <algo/core/impl/DefaultAlgoEnvironment.hpp>
#include <algo/core/impl/DefaultAlgoConfig.hpp>
#include <algo/core/impl/AbstractAlgorithm.hpp>
#include <algo/core/impl/DatabasesProvider.hpp>

#include <alignment/filter/api/IAlignmentFilter.hpp>
#include <alignment/filter/impl/AlignmentFilterXML.hpp>

#include <alignment/visitors/impl/XmlOutputVisitor.hpp>
#include <alignment/visitors/impl/NucleotidConversionVisitor.hpp>

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace misc;
using namespace database;
using namespace database::impl;

using namespace alignment::core;
using namespace alignment::filter;
using namespace alignment::filter::impl;
using namespace alignment::visitors::impl;

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
      _resultVisitor(0), _dbProvider(0)
{
    setProperties (properties);

    /** We may have to configure default parameters. */
    configure ();
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
    /** We create a configuration object for the provided program name (plastp, tplasn...) */
    setConfig (createConfiguration (_properties));

    /** We create a subject/query databases provider. */
    setDatabasesProvider (new DatabasesProvider (_config));

    /** We retrieve the type of algorithm (may be not set). */
    IProperty* algoProp = _properties->getProperty (STR_OPTION_ALGO_TYPE);
    bool inferType = (algoProp == 0);

    /** We may have a defined XML file URI. */
    IProperty* filterProp = _properties->getProperty (STR_OPTION_XML_FILTER_FILE);
    if (filterProp != 0)
    {
        setFilter (AlignmentFilterFactoryXML().createFilter (filterProp->getString()));
    }

    /** We create a visitor for visiting the resulting alignments. Note that we use only one visitor even if
     *  we have to run several algorithm; in such a case, the results are 'concatenated' by the same visitor. */
    setResultVisitor (_config->createResultVisitor ());

    u_int64_t  maxblocksize = 20*1000*1000;
    IProperty* maxBlockProp = _properties->getProperty(STR_OPTION_MAX_DATABASE_SIZE);
    if (maxBlockProp != 0)  {  maxblocksize = maxBlockProp->getInt();  }
    else  { _properties->add (0, STR_OPTION_MAX_DATABASE_SIZE, "%lld", maxblocksize); }

    /** We need to read the subject database to get its data size and the number of sequences.
     *  This information will be used for computing cutoffs for the query sequences. */
    IProperty* subjectProp = _properties->getProperty (STR_OPTION_SUBJECT_URI);
    if (subjectProp == 0)  {  subjectProp = _properties->add (0, STR_OPTION_SUBJECT_URI, "foo"); }
    if (subjectProp != 0)
    {
        setQuickSubjectDbReader (new FastaDatabaseQuickReader (subjectProp->value, inferType));
        _quickSubjectDbReader->read (maxblocksize);
    }

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
        else
        {
            /** We should not be there. Should throw an exception ?*/
        }
    }

    /** We build a list of uri for subject/query databases. */
    vector<pair<Range64,Range64> > uriList = buildUri (_quickSubjectDbReader, _quickQueryDbReader);

    /** We build a list of Parameters. We will launch the algorithm for each item of this list. */
    _parametersList = createParametersList (_config, _properties, uriList);
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

    result = new DefaultConfiguration (properties);

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
    /** We may launch an event with information about the two databases. */
    if (_quickSubjectDbReader != 0  &&  _quickQueryDbReader != 0)
    {
        this->notify (new DatabasesInformationEvent (_quickSubjectDbReader, _quickQueryDbReader) );
    }

    /** We iterate each parameters. */
    for (size_t i=0; _isRunning && i<_parametersList.size(); i++)
    {
        list<ICommand*> algorithms;

        /** We send a notification to potential listeners. */
        this->notify (new AlgorithmConfigurationEvent (_properties, i, _parametersList.size()));

        /** We create an Algorithm instance. */
        IAlgorithm* algo = this->createAlgorithm (
            _config,
            _quickSubjectDbReader,
            _parametersList[i],
            _filter,
            _resultVisitor,
            _dbProvider,
            _isRunning
        );
        if (algo == 0)  { continue; }

        /** We can register ourself to be notified by execution events. */
        algo->addObserver (this);

        /** We add this instance to the algorithms list. */
        algorithms.push_back (algo);

        /** We create a commands dispatcher. */
        ICommandDispatcher* dispatcher = new SerialCommandDispatcher ();
        LOCAL (dispatcher);

        /** We execute the algorithms through the dispatcher. */
        dispatcher->dispatchCommands (algorithms, 0);
    }

    /** We send a notification telling we are done (ie. current==total). */
    this->notify (new AlgorithmConfigurationEvent (_properties, _parametersList.size(), _parametersList.size()));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IAlgorithm* DefaultEnvironment::createAlgorithm (
    IConfiguration*             config,
    IDatabaseQuickReader*       reader,
    IParameters*                params,
    IAlignmentFilter*           filter,
    IAlignmentContainerVisitor* resultVisitor,
    IDatabasesProvider*         dbProvider,
    bool&                       isRunning
)
{
    IAlgorithm* result = 0;

    switch (params->algoKind)
    {
        case ENUM_PLASTP:
            result = new AlgorithmPlastp (
                config,
                reader,
                params,
                filter,
                resultVisitor,
                dbProvider,
                isRunning
            );
            break;

        case ENUM_TPLASTN:
            result = new AlgorithmTplastn (
                config,
                new AminoAcidDatabaseQuickReader (reader),
                params,
                filter,
                new NucleotidConversionVisitor (resultVisitor, Alignment::SUBJECT),
                dbProvider,
                isRunning
            );
            break;

        case ENUM_PLASTX:
            result = new AlgorithmPlastx (
                config,
                reader,
                params,
                filter,
                new NucleotidConversionVisitor (resultVisitor, Alignment::QUERY),
                dbProvider,
                isRunning
            );
            break;

        case ENUM_TPLASTX:
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
            if ( (prop = properties->getProperty (STR_OPTION_FILTER_QUERY)) != 0)           {  params->filterQuery          = prop->value.compare ("T") == 0;  }

            if ( (prop = properties->getProperty (STR_OPTION_UNGAP_NEIGHBOUR_LENGTH)) != 0) {  params->ungapNeighbourLength = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_UNGAP_SCORE_THRESHOLD)) != 0)  {  params->ungapScoreThreshold  = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_SMALLGAP_BAND_WITH)) != 0)     {  params->smallGapBandWidth    = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_SMALLGAP_THRESHOLD)) != 0)     {  params->smallGapThreshold    = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_OPEN_GAP_COST)) != 0)          {  params->openGapCost          = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_EXTEND_GAP_COST)) != 0)        {  params->extendGapCost        = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_EVALUE)) != 0)                 {  params->evalue               = atof (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_X_DROPOFF_GAPPED)) != 0)       {  params->XdroppofGap          = atoi (prop->value.c_str()); }
            if ( (prop = properties->getProperty (STR_OPTION_X_DROPOFF_FINAL)) != 0)        {  params->finalXdroppofGap     = atoi (prop->value.c_str()); }

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
                    ReadingFrame_e val = (ReadingFrame_e) (atoi(it.currentItem()) - 1);
                    params->strands.push_back (val);
                }
            }

            /** We set databases ranges. */
            params->subjectRange = uri[i].first;
            params->queryRange   = uri[i].second;

            result.push_back (params);
        }
    }

    return result;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
