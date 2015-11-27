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

#include <index/impl/retrievall.h>

void alloc_queue(queue_t *  queuep,unsigned int nedges)
{
	(*queuep)=malloc(nedges*sizeof(unsigned int));
};
void free_queue(queue_t * queue)
{
	free(*queue);
};

void alloc_graph3(graph3_t * graph3,unsigned int nedges,unsigned int nvertices)
{
	graph3->edges=malloc(nedges*sizeof(edge));
	graph3->first_edge=malloc(nvertices*sizeof(unsigned int));
	graph3->vert_degree=malloc(nvertices);
	graph3->marked_edge=malloc((nedges+31)/8);
};
void init_graph3(graph3_t * graph3,unsigned int nedges,unsigned int nvertices)
{
	memset(graph3->first_edge,0xff,nvertices*sizeof(unsigned int));
	memset(graph3->vert_degree,0,nvertices);
	memset(graph3->marked_edge,0,(nedges+31)/8);	
	graph3->nedges=0;
};
void free_graph3(graph3_t *graph3)
{
	free(graph3->edges);
	free(graph3->first_edge);
	free(graph3->vert_degree);
};


void dump_graph(graph3_t* graph3,unsigned int nedges,unsigned int nvertices)
{
	int i;
	for(i=0;i<nedges;i++){
		printf("\nedge %d %d %d %d ",i,graph3->edges[i].vertices[0],
				graph3->edges[i].vertices[1],graph3->edges[i].vertices[2]);
		printf(" nexts %d %d %d",graph3->edges[i].next_edges[0],
				graph3->edges[i].next_edges[1],graph3->edges[i].next_edges[2]);
	};
	
	for(i=0;i<nvertices;i++){
		printf("\nfirst for vertice %d %d ",i,graph3->first_edge[i]);
	
	};
};

/*void get_vertices(unsigned char * key,unsigned int * verts,unsigned int nverts)
{
	if(verts[0]==verts[1])
		verts[1]=(verts[0]+1)%nverts;
	if(verts[1]==verts[2])
		verts[2]=(verts[1]+1)%nverts;
	if(verts[2]==verts[0]){
		verts[2]=(verts[0]+1)%nverts;
		if(verts[2]==verts[1])
			verts[2]=(verts[1]+1)%nverts;
	};
};*/

void remove_edge(graph3_t * graph3,unsigned int curr_edge)
{
	unsigned int i,j,vert,edge1,edge2;
	j=0;
	for(i=0;i<3;i++){
		vert=graph3->edges[curr_edge].vertices[i];
		edge1=graph3->first_edge[vert];
		edge2=NULL_EDGE;
		while(edge1!=curr_edge&&edge1!=NULL_EDGE){
			edge2=edge1;
			if(graph3->edges[edge1].vertices[0]==vert){
				j=0;
			} else if(graph3->edges[edge1].vertices[1]==vert){
				j=1;
			} else 
				j=2;
			edge1=graph3->edges[edge1].next_edges[j];
		}
		if(edge1==NULL_EDGE)
			continue;
		if(edge2!=NULL_EDGE){
			graph3->edges[edge2].next_edges[j]=
				graph3->edges[edge1].next_edges[i];
		} else 
			graph3->first_edge[vert]=
				graph3->edges[edge1].next_edges[i];
		graph3->vert_degree[vert]--;
	};
	
};
int generate_queue2(unsigned int nedges,unsigned int nvertices, queue_t queue,graph3_t* graph3)
{
	unsigned int curr_edge;
	unsigned int curr_vert,v1,v2,v3;
	unsigned int queue_head=0,queue_tail=0;
	curr_vert=0;
	do{
		do {
			if(curr_vert==nvertices) 
				goto loop_end;
			if(graph3->vert_degree[curr_vert]==1)
				break;
			curr_vert++;
		} while(1);
		curr_edge=graph3->first_edge[curr_vert++];
		remove_edge(graph3,curr_edge);
		queue[queue_head++]=curr_edge;
		do 
		{
			curr_edge=queue[queue_tail++];
			v1=graph3->edges[curr_edge].vertices[0];
			v2=graph3->edges[curr_edge].vertices[1];
			v3=graph3->edges[curr_edge].vertices[2];
			if(graph3->vert_degree[v1]==1 ) {
				curr_edge=graph3->first_edge[v1];
				remove_edge(graph3,curr_edge);
				queue[queue_head++]=curr_edge;
			};
			if(graph3->vert_degree[v2]==1 ) {
				curr_edge=graph3->first_edge[v2];
				remove_edge(graph3,curr_edge);
				queue[queue_head++]=curr_edge;
			};
			if(graph3->vert_degree[v3]==1 ) {
				curr_edge=graph3->first_edge[v3];
				remove_edge(graph3,curr_edge);
				queue[queue_head++]=curr_edge;
			};
		} while(queue_head!=queue_tail);
	}while(1);
loop_end:
	return queue_head-nedges;/* returns 0 if successful otherwies return negative number*/
};


#define hashsize(n) ((unsigned int)1<<(n))
#define hashmask(n) (hashsize(n)-1)



