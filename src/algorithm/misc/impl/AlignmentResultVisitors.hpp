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

#ifndef _ALIGNMENT_RESULT_VISITORS_HPP_
#define _ALIGNMENT_RESULT_VISITORS_HPP_

/********************************************************************************/

#include "IAlignementResult.hpp"

#include <string>

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** Proxy implementation that limits the number of visits. */
class MaxHitsPerQueryAlignmentResultVisitor : public AlignmentResultVisitor
{
public:

    MaxHitsPerQueryAlignmentResultVisitor (AlignmentResultVisitor* ref, size_t maxHitsPerQuery)
        : _ref(0), _maxHitsPerQuery (maxHitsPerQuery), _currentHitsNb(0)  {  setRef (ref);  }
    virtual ~MaxHitsPerQueryAlignmentResultVisitor()   { setRef (0); }

    void visitQuerySequence   (const database::ISequence* seq);
    void visitSubjectSequence (const database::ISequence* seq);
    void visitAlignment       (const Alignment* align);

protected:
    AlignmentResultVisitor* _ref;
    void setRef (AlignmentResultVisitor* ref)  { SP_SETATTR(ref); }

    size_t _maxHitsPerQuery;
    size_t _currentHitsNb;
};

/********************************************************************************/
class AbstractAlignmentResultVisitor : public AlignmentResultVisitor
{
public:

    AbstractAlignmentResultVisitor (const std::string& uri);
    virtual ~AbstractAlignmentResultVisitor();

protected:
    std::string _uri;
    FILE*       _file;

    void safeprintf (const char* format, ...);
};

/********************************************************************************/

class AlignmentResultOutputTabulatedVisitor : public AbstractAlignmentResultVisitor
{
public:
    AlignmentResultOutputTabulatedVisitor (const std::string& uri);

    void visitQuerySequence   (const database::ISequence* seq)  { _currentQuery   = seq;  }
    void visitSubjectSequence (const database::ISequence* seq)  { _currentSubject = seq;  }

    void visitAlignment       (const Alignment* align);

private:

    const database::ISequence* _currentQuery;
    const database::ISequence* _currentSubject;
};

/********************************************************************************/

class AlignmentResultRawDumpVisitor : public AbstractAlignmentResultVisitor
{
public:

    AlignmentResultRawDumpVisitor (const std::string& uri) : AbstractAlignmentResultVisitor (uri) {}

    void visitQuerySequence   (const database::ISequence* seq)  { safeprintf ("\nQUERY SEQUENCE %s\n", seq->comment);  }
    void visitSubjectSequence (const database::ISequence* seq)  { safeprintf ("   SUBJECT SEQUENCE %s\n", seq->comment);  }
    void visitAlignment       (const Alignment* align)          { safeprintf ("         ALIGNMENT %s\n", align->toString().c_str() ); }
};

/********************************************************************************/

class AlignmentResultXmlDumpVisitor : public AbstractAlignmentResultVisitor
{
public:

    AlignmentResultXmlDumpVisitor (const std::string& uri);
    virtual ~AlignmentResultXmlDumpVisitor ();

    void visitQuerySequence   (const database::ISequence* seq);
    void visitSubjectSequence (const database::ISequence* seq);
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
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_RESULT_VISITORS_HPP_ */
