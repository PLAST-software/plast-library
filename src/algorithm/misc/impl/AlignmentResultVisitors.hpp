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
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_RESULT_VISITORS_HPP_ */
