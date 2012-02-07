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

/** \file AlignmentResultVisitors.hpp
 *  \brief A few implementations of IAlignmentResultVisitor interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALIGNMENT_RESULT_VISITORS_HPP_
#define _ALIGNMENT_RESULT_VISITORS_HPP_

/********************************************************************************/

#include <os/api/IFile.hpp>

#include <algo/align/api/IAlignmentResult.hpp>

#include <string>

/********************************************************************************/
namespace algo   {
namespace align {
/** \brief Implementation of concepts about alignments with gaps. */
namespace impl   {
/********************************************************************************/

/** \brief Visitor that limits the number of visited alignments.
 *
 * The PLAST algorithm may find a lot of alignments, which may be difficult to use
 * for the end user.
 *
 * An simple idea is to limit the number of alignments seen by the user.
 * This specific visitor takes a IAlignmentResultVisitor instance as reference and
 * limits the number of visited alignments for a given query.
 *
 * This is a Proxy Design Pattern implementation.
 *
 * Code Sample:
 * \code
 * void sample (IAlignmentResult* alignments)
 * {
 *      // we create a visitor for dumping the alignments into a file in a tabulated format
 *      IAlignmentResultVisitor* output = new AlignmentResultOutputTabulatedVisitor ("/tmp/output");
 *
 *      // we create a visitor for limiting to 5 the number of alignments per query
 *      IAlignmentResultVisitor* limit = new MaxHitsPerQueryAlignmentResultVisitor (output, 5);
 *
 *      // we visit the alignments with our proxy visitor
 *      alignments->accept (limit);
 * }
 * \endcode
 */
class MaxHitsPerQueryAlignmentResultVisitor : public IAlignmentResultVisitor
{
public:

    /** Constructor.
     * \param[in] ref : the visitor we want to limit the number of visited alignments.
     * \param[in] maxHitsPerQuery : max number of alignments per query
     */
    MaxHitsPerQueryAlignmentResultVisitor (IAlignmentResultVisitor* ref, size_t maxHitsPerQuery)
        : _ref(0), _maxHitsPerQuery (maxHitsPerQuery), _currentHitsNb(0)  {  setRef (ref);  }

    /** Destructor. */
    virtual ~MaxHitsPerQueryAlignmentResultVisitor()   { setRef (0); }

    /** \copydoc IAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence   (const database::ISequence* seq);

    /** \copydoc IAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq);

    /** \copydoc IAlignmentResultVisitor::visitAlignment */
    void visitAlignment       (const Alignment* align);

    /** \copydoc IAlignmentResultVisitor::finish */
    void finish ()  {}

protected:

    /** Proxied visitor. */
    IAlignmentResultVisitor* _ref;

    /** Smart setter for the _ref attribute. */
    void setRef (IAlignmentResultVisitor* ref)  { SP_SETATTR(ref); }

    size_t _maxHitsPerQuery;
    size_t _currentHitsNb;
};

/********************************************************************************/
/** \brief Abstract implementation of IAlignmentResultVisitor with file output management
 *
 * This is a factorization class that knows how to open/close/write a file. Note that we use
 * the operating system abstraction layer IFile for managing the file.
 *
 * It is intended to be subclassed by file dump visitors.
 */
class AbstractAlignmentResultVisitor : public IAlignmentResultVisitor
{
public:

    /** Constructor.
     * \param[in] uri : path of the file to be used.
     */
    AbstractAlignmentResultVisitor (const std::string& uri);

    /** Destructor. */
    virtual ~AbstractAlignmentResultVisitor();

    /** \copydoc IAlignmentResultVisitor::finish */
    void finish ()  {  if (_file)  { _file->flush(); } }

protected:

    /** Path of the file. */
    std::string _uri;

    /** The file. */
    os::IFile*  _file;

    /* */
    void safeprintf (const char* format, ...);
};

/********************************************************************************/
/** \brief Alignments file dump in tabulated format
 *
 * This visitor dumps alignments into a file in a tabulated format: each line in the
 * file is an alignment found in the subject and query databases.
 *
 * Each alignment line has the following columns:
 *      - query sequence short comment
 *      - subject sequence short comment
 *      - percentage of identity (nb of identical residues in both database / nb of characters of the alignment)
 *      - length of the alignment (ie. nb of characters)
 *      - number of non identical residues
 *      - number of gaps in the alignment (0 means that this is an ungap alignment)
 *      - start offset in the query sequence
 *      - stop offset in the query sequence
 *      - start offset in the subject sequence
 *      - stop offset in the subject sequence
 *      - expected value
 *      - bit score
 *
 * Example of tabulated output:
 * \code
 * ENSTTRP00000007202  sp|P48347|14310_ARATH   59.04   249  102   3   1     245   1     249   2e-78   292.4
 * ENSTTRP00000007202  sp|P93207|14310_SOLLC   60.08   238  95    4   3     235   9     246   7e-75   280.4
 * ENSTTRP00000001033  sp|P29673|APTE_DROME    28.40   81   58    0   313   393   369   449   0.018   40.8
 * \endcode
 */
class AlignmentResultOutputTabulatedVisitor : public AbstractAlignmentResultVisitor
{
public:

    /** \copydoc AbstractAlignmentResultVisitor::AbstractAlignmentResultVisitor */
    AlignmentResultOutputTabulatedVisitor (const std::string& uri);

