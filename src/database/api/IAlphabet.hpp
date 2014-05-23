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

/** \file IAlphabet.hpp
 *  \brief Define concepts of alphabet and encoding schemes.
 *  \date 07/11/2011
 *  \author edrezen
 *
 *   PLAST compares databases made of letters that belong to some alphabet (amino acids or nucleotids).
 *
 *   We define here constants for all letters we can find in databases. Note that there exist some special
 *   characters such as '*' or '-' that have specific meaning (stop codon, gap, unknown character, etc...)
 *
 *   We may also need to encode these letters in different format. For instance, we are likely to find
 *   letters in database in ASCII format. It may be easier to translate them in a smaller integer range,
 *   which is achievable with two other formats: SUBSEED and NCBI. By default, we will prefer the SUBSEED
 *   format that maps all amino acids in the smallest integer range.
 *
 *   Note that we define and EncodingManager class that allows for instance to translate from ASCII to SUBSEED
 *   (which is done for instance when reading FASTA files).
 */

#ifndef _IALPHABET_HPP_
#define _IALPHABET_HPP_

/********************************************************************************/

#include <vector>
#include <stddef.h>

/********************************************************************************/
/** \brief Definition of concepts related to genomic databases. */
namespace database {
/********************************************************************************/

/** \brief Definition of what a genomic letter is.
 */
typedef char LETTER;

/** \brief Definition of known letters.
 *
 *  We enumerate all letters we can deal with.
 */
enum LETTER_CODE
{
    CODE_A,     CODE_C,     CODE_D,     CODE_E,     CODE_F,     CODE_G,     CODE_H,     CODE_I,
    CODE_K,     CODE_L,     CODE_M,     CODE_N,     CODE_P,     CODE_Q,     CODE_R,     CODE_S,
    CODE_T,     CODE_V,     CODE_W,     CODE_Y,     CODE_X,     CODE_B,     CODE_Z,     CODE_STAR,
    CODE_DASH,  CODE_U,     CODE_O,     CODE_J,     CODE_BAD
};

/** \brief Definition of IAlphabet concept.
 *
 *  This structure provides some attributes that defines an alphabet.
 */
struct IAlphabet
{
    /** Name of the alphabet ("ascii", "subseed", "ncbi"). */
    const char* name;

    /** Array containing the letters known by the alphabet. One can reach an element by using LETTER_CODE enum as array index. */
    LETTER* letters;

    /** Size of the alphabet: number of correct characters (for instance for amino acids). */
    size_t  size;

    /** Special code that matches any character ('X' for instance). */
    LETTER any;

    /** Special code for codon stop. */
    LETTER stop;

    /** Special code for gap. */
    LETTER gap;

    /** Tells whether the letter is valid or not.
     * \param[in] l : code of the letter.
     * \return true if correct, false otherwise.
     */
    bool isValid (LETTER l)  { return l < (LETTER)size; }
};

/********************************************************************************/

/** We define what data encodings are available.  */
enum Encoding
{
    SUBSEED,
    ASCII,
    NCBI,
    NCBI_DNA_NO_AMB,
    NCBI_DNA_WITH_AMB,
    UNKNOWN
};

/********************************************************************************/

/** \brief Management of alphabet encoding.
 *
 *  This (singleton) class acts like a factory that returns IAlphabet instances given an encoding scheme.
 *
 *  It also provides conversion table from one encoding to another. This is useful for instance for
 *  translating FASTA files (that are in ASCII encoding) into SUBSEED encoding. Note that we don't provide
 *  a method that translates a character; we prefer (for optimization purpose) directly provide the full
 *  conversion table and it is up to the client to use it for making the needed translations.
 */
class EncodingManager
{
public:

    /** We define what kind of alphabets we want to deal with: nucleotides or amino acids */
    enum Kind
    {
        ALPHABET_NUCLEOTID,
        ALPHABET_AMINO_ACID,
        ALPHABET_UNKNOWN
    };

    /** Singleton definition.
     * \return the singleton instance.
     */
    static EncodingManager& singleton ()  { static EncodingManager instance; return instance; }

    /** Set the kind of alphabet: nucleotid or amino acid (default)
     * \param[in] kind : kind of the alphabet
     */
    void setKind (Kind kind) { _kind = kind; }

    /** Get the kind of alphabet: nucleotid or amino acid (default)
     * \return alphabet kind
     */
    Kind getKind () { return _kind; }

    /** Returns an IAlphabet instance given an encoding scheme.
     * \param[in] encoding : encoding scheme
     * \return the IAlphabet instance.
     */
    IAlphabet* getAlphabet (Encoding encoding);

    /** Provides a conversion table from one encoding to another.
     * \param[in] from : the source encoding scheme
     * \param[in] to   : the destination encoding scheme
     * \return the conversion array of LETTER*/
    const LETTER* getEncodingConversion (Encoding from, Encoding to);

private:

    /** We configure to amino acid alphabet by default. */
    EncodingManager ()  : _kind (ALPHABET_AMINO_ACID) {}

    Kind _kind;

    /** */
    const LETTER* getEncodingConversion_aminoacid (Encoding from, Encoding to);
    const LETTER* getEncodingConversion_nucleotid (Encoding from, Encoding to);
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALPHABET_HPP_ */
