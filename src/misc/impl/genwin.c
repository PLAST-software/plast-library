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

#include "genwin.h"

void decrementsv (register int* sv, register int class);
void incrementsv (register int* sv, int class);

/********************************************************************************/

#define STRSIZE 100

/********************************************************************************/

char *blastdbs[] = {
    "bba", "bbn", "embl", "gbupdate", "genbank", "genpept", "gpupdate",
    "nr", "nrdb", "nrdb.shuf", "pir", "pseq", "swissprot", "tfdaa"
};

int nblastdbs = 14;

int nabets;
struct Alphabet** abets;

int ntvecs;
struct TransVector** tvecs;

int nsvecs;
struct ScoreVector** svecs;

int nsmats;
struct ScoreMatrix** smats;

int aaindex[128];
unsigned char	aaflag[128];
char aachar[20];

struct strlist
{
    char string[STRSIZE];
    struct strlist *next;
} *str, *curstr;

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

    qsort (win->state, nel, sizeof(win->state[0]), state_cmp);
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
static double	*entray;

#define LN2	0.69314718055994530941723212145818

void entropy_init (int	window)
{
    int		i;
    double	x, xw;

    entray = (double *)malloc((window+1) * sizeof(*entray));
    xw = window;
    for (i = 1; i <= window; ++i) {
        x = i / xw;
        entray[i] = -x * log(x) / LN2;
    }

    thewindow = window;
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
    if (entray != 0)  { free (entray); }
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
struct Sequence* readentry (struct Database *dbase)
{
    struct Sequence *seq;
    int	c;

    seq = (struct Sequence *) malloc(sizeof(struct Sequence));

    seq->db         = dbase;
    seq->parent     = (struct Sequence *) NULL;
    seq->root       = (struct Sequence *) NULL;
    seq->children   = (struct Sequence **) NULL;
    seq->rubberwin  = FALSE;
    seq->floatwin   = FALSE;

    if (!readhdr(seq))
    {
        return((struct Sequence *) NULL);
    }

    while (1)  /*---[skip multiple headers]---*/
    {
        c = getc(dbase->fp);

        if (c == EOF)
            break;

        if (c != '>')
        {
            ungetc(c, dbase->fp);
            break;
        }

        while ((c=getc(dbase->fp)) != EOF && c !='\n')
            ;

        if (c == EOF)
            break;
    }
    readseq(seq);

    seq->entropy     = -2.;
    seq->state       = (int *) NULL;
    seq->composition = (int *) NULL;
    seq->classvec    = (char *) NULL;
    seq->scorevec    = (double *) NULL;

    return (seq);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int readhdr (struct Sequence *seq)
{
    FILE *fp;
    char *bptr, *curpos;
    int	c, i, itotal;
    int idend, namend, orgend;

    fp = seq->db->fp;

    if ((c=getc(fp)) == EOF)
    {
        free(seq);
        return (FALSE);
    }

    while (c != EOF && isspace(c))
    {
        c = getc(fp);
    }

    if (c!='>')
    {
        fprintf(stderr, "Error reading fasta format - '>' not found.\n");
        exit(1);
    }
    ungetc(c, fp);

    /*                                               ---[read the header line]---*/
    str = (struct strlist *) malloc (sizeof(struct strlist));
    str->next = NULL;
    curstr = str;

    for (i=0,itotal=0,c=getc(fp); c != EOF; c=getc(fp))
    {
        if (c=='\n') break;

        if (i==STRSIZE-1)
        {
            curstr->string[i] = '\0';
            curstr->next = (struct strlist *) malloc (sizeof(struct strlist));
            curstr = curstr->next;
            curstr->next = NULL;
            i = 0;
        }

        curstr->string[i] = c;
        itotal++;
        i++;
    }

    curstr->string[i] = '\0';
    seq->header = (char *) malloc (itotal+2);
    seq->header[0] = '\0';

    for (curstr=str, curpos=seq->header; curstr!=NULL;)
    {
        if (curstr->next==NULL)
        {
            memccpy(curpos, curstr->string, '\0', STRSIZE);
        }
        else
        {
            memccpy(curpos, curstr->string, '\0', STRSIZE-1);
        }

        str = curstr;
        curstr = curstr->next;
        free (str);

        if (curstr!=NULL)   {  curpos = curpos+STRSIZE-1;  }
    }

    bptr = (seq->header)+1;
    seq->name     = (char *) NULL;
    seq->organism = (char *) NULL;
    /*                                                   ---[parse out the id]---*/
    idend = findchar(bptr, ' ');
    if (idend==-1)  {  idend = findchar(bptr, '\n');  }
    if (idend==-1)  {  idend = findchar(bptr, '\0');  }
    if (idend==-1)
    {
        fprintf(stderr, "Error parsing header line - id.\n");
        fputs(seq->header, fp);
        exit(1);
    }

    seq->id = (char *) malloc((idend+1)*sizeof(char));
    memcpy(seq->id, bptr, idend);
    seq->id[idend] = '\0';

    if (bptr[idend]=='\n' || bptr[idend]=='\0')   {  return(TRUE);  }

    /*                                         ---[parse out the protein name]---*/
    bptr = bptr + idend + 1;
    while (bptr[0]==' ')  {  bptr++;  }

    namend = findchar(bptr, '-');
    if (namend==-1) {  namend = findchar(bptr, '\n');  }
    if (namend==-1) {  namend = findchar(bptr, '\0');  }
    if (namend==-1)
    {
        fprintf(stderr, "Error parsing header line - name.\n");
        fputs(seq->header, fp);
        return(TRUE);
    }

    seq->name = (char *) malloc((namend+1)*sizeof(char));
    memcpy(seq->name, bptr, namend);
    seq->name[namend] = '\0';

    if (bptr[namend]=='\n' || bptr[namend]=='\0')   {  return(TRUE);  }

    /*                                                 ---[parse out organism]---*/
    bptr = bptr + namend + 1;
    while (bptr[0]==' ') {bptr++;}

    orgend = findchar(bptr, '|');
    if (orgend==-1) {  orgend = findchar(bptr, '#');   }
    if (orgend==-1) {  orgend = findchar(bptr, '\n');  }
    if (orgend==-1) {  orgend = findchar(bptr, '\0');  }
    if (orgend==-1)
    {
        fprintf(stderr, "Error parsing header line - organism.\n");
        fputs(seq->header, fp);
        return(TRUE);
    }

    seq->organism = (char *) malloc((orgend+1)*sizeof(char));
    memcpy(seq->organism, bptr, orgend);
    seq->organism[orgend] = '\0';

    /*                                    ---[skip over multiple header lines]---*/
    while (TRUE)
    {
        c = getc(fp);
        if (c == EOF)
            return(TRUE);
        if (c=='>')
        {
            skipline(fp);
        }
        else
        {
            ungetc(c,fp);
            break;
        }
    }

    return(TRUE);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void skipline (FILE *fp)
{
    int	c;
    while ((c=getc(fp))!='\n' && c!=EOF)   ;
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
int findchar (char *str, char chr)
{
    int i;

    for (i=0; ; i++)
    {
        if (str[i]==chr)
        {
            return(i);
        }
        if (str[i]=='\0')
        {
            return(-1);
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
void readseq (struct Sequence* seq)
{
    FILE *fp;
    int i, itotal;
    int	c;
    char *curpos;

    fp = seq->db->fp;

    seq->punctuation = FALSE;

    str = (struct strlist *) malloc (sizeof(struct strlist));
    str->next = NULL;
    curstr = str;

    for (i = 0, itotal = 0, c = getc(fp); c != EOF; c = getc(fp))
    {
        if (!aaflag[c])
        {
            Keep:
            if (i < STRSIZE-1)
            {
                curstr->string[i++] = c;
                continue;
            }
            itotal += STRSIZE-1;
            curstr->string[STRSIZE-1] = '\0';
            curstr->next = (struct strlist *) malloc(sizeof(*curstr));
            curstr = curstr->next;
            curstr->next = NULL;
            curstr->string[0] = c;
            i = 1;
            continue;
        }

        switch (c)
        {
        case '>':
            ungetc(c, fp);
            goto EndLoop;
        case '*': case '-':
            seq->punctuation = TRUE;
            goto Keep;
        case 'b': case 'B':
        case 'u': case 'U': /* selenocysteine */
        case 'x': case 'X':
        case 'z': case 'Z':
            goto Keep;
        default:
            continue;
        }
    }
    EndLoop:
    itotal += i;

    curstr->string[i] = '\0';
    seq->seq = (char *) malloc (itotal+2);
    seq->seq[0] = '\0';

    for (curstr = str, curpos = seq->seq; curstr != NULL;)
    {
        if (curstr->next == NULL)
            memccpy(curpos, curstr->string, '\0', STRSIZE);
        else
            memccpy(curpos, curstr->string, '\0', STRSIZE-1);

        str = curstr;
        curstr = curstr->next;
        free(str);

        if (curstr != NULL)
            curpos = curpos+STRSIZE-1;
    }

    seq->length = strlen(seq->seq);
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

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int min (int a, int b)
{
    if (a<b)    { return(a); }
    else        { return(b); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int max (int a, int b)
{
    if (a<b)    { return(b); }
    else        { return(a); }
}
