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

#ifndef SEG_H_
#define SEG_H_

/********************************************************************************/

struct Segment
{
    int begin;
    int end;
    struct Segment* next;
};

/********************************************************************************/

double getprob (int* sv, int total);
double lnperm  (int* sv, int tot);
double lnass   (int* sv);

void seg_filterSequence (char* sequence, int length);
void segseq (struct Sequence* seq, struct Segment** segs, int offset);
double* seqent (struct Sequence* seq);
int hasdash (struct Sequence* win);
int findlo (int i, int limit, double* H);
int findhi (int i, int limit, double* H);
void trim (struct Sequence* seq, int* leftend, int* rightend);
double getprob (int* sv, int total);
double lnperm (int* sv, int tot);
double lnass (int* sv);
void mergesegs (struct Sequence* seq, struct Segment* segs);
void report (struct Sequence* seq, struct Segment* segs);
void singreport (struct Sequence* seq, struct Segment* segs);
void prettyreport (struct Sequence* seq, struct Segment* segs);
void pretreereport (struct Sequence* seq, struct Segment* segs);
void space (register int len);
void seqout (struct Sequence* seq, int hilo, int begin, int end);
void appendseg (struct Segment* segs, struct Segment* seg);
void freesegs (struct Segment* segs);
void usageseg (void);



/********************************************************************************/

#endif /*SEG_H_*/
