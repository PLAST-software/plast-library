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

#include <seg/api/genwin.h>
#include <seg/api/seg.h>
#include <seg/impl/lnfac.pri>

int window = 12;
int downset, upset;
double locut = 2.2;
double hicut = 2.5;

int overlaps = FALSE;
int hionly = FALSE;
int loonly = FALSE;
int entinfo = TRUE;
int singleseq = FALSE;
int prettyseq = FALSE;
int prettytree = TRUE;

int hilenmin = 0;
int charline = 60;
int maxtrim = 100;

#define MIN(a,b) ((a) <= (b) ? (a) : (b))

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void seg_segSequence (char* sequence, size_t length)
{
    struct Sequence* seq  = 0;
    struct Segment*  segs = 0;

    seq = (struct Sequence *) malloc(sizeof(struct Sequence));

    seq->seq         = sequence;
    seq->length      = length;  //strlen(sequence);
    seq->db          = NULL;
    seq->parent      = (struct Sequence *)  NULL;
    seq->root        = (struct Sequence *)  NULL;
    seq->children    = (struct Sequence **) NULL;
    seq->rubberwin   = FALSE;
    seq->floatwin    = FALSE;
    seq->punctuation = FALSE;
    seq->entropy     = -2.;
    seq->state       = (int*)    NULL;
    seq->composition = (int*)    NULL;
    seq->classvec    = (char*)   NULL;
    seq->scorevec    = (double*) NULL;

    genwininit();

    downset = (window+1)/2 - 1;
    upset   = window - downset;

    singleseq  = TRUE;
    prettyseq  = FALSE;
    prettytree = FALSE;
    hionly     = TRUE;
    loonly     = FALSE;

    entropy_init (window);

    segs = (struct Segment *) NULL;

    segseq (seq, &segs, 0);

    mergesegs (seq, segs);

    singreport (seq, segs);

    entropy_exit ();

    freesegs (segs);

    free (seq);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void segseq (struct Sequence* seq, struct Segment** segs, int offset)
{
    struct Segment *seg, *leftsegs;
    struct Sequence *leftseq;
    int first, last, lowlim;
    int loi, hii, i;
    int leftend, rightend, lend, rend;
    double *H, *seqent();

    H = seqent(seq);
    if (H==NULL) return;

    first = downset;
    last = seq->length - upset;
    lowlim = first;

    for (i=first; i<=last; i++)
    {
        if (H[i]<=locut && H[i]!=-1)
        {
            loi = findlo(i, lowlim, H);
            hii = findhi(i, last, H);

            leftend = loi - downset;
            rightend = hii + upset - 1;

            trim(openwin(seq, leftend, rightend-leftend+1), &leftend, &rightend);

            if (i+upset-1<leftend)   /* check for trigger window in left trim */
            {
                lend = loi - downset;
                rend = leftend - 1;

                leftseq = openwin(seq, lend, rend-lend+1);
                leftsegs = (struct Segment *) NULL;
                segseq(leftseq, &leftsegs, offset+lend);
                if (leftsegs!=NULL)
                {
                    if (*segs==NULL) *segs = leftsegs;
                    else appendseg(*segs, leftsegs);
                }
                closewin(leftseq);

                /*          trim(openwin(seq, lend, rend-lend+1), &lend, &rend);
            seg = (struct Segment *) malloc(sizeof(struct Segment));
            seg->begin = lend;
            seg->end = rend;
            seg->next = (struct Segment *) NULL;
            if (segs==NULL) segs = seg;
            else appendseg(segs, seg);  */
            }

            seg = (struct Segment *) malloc(sizeof(struct Segment));
            seg->begin = leftend + offset;
            seg->end = rightend + offset;
            seg->next = (struct Segment *) NULL;

            if (*segs==NULL) *segs = seg;
            else appendseg(*segs, seg);

            i = MIN (hii, rightend+downset);
            lowlim = i + 1;
            /*       i = hii;     this ignores the trimmed residues... */
        }
    }

    free(H);
    return;
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
double* seqent (struct Sequence* seq)
{
    struct Sequence *win;
    double *H;
    int i, first, last;

    if (window>seq->length)
    {
        return((double *) NULL);
    }

    H = (double *) malloc(seq->length*sizeof(double));

    for (i=0; i<seq->length; i++)
    {
        H[i] = -1.;
    }

    win = openwin(seq, 0, window);
    enton(win);

    first = downset;
    last = seq->length - upset;

    for (i=first; i<=last; i++)
    {
        if (seq->punctuation && hasdash(win))
        {H[i] = -1;
        shiftwin1(win);
        continue;}
        H[i] = win->entropy;
        shiftwin1(win);
    }

    closewin(win);
    return(H);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int hasdash (struct Sequence* win)
{
    register char	*seq, *seqmax;

    seq = win->seq;
    seqmax = seq + win->length;

    while (seq < seqmax)
    {
        if (*seq++ == '-')
            return TRUE;
    }
    return FALSE;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int findlo (int i, int limit, double* H)
{
    int j;

    for (j=i; j>=limit; j--)
    {
        if (H[j]==-1) break;
        if (H[j]>hicut) break;
    }

    return(j+1);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int findhi (int i, int limit, double* H)
{
    int j;

    for (j=i; j<=limit; j++)
    {
        if (H[j]==-1) break;
        if (H[j]>hicut) break;
    }

    return(j-1);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void trim (struct Sequence* seq, int* leftend, int* rightend)
{
    struct Sequence *win;
    double prob, minprob;
    int shift, len, i;
    int lend, rend;
    int minlen;

    /* fprintf(stderr, "%d %d\n", *leftend, *rightend);  */

    lend = 0;
    rend = seq->length - 1;
    minlen = 1;
    if ((seq->length-maxtrim)>minlen) minlen = seq->length-maxtrim;

    minprob = 1.;
    for (len=seq->length; len>minlen; len--)
    {
        win = openwin(seq, 0, len);
        stateon(win);
        i = 0;

        shift = TRUE;
        while (shift)
        {
            prob = getprob (win->state, len);
            if (prob<minprob)
            {
                minprob = prob;
                lend = i;
                rend = len + i - 1;
            }
            shift = shiftwin1(win);
            i++;
        }
        closewin(win);
    }

    /* fprintf(stderr, "%d-%d ", *leftend, *rightend);  */

    *leftend = *leftend + lend;
    *rightend = *rightend - (seq->length - rend - 1);

    /* fprintf(stderr, "%d-%d\n", *leftend, *rightend);  */

    closewin(seq);
    return;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
double getprob (int* sv, int total)
{
    double ans, totseq;

#define LN20	2.9957322735539909
    totseq = ((double) total) * LN20;

    ans = lnass(sv) + lnperm(sv, total) - totseq;

    return(ans);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
double lnperm (int* sv, int tot)
{
    double ans;
    int i;

    ans = lnfac[tot];

    for (i=0; sv[i]!=0; i++)
    {
        ans -= lnfac[sv[i]];
    }

    return(ans);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
double lnass (int* sv)
{
    double	ans;
    register int	svi, svim1;
    register int	class, total;
    register int    i;

    ans = lnfac[20];
    if (sv[0] == 0)
        return ans;

    total = 20;
    class = 1;
    svim1 = sv[0];
    for (i=0;; svim1 = svi)
    {
        if (++i==20)
        {
            ans -= lnfac[class];
            break;
        }
        else if ((svi = *++sv) == svim1)
        {
            class++;
            continue;
        }
        else
        {
            total -= class;
            ans -= lnfac[class];
            if (svi == 0)
            {
                ans -= lnfac[total];
                break;
            }
            else
            {
                class = 1;
                continue;
            }
        }
    }

    return ans;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void mergesegs (struct Sequence* seq, struct Segment* segs)
{
    struct Segment *seg, *nextseg;
    int len;

    if (overlaps)   return;
    if (segs==NULL) return;

    if (segs->begin<hilenmin) segs->begin = 0;

    seg = segs;
    nextseg = seg->next;

    while (nextseg!=NULL)
    {
        if (seg->end>=nextseg->begin)               /* overlapping segments */
        {
            seg->end = nextseg->end;
            seg->next = nextseg->next;
            free(nextseg);
            nextseg = seg->next;
            continue;
        }
        len = nextseg->begin - seg->end - 1;
        if (len<hilenmin)                            /* short hient segment */
        {
            seg->end = nextseg->end;
            seg->next = nextseg->next;
            free(nextseg);
            nextseg = seg->next;
            continue;
        }
        seg = nextseg;
        nextseg = seg->next;
    }

    len = seq->length - seg->end - 1;
    if (len<hilenmin) seg->end = seq->length - 1;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void singreport (struct Sequence* seq, struct Segment* segs)
{
    char	*proseq, *proseqmax;
    struct Segment	*seg;
    int	begin, end;

    proseq = seq->seq;
    proseqmax = proseq + seq->length;
    upper(proseq, seq->length);

    for (seg=segs; seg!=NULL; seg=seg->next)
    {
        begin = seg->begin;
        end   = seg->end;
        memset (proseq + begin, 'X', end - begin +1);
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
void appendseg (struct Segment* segs, struct Segment* seg)
{
    struct Segment *temp;

    temp = segs;
    while (1)
    {
        if (temp->next==NULL)
        {
            temp->next = seg;
            break;
        }
        else
        {
            temp = temp->next;
        }
    }

    return;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void freesegs (struct Segment* segs)
{
    struct Segment *temp;

    while (segs!=NULL)
    {
        temp = segs->next;
        free(segs);
        segs = temp;
    }
}
