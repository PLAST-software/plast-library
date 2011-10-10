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

#include "AbstractAlignmentResult.hpp"
#include "Property.hpp"

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace database;
using namespace indexation;
using namespace seed;

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
dp::IProperties* AbstractAlignmentResult::getProperties (const std::string& root, size_t nbDump)
{
    dp::IProperties* props = new dp::Properties();

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
    FILE* file = fopen (fileuri, "r");
    if (file != 0)
    {
        char buffer[256];
        char* loop = 0;
        const char* sep = "\t ";

        int queryIdx   = 0;
        int subjectIdx = 0;

        while (fgets(buffer, sizeof(buffer), file) != NULL)
        {
            Alignment align;

            /** query comment. */
            loop = strtok (buffer, sep);  if (!loop)  { ERR; continue; }
            map<string,int>::iterator qsearch = queryComments.find (loop);
            if (qsearch == queryComments.end())
            {
                queryComments[loop] = queryIdx;
                align._queryIdx     = queryIdx;
                queryIdx++;
            }
            else
            {
                align._queryIdx = qsearch->second;
            }

            /** subject comment. */
            loop = strtok (NULL, sep);   if (!loop)  { ERR; continue; }
            map<string,int>::iterator ssearch = subjectComments.find (loop);
            if (ssearch == subjectComments.end())
            {
                subjectComments[loop] = subjectIdx;
                align._subjectIdx     = subjectIdx;
                subjectIdx++;
            }
            else
            {
                align._subjectIdx = ssearch->second;
            }

            /** Identity. */
            loop = strtok (NULL, sep);   if (!loop)  { ERR; continue; }
            align._identity = atof (loop);

            /** Length align. */
            loop = strtok (NULL, sep);   if (!loop)  { ERR; continue; }
            align._length = atoi (loop);

            /** WARNING !!! the 'identity' column in the file is actually  nbIdenticalResidues / nbTotalResidues.
             *  The Alignment::_identity is the number of identical residues, so we have to rescale it from the length.
             */
            align._identity *= align._length / 100.0;

            /** Nb miss. */
            loop = strtok (NULL, sep);   if (!loop)  { ERR; continue; }
            align._nbMis = atoi (loop);

            /** Nb gaps. */
            loop = strtok (NULL, sep);   if (!loop)  { ERR; continue; }
            align._nbGap = atoi (loop);

            /** Query bounds. */
            loop = strtok (NULL, sep);   if (!loop)  { ERR; continue; }
            align._queryStartInSeq = atoi (loop) - 1;
            loop = strtok (NULL, sep);   if (!loop)  { ERR; continue; }
            align._queryEndInSeq = atoi (loop) - 1;

            /** Subject bounds. */
            loop = strtok (NULL, sep);   if (!loop)  { ERR; continue; }
            align._subjectStartInSeq = atoi (loop) - 1;
            loop = strtok (NULL, sep);   if (!loop)  { ERR; continue; }
            align._subjectEndInSeq = atoi (loop) - 1;

            /** Evalue. */
            loop = strtok (NULL, sep);   if (!loop)  { ERR; continue; }
            align._evalue = atof (loop);

            /** Bitscore. */
            loop = strtok (NULL, sep);   if (!loop)  { ERR; continue; }
            align._bitscore = atof (loop);

            /** We add the alignment to the result. */
            this->insert (align, NULL);
        }

        fclose (file);
    }
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
