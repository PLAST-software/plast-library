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

/**\page todo_page Improvements for the future
 *
 * We give here some possible improvements for the PLAST library.
 *
 * \ref improvement_1 \n
 * \ref improvement_2 \n
 * \ref improvement_3 \n
 * \ref improvement_4 \n
 * \ref improvement_5 \n
 * \ref improvement_6 \n
 * \ref improvement_7 \n
 * \ref improvement_8 \n
 * \ref improvement_9 \n
 * \ref improvement_10 \n
 * \ref improvement_11 \n
 * \ref improvement_12 \n
 * \ref improvement_13 \n
 * \ref improvement_14 \n
 * \ref improvement_15 \n
 *
 ************************************************************************************
 * \section improvement_1 Efficient parallelized indexing for amino acids databases
 *
 * Currently, the indexing for nucleotides databases is efficient in parallelization terms.
 * It is done in 3 phases:
 *   - we count the occurrences number for each possible seed of the seeds model
 *   - we allocate structures according to the information retrieved during phase 1
 *   - we fill the actual structures with seeds occurrences offsets
 *
 * Phases 1 and 3 are parallelized and take full advantage of being executed on several cores.
 *
 * This implementation should be generalized in order to be usable also on amino acids databases.
 * The improvement at stake is to find an efficient way to compute on the fly the successive
 * seeds hash codes for a seeds subset model.
 *
 *
 ************************************************************************************
 * \section improvement_2  Sorting seeds by hits occurrences for nucleotides databases
 *
 * Currently, the indexing of amino acids databases reorder the seeds by decreasing hits number.
 * Initially, this part was done for having no core starvation due to bad balance on job to be done
 * on each core (recall that one seed is processed on one core).
 *
 * As a matter of fact, this ordering allows to cut to some ratio the number of seeds to be used, without
 * a big loss in the result quality (ie. loss of alignments).
 *
 * This ordering should therefore be implemented for nucleotides databases.
 *
 ************************************************************************************
 * \section improvement_3  Iterative seeds ratio
 *
 * We could launch the algorithm in an iterative way:
 *    - with a low seeds ratio, we find some set of alignments and detect unmatched queries
 *    - with a bigger seeds ratio, we launch again the algorithm on only the unmatched queries found previously with the lower seeds ratio
 *
 * This process could be repeated until reaching a seeds ratio of 1.
 *
 ************************************************************************************
 * \section improvement_4   Using heuristics that avoids redundant computation
 *
 * The PlastN algorithm is optimized for skipping HSP where one finds seeds already processed. This
 * optimization should be done also for amino acids algorithms.
 *
 * The main difficulty here is to find an efficient way to compute on the fly the seeds hash codes
 * for a subset seeds model.
 *
 ************************************************************************************
 * \section improvement_5   Lower memory usage by compacting databases
 *
 * By now, one uses one byte for storing amino acids and nucleotides, although the respective alphabets
 * uses only 20 and 4 letters.
 *
 * For the amino acids, we could encode 6 residues on 4 bytes (ie 6*5 bits = 30 bits) or 3 residues on
 * 2 bytes (3*5 = 15 bits)
 *
 * This would implies a gain of 33% in terms of memory usage. As an example, a crude gzip on a big database
 * (SwissProt 300 MBytes) shows that the compressed file is about 25% the size of the original file.
 * So this simple encoding of 3 residues on 2 bytes is not so far from a complex compression procedure.
 *
 * Note also a possible memory improvement. The amino acids have not the same occurrence probability; this
 * simple fact could be a scheme for using Huffman encoding with the hope that some characters could be
 * encoded on less than 5 bits. Note however that it surely would lead to an extra cost in execution time.
 *
 *
 ************************************************************************************
 * \section improvement_6   Parallelization of 'dust' and 'seg' algorithms
 *
 * These two algorithms tag regions of low information in genomic databases.
 *
 * PLAST uses these algorithms but in a serial way. A simple optimization would be
 * to execute these algorithms in ICommand instances dispatched by a parallel dispatcher.
 *
 * Note however that some work has to be done for making the 'seg' algorithm reentrant.
 *
 ************************************************************************************
 * \section improvement_7  Using SSE for PlastN algorithm
 *
 * Currently, the PlastN algorithm is quite trivial and doesn't take full advantage of
 * SIMD architecture. Maybe some inspiration from the amino acids algorithms could be used
 * for PlastN improvements.
 *
 ************************************************************************************
 * \section improvement_8  Possibility to sort results by best hits
 *
 * Currently, the container structure holding the alignments
 * (likeky a alignment::core::impl::BasicAlignmentContainer) makes it difficult to sort the results
 * by best hits (ie for a given query, sorting the matched subject sequence having the biggest score for their
 * first alignment)
 *
 * A better structure could get rid of this limitation.
 *
 ************************************************************************************
 * \section improvement_9  Using OpenMP for local parallelization
 *
 * Some atomic parts of the algorithms are not parallelized. For instance, merging HSP lists
 * for PlastN is now serialized and may take a long time for big lists.
 *
 ************************************************************************************
 * \section improvement_10  Lower memory usage by indexing
 *
 * Currently, the indexing takes a lot of memory. If the size of the database is N, the basic
 * indexing takes about 8*N bytes (4 bytes for the absolute offset in the database and 4 bytes for
 * the number of sequence of the seed occurrence). Note that the sequence number is kept for
 * time optimization matter; we could keep only the offset in the database and recover the sequence
 * number by dichotomy, but it can take a long time when done a lot of times.
 *
 * This is not a big deal since we have the possibility to split the databases when they are too
 * big (and therefore limit the size of the indexing); see for this the -max-database-size.
 *
 * It would be however better to take less memory. For instance, we could keep deltas instead of actual
 * values:
 *  - SeqNb(n+1) - SeqNb(n)
 *  - Offset(n+1) - Offset(n)
 *
 *  These values are likely to be shorter than the absolute values. We could hope for instance having a
 *  3*N memory usage (1 byte for delta(seqnb), 2 bytes for delta(offset)).
 *
 ************************************************************************************
 * \section improvement_11  Customized memory allocators
 *
 * We could use fast memory allocators. For instance, we can know in advance how much memory a part of the
 * algorithm is about to use.
 *
 * Instead of calling a lot of times malloc/calloc/free, we could reserve a big buffer and make point
 * different pointers to different locations in this big buffer for simulating malloc/calloc.
 *
 * The several free calls would be replaced by only one free, the one on the big buffer.
 *
 *************************************************************************************
 * \section improvement_12  Keeping indexing in filesystem
 *
 * When we split the query and subject databases, we can deal with arbitrary big databases.
 *
 * On the other hand, it means that indexing is done several times on the same (subject) database
 * blocks.
 *
 * A potential solution would be to save in filesystem the whole (subject) database, and then retrieve
 * at demand the index block from the filesystem.
 *
 *************************************************************************************
 * \section improvement_13  Reuse of Blast traceback
 *
 * At the end of the dynamic programming, PLAST has the bounds of the alignment, but it needs to perform
 * a traceback for having information such as identity percentage.
 *
 * Currently, the traceback is not very efficient and needs to redo some dynamic programming (in particular,
 * filling the 3 matrixes in the Gotoh procedure)
 *
 * We could instead use the traceback in the Blast way: when computing the alignment bound, Blast keep some
 * information for the future traceback, information that speeds up the computation of identity percentage
 * for instance.
 *
 * Preliminary tests have been done showing that it is feasible. However, surprising extra time costs have
 * been noticed. If the reason of this extra time is found and solved, using this Blast part could be the
 * best solution.
 *
 *************************************************************************************
 * \section improvement_14   Score matrixes
 *
 * When profiling PLAST, it seems that a lot of time is consumed by getting a score between two residues, for
 * instance with the BLOSUM62 matrix.
 *
 * It would be interesting to see if it is possible to "compute" such a score for two residues, rather than
 * getting the score from some pre-computed table in memory (like the BLOSUM62 matrix).
 *
 * We can hope to have time improvement if this computation can be done by a massive (and clever) usage of SIMD
 * instructions set, like SSE2.
 *
 * In the best case, we could have 16 computations done in a single shot, if we can perform such computations on
 * 1 byte integer values.
 *
 * Note however that it is still unknown whether such computation is possible, with (if possible) only simple
 * arithmetic operations.
 *
 *************************************************************************************
 * \section improvement_15   Future usage of AVX2
 *
 * In a near future, some computers will have the brand new AVX2 architecture, which is just an improvement
 * of the current SSE architecture.
 *
 * The advantage is that AVX2 will provide registers on 256 bits instead of 128 bits for SSE. So, we can hope
 * to divide by two the execution time of some algorithm parts (ungap and small gap parts)
 *
 * Using intrinsics GCC instructions should make the evolution pretty simple (virtually replacing __m128i by
 * the future __m256i)
 *
 * Note that the AVX2 commercialization should take place (for Intel) during first 2013 semester.
 */
