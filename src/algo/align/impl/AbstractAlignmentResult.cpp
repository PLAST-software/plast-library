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

#include <os/impl/DefaultOsFactory.hpp>

#include <designpattern/impl/Property.hpp>
#include <designpattern/impl/FileLineIterator.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>

#include <algo/align/impl/AbstractAlignmentResult.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace os;
using namespace os::impl;
using namespace dp;
using namespace dp::impl;
using namespace database;
using namespace indexation;
using namespace seed;

/********************************************************************************/
namespace algo   {
namespace align {
namespace impl   {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractAlignmentResult::AbstractAlignmentResult ()
    : _synchro(0)
{
    _synchro = DefaultFactory::thread().newSynchronizer();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractAlignmentResult::~AbstractAlignmentResult ()
{
    if (_synchro)  { delete _synchro; }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IProperties* AbstractAlignmentResult::getProperties (const std::string& root)
{
    IProperties* props = new Properties();

    props->add (0, root,   "%ld", getSize());
    props->add (1, "size", "%ld", getSize());

    return props;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
#define ERR printf ("ERROR WHILE PARSING\n");

void AbstractAlignmentResult::readFromFile (
    const char* fileuri,
    std::map<std::string,int>& subjectComments,
    std::map<std::string,int>& queryComments
)
{
    int queryIdx   = 0;
    int subjectIdx = 0;

    FileLineIterator fileIt (fileuri, 1024);
    for (fileIt.first(); !fileIt.isDone(); fileIt.next())
    {
        Alignment align;

        size_t idx = 0;
        TokenizerIterator tokenizer (fileIt.currentItem(), " \t");
        for (tokenizer.first(); !tokenizer.isDone(); tokenizer.next(), idx++)
        {
            char* token = tokenizer.currentItem();

            if (idx==0)
            {
                map<string,int>::iterator qsearch = queryComments.find (token);
                if (qsearch == queryComments.end())
                {
                    queryComments[token] = queryIdx;
                    align._queryIdx      = queryIdx;
                    queryIdx++;
                }
                else
                {
                    align._queryIdx = qsearch->second;
                }
            }

            else if (idx==1)
            {
                map<string,int>::iterator ssearch = subjectComments.find (token);
                if (ssearch == subjectComments.end())
                {
                    subjectComments[token] = subjectIdx;
                    align._subjectIdx      = subjectIdx;
                    subjectIdx++;
                }
                else
                {
                    align._subjectIdx = ssearch->second;
                }
            }

            else if (idx==2)   {  align._identity           = atof (token);   }
            else if (idx==3)   {  align._length             = atoi (token);   }
            else if (idx==4)   {  align._nbMis              = atoi (token);   }
            else if (idx==5)   {  align._nbGap              = atoi (token);   }
            else if (idx==6)   {  align._queryStartInSeq    = atoi (token);   }
            else if (idx==7)   {  align._queryEndInSeq      = atoi (token);   }
            else if (idx==8)   {  align._subjectStartInSeq  = atoi (token);   }
            else if (idx==9)   {  align._subjectEndInSeq    = atoi (token);   }
            else if (idx==10)  {  align._evalue             = atof (token);   }
            else if (idx==11)  {  align._bitscore           = atof (token);   }
        }

        /** We read from a human readable file that counts range with a 1 starting position.
         *  We convert back to a 0 starting position. */
        align._queryStartInSeq    --;
        align._queryEndInSeq      --;
        align._subjectStartInSeq  --;
        align._subjectEndInSeq    --;

        align._identity *= align._length / 100.0;

        this->insert (align, NULL);
    }
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
