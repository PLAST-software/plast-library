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

#include <math.h>
#include <ctype.h>

#define TRUE   1
#define FALSE  0

static int word     = 3;
static int window   = 64;
static int window2  = 32;
static int level    = 20;

static int mv, iv, jv;

#define MAXREG    1001

typedef struct {
    int from;
    int to;
    int score;
} REGION;

static REGION reg[MAXREG];
static int nreg;

#include <stdio.h>
#define DEBUG(a)  //printf a

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void set_dust_level (int value)
{
    level = value;
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void set_dust_window (int value)
{
    window = value;
    window2 = window / 2;
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void set_dust_word (int value)
{
    word = value;
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
static void wo1 (int len, char* s,  int ivv)
{
    int i, ii, j, v, t, n, n1, sum;
    static int counts[32*32*32];
    static int iis[32*32*32];
    int js, nis;

    n = 32 * 32 * 32;
    n1 = n - 1;
    nis = 0;
    i = 0;
    ii = 0;
    sum = 0;
    v = 0;
    for (j=0; j < len; j++, s++)
    {
        ii <<= 5;
        if (isalpha(*s))
        {
            if (islower(*s))
            {
                ii |= *s - 'a';
            }
            else
            {
                ii |= *s - 'A';
            }
        }
        else
        {
            i = 0;
            continue;
        }
        ii &= n1;
        i++;
        if (i >= word)
        {
            for (js=0; js < nis && iis[js] != ii; js++) ;
            if (js == nis)
            {
                iis[nis] = ii;
                counts[ii] = 0;
                nis++;
            }
            if ((t = counts[ii]) > 0)
            {
                sum += t;
                v = 10 * sum / j;
                if (mv < v)
                {
                    mv = v;
                    iv = ivv;
                    jv = j;
                }
            }
            counts[ii]++;
        }
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
static int wo (int len, char* s, int* beg, int* end)
{
    int i, l1;

    l1 = len - word + 1;
    if (l1 < 0)
    {
        *beg = 0;
        *end = len - 1;
        return 0;
    }
    mv = 0;
    iv = 0;
    jv = 0;
    for (i=0; i < l1; i++)
    {
        wo1(len-i, s+i, i);
    }
    *beg = iv;
    *end = iv + jv;
    return mv;
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void dust_filterSequence (char* s, int len)
{
#if 0
    printf ("--------------------------------------------------------------------------\n");
    printf ("dust_filterSequence:  [0]=%c  len=%d \n", *s, len);
    int k;
    for (k=0; k<len; k++)  { printf ("%c", s[k]);  }  printf ("\n");
#endif

    int i, j, l, from, to, a, b, v;

    from = 0;
    to = -1;
    for (i=0; i < len; i += window2)
    {
        from -= window2;
        to -= window2;
        l = (len > i+window) ? window : len-i;
        v = wo(l, s+i, &a, &b);
        for (j = from; j <= to; j++)
        {
            s[i+j] = 'N';
        }
        if (v > level)
        {
            for (j = a; j <= b && j < window2; j++)
            {
                s[i+j] = 'N';
            }
            from = j;
            to = b;
        }
        else
        {
            from = 0;
            to = -1;
        }
    }

#if 0
    printf ("\n");
    for (k=0; k<len; k++)  { printf ("%c", s[k]);  }  printf ("\n");
#endif
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
REGION *dust_segs (int len, char* s)
{
    int i, l, from, to, a, b, v;

    nreg = 0;
    from = 0;
    to = -1;
    for (i=0; i < len; i += window2)
    {
        l = (len > i+window) ? window : len-i;
        v = wo(l, s+i, &a, &b);
        if (v > level)
        {
            if (nreg > 0 && reg[nreg-1].to+1 >= a+i)
            {
                reg[nreg-1].to = b + i;
            }
            else if (nreg < MAXREG-1)
            {
                reg[nreg].from = a + i;
                reg[nreg].to = b + i;
                nreg++;
            }
            if (b < window2)
            {
                i += b - window2;
            }
        }
    }
    reg[nreg].from = 0;
    reg[nreg].to = -1;
    return reg;
}
