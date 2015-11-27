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

#include <designpattern/impl/TokenizerIterator.hpp>
#include <os/impl/DefaultOsFactory.hpp>

#include <string.h>

using namespace std;
using namespace os;
using namespace os::impl;

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
TokenizerIterator::TokenizerIterator (const char* text, const char* separator)
    : _sep(separator), _text(0), _str(0), _loop(0), _save(0)
{
    DEBUG (("TokenizerIterator::TokenizerIterator: text='%s'  sep='%s'\n", text, separator));

    if (text != 0)  { _text = DefaultFactory::memory().strdup (text); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
TokenizerIterator::~TokenizerIterator ()
{
    DefaultFactory::memory().free (_text);
    DefaultFactory::memory().free (_str);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void TokenizerIterator::first()
{
    DEBUG (("TokenizerIterator::first\n"));

    /** We init the _str attribute. */
    if (_str  != 0)  { DefaultFactory::memory().free (_str); }
    if (_text != 0)  {  _str = DefaultFactory::memory().strdup (_text);  }

    if (_str)  {  _loop = tok_r (_str, _sep.c_str(), &_save);  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
dp::IteratorStatus TokenizerIterator::next()
{
    _loop = tok_r (NULL, _sep.c_str(), &_save);
    return ITER_UNKNOWN;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS : Here because of lack of strtok_r in mingw32 distributions
*********************************************************************/
char* TokenizerIterator::tok_r (char* s, const char* delim, char** lasts)
{
    char* spanp = 0;
    int c, sc;
    char* tok = 0;

    if (s == NULL && (s = *lasts) == NULL)  {  return (NULL);  }

cont:
    c = *s++;
    for (spanp = (char *)delim; (sc = *spanp++) != 0;)  {  if (c == sc)  {  goto cont;  }  }

    /* no non-delimiter characters */
    if (c == 0)
    {
        *lasts = NULL;
        return (NULL);
    }

    tok = s - 1;

    /* Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for (;;)
    {
        c = *s++;
        spanp = (char *)delim;
        do
        {
            if ((sc = *spanp++) == c)
            {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *lasts = s;
                return (tok);
            }
        } while (sc != 0);
    }

    return (NULL); /* NOTREACHED */
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
