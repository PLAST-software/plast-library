/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
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
    : _length(0), _evalue(0), _bitscore(0), _score(0), _nbIdentities(0), _nbPositives(0), _nbMisses(0), _extraInfo(0)
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

        else if (idx==2)   {  identitiesPercent = misc::atof (token);     }
        else if (idx==3)   {  this->setLength   (misc::atoi (token));     }
        else if (idx==4)   {  this->setNbMisses (misc::atoi (token));     }
        else if (idx==5)   {  nbGaps = misc::atoi (token);                }
        else if (idx==6)   {  qryRange.begin = misc::atoi (token);        }
        else if (idx==7)   {  qryRange.end   = misc::atoi (token);        }
        else if (idx==8)   {  sbjRange.begin = misc::atoi (token);        }
        else if (idx==9)   {  sbjRange.end   = misc::atoi (token);        }
        else if (idx==10)  {  this->setEvalue   (misc::atof (token));     }
        else if (idx==11)  {  this->setBitScore (misc::atof (token));     }
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
