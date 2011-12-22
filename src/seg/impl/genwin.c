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

void decrementsv (register int* sv, register int class);
void incrementsv (register int* sv, int class);

/********************************************************************************/

static int              aaindex[128];
static unsigned char	aaflag[128];
static char             aachar[20];

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void genwininit (void)
{
    static char first = 1;

    if (first)
    {
        char *cp, *cp0;
        int		i;
        char	c;

        for (i = 0; i < sizeof(aaindex)/sizeof(aaindex[0]); ++i)
        {
            aaindex[i] = 20;
            aaflag[i] = TRUE;
        }

        for (cp = cp0 = "ACDEFGHIKLMNPQRSTVWY"; (c = *cp) != '\0'; ++cp)
        {
            i = cp - cp0;
            aaindex[(int)c] = i;
            aaindex[tolower(c)] = i;
            aachar[i] = tolower(c);
            aaflag[(int)c] = FALSE;
            aaflag[tolower(c)] = FALSE;
        }

        first = 0;
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
struct Sequence* openwin (struct Sequence *parent,  int start, int length)
{
    struct Sequence* win;

    if (start<0 || length<0 || start+length>parent->length)
    {
        return((struct Sequence *) NULL);
    }

    win = (struct Sequence *) malloc(sizeof(struct Sequence));

    /*------[set links, up and down]---*/

    win->parent = parent;
    if (parent->root==NULL)     {  win->root = parent;          }
    else                        {  win->root = parent->root;    }
    win->children = (struct Sequence **) NULL;

    /* parent->children = ***foo*** ---[not yet implemented]---*/

    win->id         = (char *) NULL;
    win->name       = (char *) NULL;
    win->organism   = (char *) NULL;
    win->header     = (char *) NULL;

    /*------[install the local copy of the sequence]---*/

    win->start  = start;
    win->length = length;

#if 0
    win->seq = (char *) malloc(sizeof(char)*length + 1);
    memcpy(win->seq, (parent->seq)+start, length);
    win->seq[length] = '\0';
#else
    win->seq = parent->seq + start;
#endif

    /*------[setup window implementation parameters]---*/

    /*------[set local flags]---*/

    win->rubberwin      = FALSE;
    win->floatwin       = FALSE;
    win->punctuation    = FALSE;

    /*------[initially unconfiguerd window]---*/

    win->entropy        = -2.;
    win->state          = (int*)    NULL;
    win->composition    = (int*)    NULL;
    win->classvec       = (char*)   NULL;
    win->scorevec       = (double*) NULL;

    stateon (win);

    return win;
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
struct Sequence* nextwin (struct Sequence* win, int shift)
{
    if ((win->start+shift)<0 || (win->start+win->length+shift)>win->parent->length)
    {
        return (struct Sequence *) NULL;
    }
    else
    {
        return openwin (win->parent, win->start+shift, win->length);
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
int shiftwin1 (struct Sequence	*win)
{
    register int	j, length;
    register int	*comp;

    length = win->length;
    comp = win->composition;

    if ((++win->start + length) > win->parent->length)
    {
        --win->start;
        return FALSE;
    }

    if (!aaflag[j = win->seq[0]])
        decrementsv(win->state, comp[aaindex[j]]--);

    j = win->seq[length];
    ++win->seq;

    if (!aaflag[j])
        incrementsv(win->state, comp[aaindex[j]]++);

    if (win->entropy > -2.)
        win->entropy = entropy(win->state);

    return TRUE;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void closewin (struct Sequence* win)
{
    if (win==NULL) return;

    if (win->state!=NULL)       free(win->state);
    if (win->composition!=NULL) free(win->composition);
    if (win->classvec!=NULL)    free(win->classvec);
    if (win->scorevec!=NULL)    free(win->scorevec);

    free(win);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void compon (struct Sequence* win)
{
    register int	*comp;
    register int	aa;
    register char	*seq, *seqmax;

    win->composition = comp = (int *) calloc(20*sizeof(*comp), 1);
    seq = win->seq;
    seqmax = seq + win->length;

    while (seq < seqmax)
    {
        aa = *seq++;
        if (!aaflag[aa])
            comp[aaindex[aa]]++;
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
static int state_cmp (int* s1, int* s2)
{
    return *s2 - *s1;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void stateon (struct Sequence* win)
{
    register int	aa, nel, c;

    if (win->composition == NULL)
        compon(win);

    if (win->state == NULL)
        win->state = (int *) malloc(21*sizeof(win->state[0]));

    for (aa = nel = 0; aa < 20; ++aa) {
        if ((c = win->composition[aa]) == 0)
            continue;
        win->state[nel++] = c;
    }
    for (aa = nel; aa < 21; ++aa)
        win->state[aa] = 0;

    qsort (win->state, nel, sizeof(win->state[0]), (int (*) (const void *,const void *)) state_cmp);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void enton (struct Sequence* win)
{
    if (win->state==NULL) {stateon(win);}

    win->entropy = entropy(win->state);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
static int		thewindow;
static double	entray[256];

#define LN2	0.69314718055994530941723212145818

void entropy_init (int	window)
{
    static char first = 1;

    if (first)
    {
        int		i;
        double	x, xw;

        if (window >= sizeof(entray)/sizeof(entray[0]))
        {
            fprintf (stderr, "entropy_init: window size too big (%d)\n", window);
            exit (1);
        }

        xw = window;
        for (i = 1; i <= window; ++i)
        {
            x = i / xw;
            entray[i] = -x * log(x) / LN2;
        }

        thewindow = window;

        first = 0;
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
void entropy_exit ()
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
double entropy (register int* sv)
{
    int	*sv0 = sv;
    register double	ent;
    register int	i, total;
    register int	*svmax;
    register double	xtotrecip, xsv;

    for (total = 0; (i = *sv) != 0; ++sv)
        total += i;
    svmax = sv;
    ent = 0.0;
    if (total == thewindow)
    {
        for (sv = sv0; sv < svmax; )
        {
            ent += entray[*sv++];
        }
        return ent;
    }

    if (total == 0)
        return 0.;

    xtotrecip = 1./(double)total;
    for (sv = sv0; sv < svmax; )
    {
        xsv = *sv++;
        ent += xsv * log(xsv * xtotrecip);
    }

    return -ent * xtotrecip / LN2;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void decrementsv (register int* sv, register int class)
{
    register int	svi;

    while ((svi = *sv++) != 0)
    {
        if (svi == class && *sv < class)
        {
            sv[-1] = svi - 1;
            break;
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
void incrementsv (register int* sv, int class)
{
    for (;;)
    {
        if (*sv++ == class)
        {
            sv[-1]++;
            break;
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
void upper (register char* string, size_t len)
{
    register char	*stringmax, c;

    for (stringmax = string + len; string < stringmax; ++string)
        if (islower(c = *string))
            *string = toupper(c);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void lower (char* string, size_t len)
{
    register char	*stringmax, c;

    for (stringmax = string + len; string < stringmax; ++string)
        if (isupper(c = *string))
            *string = tolower(c);
}
