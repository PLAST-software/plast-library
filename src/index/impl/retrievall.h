#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef hashes_h
#define hashes_h
/* ordinary jenkins hash function*/
unsigned int jenkins_hash(unsigned int seed, const char *k,unsigned int keylen);
/* jenkins hash functions that return 96 bits*/
void jenkins_hash_vec1(unsigned int seed, const char *k, unsigned int length,unsigned int * res_vec);
/* The second variant is used when length is expressed in bits*/
void jenkins_hash_vec2(unsigned int seed, const char *k, unsigned int length,unsigned int * res_vec); 

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

static inline uint32_t SuperFastHash12 (const char * data) {
uint32_t hash = 12, tmp;
uint32_t len = 3;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }
    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

#endif
#ifndef graph3_h
#define graph3_h

typedef struct 
{
	unsigned int vertices[3];
	unsigned int next_edges[3];
}edge;
typedef struct 
{
	unsigned int nedges;
	edge * edges;
	unsigned int * first_edge;
	unsigned char * vert_degree;
	unsigned int * marked_edge;
}graph3_t;
typedef unsigned int * queue_t;
typedef unsigned char * mphf_table_t;

/* some macro definitions*/
#define notassigned 3
#define notvisited 255
#define NULL_EDGE 0xffffffff

void alloc_queue(queue_t *  queuep,unsigned int nedges);
void free_queue(queue_t * queue);
void alloc_graph3(graph3_t * graph3,unsigned int nedges,unsigned int nvertices);
void init_graph3(graph3_t * graph3,unsigned int nedges,unsigned int nvertices);
void free_graph3(graph3_t *graph3);
void dump_graph(graph3_t* graph3,unsigned int nedges,unsigned int nvertices);
//void get_vertices(unsigned char * key,unsigned int * verts,unsigned int nverts);
void remove_edge(graph3_t * graph3,unsigned int curr_edge);
int generate_queue2(unsigned int nedges,unsigned int nvertices, queue_t queue,graph3_t* graph3);
/*inline int get_next_edge(graph3_t * graph3,unsigned int nverts,unsigned int edge_num,unsigned int vert_num)
{
	unsigned int i;
	for(i=0;i<3;i++)
		if(graph3->edges[edge_num].vertices[i]==vert_num)
			return graph3->edges[edge_num].next_edges[i];
	return -1;
};*/
static inline void add_edge(graph3_t * graph3,unsigned int v1,unsigned int v2,unsigned int v3)
{
	graph3->edges[graph3->nedges].vertices[0]=v1;
	graph3->edges[graph3->nedges].vertices[1]=v2;
	graph3->edges[graph3->nedges].vertices[2]=v3;
	graph3->edges[graph3->nedges].next_edges[0]=graph3->first_edge[v1];
	graph3->edges[graph3->nedges].next_edges[1]=graph3->first_edge[v2];
	graph3->edges[graph3->nedges].next_edges[2]=graph3->first_edge[v3];
	graph3->first_edge[v1]=graph3->first_edge[v2]=graph3->first_edge[v3]=graph3->nedges;
	graph3->vert_degree[v1]++;
	graph3->vert_degree[v2]++;
	graph3->vert_degree[v3]++;
	graph3->nedges++;
};
#endif
#ifndef mmph_bitops_h
#define mmph_bitops_h
#define mark_bit(bmp,idx)  (bmp[idx/32]|=(1<<(idx%32)))
#define marked_bit(bmp,idx) (bmp[idx/32]&(1<<(idx%32)))
#define alloc_bmp(bmp,size) (bmp=malloc(((size+31)/32)*4))
#define init_bmp(bmp,size) (memset(bmp,0,(((size+31)/32)*4)))
#define bmp_size(size) (((size+31)/32)*4)
#define bits_table_size(nverts,bits_length) ((nverts*bits_length+37)/32)

static inline void set_bits_value(unsigned int * bits_table,unsigned int vert,
	unsigned int bits_string,unsigned int string_length,unsigned int string_mask)
{
	unsigned int bit_idx=vert*string_length;
	unsigned int word_idx=bit_idx/32;
	unsigned int shift1=bit_idx%32;
	unsigned int shift2=32-shift1;
	bits_table[word_idx]&=~((string_mask)<<shift1);
	bits_table[word_idx]|=bits_string<<shift1;
	//printf("\nwrite value %d in vert %d",bits_string,vert);
	if(shift2<string_length)
	{
		bits_table[word_idx+1]&=~((string_mask)>>shift2);
		bits_table[word_idx+1]|=bits_string>>shift2;
	};
};
static inline unsigned int get_bits_value(unsigned int * bits_table,unsigned int vert,unsigned int string_length,unsigned int string_mask)
{
	unsigned int bit_idx=vert*string_length;
	unsigned int word_idx=bit_idx/32;
	unsigned int shift1=bit_idx%32;
	unsigned int shift2=32-shift1;
	unsigned int bits_string;

	bits_string = (bits_table[word_idx]>>shift1) & string_mask;

	if (shift2 < string_length)
		bits_string |= (bits_table[word_idx+1]<<shift2)&string_mask;

	return bits_string;
};
#endif

#ifndef retrieval1_h
#define retrieval1_h

#define retr_is_packed 1
#define retr_is_packed_3 2
#define retr_is_packed_5 3
#define retr_is_packed_6 4

struct _retr_t
{
	unsigned int vals_length;
	unsigned int bits_mask;
	
	unsigned int nkeys;
	char * keys_table;
	unsigned int * values_table;
	
	unsigned int * retr_table;
	unsigned int * bits_table;
	unsigned int * packed_table;
	unsigned int * keys_lengths;
	
	unsigned int is_packed;
	unsigned int keys_length;
	unsigned int length_in_bits;
	unsigned int seed;
	
	queue_t queue;
	graph3_t graph3;
	unsigned int nverts;
	double space_usage;
} ;

typedef struct _retr_t retr_t;

void init_retr(retr_t* retr);
int generate_retr(retr_t* retr);
void free_retr(retr_t * retr);
unsigned int query_retr(char *key,retr_t*retr);
unsigned int query_retr_b(char *key,unsigned int key_len,retr_t*retr);

#endif
