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

#include <database/impl/SequenceTokenizer.hpp>

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define DEBUG(a)  //printf a

using namespace std;

/********************************************************************************/
namespace database { namespace impl  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SequenceTokenizer::SequenceTokenizer (const database::IWord& sequence)
 : _sequence (sequence), _currentIdx(0), _size(0)
{
    /** We build the list of pairs. */
    build ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SequenceTokenizer::build ()
{
    /** We retrieve the alphabet for the encoding of the sequence. */
    IAlphabet* alphabet = EncodingManager::singleton().getAlphabet (_sequence.encoding);

    if (alphabet != 0)
    {
        size_t validIndex = 0;

        /** Shortcuts. */
        LETTER* data = _sequence.letters.data;
        size_t  size = _sequence.letters.size;

        /** We loop each letter of the sequence. We maintain a small state machine
         *  for tracking areas of valid letters.
         */
        bool isOK = false;

        for (size_t i=0; i<size; i++)
        {
            /** We get the current letter validity status. */
            bool isValid = alphabet->isValid (data[i]);

            if (isOK==true && !isValid)
            {
                isOK = false;
                _pairs.push_back (pair<size_t,size_t> (validIndex, i-1));
            }

            else if (isOK==false && isValid)
            {
                isOK = true;
                validIndex = i;
            }

        } /* end of for (size_t i=0; i<size; i++) */

        /** We may have to add an extra pair. */
        if (isOK==true)
        {
            _pairs.push_back (pair<size_t,size_t> (validIndex, size-1));
        }
    }

    /** We update our shortcut. */
    _size = _pairs.size();
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

