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

#include <database/impl/StringSequenceIterator.hpp>
#include <database/impl/BasicSequenceBuilder.hpp>

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define DEBUG(a)  //printf a

using namespace std;

/********************************************************************************/
namespace database { namespace impl  {
/********************************************************************************/

static const char* commentFormat = "> comment %d";

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
StringSequenceIterator::StringSequenceIterator (size_t nb, ...)
    :  _currentIdx(0)
{
    /** We set the encoding scheme. */
    setEncoding (SUBSEED);

    /** We set the builder. */
    setBuilder (new BasicSequenceBuilder (SUBSEED));

    /** We memorize provided strings. */
    va_list ap;
    va_start  (ap, nb);

    for (size_t i = 0; i<nb;  i++)
    {
        /** We get the current string. */
        char* str = va_arg (ap, char*);

        /** We want an uppercase string. */
        string s (str);   for (size_t i=0; i<s.size(); i++)  { s[i] = toupper (s[i]);  }

        /** We add the string into the container. */
        _strings.push_back (s);
    }
    va_end(ap);

    DEBUG (("StringSequenceIterator::StringSequenceIterator:  FOUND %d sequences\n", _strings.size()));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
StringSequenceIterator::StringSequenceIterator (const std::vector<std::string>& strings)
    : _strings(strings), _currentIdx(0)
{
    /** We set the encoding scheme. */
    setEncoding (SUBSEED);

    /** We set the builder. */
    setBuilder (new BasicSequenceBuilder (SUBSEED));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
StringSequenceIterator::~StringSequenceIterator ()
{
    DEBUG (("StringSequenceIterator::~StringSequenceIterator\n"));

    setBuilder (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void StringSequenceIterator::updateItem ()
{
    if (!isDone ())
    {
        /** We fill the content of the current sequence. */

        /** We set the comment. */
        char comment[64];
        snprintf (comment, sizeof(comment), commentFormat, _currentIdx+1);

        /** We set the comment through the builder. */
        getBuilder()->setComment (comment, strlen(comment));

        DEBUG (("StringSequenceIterator::changeSequence:  comment='%s' \n", comment));

        /** We retrieve the current string. */
        string& currentStr = _strings[_currentIdx];

        DEBUG (("StringSequenceIterator::changeSequence:  str='%s'   size=%d \n", currentStr.c_str(), currentStr.size() ));

        /** We reset the data. */
        getBuilder()->resetData();

        DEBUG (("StringSequenceIterator::changeSequence:  reset done\n"));

        /** We set the data (currently encoded in ASCII). */
        getBuilder()->addData ((const LETTER*)currentStr.c_str(), currentStr.size(), ASCII);

        DEBUG (("StringSequenceIterator::changeSequence:  addData done\n"));
    }
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

