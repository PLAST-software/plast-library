
#include "ObsfucatedString.hpp"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

#define MIN(a,b)  ((a)<(b) ? (a) : (b))

#define DEBUG(a)  a

/********************************************************************************/
namespace misc {
namespace impl {
/********************************************************************************/

ObsfucatedString::LONG ObsfucatedString::seed = 123456789;

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ObsfucatedString::ObsfucatedString (LONG l, ...)
{
    vector<LONG> obsfucated;
    obsfucated.push_back (l);

    va_list ap;
    va_start  (ap, l);
    for (LONG val=0; (val = va_arg (ap, LONG)) != 0; )    {  obsfucated.push_back (val);   }
    va_end (ap);

    size_t length = obsfucated.size();

    size_t sizeEncoded = 8*(length-1);
    char* encoded = new char[sizeEncoded];

    LONG seed = obsfucated[0];
    srand (seed);

    for (size_t i=1; i<length; i++)
    {
        LONG key =  rand ();
        toBytes (obsfucated[i]^key, encoded, sizeEncoded, 8*(i-1));
    }
    
    for ( ; sizeEncoded >= 1  &&  encoded[--sizeEncoded] == 0; )  {}

    s.assign (encoded, sizeEncoded+1);

    delete[] encoded;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void ObsfucatedString::toBytes (LONG l, char* bytes, size_t length, int off)
{
    int end = MIN (length, off + 8);
    for (int i = off; i < end; i++) 
    {
        bytes[i] = (char) l;
        l >>= 8;
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ObsfucatedString::LONG ObsfucatedString::toLong (const char* bytes, size_t length, int off)
{
    LONG l = 0;

    int end = MIN (length, off + 8);
    for (int i = end; --i >= off; ) 
    {
        l <<= 8;
        l |= bytes[i];
    }

    return l;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
string ObsfucatedString::obsfucate (const string& s)
{
    stringstream ss;

    vector<LONG> obsfucatedVec;

    /** We retrieve an access to the array of characters. */
    const char* encoded = s.c_str();
    size_t length = s.size ();

    LONG seed = time(NULL);
    srand (seed);
    obsfucatedVec.push_back (seed);
    
    for (size_t i=0; i<length; i+=8)
    {
        LONG key = rand ();

        LONG obsfucated = toLong (encoded, length, i) ^ key;
        obsfucatedVec.push_back (obsfucated);
    }    

    //for (size_t i=0; i<obsfucatedVec.size(); i++)  { printf ("0x%lx,  ", obsfucatedVec[i]);  }    printf ("\n");

    // obsfucate 'ObsfucatedString ('
    ss << ObsfucatedString(1339595508, 8386093337020575146, 7453010373642693831, 9275675, 0).toString();

    for (size_t i=0; i<obsfucatedVec.size(); i++)   {  ss << obsfucatedVec[i] << ", ";  } 

    // obsfucate '0);  /* => "'
    ss << ObsfucatedString (1343321365, 4404566817465271842, 3271637, 0).toString();

    ss << s;

    // obsfucate '\" */'
    ss << ObsfucatedString(1339595360, 796737859, 0).toString();

    return ss.str();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ObsfucatedString::LONG ObsfucatedString::rand ()
{
    LONG m = 10000000;
    LONG b = 98753421;

    seed = (seed*b + 1) % m;

    return seed;
}

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/
