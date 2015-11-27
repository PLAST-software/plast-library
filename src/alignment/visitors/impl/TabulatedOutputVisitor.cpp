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

#include <stdio.h>
#define DEBUG(a)  //printf a

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
TabulatedOutputVisitor::TabulatedOutputVisitor (const std::string& uri)
    : _file(0), _currentQuery(0), _currentSubject(0), _sep('\t')
{
    _file = fopen (uri.c_str(), "w");
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
TabulatedOutputVisitor::~TabulatedOutputVisitor ()
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
void TabulatedOutputVisitor::visitAlignment (Alignment* align, const misc::ProgressInfo& progress)
{
    DEBUG (("TabulatedOutputVisitor::visitAlignment  align=%p\n", align));

    /** We dump the line. */
    dumpLine (align);

    /** We add a return line. */
    fprintf (_file, "\n");
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void TabulatedOutputVisitor::dumpLine (core::Alignment* align)
{
    char* locate = 0;  // used for truncating comments to the first space character.

    DEBUG (("TabulatedOutputVisitor::dumpLine  align=%p\n", align));

    const size_t queryNameMaxSize = 128;
    char queryName[queryNameMaxSize];

    snprintf (queryName, queryNameMaxSize, "%s", _currentQuery->comment);
    if ( (locate = database::ISequence::searchIdSeparator (queryName)) != 0)  { *locate = 0; }

    char subjectName[128] = "";
    //snprintf (subjectName, sizeof(subjectName), "%s", _currentSubject->comment);
    //snprintf (subjectName, sizeof(subjectName), "TEST");
    snprintf (subjectName, sizeof(subjectName), "%s", _currentSubject->getComment().c_str());

    if ( (locate = database::ISequence::searchIdSeparator (subjectName)) != 0)  { *locate = 0; }

    char evalueStr[32];
    double ev = align->getEvalue();

         if (ev < 1.0e-99)  {   sprintf (evalueStr, "%2.0le", ev);  }
    else if (ev < 0.0009)   {   sprintf (evalueStr, "%3.0le", ev);  }
    else if (ev < 0.1)      {   sprintf (evalueStr, "%4.3lf", ev);  }
    else if (ev < 1.0)      {   sprintf (evalueStr, "%2.1lf", ev);  }
    else if (ev < 10.0)     {   sprintf (evalueStr, "%2.1lf", ev);  }
    else                    {   sprintf (evalueStr, "%5.0lf", ev);  }

    fprintf (_file,  "%s%c%s%c%.2f%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%s%c%.1lf",
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
void TabulatedOutputExtendedVisitor::dumpLine (core::Alignment* align)
{
    /** We first call the parent method. */
    TabulatedOutputVisitor::dumpLine (align);

    fprintf (_file, "%c%d %c%d%c%d%c%d%c%.1f%c%d %c%d%c%d%c%d%c%.1f%c%d",

        _sep,  align->getNbPositives(),

        _sep,  align->getSequence (Alignment::QUERY)->getLength(),
        _sep,  align->getFrame    (Alignment::QUERY),
        _sep,  align->isTranslated(Alignment::QUERY) ? 1 : 0,
        _sep,  align->getCoverage (Alignment::QUERY)*100.0,
        _sep,  align->getNbGaps   (Alignment::QUERY),

        _sep,  align->getSequence (Alignment::SUBJECT)->getLength(),
        _sep,  align->getFrame    (Alignment::SUBJECT),
        _sep,  align->isTranslated(Alignment::SUBJECT) ? 1 : 0,
        _sep,  align->getCoverage (Alignment::QUERY)*100.0,
        _sep,  align->getNbGaps   (Alignment::SUBJECT)
    );
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
