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

/** \file IWord.hpp
 *  \brief Definition of IWord interface.
 *  \date 07/11/2011
 *  \author edrezen
 *
 * We define here what a word is: a series of letter in some encoding scheme.
 */

#ifndef _IWORD_HPP_
#define _IWORD_HPP_

/********************************************************************************/

#include <database/api/IAlphabet.hpp>
#include <misc/api/Vector.hpp>

/********************************************************************************/
/** \brief Definition of concepts related to genomic databases. */
namespace database {
/********************************************************************************/

/** \brief Definition of a word
 *
 *  We define what a word is: some raw content encoded in some encoding scheme.
 *
 *  Note that the content of the word may be:
 *    - copied from another source
 *    - a reference to another source (no copy)
 *
 *  The referenced mode is quicker because we don't need to copy bunch of memory.
 */
struct IWord
{
    /** Vector holding the genomic letters. */
    misc::Vector<LETTER> letters;

    /** Encoding scheme for the letters. */
    Encoding           encoding;

    /** Constructor. */
    IWord ()                : letters(0),         encoding(SUBSEED)       {}

    /** Copy constructor.
     * \param[in] w : the object to be copied.
     */
    IWord (const IWord& w)  : letters(w.letters), encoding(w.encoding)    {}

    /** Constructor. Some kind of copy constructor (no reference).
     * \param[in] size : size of the buffer to be copied
     * \param[in] buf  : buffer to be copied
     */
    IWord (size_t size, const LETTER* buf=0)  : letters(size), encoding(SUBSEED)
    {
        if (buf != 0)  {  for (size_t i=0; i<size; i++)  { letters.data[i] = buf[i]; }  }
    }

    /** Affectation operator.
     * \param[in] w : the word to be copied.
     * \return the affected instance.
     */
    IWord& operator= (const IWord& w)
    {
        if (this != &w)
        {
            letters  = w.letters;
            encoding = w.encoding;
        }

        return *this;
    }

    /** Update instance by providing a reference to a buffer. There is no data copy here, just references.
     * When using this, it is important to ensure that the referenced data lives longer than the IWord
     * instance that refers it.
     * \param[in]  size : size of the data to be referenced.
     * \param[in] aBuffer : buffer to be referenced.
     */
    void setReference (size_t size, LETTER* aBuffer)
    {
        letters.setReference (size, aBuffer);
    }

    /** Update instance by providing a reference to a buffer. There is no data copy here, just references.
     * When using this, it is important to ensure that the referenced data lives longer than the IWord
     * instance that refers it.
     * \param[in]  w : the word instance to be referenced
     */
    void setReference (const IWord& w)
    {
        setReference (w.letters.size, w.letters.data);
    }

    /** Comparison operator.
     * \param[in] w : instance to be compared to.
     * \return true if identical, false otherwise.
     */
    bool operator== (const IWord& w) const
    {
        return (encoding == w.encoding) &&  (letters == w.letters);
    }

    /** Method for having textual representation of the IWord instance. Mainly for debug purpose.
     * \return the string
     */
    std::string toString () const
    {
        std::string result (letters.size, 'X');

        const LETTER* convert = EncodingManager::singleton().getEncodingConversion (encoding, ASCII);

        if (convert != 0)
        {
            for (size_t i=0; i<letters.size; i++)
            {
                result[i] =  convert[(int)letters.data[i]];
            }
        }
        else
        {
            /** We throw some exception. */
            //throw GenericFailure ("no conversion table");
        }
        return result;
    }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IWORD_HPP_ */
