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

#include <alignment/visitors/impl/QueryReorderVisitor.hpp>
#include <alignment/core/impl/AlignmentContainerFactory.hpp>
#include <alignment/core/impl/BasicAlignmentContainer.hpp>

#include <alignment/visitors/impl/TabulatedOutputVisitor.hpp>

#include <algo/core/api/IAlgoEvents.hpp>

#include <os/impl/DefaultOsFactory.hpp>

#include <designpattern/impl/FileLineIterator.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>

#include <map>
#include <list>
#include <set>

#include <iostream>
#include <fstream>

#include <stdio.h>
#define DEBUG(a)     //a
#define VERBOSE(a)

using namespace std;

using namespace database;

using namespace misc;

using namespace dp;
using namespace dp::impl;

using namespace os;
using namespace os::impl;

using namespace alignment;
using namespace alignment::core;
using namespace alignment::core::impl;

using namespace algo::core;

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
class AlignmentContainerBuilderStream
{
private:

    /** */
    struct Entry
    {
        Entry (const string& cmt, size_t idx) : comment(cmt), index(idx) {}

        string comment;  size_t index;
    };

public:

    /** */
    AlignmentContainerBuilderStream (QueryReorderVisitor* queryVisitor)
		: _container(0), _nbQrySeq(0), _nbSbjSeq(), _queryVisitor(queryVisitor)
    {
        setContainer (_queryVisitor->_config->createGapAlignmentResult());
    }

    /** */
    ~AlignmentContainerBuilderStream ()  {  setContainer (0);  }


    /** */
    void putFirstLevelSequence (const ISequence* sequence)
    {
        VERBOSE (cout << "AlignmentContainerBuilderStream::putFirstLevelSequence"
            << "  _nbQrySeq='" << _nbQrySeq
            << "  comment='" << sequence->comment << "'"
            << "  length=" << sequence->getLength()
            << endl
        );

        /** We update the field of the placeholder sequence to be added in the alignments container. */
        _qrySeq.index   = _nbQrySeq++;
        _qrySeq.comment = sequence->comment;
        _qrySeq.length  = sequence->getLength();

        /** We add the placeholder into the container. */
        _container->insertFirstLevel (&_qrySeq);
    }

    /** */
    void put (char* line)
    {
        if (!line)  { return; }

        char c;

        stringstream ss (line);

        /** We read the first character which determines which kind of item we are going to deal with. */
        ss >> c;

        switch (c)

        {
            case 'Q':
            {
                /** We have to reset the index of subject sequence. */
                _sbjSeq.index = 0;

                manageSequence (c, _qryIdMap, _qrySeq, _nbQrySeq, ss);

                break;
            }

            case 'S':
            {
                manageSequence (c, _sbjIdMap, _sbjSeq, _nbSbjSeq, ss);

                break;
            }

            case 'H':
            {
                misc::Range32 qryRange;
                u_int32_t     qryNbGaps   = 0;
                u_int32_t     qryFrame    = 0;
                double        qryCoverage = 0;

                misc::Range32 sbjRange;
                u_int32_t     sbjNbGaps   = 0;
                u_int32_t     sbjFrame    = 0;
                double        sbjCoverage = 0;

                double    evalue   = 0;
                double    bitscore = 0;
                u_int32_t score    = 0;

                u_int32_t length = 0;

                u_int32_t nbIdentities = 0;
                u_int32_t nbPositives  = 0;
                u_int32_t nbMismatches = 0;

                core::Alignment a;

                ss  >> qryRange.begin  >> qryRange.end  >> qryNbGaps  >> qryFrame  >> qryCoverage
                    >> sbjRange.begin  >> sbjRange.end  >> sbjNbGaps  >> sbjFrame  >> sbjCoverage
                    >> evalue  >> bitscore  >> score  >> length
                    >> nbIdentities  >> nbPositives  >> nbMismatches;

                qryRange = qryRange.shift (-1);
                sbjRange = sbjRange.shift (-1);

                a.setRange  (alignment::core::Alignment::QUERY,   qryRange);
                a.setNbGaps (alignment::core::Alignment::QUERY,   qryNbGaps);
                a.setFrame  (alignment::core::Alignment::QUERY,   qryFrame);

                a.setRange  (alignment::core::Alignment::SUBJECT, sbjRange);
                a.setNbGaps (alignment::core::Alignment::SUBJECT, sbjNbGaps);
                a.setFrame  (alignment::core::Alignment::SUBJECT, sbjFrame);

                a.setEvalue   (evalue);
                a.setBitScore (bitscore);
                a.setScore    (score);
                a.setLength   (length);

                a.setNbIdentities (nbIdentities);
                a.setNbPositives  (nbPositives);
                a.setNbMisses     (nbMismatches);

                a.setSequence (alignment::core::Alignment::QUERY,   &_qrySeq);
                a.setSequence (alignment::core::Alignment::SUBJECT, &_sbjSeq);

                /** We can now insert the alignment into the container. */
                _container->insert (a, 0);

                VERBOSE (cout << "AlignmentContainerBuilderStream::put"
                    << "  NEW ALIGNMENT, now " << _container->getAlignmentsNumber()
                    << "  qry='" << _qrySeq.comment << "'"
                    << "  sbj='" << _sbjSeq.comment << "'"
                    << "  " << a.toString()
                    << endl
                );

                break;
            }

            default:
            {
                break;
            }
        }
    }

