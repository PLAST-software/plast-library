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

#include <designpattern/impl/DirectoryIterator.hpp>

#include <misc/api/macros.hpp>

#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include <string>
#include <iostream>
#include <sstream>

using namespace std;

#include <stdio.h>
#define DEBUG(a)  //printf a

/********************************************************************************/
namespace dp { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DirectoryIterator::DirectoryIterator (const char* dirname, const char* match, bool dirOnly, bool recursive)
    : _dirname (dirname ? dirname : "."), _match (match ? match : ""), _dirOnly(dirOnly), _recursive (recursive)
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
DirectoryIterator::~DirectoryIterator ()
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
void DirectoryIterator::first ()
{
    _entries.clear();

    buildList (_dirname, _match, _recursive, _entries);

    _entriesIterator = _entries.begin();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
static inline bool isDirectory (const char* name, const char* fullpath, u_int32_t& type)
{
    struct stat statBuf;

    if (strcmp(name,".")==0 || strcmp(name,"..")==0)  { return false; }

    if( stat(fullpath, &statBuf) < 0 ) { return false; }

    type = statBuf.st_mode;

    return S_ISDIR(statBuf.st_mode);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DirectoryIterator::buildList (const string& dirname, const string& match, bool recursive, list<string>& entries)
{
    /** We try to open the directory given its name. */
    DIR* dp  = opendir (dirname.c_str());

    if (dp == 0)  { return; }

    for (struct dirent* dirp = readdir(dp);  dirp != 0; dirp = readdir(dp))
    {
        /** We build the full path. */
        stringstream ss;  ss << dirname << "/" << dirp->d_name;

        u_int32_t type = 0;

        if (recursive  &&  isDirectory(dirp->d_name, ss.str().c_str(), type) )
        {
            buildList (ss.str(), match, recursive, entries);
        }

        if (strstr (dirp->d_name, match.c_str()) != 0)
        {
            if (_dirOnly==false  ||  (_dirOnly && S_ISDIR(type) ) )
            {
                entries.push_back (ss.str());
            }
        }
    }

    /** We close the directory. */
    closedir(dp);
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
