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

#include <alignment/visitors/impl/TabulatedOutputVisitor.hpp>
#include <alignment/core/api/Alignment.hpp>

using namespace std;
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
TabulatedOutputVisitor::TabulatedOutputVisitor (std::ostream* ostream)
    : OstreamVisitor (ostream), _currentQuery(0), _currentSubject(0), _sep('\t')
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
TabulatedOutputVisitor::TabulatedOutputVisitor (const std::string& uri)
    : OstreamVisitor (uri), _currentQuery(0), _currentSubject(0), _sep('\t')
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void TabulatedOutputVisitor::visitAlignment (Alignment* align, const misc::ProgressInfo& progress)
{
    DEBUG (("TabulatedOutputVisitor::visitAlignment  align=%p\n", align));

    /** We fill the buffer. */
    char buffer[1024];
    fillBuffer (align, buffer, sizeof(buffer));

    /** We dump the buffer to the outputstream. */
    getStream() << buffer << endl;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void TabulatedOutputVisitor::fillBuffer (core::Alignment* align, char* buffer, size_t size)
{
    char* locate = 0;  // used for truncating comments to the first space character.

    DEBUG (("TabulatedOutputVisitor::fillBuffer  align=%p\n", align));

    if (buffer == 0  ||  _currentQuery==0  ||  _currentSubject==0) { return ; }

    char queryName[32];
    snprintf (queryName,   sizeof(queryName),   "%s", _currentQuery->comment);
    if ( (locate = strchr (queryName, ' ')) != 0)  { *locate = 0; }

    char subjectName[32];
    snprintf (subjectName, sizeof(subjectName), "%s", _currentSubject->comment);
    if ( (locate = strchr (subjectName, ' ')) != 0)  { *locate = 0; }

    char evalueStr[32];
    double ev = align->getEvalue();

         if (ev < 1.0e-99)  {   sprintf (evalueStr, "%2.0le", ev);  }
    else if (ev < 0.0009)   {   sprintf (evalueStr, "%3.0le", ev);  }
    else if (ev < 0.1)      {   sprintf (evalueStr, "%4.3lf", ev);  }
    else if (ev < 1.0)      {   sprintf (evalueStr, "%2.1lf", ev);  }
    else if (ev < 10.0)     {   sprintf (evalueStr, "%2.1lf", ev);  }
    else                    {   sprintf (evalueStr, "%5.0lf", ev);  }

    snprintf (buffer, size, "%s%c%s%c%.2f%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%s%c%.1lf",
        queryName,                              _sep,
        subjectName,                            _sep,
        (100.0* align->getPercentIdentities()), _sep,
        align->getLength(),                     _sep,
        align->getNbMisses(),                   _sep,
        align->getNbGaps(),                     _sep,
        align->getRange(Alignment::QUERY).begin   + 1,     _sep,  // add +1 because real people count from 1...
        align->getRange(Alignment::QUERY).end     + 1,     _sep,
        align->getRange(Alignment::SUBJECT).begin + 1,     _sep,
        align->getRange(Alignment::SUBJECT).end   + 1,     _sep,
        evalueStr,                              _sep,
        align->getBitScore()
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
void TabulatedOutputExtendedVisitor::fillBuffer (core::Alignment* align, char* buffer, size_t size)
{
    /** We first call the parent method. */
    TabulatedOutputVisitor::fillBuffer (align, buffer, size);

    /** We format extra information. */
    char extra[128];
    snprintf (extra, sizeof(extra), "%c%.1lf%c%.1lf",
        _sep,
        100.0 * align->getCoverage(Alignment::QUERY),
        _sep,
        100.0 * align->getCoverage(Alignment::SUBJECT)
    );

    /** We add some information at the end of the buffer. */
    strcat (buffer, extra);
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