    /** */
    void clear ()
    {
        setContainer (_queryVisitor->_config->createGapAlignmentResult());
        _qryIdMap.clear ();
        _sbjIdMap.clear ();
        _qrySeq.index = _sbjSeq.index = 0;
        _nbQrySeq     = _nbSbjSeq     = 0;
    }

    /** */
    IAlignmentContainer* getContainer ()  { return _container; }

    /** */
    u_int64_t getAlignmentsNumber ()  { return (_container ? _container->getAlignmentsNumber() : 0); }

    /** */
    void dump (QueryReorderVisitor* queryVisitor, u_int32_t threshold=0)
    {
        VERBOSE (cout << "AlignmentContainerBuilderStream::dump"
            << "  alignNb="         << getAlignmentsNumber()
            << "  nbFirstLevel="    << _container->getFirstLevelNumber()
            << "  threshold="       << threshold << endl
        );

        if (getAlignmentsNumber() >= threshold  ||  threshold==0)
        {
            /** We notify if we found some query sequences (with potentially no hits) or alignments. */
            if (_container && (_container->getAlignmentsNumber()>0  || _container->getFirstLevelNumber()>0) )
            {
                VERBOSE (cout << "AlignmentContainerBuilderStream::dump    accept final visitor " << queryVisitor->getFinalVisitor() << endl);

				/** We must first shrink the container since we may have join overlapping alignments. */
			    _container->shrink ();

                /** We accept the provided visitor. */
                _container->accept (queryVisitor->getFinalVisitor());

                /** We send a notification to potential listeners. */
                queryVisitor->notify (new AlignmentsContainerEvent (_container));
                VERBOSE (cout << "AlignmentContainerBuilderStream::dump    notifying..."
                    << "  alignNb="  << _container->getAlignmentsNumber()
                    << "  queryNb="  << _container->getFirstLevelNumber()
                    << endl
                );
            }

            /** We reset the alignment builder for the other alignments to be parsed from the temporary file. */
            VERBOSE (cout << "AlignmentContainerBuilderStream::dump    clearing content..." << endl);
            clear ();
        }
    }

private:

    /** */
    void manageSequence (char kind, map<string,Entry>& theMap, ISequence& seq, size_t& nbSeq, stringstream& ss)
    {
        char  commentBuffer[4*1024];

        /** We read the sequence length from the stream. */
        ss >> seq.length;

        /** We read the remaining of the comment line from the stream. */
        ss.getline (commentBuffer, sizeof(commentBuffer)-1);

        VERBOSE (cout << endl);

        VERBOSE (cout << "AlignmentContainerBuilderStream::manageSequence "
            << "  kind="    << kind
            << "  length="  << seq.length
            << "  buffer='" << commentBuffer << "'"
            << endl
        );

        char* commentBufferCursor = commentBuffer;

        /** We skip potential leading spaces. */
        while (*commentBufferCursor==' ')  { commentBufferCursor++; }

        /** We retrieve the sequence id from the full comment. */
        char* endIdCursor = strchr (commentBufferCursor, ' ');

        bool transientChange = false;

        if (endIdCursor != 0)
        {
            transientChange = true;
            *endIdCursor = 0;
        }
        else
        {
            transientChange = false;
            endIdCursor = commentBufferCursor;
        }

        VERBOSE (cout << "AlignmentContainerBuilderStream::manageSequence "
            << "  transient=" << transientChange
            << "  cursor='"   << commentBufferCursor << "'"
            << endl
        );

        /** We look whether the read id is already known. */
        map<string,Entry>::iterator lookup = theMap.find (commentBufferCursor);

        if (transientChange)  { *endIdCursor = ' '; }

        if (lookup != theMap.end())
        {
            seq.comment = lookup->second.comment.c_str();
            seq.index   = lookup->second.index;
        }
        else
        {
            pair<map<string,Entry>::iterator,bool> ret = theMap.insert (
                pair<string,Entry> (
                    commentBufferCursor,
                    Entry(commentBufferCursor, nbSeq++)
                )
            );

            seq.comment = ret.first->second.comment.c_str();
            seq.index   = ret.first->second.index;
        }

        VERBOSE (cout << "AlignmentContainerBuilderStream::manageSequence "
            << "  comment='" << seq.comment << "'"
            << "  index=" << seq.index
            << endl
        );

    }

