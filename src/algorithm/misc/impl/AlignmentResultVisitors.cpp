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

#include "AlignmentResultVisitors.hpp"

using namespace std;
using namespace dp;
using namespace database;
using namespace seed;
using namespace indexation;

#include <stdarg.h>

#include <stdio.h>
#define DEBUG(a)  //printf a

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void MaxHitsPerQueryAlignmentResultVisitor::visitQuerySequence   (const database::ISequence* seq)
{
    if (_ref)  { _ref->visitQuerySequence (seq); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void MaxHitsPerQueryAlignmentResultVisitor::visitSubjectSequence (const database::ISequence* seq)
{
    _currentHitsNb = 0;

    if (_ref)  { _ref->visitSubjectSequence (seq); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void MaxHitsPerQueryAlignmentResultVisitor::visitAlignment (const Alignment* align)
{
    _currentHitsNb ++;

    if (_currentHitsNb <= _maxHitsPerQuery  &&  _ref)  { _ref->visitAlignment (align); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractAlignmentResultVisitor::AbstractAlignmentResultVisitor (const std::string& uri)
    : _uri(uri), _file(0)
{
    _file = fopen (_uri.c_str(), "w");
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractAlignmentResultVisitor::~AbstractAlignmentResultVisitor ()
{
    if (_file)  { fclose (_file); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AbstractAlignmentResultVisitor::safeprintf (const char* format, ...)
{
    if (_file != 0)
    {
        va_list va;
        va_start (va, format);
        vfprintf (_file, format, va);
        va_end (va);
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
AlignmentResultOutputTabulatedVisitor::AlignmentResultOutputTabulatedVisitor (const string& uri)
    : AbstractAlignmentResultVisitor (uri), _currentQuery(0), _currentSubject(0)
{
    DEBUG (("AlignmentResultOutputTabulatedVisitor::AlignmentResultOutputTabulatedVisitor   uri='%s'\n",
        uri.c_str()
    ));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlignmentResultOutputTabulatedVisitor::visitAlignment (const Alignment* align)
{
    char* locate = 0;  // used for truncating comments to the first space character.

    DEBUG (("AlignmentResultOutputTabulatedVisitor::visitAlignment  align=%p\n", align));

    if (_file == 0  ||  _currentQuery==0  ||  _currentSubject==0) { return ; }

    char queryName[32];
    snprintf (queryName,   sizeof(queryName),   "%s", _currentQuery->comment);
    if ( (locate = strchr (queryName, ' ')) != 0)  { *locate = 0; }

    char subjectName[32];
    snprintf (subjectName, sizeof(subjectName), "%s", _currentSubject->comment);
    if ( (locate = strchr (subjectName, ' ')) != 0)  { *locate = 0; }

    char evalueStr[32];
    double ev = align->_evalue;

         if (ev < 1.0e-99)  {   sprintf (evalueStr, "%2.0le", ev);  }
    else if (ev < 0.0009)   {   sprintf (evalueStr, "%3.0le", ev);  }
    else if (ev < 0.1)      {   sprintf (evalueStr, "%4.3lf", ev);  }
    else if (ev < 1.0)      {   sprintf (evalueStr, "%2.1lf", ev);  }
    else if (ev < 10.0)     {   sprintf (evalueStr, "%2.1lf", ev);  }
    else                    {   sprintf (evalueStr, "%5.0lf", ev);  }

    safeprintf ("%s\t%s\t%.2f\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%.1lf\n",
        queryName, subjectName,
        (100.0* (float)align->_identity / (float) align->_length),
        align->_length,
        align->_nbMis,
        align->_nbGap,
        align->_queryStartInSeq+1,   align->_queryEndInSeq+1,       // add +1 because real people count from 1...
        align->_subjectStartInSeq+1, align->_subjectEndInSeq+1,
        evalueStr,
        align->_bitscore
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
AlignmentResultXmlDumpVisitor::AlignmentResultXmlDumpVisitor (const std::string& uri)
    : AbstractAlignmentResultVisitor (uri),
      _nbQuery (0), _nbSubject (0), _nbAlign(0)

{
    printline (0, "<?xml version=\"1.0\" ?>");
    printline (0, "<BlastOutput>");
    printline (1, "<BlastOutput_iterations>");
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlignmentResultXmlDumpVisitor::~AlignmentResultXmlDumpVisitor ()
{
    if (_nbSubject > 0)
    {
        printline (5, "</Hit_hsps>");
        printline (4, "</Hit>");
    }

    if (_nbQuery > 0)
    {
        printline (3, "</Iteration_hits>");
        printline (2, "</Iteration>");
    }

    printline (1, "</BlastOutput_iterations>");
    printline (0, "</BlastOutput>");
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlignmentResultXmlDumpVisitor::visitQuerySequence (const database::ISequence* seq)
{
    if (_nbSubject > 0)
    {
        printline (5, "</Hit_hsps>");
        printline (4, "</Hit>");
    }

    if (_nbQuery > 0)
    {
        printline (3, "</Iteration_hits>");
        printline (2, "</Iteration>");
    }

    _currentQuery = seq;
    _nbQuery++;
    _nbSubject = 0;

    printline (2, "<Iteration>");
    printline (3, "<Iteration_iter-num>%d</Iteration_iter-num>", _nbQuery);
    printline (3, "<Iteration_query-def>%s</Iteration_query-def>", _currentQuery->comment);
    printline (3, "<Iteration_query-len>%d</Iteration_query-len>", _currentQuery->data.letters.size);
    printline (3, "<Iteration_hits>");
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlignmentResultXmlDumpVisitor::visitSubjectSequence (const database::ISequence* seq)
{
    if (_nbSubject > 0)
    {
        printline (5, "</Hit_hsps>");
        printline (4, "</Hit>");
    }
    _currentSubject = seq;
    _nbSubject++;
    _nbAlign = 0;

    printline (4, "<Hit>");
    printline (5, "<Hit_num>%d</Hit_num>", _nbSubject);
    printline (5, "<Hit_def>%s</Hit_def>", _currentSubject->comment);
    printline (5, "<Hit_len>%d</Hit_len>", _currentSubject->data.letters.size);
    printline (5, "<Hit_hsps>");
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlignmentResultXmlDumpVisitor::visitAlignment (const Alignment* align)
{
    _nbAlign++;

    printline (6, "<Hsp>");

    printline (7, "<Hsp_num>%d</Hsp_num>",                  _nbAlign);
    printline (7, "<Hsp_bit-score>%g</Hsp_bit-score>",      align->_bitscore);
    printline (7, "<Hsp_evalue>%g</Hsp_evalue>",            align->_evalue);
    printline (7, "<Hsp_query-from>%d</Hsp_query-from>",    align->_queryStartInSeq   + 1);
    printline (7, "<Hsp_query-to>%d</Hsp_query-to>",        align->_queryEndInSeq     + 1);
    printline (7, "<Hsp_hit-from>%d</Hsp_hit-from>",        align->_subjectStartInSeq + 1);
    printline (7, "<Hsp_hit-to>%d</Hsp_hit-to>",            align->_subjectEndInSeq   + 1);
    printline (7, "<Hsp_identity>%d</Hsp_identity>",        align->_identity);
    printline (7, "<Hsp_gaps>%d</Hsp_gaps>",                align->_nbGap);
    printline (7, "<Hsp_align-len>%d</Hsp_align-len>",      align->_length);
    printline (7, "<Hsp_hseq/>");

    printline (6, "</Hsp>");
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlignmentResultXmlDumpVisitor::printline (size_t depth, const char* format, ...)
{
    if (_file != 0)
    {
        /** We dump the indentation. */
        for (size_t i=0; i<depth; i++)   { fprintf (_file, "    ");  }

        va_list va;
        va_start (va, format);
        vfprintf (_file, format, va);
        fprintf  (_file, "\n");
        va_end (va);
    }
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