//#define NM2 /* Define this if you do not want power of 2 table sizes*/


/*
   --------------------------------------------------------------------
   mix -- mix 3 32-bit values reversibly.
   For every delta with one or two bits set, and the deltas of all three
   high bits or all three low bits, whether the original value of a,b,c
   is almost all zero or is uniformly distributed,
 * If mix() is run forward or backward, at least 32 bits in a,b,c
 have at least 1/4 probability of changing.
 * If mix() is run forward, every bit of c will change between 1/3 and
 2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)
 mix() was built out of 36 single-cycle latency instructions in a 
 structure that could supported 2x parallelism, like so:
 a -= b; 
 a -= c; x = (c>>13);
 b -= c; a ^= x;
 b -= a; x = (a<<8);
 c -= a; b ^= x;
 c -= b; x = (b>>13);
 ...
 Unfortunately, superscalar Pentiums and Sparcs can't take advantage 
 of that parallelism.  They've also turned some of those single-cycle
 latency instructions into multi-cycle latency instructions.  Still,
 this is the fastest good hash I could find.  There were about 2^^68
 to choose from.  I only looked at a billion or so.
 --------------------------------------------------------------------
 */
#if 0
#define mix(_a_,_b_,_c_) \
{ \
    unsigned int a = _a_; \
    unsigned int b = _b_; \
    unsigned int c = _c_; \
    a -= b; a -= c; a ^= (c>>13); \
    b -= c; b -= a; b ^= (a<<8); \
    c -= a; c -= b; c ^= (b>>13); \
    a -= b; a -= c; a ^= (c>>12);  \
    b -= c; b -= a; b ^= (a<<16); \
    c -= a; c -= b; c ^= (b>>5); \
    a -= b; a -= c; a ^= (c>>3);  \
    b -= c; b -= a; b ^= (a<<10); \
    c -= a; c -= b; c ^= (b>>15); \
    _a_ = a; \
    _b_ = b; \
    _c_ = c; \
}
#else
#define mix(a,b,c) \
{ \
    a -= b; a -= c; a ^= (c>>13); \
    b -= c; b -= a; b ^= (a<<8); \
    c -= a; c -= b; c ^= (b>>13); \
    a -= b; a -= c; a ^= (c>>12);  \
    b -= c; b -= a; b ^= (a<<16); \
    c -= a; c -= b; c ^= (b>>5); \
    a -= b; a -= c; a ^= (c>>3);  \
    b -= c; b -= a; b ^= (a<<10); \
    c -= a; c -= b; c ^= (b>>15); \
}
#endif

/*
   --------------------------------------------------------------------
   hash() -- hash a variable-length key into a 32-bit value
k       : the key (the unaligned variable-length array of bytes)
len     : the length of the key, counting by bytes
initval : can be any 4-byte value
Returns a 32-bit value.  Every bit of the key affects every bit of
the return value.  Every 1-bit and 2-bit delta achieves avalanche.
About 6*len+35 instructions.

The best hash table sizes are powers of 2.  There is no need to do
mod a prime (mod is sooo slow!).  If you need less than 32 bits,
use a bitmask.  For example, if you need only 10 bits, do
h = (h & hashmask(10));
In which case, the hash table should have hashsize(10) elements.

If you are hashing n strings (cmph_uint8 **)k, do it like this:
for (i=0, h=0; i<n; ++i) h = hash( k[i], len[i], h);

By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net.  You may use this
code any way you wish, private, educational, or commercial.  It's free.

See http://burtleburtle.net/bob/hash/evahash.html
Use for hash table lookup, or anything where one collision in 2^^32 is
acceptable.  Do NOT use for cryptographic purposes.
--------------------------------------------------------------------
 */
unsigned int jenkins_hash(unsigned int seed, const char *k,unsigned int keylen)
{
	unsigned int a, b, c;
	unsigned int len, length;

	/* Set up the internal state */
	length = keylen;
	len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = seed;   /* the previous hash value - seed in our case */

	/*---------------------------------------- handle most of the key */
	while (len >= 12)
	{
		a += (k[0] +((unsigned int)k[1]<<8) +((unsigned int)k[2]<<16) +((unsigned int)k[3]<<24));
		b += (k[4] +((unsigned int)k[5]<<8) +((unsigned int)k[6]<<16) +((unsigned int)k[7]<<24));
		c += (k[8] +((unsigned int)k[9]<<8) +((unsigned int)k[10]<<16)+((unsigned int)k[11]<<24));
		mix(a,b,c);
		k += 12; len -= 12;
	}

	/*------------------------------------- handle the last 11 bytes */
	c  += length;
	switch(len)              /* all the case statements fall through */
	{
		case 11: 
			c +=((unsigned int)k[10]<<24);
		case 10: 
			c +=((unsigned int)k[9]<<16);
		case 9 : 
			c +=((unsigned int)k[8]<<8);
			/* the first byte of c is reserved for the length */
		case 8 : 
			b +=((unsigned int)k[7]<<24);
		case 7 : 
			b +=((unsigned int)k[6]<<16);
		case 6 : 
			b +=((unsigned int)k[5]<<8);
		case 5 : 
			b +=k[4];
		case 4 : 
			a +=((unsigned int)k[3]<<24);
		case 3 : 
			a +=((unsigned int)k[2]<<16);
		case 2 : 
			a +=((unsigned int)k[1]<<8);
		case 1 : 
			a +=k[0];
			/* case 0: nothing left to add */
			break;
	}

	mix(a,b,c);

	/*-------------------------------------------- report the result */

	//c = (c & hashmask(state->size));
	//c = (c >= state->size) ? c ^ state->size: c;  

	//state->last_hash = c; Do not update last_hash because we use a fixed
	//seed
	return c;
}

