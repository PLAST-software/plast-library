/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

#ifndef _IALPHABET_HPP_
#define _IALPHABET_HPP_

/********************************************************************************/

#include <vector>

/********************************************************************************/
namespace database {
/********************************************************************************/

/** We define what a letter is (merely an integer). */
typedef char LETTER;

/** */
enum LETTER_CODE
{
    CODE_A,     CODE_C,     CODE_D,     CODE_E,     CODE_F,     CODE_G,     CODE_H,     CODE_I,
    CODE_K,     CODE_L,     CODE_M,     CODE_N,     CODE_P,     CODE_Q,     CODE_R,     CODE_S,
    CODE_T,     CODE_V,     CODE_W,     CODE_Y,     CODE_X,     CODE_B,     CODE_Z,     CODE_STAR,
    CODE_DASH,  CODE_U,     CODE_O,     CODE_J,     CODE_BAD
};

/**
 */
struct IAlphabet
{
    const char* name;

    LETTER* letters;
    size_t  size;

    LETTER any;
    LETTER stop;
    LETTER gap;

    bool isValid (LETTER l)  { return l < (LETTER)size; }
};

/********************************************************************************/

/** We define what data encodings are available. */
enum Encoding  {  SUBSEED, ASCII, NCBI, UNKNOWN };

/********************************************************************************/

class EncodingManager
{
public:

    /** Singleton definition. */
    static EncodingManager& singleton ()  { static EncodingManager instance; return instance; }

    /** */
    IAlphabet* getAlphabet (Encoding encoding);

    /** */
    const LETTER* getEncodingConversion (Encoding from, Encoding to);
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALPHABET_HPP_ */