    /** \copydoc AbstractAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence   (const database::ISequence* seq)  { _currentQuery   = seq;  }

    /** \copydoc AbstractAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq)  { _currentSubject = seq;  }

    /** \copydoc AbstractAlignmentResultVisitor::visitAlignment */
    void visitAlignment       (const Alignment* align);

protected:

    const database::ISequence* _currentQuery;
    const database::ISequence* _currentSubject;

    virtual void fillBuffer (const Alignment* align, char* buffer, size_t size);
};

/********************************************************************************/
/** \brief Alignments file dump in tabulated format
 *  Extends the AlignmentResultOutputTabulatedVisitor implementation by adding two columns:
 *      - query coverage
 *      - subject coverage
 */
class AlignmentResultOutputTabulatedExtendedVisitor : public AlignmentResultOutputTabulatedVisitor
{
public:

    /** \copydoc AbstractAlignmentResultVisitor::AbstractAlignmentResultVisitor */
    AlignmentResultOutputTabulatedExtendedVisitor (const std::string& uri)
        : AlignmentResultOutputTabulatedVisitor (uri)  {}

protected:

    void fillBuffer (const Alignment* align, char* buffer, size_t size);
};

/********************************************************************************/

/** \brief Alignments file dump in raw format
 *
 * This visitor dumps alignments into a file in a raw format.
 *
 * Example of raw output:
 * \code
 * QUERY SEQUENCE ENSTTRP00000007202 pep:novel scaffold:turTru1:scaffold_113855:32105:37173:1 gene:ENSTTRG00000007615
 *    SUBJECT SEQUENCE ENSTTRP00000007202 pep:novel scaffold:turTru1:scaffold_113855:32105:37173:1 gene:ENSTTRG00000007615
 *          ALIGNMENT [ALIGNMENT Q(0) 1 246   S(0) 1 246  len=246 nbgap=0 identity=246 nbMiss=0]
 *    SUBJECT SEQUENCE ENSTTRP00000003468 pep:novel genescaffold:turTru1:GeneScaffold_1110:25324:562263:-1 gene:ENSTTRG0000
 *          ALIGNMENT [ALIGNMENT Q(0) 27 81   S(982) 2627 2681  len=55 nbgap=0 identity=18 nbMiss=37]
 * \endcode
*/
class AlignmentResultRawDumpVisitor : public AbstractAlignmentResultVisitor
{
public:

    /** \copydoc AbstractAlignmentResultVisitor::AbstractAlignmentResultVisitor */
    AlignmentResultRawDumpVisitor (const std::string& uri) : AbstractAlignmentResultVisitor (uri) {}

    /** \copydoc AbstractAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence   (const database::ISequence* seq)  { safeprintf ("\nQUERY SEQUENCE %s\n", seq->comment);  }

    /** \copydoc AbstractAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq)  { safeprintf ("   SUBJECT SEQUENCE %s\n", seq->comment);  }

    /** \copydoc AbstractAlignmentResultVisitor::visitAlignment */
    void visitAlignment       (const Alignment* align)          { safeprintf ("         ALIGNMENT %s\n", align->toString().c_str() ); }
};

/********************************************************************************/

/** \brief Alignments file dump in XML format
 *
 * This visitor dumps alignments into a file in XML format.
 */
class AlignmentResultXmlDumpVisitor : public AbstractAlignmentResultVisitor
{
public:

    /** \copydoc AbstractAlignmentResultVisitor::AbstractAlignmentResultVisitor */
    AlignmentResultXmlDumpVisitor (const std::string& uri);

    /** Destructor. */
    virtual ~AlignmentResultXmlDumpVisitor ();

    /** \copydoc AbstractAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence   (const database::ISequence* seq);

    /** \copydoc AbstractAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq);

    /** \copydoc AbstractAlignmentResultVisitor::visitAlignment */
    void visitAlignment       (const Alignment* align);

private:
    void printline (size_t depth, const char* format, ...);

    const database::ISequence* _currentQuery;
    const database::ISequence* _currentSubject;

    u_int32_t _nbQuery;
    u_int32_t _nbSubject;
    u_int32_t _nbAlign;
};

/********************************************************************************/

/** \brief Visitor that fills a list holding all Alignment instances
 *
 * This visitor dumps alignments into a list
 */
class AlignmentResultListVisitor : public IAlignmentResultVisitor
{
public:

    /** \copydoc IAlignmentResultVisitor::AbstractAlignmentResultVisitor */
    AlignmentResultListVisitor (std::list<Alignment>& alignList) : _alignList(alignList)  {}

    /** Destructor. */
    virtual ~AlignmentResultListVisitor () {}

    /** \copydoc IAlignmentResultVisitor::visitQuerySequence */
    void visitQuerySequence   (const database::ISequence* seq) {}

    /** \copydoc IAlignmentResultVisitor::visitSubjectSequence */
    void visitSubjectSequence (const database::ISequence* seq) {}

    /** \copydoc IAlignmentResultVisitor::visitAlignment */
    void visitAlignment  (const Alignment* align)  { _alignList.push_back (*align);  }

    /** \copydoc IAlignmentResultVisitor::finish */
    void finish ()  {}

private:
    std::list<Alignment>& _alignList;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_RESULT_VISITORS_HPP_ */
