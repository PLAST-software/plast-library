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

#ifndef _GENWIN_H_
#define _GENWIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <ctype.h>

/********************************************************************************/

typedef int Bool;

/********************************************************************************/
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/********************************************************************************/

struct Database
{
    char *filename;
    FILE *fp;
    char *indexname;
    long int filepos;
};

/********************************************************************************/

struct Sequence
{
    struct Database *db;

    char *id;
    char *name;
    char *organism;
    char *header;

    char *seq;
    int start;                     /* for windows */
    int length;
    struct Alphabet *alphabet;

    struct Sequence *parent;       /* for windows */
    struct Sequence *root;         /* for subwindows */
    struct Sequence **children;    /* only the floaters? */

    Bool bogus;
    Bool punctuation;
    Bool rubberwin;             /* for windows */
    Bool floatwin;              /* for subwindows */
    Bool seedwin;

    int *state;
    double entropy;
    int *composition;
    Bool	*comptst;
    int	*compindex;

    char *classvec;               /* from ClaVec[aa] */
    struct Alphabet *clalphabet;
    double *scorevec;             /* from ScoVec[aa] or ScoFun(pos) */
    double score;                 /* from ScoFun(win) */
    /* union, for integer scores? */
    struct PerScoreVec *foo;
    int *bar;
};

/********************************************************************************/

struct Configuration
{
    char *iseq;
    int ilength;

    int printper;
};

/********************************************************************************/

struct Matrix
{
    struct Sequence *parent;

    char **seq;
    int start;
    int period;
    int length;

    int **rowcomp;
    int **colcomp;
};

/********************************************************************************/

struct Alphabet
{
    char *name;
    int size;

    int *index[128];
    char *chars;       /*  [size]  */
    char **charnames;  /*  [size]  */

    Bool caseinvariant;
};

/********************************************************************************/

struct TransVector
{
    struct Alphabet *from;
    struct Alphabet *to;

    int *index;     /*  [from->size]  */
};

/********************************************************************************/

struct ScoreVector
{
    struct Alphabet *from;

    double *score;   /*  [from->size]  */
    char *units;
};

/********************************************************************************/

struct ScoreMatrix
{
    struct Alphabet *from1;
    struct Alphabet *from2;

    double **score;  /*  [from1->size][from2->size]  */
    char *units;
};

/********************************************************************************/

struct PerScoreVec
{
    int hits;
    int tot;
    double pct;
    double std;
    double prob;
};

/********************************************************************************/

void             genwininit (void);
struct Sequence* openwin    (struct Sequence* parent,  int start, int length);
struct Sequence* nextwin    (struct Sequence* win,     int shift);
int              shiftwin1  (struct Sequence * win);
void             closewin   (struct Sequence* win);
void             compon     (struct Sequence* win);
void             stateon    (struct Sequence* win);
void             enton      (struct Sequence* win);
int              readhdr    (struct Sequence *seq);
void             readseq    (struct Sequence* seq);

void   entropy_init (int  window);
void   entropy_exit ();
double entropy (int* sv);

struct Sequence* readentry (struct Database *dbase);

void skipline (FILE *fp);
int  findchar (char *str, char chr);

void upper (char* string, size_t len);
void lower (char* string, size_t len);

/********************************************************************************/

#endif /* _GENWIN_H_ */
