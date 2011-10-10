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



/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
