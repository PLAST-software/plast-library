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

#include <alignment/core/api/Alignment.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>

#include <math.h>

using namespace std;

using namespace os;
using namespace os::impl;

using namespace database;

using namespace dp;
using namespace dp::impl;

/********************************************************************************/
namespace alignment {
namespace core      {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
Alignment::Alignment (const std::string& source, const std::string& format)
{
    double identitiesPercent = 0.0;
    u_int16_t nbGaps = 0;
    misc::Range32 qryRange;
    misc::Range32 sbjRange;

    size_t idx = 0;
    TokenizerIterator tokenizer (source.c_str(), " \t");
    for (tokenizer.first(); !tokenizer.isDone(); tokenizer.next(), idx++)
    {
        char* token = tokenizer.currentItem();

        if (idx==0)
        {
        }

        else if (idx==1)
        {
        }

        else if (idx==2)   {  identitiesPercent = atof (token);     }
        else if (idx==3)   {  this->setLength   (atoi (token));     }
        else if (idx==4)   {  this->setNbMisses (atoi (token));     }
        else if (idx==5)   {  nbGaps = atoi (token);                }
        else if (idx==6)   {  qryRange.begin = atoi (token);        }
        else if (idx==7)   {  qryRange.end   = atoi (token);        }
        else if (idx==8)   {  sbjRange.begin = atoi (token);        }
        else if (idx==9)   {  sbjRange.end   = atoi (token);        }
        else if (idx==10)  {  this->setEvalue   (atof (token));     }
        else if (idx==11)  {  this->setBitScore (atof (token));     }
    }

    /** We read from a human readable file that counts range with a 1 starting position.
     *  We convert back to a 0 starting position. */
    qryRange.begin  --;
    qryRange.end    --;
    sbjRange.begin  --;
    sbjRange.end    --;

    /** We finalize the alignment. */
    this->setRange     (Alignment::QUERY,   qryRange);
    this->setRange     (Alignment::SUBJECT, sbjRange);
    this->setNbGaps    (nbGaps);
    this->setNbIdentities ((u_int32_t) round (identitiesPercent * this->getLength() / 100.0));
}

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/