void jenkins_hash_vec1(unsigned int seed, const char *k, unsigned int length,unsigned int * res_vec)
{
	unsigned int len=length;
	res_vec[0] = res_vec[1] = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	res_vec[2] = seed;   /* the previous hash value - seed in our case */

	/*---------------------------------------- handle most of the key */
	while (len >= 12)
	{
		res_vec[0] += (k[0] +((unsigned int)k[1]<<8) +((unsigned int)k[2]<<16) +((unsigned int)k[3]<<24));
		res_vec[1] += (k[4] +((unsigned int)k[5]<<8) +((unsigned int)k[6]<<16) +((unsigned int)k[7]<<24));
		res_vec[2] += (k[8] +((unsigned int)k[9]<<8) +((unsigned int)k[10]<<16)+((unsigned int)k[11]<<24));

#if 1
		unsigned int rv0 = res_vec[0];
        unsigned int rv1 = res_vec[1];
        unsigned int rv2 = res_vec[2];
		mix (rv0,rv1,rv2);
		res_vec[0] = rv0;
        res_vec[1] = rv1;
        res_vec[2] = rv2;
#else
        mix(res_vec[0],res_vec[1],res_vec[2]);
#endif
		k += 12; len -= 12;
	}

	/*------------------------------------- handle the last 12 bytes  and length*/
	res_vec[2]  += length;
	switch(len)              /* all the case statements fall through */
	{
		case 11: 
			res_vec[2] +=((unsigned int)k[10]<<24);
		case 10 : 
			res_vec[2] +=((unsigned int)k[9]<<16);
		case 9 : 
			res_vec[2] +=((unsigned int)k[8]<<8);
			/* the first two bytee of res_vec[2] are reserved for the length */
		case 8 : 
			res_vec[1] +=((unsigned int)k[7]<<24);
		case 7 : 
			res_vec[1] +=((unsigned int)k[6]<<16);
		case 6 : 
			res_vec[1] +=((unsigned int)k[5]<<8);
		case 5 : 
			res_vec[1] +=k[4];
		case 4 : 
			res_vec[0] +=((unsigned int)k[3]<<24);
		case 3 : 
			res_vec[0] +=((unsigned int)k[2]<<16);
		case 2 : 
			res_vec[0] +=((unsigned int)k[1]<<8);
		case 1 : 
			res_vec[0] +=k[0];
			/* case 0: nothing left to add */
	}
	mix(res_vec[0],res_vec[1],res_vec[2]);
};

/* Next function is reserved for hashes for strings with lentgh in bits instead of bytes*/

