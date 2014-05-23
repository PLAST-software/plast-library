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

#include <alignment/visitors/impl/XmlOutputVisitor.hpp>

#include <database/api/ISequenceDatabase.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace database;
using namespace alignment;
using namespace alignment::core;

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
XmlOutputVisitor::XmlOutputVisitor (std::ostream* ostream)
    : OstreamVisitor(ostream), _nbQuery (0), _nbSubject (0), _nbAlign(0)
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
XmlOutputVisitor::XmlOutputVisitor (const std::string& uri)
    : OstreamVisitor(uri), _nbQuery (0), _nbSubject (0), _nbAlign(0)
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
XmlOutputVisitor::~XmlOutputVisitor ()
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
void XmlOutputVisitor::visitQuerySequence (
    const database::ISequence* seq,
    const misc::ProgressInfo& progress
)
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
void XmlOutputVisitor::visitSubjectSequence (
    const database::ISequence* seq,
    const misc::ProgressInfo& progress
)
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
    printline (5, "<Hit_def>%s</Hit_def>", _currentSubject->getComment(_currentSubject->comment).c_str());
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
void XmlOutputVisitor::visitAlignment (
    core::Alignment* align,
    const misc::ProgressInfo& progress
)
{
    _nbAlign++;

    printline (6, "<Hsp>");

    printline (7, "<Hsp_num>%d</Hsp_num>",                  _nbAlign);
    printline (7, "<Hsp_bit-score>%g</Hsp_bit-score>",      align->getBitScore());
    printline (7, "<Hsp_evalue>%g</Hsp_evalue>",            align->getEvalue());
    printline (7, "<Hsp_query-from>%d</Hsp_query-from>",    align->getRange(Alignment::QUERY).begin + 1);
    printline (7, "<Hsp_query-to>%d</Hsp_query-to>",        align->getRange(Alignment::QUERY).end   + 1);
    printline (7, "<Hsp_hit-from>%d</Hsp_hit-from>",        align->getRange(Alignment::SUBJECT).begin + 1);
    printline (7, "<Hsp_hit-to>%d</Hsp_hit-to>",            align->getRange(Alignment::SUBJECT).end   + 1);
    printline (7, "<Hsp_identity>%d</Hsp_identity>",        align->getNbIdentities());
    printline (7, "<Hsp_gaps>%d</Hsp_gaps>",                align->getNbGaps());
    printline (7, "<Hsp_align-len>%d</Hsp_align-len>",      align->getLength());
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
void XmlOutputVisitor::printline (size_t depth, const char* format, ...)
{
    /** We dump the indentation. */
    for (size_t i=0; i<depth; i++)   { getStream() << "    ";  }

    char buffer[1024];

    va_list va;
    va_start (va, format);
    vsprintf (buffer, format, va);
    va_end (va);

    getStream() << buffer << endl;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