    IAlignmentContainer* _container;
    void setContainer (IAlignmentContainer* container)  { SP_SETATTR(container); }

    ISequence _qrySeq;
    ISequence _sbjSeq;

    map<string,Entry> _qryIdMap;
    map<string,Entry> _sbjIdMap;

    size_t _nbQrySeq;
    size_t _nbSbjSeq;

    QueryReorderVisitor* _queryVisitor;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
QueryReorderVisitor::QueryReorderVisitor (
    algo::core::IConfiguration*         config,
    const std::string&                  outputUri,
    core::IAlignmentContainerVisitor*   realVisitor,
    core::IAlignmentContainerVisitor*   finalVisitor,
    database::IDatabaseQuickReader*     qryReader,
    u_int32_t                           nbAlignmentsThreshold
)
    :  AlignmentsProxyVisitor(realVisitor),
       _config (config),
       _outputUri(outputUri),
       _finalVisitor(0),
       _qryReader(0),
       _prevPos(0), _newPos(0),
       _nbAlignmentsThreshold(nbAlignmentsThreshold)
{
    /** We keep a reference on the provided visitors. */
    setFinalVisitor (finalVisitor);

    /** We keep a reference on the query reader. */
    setQryReader (qryReader);

    /** We create the queries indexes file. */
    _indexesFile.open (getIndexesFileUri().c_str(), ios::out | ios::in | ios::trunc);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
QueryReorderVisitor::~QueryReorderVisitor ()
{
    /** We clean up resources. */
    setFinalVisitor (0);
    setQryReader    (0);

    /** Important ! Normally, the reference should be released by the destructor of
     * the parent class (which should occur just after this destructor call).
     * However, we need to remove the temporary file (ie. the file of the referenced
     * visitor), and we need to have this file closed before doing the remove. A way
     * to achieve this is to anticipate the setRef(0) of the parent class destructor
     * with the consequence of closing the file.
     */
    setRef (0);

    /** We close the indexes file and delete it. */
    _indexesFile.close();
    remove (getIndexesFileUri().c_str());

    /** We delete the temporary file.
     *  Note that we should be sure that it has been closed before doing this. */
    remove (getTmpFileUri().c_str());
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void QueryReorderVisitor::visitQuerySequence (const database::ISequence* seq, const misc::ProgressInfo& progress)
{
    /** We dump the index of the previous query. */
    dumpIndex ();

    /** We call the delegate. */
    _ref->visitQuerySequence (seq, progress);

    /** We retrieve the query sequence identifier. */
    if (seq)  {  seq->retrieveId (_queryId, sizeof(_queryId));  }

    /** We keep the previous position. */
    _prevPos = _newPos;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void QueryReorderVisitor::dumpIndex (void)
{
    /** We get the new cursor location in the output stream. */
    _newPos = _ref->getPosition ();

    if (_newPos > _prevPos &&  _indexesFile.is_open())
    {
        char sep = ' ';

        _indexesFile << _queryId << sep << _prevPos << sep << (_newPos-1) << endl;
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
void QueryReorderVisitor::finalize (void)
{
    DEBUG (cout << "QueryReorderVisitor::finalize   uri=" << _qryReader->getUri() << endl);

    /** We dump the last index. */
    dumpIndex();

    /** We force to flush the indexes file. */
    if (_indexesFile.is_open())  { _indexesFile.flush();  }

    /** We create a file iterator on the temporary file. Note that we don't give here any range, it will be
     *  updated during the loop over all ranges for a given query.
     *  IMPORTANT... use a big line size since a line can be a header + a fasta comment
     */
    FileLineIterator lineIt (getTmpFileUri().c_str());

    /** We retrieve the partition of the query database as a vector of file offsets. */
    vector<u_int64_t>& qryOffsets = _qryReader->getOffsets();

    for (size_t j=0; j<qryOffsets.size()-1; j++)
    {
        /** We get the current range to be read in the full query database. */
        Range64 range (qryOffsets[j], qryOffsets[j+1]-1);

        /** We create the query database for the given range. Note we are interested mainly by sequences
         *  identifiers.
         */
        ISequenceDatabase* db = _config->createDatabase (_qryReader->getUri(), range, false, 0);
        LOCAL (db);

        DEBUG (cout << "-----------------------------------------------------------------------------" << endl);

        /** We retrieve the set of sequences identifiers from the current query database. */
        set<string> queriesId;
        db->retrieveSequencesIdentifiers (queriesId);

        DEBUG (cout << "QueryReorderVisitor::finalize   db created for range " << range
            << "  nbSeq="  << db->getSequencesNumber()
            << "  nbIds="  << queriesId.size()
            << "  first="  << *(queriesId.begin())
            << endl
        );

        /** We need to build a list of valid indexes ranges for the current database. */
        map<string,list<Range64> > indexes;

        /** We need to reset the index file status (for instance for clearing a EOF status from
         *  the previous loop. */
        _indexesFile.clear ();

        /** We look for all indexes ranges for each sequence of the current query database. */
        for (_indexesFile.seekg(0, ios::beg);  _indexesFile.eof() == false;  )
        {
            string    queryId;
            Range64   range;

            _indexesFile >> queryId >> range.begin >> range.end;

            /** We look whether this index belongs to a sequence of the current database. */
            set<string>::iterator lookup = queriesId.find (queryId);

            /** We keep the entry if it matches a known sequence. */
            if (lookup != queriesId.end())  {  (indexes[queryId]).push_back (range);  }
        }

        DEBUG (cout << "QueryReorderVisitor::finalize   " << indexes.size() << " indexes found for the current database" << endl;)

        AlignmentContainerBuilderStream alignStream (this);

        /** We loop over the sequences of the current database. */
        ISequenceIterator* seqIterator = db->createSequenceIterator ();
        LOCAL (seqIterator);

        for (seqIterator->first(); !seqIterator->isDone(); seqIterator->next ())
        {
            /** Shortcut. */
            const ISequence* seq = seqIterator->currentItem();

            /** We retrieve the sequence identifier. */
            char id[1024];
            size_t len = seq->retrieveId (id, sizeof(id));

            /** We check that we got a valid id. */
            if (len == 0)  { continue; }

            /** Shortcut. */
            list<Range64>& l = indexes[id];

            DEBUG (cout << endl);
            DEBUG (cout << "QueryReorderVisitor::finalize   retrieveId : l.size=" << l.size() << "  id='" << id << "'" << endl);

            if (! l.empty())
            {
                bool firstEntry = true;

                for (list<Range64>::iterator entriesIt=l.begin(); entriesIt != l.end(); entriesIt++, firstEntry=false)
                {
                    /** We update the range to be read within the file. */
                    lineIt.setRange (entriesIt->begin);

                    DEBUG (cout <<  "QueryId=" << id
                        << "  l.size=" << l.size()
                        << "  range=" << (*entriesIt)
                        << "  line=" << lineIt.currentItem()
                        << endl
                    );

                    CHECK_BLOCK_BEGIN
                        if (lineIt.currentItem()[0] != 'Q')  {  throw "QueryReorderVisitor::finalize => FOUND BAD CHARACTER"; }
                    CHECK_BLOCK_END

                    /** We loop each file line and parse it through an alignment builder. */
                    bool stop = false;
                    for (bool firstLine=true; !stop && !lineIt.isDone(); lineIt.next(), firstLine=false)
                    {
                        const char* current = lineIt.currentItem();

                        if (*current == 'Q')
                        {
                            if (firstLine)
                            {
                                if (!firstEntry)
                                {
                                    /** Note the trick: we get rid of redundant 'Q' headed lines (just keep the first). */
                                    continue;
                                }
                            }
                            else
                            {
                                /** We got a query that is not the currently processed, which means
                                 * that we found all required alignments for the current query. */
                                stop=true;  continue;
                            }
                        }

                        alignStream.put (lineIt.currentItem());
                    }

                }  /* end of for (list<Entry>::iterator entriesIt=l.begin(); */
            }
            else
            {
                /** We have no hit for the current sequence. We just add a placeholder for this sequence. */
                alignStream.putFirstLevelSequence (seq);
            }

            /** After we reach some alignments number, we make it visited by the final visitor. */
            alignStream.dump (this, _nbAlignmentsThreshold);

        }  /* end of for (seqIterator->first(); */

        /** We get the alignments container. */
        alignStream.dump (this, 0);
    }
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