void jenkins_hash_vec2(unsigned int seed, const char *k, unsigned int length,unsigned int * res_vec)
{
	unsigned int len;
	len = (length + 7 )/ 8 ;
	unsigned char last_char = k[len-1] >> ((8 - (length % 8)) % 8);
	res_vec[0] = res_vec[1] = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	res_vec[2] = seed;   /* the previous hash value - seed in our case */
	/*---------------------------------------- handle most of the key */
	while (len >= 13)
	{
		res_vec[0] += (k[0] +((unsigned int)k[1]<<8) +((unsigned int)k[2]<<16) +((unsigned int)k[3]<<24));
		res_vec[1] += (k[4] +((unsigned int)k[5]<<8) +((unsigned int)k[6]<<16) +((unsigned int)k[7]<<24));
		res_vec[2] += (k[8] +((unsigned int)k[9]<<8) +((unsigned int)k[10]<<16)+((unsigned int)k[11]<<24));
		mix(res_vec[0],res_vec[1],res_vec[2]);
		k += 12; len -= 12;
	}

	/*------------------------------------- handle the last 12 bytes  and length*/
	res_vec[2]  += length;
	switch(len)              /* all the case statements fall through */
	{
		case 12 : 
			res_vec[0] +=((unsigned int)k[10]<<8);
		case 11 : 
			res_vec[0] +=k[9];
		mix(res_vec[0],res_vec[1],res_vec[2]);
		case 10 : 
			res_vec[2] +=((unsigned int)k[8]<<24);
		case 9 : 
			res_vec[2] +=((unsigned int)k[7]<<16);
			/* the first two bytee of res_vec[2] are reserved for the length */
		case 8 : 
			res_vec[1] +=((unsigned int)k[6]<<24);
		case 7 : 
			res_vec[1] +=((unsigned int)k[5]<<16);
		case 6 : 
			res_vec[1] +=((unsigned int)k[4]<<8);
		case 5 : 
			res_vec[1] +=k[3];
		case 4 : 
			res_vec[0] +=((unsigned int)k[2]<<24);
		case 3 : 
			res_vec[0] +=((unsigned int)k[1]<<16);
		case 2 : 
			res_vec[0] +=((unsigned int)k[0]<<8);
		case 1 : 
			res_vec[0] += last_char;
			/* case 0: nothing left to add */
	}
	mix(res_vec[0],res_vec[1],res_vec[2]);
};
/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializes mt[N] with a seed */
void init_genrand(unsigned long long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] = 
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void init_by_array(unsigned long long init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (N>key_length ? N : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long long genrand_int32(void)
{
    unsigned long long y;
    static unsigned long long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }
  
    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long genrand_int31(void)
{
    return (long)(genrand_int32()>>1);
}

/* generates a random number on [0,1]-real-interval */
double genrand_real1(void)
{
    return genrand_int32()*(1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
}

/* generates a random number on [0,1)-real-interval */
double genrand_real2(void)
{
    return genrand_int32()*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double genrand_real3(void)
{
    return (((double)genrand_int32()) + 0.5)*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double genrand_res53(void) 
{ 
    unsigned long long a=genrand_int32()>>5, b=genrand_int32()>>6; 
    return(a*67108864.0+b)*(1.0/9007199254740992.0); 
} 
/* These real versions are due to Isaku Wada, 2002/01/09 added */

unsigned char retr_count_table[256] ={
   0 , 1 , 1 , 2 , 1 , 2 , 2 , 3 , 1 , 2 , 2 , 3 , 2 , 3 , 3 , 4
,  1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 , 2 , 3 , 3 , 4 , 3 , 4 , 4 , 5
,  1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 , 2 , 3 , 3 , 4 , 3 , 4 , 4 , 5
,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 , 3 , 4 , 4 , 5 , 4 , 5 , 5 , 6
,  1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 , 2 , 3 , 3 , 4 , 3 , 4 , 4 , 5
,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 , 3 , 4 , 4 , 5 , 4 , 5 , 5 , 6
,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 , 3 , 4 , 4 , 5 , 4 , 5 , 5 , 6
,  3 , 4 , 4 , 5 , 4 , 5 , 5 , 6 , 4 , 5 , 5 , 6 , 5 , 6 , 6 , 7
,  1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 , 2 , 3 , 3 , 4 , 3 , 4 , 4 , 5
,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 , 3 , 4 , 4 , 5 , 4 , 5 , 5 , 6
,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 , 3 , 4 , 4 , 5 , 4 , 5 , 5 , 6
,  3 , 4 , 4 , 5 , 4 , 5 , 5 , 6 , 4 , 5 , 5 , 6 , 5 , 6 , 6 , 7
,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 , 3 , 4 , 4 , 5 , 4 , 5 , 5 , 6
,  3 , 4 , 4 , 5 , 4 , 5 , 5 , 6 , 4 , 5 , 5 , 6 , 5 , 6 , 6 , 7
,  3 , 4 , 4 , 5 , 4 , 5 , 5 , 6 , 4 , 5 , 5 , 6 , 5 , 6 , 6 , 7
,  4 , 5 , 5 , 6 , 5 , 6 , 6 , 7 , 5 , 6 , 6 , 7 , 6 , 7 , 7 , 8 
 };
unsigned char retr_pack3_table[15]={0,1,2,0,3,6,0,9,18,0,27,54,0,81,162};
unsigned char retr_decode3_table[256*5] ={
0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 ,
1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 ,
2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 ,
0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 ,
1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 ,
2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 ,
0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 ,
1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 ,
2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 ,
0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 ,
1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 ,
2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 ,
0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 ,
1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 ,
2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 ,
0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 ,
0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 ,
2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 ,
1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 ,
1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 ,
0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 ,
2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 ,
2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 ,
1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 ,
0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 ,
0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 ,
2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 ,
1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 ,
1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 ,
0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 ,
2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 ,
2 , 2 , 2 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 2 , 0 , 0 , 0 , 1 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 ,
2 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 ,
2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 ,
2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 1 , 1 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };
unsigned char retr_pack5_table[15]={0,1,2,3,4,0,5,10,15,20,0,25,50,75,100};
unsigned char retr_decode5_table[128*3]={
0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,
2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,
3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,
4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,
0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,
2,3,4,0,1,2,3,4,0,1,2,3,4,0,0,0,
0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,
3,3,3,3,4,4,4,4,4,0,0,0,0,0,1,1,
1,1,1,2,2,2,2,2,3,3,3,3,3,4,4,4,
4,4,0,0,0,0,0,1,1,1,1,1,2,2,2,2,
2,3,3,3,3,3,4,4,4,4,4,0,0,0,0,0,
1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,4,
4,4,4,4,0,0,0,0,0,1,1,1,1,1,2,2,
2,2,2,3,3,3,3,3,4,4,4,4,4,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,4,4,4,0,0,0};
unsigned char retr_mod3_table[9]={0,1,2,0,1,2,0,1,2}; 
unsigned char retr_mod5_table[15]={0,1,2,3,4,0,1,2,3,4,0,1,2,3,4}; 
unsigned char retr_mod6_table[18]={0,1,2,3,4,5,0,1,2,3,4,5,0,1,2,3,4,5}; 
unsigned char retr_fls_table[256]={ 
8 , 7 , 6 , 6 , 5 , 5 , 5 , 5 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 4,
3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3,
2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2,
2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };


#define bits_per_byte 8
inline void get_hash1(retr_t * retr,char * key,unsigned int *v)
{
	unsigned int key_len;
	if(retr->keys_length)
		key_len=retr->keys_length;
	else
	{
		key_len=strlen(key);
	};
	jenkins_hash_vec1(retr->seed,key,key_len,v);
	v[0]%=retr->nverts;
	v[1]%=(retr->nverts-1);
	v[2]%=(retr->nverts-2);
};

/* the same as gethash1 except that length is in bits instead of bytes*/

inline void get_hash1_b(retr_t * retr,char * key,unsigned int key_len,unsigned int *v)
{
	jenkins_hash_vec2(retr->seed,key,key_len,v);
	v[0]%=retr->nverts;
	v[1]%=(retr->nverts-1);
	v[2]%=(retr->nverts-2);
	//printf("\n verts are %d %d %d",v[0],v[1],v[2]);
};


inline void get_hash2(retr_t*retr,unsigned int i,unsigned int *v)
{
	unsigned int key_len;
	char * key;
	if(retr->length_in_bits)
	{
		key=((char**)retr->keys_table)[i];
		key_len=retr->keys_lengths[i];
		jenkins_hash_vec2(retr->seed,key,key_len,v);
	}
	else 
	{
		if(retr->keys_length)
		{
			key_len=retr->keys_length;
			key=&retr->keys_table[i*key_len];
		}
		else
		{
			key=((char**)retr->keys_table)[i];
			key_len=strlen(key);
		};
		jenkins_hash_vec1(retr->seed,key,key_len,v);
	}
	v[0]%=retr->nverts;
	v[1]%=(retr->nverts-1);
	v[2]%=(retr->nverts-2);
	//printf("\n verts are %d %d %d",v[0],v[1],v[2]);
};

inline void get_vertices(retr_t * retr,char * key,unsigned int * v,unsigned int nverts)
{
	get_hash1(retr,key,v);
	if(v[0]==v[1])
		v[1]=retr->nverts-1;
	if(v[2]==v[0])
	{
  		v[2]=retr->nverts-2;
  		if(v[2]==v[1])
    			v[2]=retr->nverts-1;
	}else if(v[2]==v[1])
	{
  		v[2]=retr->nverts-1;
  		if(v[2]==v[0])
    			v[2]=retr->nverts-2;
	};
};

inline void get_vertices_b(retr_t * retr,char * key,unsigned int key_len,unsigned int * v,unsigned int nverts)
{
	get_hash1_b(retr,key,key_len,v);
	if(v[0]==v[1])
		v[1]=retr->nverts-1;
	if(v[2]==v[0])
	{
  		v[2]=retr->nverts-2;
  		if(v[2]==v[1])
    			v[2]=retr->nverts-1;
	}else if(v[2]==v[1])
	{
  		v[2]=retr->nverts-1;
  		if(v[2]==v[0])
    			v[2]=retr->nverts-2;
	};
};


/* is_packed=1 means that retrieval data structure will be packed. Keys_lentghs=0 means that we will have keys of variable length*/
void init_retr(retr_t* retr)
{
	if(retr->vals_length>0)
		retr->bits_mask=(1<<retr->vals_length)-1;
	retr->nverts=(retr->nkeys*123)/100+2;
	//printf("\n nverts is %d",retr->nverts);
	if(retr->vals_length<=6 && retr->is_packed==retr_is_packed)
		retr->is_packed=0;
	retr->space_usage=sizeof(retr_t)*bits_per_byte;
	switch(retr->is_packed)
	{
		case 0:	
		retr->retr_table=(unsigned int *)malloc(bits_table_size(retr->nverts,retr->vals_length)*
			sizeof(unsigned int));
		retr->space_usage+=bits_table_size(retr->nverts,retr->vals_length)*
			sizeof(unsigned int)*bits_per_byte;		
		break;
		case retr_is_packed:
		retr->retr_table=(unsigned int *)malloc(bits_table_size(retr->nverts,retr->vals_length)*
			sizeof(unsigned int));
		alloc_bmp(retr->bits_table,retr->nverts+(retr->nverts/256)*32+32);
		retr->packed_table=(unsigned int*)malloc(bits_table_size(retr->nkeys,retr->vals_length)*
			sizeof(unsigned int));
		retr->space_usage+=bmp_size(retr->nverts+(retr->nverts/256)*32+32)*bits_per_byte;
		retr->space_usage+=bits_table_size(retr->nkeys,retr->vals_length)*sizeof(unsigned int)*bits_per_byte;
		break;
		case retr_is_packed_3:
		retr->retr_table=(unsigned int *)calloc(((retr->nverts+4)/5+3)/4,sizeof(unsigned int));
		retr->space_usage+=((retr->nverts+4)/5+3)/4*sizeof(unsigned int)*bits_per_byte;
		break;
		case retr_is_packed_5:
		retr->retr_table=(unsigned int *)calloc(((retr->nverts+2)/3*7+31)/32,sizeof(unsigned int));
		retr->space_usage+=(((retr->nverts+2)/3*7+31)/32)*sizeof(unsigned int)*bits_per_byte;
		break;
		case retr_is_packed_6:
		retr->retr_table=(unsigned int *)calloc((((retr->nverts+4)/5)*13+31)/32,sizeof(unsigned int));
		retr->space_usage+=(((retr->nverts+4)/5)*13+31)/32*sizeof(unsigned int)*bits_per_byte;
		break;
		default:
		break;
	};
		
	
};

void free_retr(retr_t * retr)
{
	if(retr==0)
		return;
	if(retr->is_packed==retr_is_packed)
	{
		free(retr->bits_table);
		free(retr->packed_table);
	} else
		free(retr->retr_table);
};

static void generate_graph(retr_t * retr)
{
	unsigned int i=0;
	unsigned int v[3];
	init_graph3(&retr->graph3,retr->nkeys,retr->nverts);
	for(i=0;i<retr->nkeys;i++){
		get_hash2(retr,i,v);
		if(v[0]==v[1])
			v[1]=retr->nverts-1;
		if(v[2]==v[0])
		{
	  		v[2]=retr->nverts-2;
  			if(v[2]==v[1])
    				v[2]=retr->nverts-1;
		}else if(v[2]==v[1])
		{
  			v[2]=retr->nverts-1;
  			if(v[2]==v[0])
    				v[2]=retr->nverts-2;
		}
		add_edge(&retr->graph3,v[0],v[1],v[2]);
	};
	//exit(-1);
};
void static inline assign_value(retr_t * retr,unsigned int vert,unsigned int val)
{
	unsigned int tmp_bits_value;
	unsigned int out_pos,in_pos;
	switch(retr->is_packed)
	{
		case 0:
		case retr_is_packed:
		set_bits_value(retr->retr_table,vert,val&retr->bits_mask,retr->vals_length,retr->bits_mask);
		break;
		case retr_is_packed_3:
		if(val+6>8)
		{
			printf("\n out of bounds for mod3_table with value %d",val);
			exit(-1);
		};
		val=retr_mod3_table[val+6];
		in_pos=vert%5;
		out_pos=vert/5;
		((unsigned char *)retr->retr_table)[out_pos]+=retr_pack3_table[in_pos*3+val];
		break;
		case retr_is_packed_5:
		if(val+10>14)
		{
			printf("\n out of bounds for mod5_table with value %d",val);
			exit(-1);
		};

		val=retr_mod5_table[val+10];
		in_pos=vert%3;
		out_pos=vert/3;
		tmp_bits_value=get_bits_value(retr->retr_table,out_pos,7,127);
		tmp_bits_value+=retr_pack5_table[in_pos*5+val];
		set_bits_value(retr->retr_table,out_pos,tmp_bits_value,7,127);
		break;
		case retr_is_packed_6:
		if(val+12>17)
		{
			printf("\n out of bounds for mod6_table with value %d",val);
			exit(-1);
		};
		val=retr_mod6_table[val+12];
		in_pos=vert%5;
		out_pos=vert/5;
		tmp_bits_value=get_bits_value(retr->retr_table,out_pos,13,8191);
		tmp_bits_value+=retr_pack3_table[in_pos*3+val/2];
		tmp_bits_value|=(val%2)<<(8+in_pos);
		set_bits_value(retr->retr_table,out_pos,tmp_bits_value,13,8191);
		break;
		default:
		break;
	};		
};

unsigned int get_retr_value(retr_t * retr,unsigned int vert)
{
	unsigned int tmp_bits_value;
	unsigned int out_pos,in_pos;
	switch(retr->is_packed)
	{
		case 0:
		case retr_is_packed:
		return get_bits_value(retr->retr_table,vert,retr->vals_length,retr->bits_mask);
		break;
		case retr_is_packed_3:
		in_pos=vert%5;
		out_pos=vert/5;
		tmp_bits_value=((unsigned char *)retr->retr_table)[out_pos];
		return retr_decode3_table[256*in_pos+tmp_bits_value];
		break;
		case retr_is_packed_5:
		in_pos=vert%3;
		out_pos=vert/3;
		tmp_bits_value=get_bits_value(retr->retr_table,out_pos,7,127);
		return retr_decode5_table[128*in_pos+tmp_bits_value];
		break;		
		case retr_is_packed_6:
		in_pos=vert%5;
		out_pos=vert/5;
		tmp_bits_value=get_bits_value(retr->retr_table,out_pos,13,8191);
		return retr_decode3_table[256*in_pos+(tmp_bits_value&255)]*2+
			(tmp_bits_value>>(8+in_pos))%2;
		break;
		default:
		break;
	};		
	return 0;
};

static void retr_assignment(retr_t * retr)
{
	unsigned int i;
	unsigned int nedges=retr->graph3.nedges;
	unsigned int curr_edge;
	unsigned int v1,v2,v3;
	unsigned int * marked_verts;
	unsigned int value;
	unsigned int retr_value;
	alloc_bmp(marked_verts,retr->nverts);
	init_bmp(marked_verts,retr->nverts);
	memset(retr->retr_table,0,bits_table_size(retr->nverts,retr->vals_length)*4);
	for(i=nedges-1;i+1>=1;i--){
		curr_edge=retr->queue[i];
		value=retr->values_table[curr_edge];
		v1=retr->graph3.edges[curr_edge].vertices[0];
		v2=retr->graph3.edges[curr_edge].vertices[1];
		v3=retr->graph3.edges[curr_edge].vertices[2];
		if(!marked_bit(marked_verts,v1)){
			mark_bit(marked_verts,v1);
			mark_bit(marked_verts,v2);
			mark_bit(marked_verts,v3);
			retr_value=value-get_retr_value(retr,v2)-get_retr_value(retr,v3);
			assign_value(retr,v1,retr_value);
		} else if(!marked_bit(marked_verts,v2)) {
			mark_bit(marked_verts,v2);
			mark_bit(marked_verts,v3);
			retr_value=value-get_retr_value(retr,v1)-get_retr_value(retr,v3);
			assign_value(retr,v2,retr_value);
		}else 
		{	
			if(marked_bit(marked_verts,v3))
			{
				printf("\nacyclicity error");
				exit(-1);
			};
			mark_bit(marked_verts,v3);
			retr_value=value-get_retr_value(retr,v1)-get_retr_value(retr,v2);	
			assign_value(retr,v3,retr_value);
		};
	};
};

inline unsigned  int part_count(unsigned char * table,unsigned int nbytes)
{
	int i;
	unsigned int part_sum=0;
	for(i=0;i<nbytes;i++)
		part_sum+=retr_count_table[table[i]];
	return part_sum;
};

inline unsigned int get_packed_value(retr_t*retr,unsigned int vert)
{
	unsigned int count;
	unsigned char* byte_table=(unsigned char *)retr->bits_table;
	unsigned int bit_idx=vert+(vert/256)*32;
	unsigned int byte_idx=(bit_idx-(vert%256))/8;
	unsigned int byte_count=(vert%256)/8;
	//printf("\n byte is %d",byte_table[byte_idx+4+byte_count]);
	if(!(byte_table[byte_idx+4+byte_count]&(1<<(vert%8))))
		return 0;
	count=*((unsigned int*)&byte_table[byte_idx]);
	count+=part_count(&byte_table[byte_idx+4],byte_count);
	count+=retr_count_table[(byte_table[byte_idx+4+byte_count]<<(8-vert%8))&255];
	return get_bits_value(retr->packed_table,count,retr->vals_length,retr->bits_mask);
}

static void pack_retr(retr_t*retr)
{
	unsigned int i,bit_idx,j;
	unsigned int value;
	init_bmp(retr->bits_table,retr->nverts+(retr->nverts/256)*32+32);
	for(bit_idx=0,i=0,j=0;i<retr->nverts;i++)
	{
		if((i%256)==0)
		{
			retr->bits_table[bit_idx/32]=j;
			//printf("\npart_count_value is %d",j);
			bit_idx+=32;
		};
		value=get_bits_value(retr->retr_table,i,retr->vals_length,retr->bits_mask);
		if(value)
		{
			mark_bit(retr->bits_table,bit_idx);
			set_bits_value(retr->packed_table,j,value,retr->vals_length,retr->bits_mask);
			j++;
		};
		bit_idx++;
	}
	retr->packed_table=(unsigned int*)realloc(retr->packed_table,bits_table_size(j,retr->vals_length)*4);
	//printf("\n j was %d",j);
	for(i=0;i<retr->nverts;i++)
	{
		if(get_bits_value(retr->retr_table,i,retr->vals_length,retr->bits_mask)!=get_packed_value(retr,i))
		{
			printf("\nerror in packing %d %d %d %d %d",i,get_bits_value(retr->retr_table,i
					,retr->vals_length,retr->bits_mask),get_packed_value(retr,i)
						,retr->vals_length,retr->bits_mask);
			exit(-1);
		};
	};
};

static inline unsigned int get_retr3_value(retr_t*retr,unsigned int pos)
{
	unsigned int in_pos,out_pos;	
	unsigned int packed3_vals;
	in_pos=pos%5;
	out_pos=pos/5;
	packed3_vals=((unsigned char *)retr->retr_table)[out_pos];
	return retr_decode3_table[256*in_pos+packed3_vals];
};

static inline unsigned int get_retr5_value(retr_t*retr,unsigned int pos)
{
	unsigned int in_pos,out_pos;	
	unsigned int packed5_vals;
	in_pos=pos%3;
	out_pos=pos/3;
	packed5_vals=get_bits_value(retr->retr_table,out_pos,7,127);
	return retr_decode5_table[128*in_pos+packed5_vals];
};

static inline unsigned int get_retr6_value(retr_t*retr,unsigned int pos)
{
	unsigned int in_pos,out_pos;	
	unsigned int packed6_vals;
	in_pos=pos%5;
	out_pos=pos/5;
	packed6_vals=get_bits_value(retr->retr_table,out_pos,13,8191);
	return retr_decode3_table[256*in_pos+(packed6_vals&255)]*2+
		(packed6_vals>>(8+in_pos))%2;
};


unsigned int query_retr(char *key,retr_t* retr)
{
	unsigned int verts[3];
	unsigned int retr_value=0;
	get_vertices(retr,key,verts,retr->nverts);
	switch(retr->is_packed)
	{
		case 0:
		retr_value=get_bits_value(retr->retr_table,verts[0],retr->vals_length,retr->bits_mask);
		retr_value+=get_bits_value(retr->retr_table,verts[1],retr->vals_length,retr->bits_mask);
		retr_value+=get_bits_value(retr->retr_table,verts[2],retr->vals_length,retr->bits_mask);
		break;
		case retr_is_packed:
		retr_value=get_packed_value(retr,verts[0]);
		retr_value+=get_packed_value(retr,verts[1]);
		retr_value+=get_packed_value(retr,verts[2]);
		break;
		case retr_is_packed_3:
		retr_value=get_retr3_value(retr,verts[0]);
		retr_value+=get_retr3_value(retr,verts[1]);
		retr_value+=get_retr3_value(retr,verts[2]);
		return retr_mod3_table[retr_value];
		break;
		case retr_is_packed_5:
		retr_value=get_retr5_value(retr,verts[0]);
		retr_value+=get_retr5_value(retr,verts[1]);
		retr_value+=get_retr5_value(retr,verts[2]);
		return retr_mod5_table[retr_value];
		break;
		case retr_is_packed_6:		
		retr_value=get_retr6_value(retr,verts[0]);
		retr_value+=get_retr6_value(retr,verts[1]);
		retr_value+=get_retr6_value(retr,verts[2]);
		return retr_mod6_table[retr_value];
		default:
		break;
	};
	retr_value&=retr->bits_mask;
	return retr_value;
};

// query_retr_b() is used for non packed retrieval data structure with length expressed in bits
unsigned int query_retr_b(char *key,unsigned int key_len,retr_t*retr)
{
	unsigned int verts[3];
	unsigned int retr_value=0;
	get_vertices_b(retr,key,key_len,verts,retr->nverts);
	switch(retr->is_packed)
	{
		case 0:
		retr_value=get_bits_value(retr->retr_table,verts[0],retr->vals_length,retr->bits_mask);
		retr_value+=get_bits_value(retr->retr_table,verts[1],retr->vals_length,retr->bits_mask);
		retr_value+=get_bits_value(retr->retr_table,verts[2],retr->vals_length,retr->bits_mask);
		break;
		case retr_is_packed:
		retr_value=get_packed_value(retr,verts[0]);
		retr_value+=get_packed_value(retr,verts[1]);
		retr_value+=get_packed_value(retr,verts[2]);
		break;
		case retr_is_packed_3:
		retr_value=get_retr3_value(retr,verts[0]);
		retr_value+=get_retr3_value(retr,verts[1]);
		retr_value+=get_retr3_value(retr,verts[2]);
		return retr_mod3_table[retr_value];
		break;
		case retr_is_packed_5:
		retr_value=get_retr5_value(retr,verts[0]);
		retr_value+=get_retr5_value(retr,verts[1]);
		retr_value+=get_retr5_value(retr,verts[2]);
		return retr_mod5_table[retr_value];
		break;
		case retr_is_packed_6:
		retr_value=get_retr6_value(retr,verts[0]);
		retr_value+=get_retr6_value(retr,verts[1]);
		retr_value+=get_retr6_value(retr,verts[2]);
		return retr_mod6_table[retr_value];
		break;
		default:
		break;
	};
	retr_value&=retr->bits_mask;
	return retr_value;
};

void init_genrand(unsigned long long s);
unsigned long long genrand_int32(void);

#define max_trials 200

int generate_retr(retr_t* retr)
{
	unsigned int result;
	unsigned int i;
	alloc_graph3(&retr->graph3,retr->nkeys,retr->nverts);
	alloc_queue(&retr->queue,retr->nkeys);
	for(i=0;i<max_trials;i++)
	{
		retr->seed=genrand_int32();
		generate_graph(retr);
		result=generate_queue2(retr->nkeys,retr->nverts,retr->queue,&retr->graph3);
		if(result==0)
			break;
	};
	//printf("\n ntrials was %d",i+1);
	if(result)
		return result;
	retr_assignment(retr);
	if(retr->is_packed==retr_is_packed)
	{
		pack_retr(retr);
		free(retr->retr_table);
	};
	free_queue(&retr->queue);
	free_graph3(&retr->graph3);
	return 0;
};

