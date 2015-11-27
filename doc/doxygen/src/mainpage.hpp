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

/** \mainpage PlastLibrary Documentation
 *
 ****************************************************************************************************
 *
 * \section intro What is PlastLibrary ?
 *
 * The PlastLibrary is a library containing all the needed software components
 * required for comparing two genomic databases with the PLAST algorithm.
 *
 * As a library, it should not contain any 'main' function. However, to simplify the use
 * of the library as a command-line tool, there is a main function provided in:
 *  - tools::PlastCmd: binary for end user that wants to compare genomic databases
 *
 * You will find here the code documentation for namespaces, classes, methods of the different
 * components that makes the PlastLibrary design.
 *
 ****************************************************************************************************
 *
 * \section concepts Main concepts
 *
 * \subsection concepts_intro Introduction
 *
 * PLAST is a parallel local alignment search tool for database comparison. It scans two provided
 * genomic databases (in FASTA format for instance) and starts its search by looking for small
 * similarities (3 or 4 letters for instance) between the two databases.
 *
 * For efficiency issues, PLAST first indexes the two databases with an index model based on seeds,
 * ie. small words of nucleotides or amino acids. Once the indexes are built, it looks for each occurrence
 * in the subject and query databases of each possible seed. Such an occurrence in both databases
 * is called a hit.
 *
 * Then, the PLAST algorithm tries to keep only the best alignments from these initial hits set.
 * This is done through different steps:
 *      - ungap alignments : score computed on ungap alignments (about 40 letters in each hit)
 *      - small gap alignments : score computed on small gap alignments (about 130 letters in each hit)
 *      - full gap alignments  : dynamic programming for finding gap alignments (arbitrary length)
 *
 * Then, the PLAST algorithm is composed of several steps, each step filtering out the result of previous step.
 * At the end, only the alignments that fulfill the end user parameters are kept.
 *
 * So, the PLAST algorithm looks like a unix pipe; actually, the chosen design will keep this point of view:
 * an initial set of hits (from the subject and query indexations) is iterated at each step and filtered out
 * if needed.
 *
 *
 * \subsection concepts_perf Performance
 *
 * PLAST is proposed as a parallel alternative of the well known BLAST tool. The main idea is to use hardware
 * capacities for speeding up the algorithm execution.
 *
 * In particular, the following hardware capacities are used by PLAST:
 *   - multicore architectures : N cores can be used for processing N pieces of the algorithm
 *   - Simple Instruction Multiple Data (SIMD) : can be used for computing several algorithm parts in a single instruction;
 *     a SSE implementation for specific components of the PLAST algorithm is proposed
 *
 * \subsection concepts_details Key concepts
 *
 * The key concept of the PLAST design is the iteration. As said above, the algorithm iterates an initial set of hits and
 * tries to filter it out. Then, the resulting filtered set of hits is iterated by the next algorithm step, and so on.
 *
 * The Iterator concept (see GOF[94]) is the object oriented view of the iteration. An Iterator object knows how to iterate
 * some set. The interesting part is that the Iterator object uses always the same API for iterating the underlying set,
 * whatever the set can be.
 *
 * For PLAST, using Iterators will uniformize the design of the algorithm components. In particular, we will define a specific
 * interface for iterating hits (called IHitIterator) that will be used throughout the algorithm steps. Different implementations
 * of the IHitIterator interface will correspond to the ungap alignments step, small gap alignments step and so on.
 *
 * Beyond this uniformization aspect, the Iterator pattern can be used for implementing our parallelization scheme for multicores
 * architectures. We can split an Iterator instance in several Iterator instances, where the iterated set of the initial instance
 * is the same as the union of the iterated sets of split instances. It is then easy to iterate each split iterator in a specific
 * thread, running on a specific core and we are done with this parallelization aspect.
 *

 * \subsection concepts_entrypoint Entry point
 *
 * People who wants to use the library for comparing two databases have to know its "entry point", ie.
 * what they have to call in their main function.
 *
 * The launcher::core::PlastCmd class may be seen as such an "entry point". It provides an 'execute' method to be called for
 * launching the PLAST algorithm. The parameterization of the algorithm (for instance the path of the two genomic databases
 * to be compared) is provided to the constructor of this class.
 *
 * Note that launcher::core::PlastCmd is an implementation of the dp::ICommand interface. Therefore, one can use some
 * command dispatcher instead of using directly the 'execute' method for launching the algorithm.
 *
 * Finally, tools::PlastCmd provides directly a main function to enable the use of the PlastLibrary as
 * a command-line tool.
 *
 ****************************************************************************************************
 *
 * \section namespaces Namespaces architecture
 *
 * The top level namespaces are the following:
 *  - os : contains an operating system abstraction layer for making multi plateform development easier
 *  - designpattern : contains several Design Pattern (Iterator, Command, Observer...) used throughout the code
 *  - database : provides concepts for using genomic databases
 *  - seed : provides concepts related to seeds (ie. small words of genomic letters)
 *  - index : provides tools for indexing genomic databases, with seeds as indexes
 *  - seg: algorithms for removing low informative regions from databases (seg for amino acids, dust for nucleotides)
 *  - alignment: provides structures representing alignments containers; also holds some tools to work on such containers
 *  - algorithm : components of the PLAST algorithm
 *  - pcre: tool for perl like regular expressions (see http://www.pcre.org/)
 *  - misc : miscellaneous (types definitions for instance)
 *  - launcher : high level API for running PLAST algorithm; also contains a JNI interface for launching PLAST from Java world.
 *
 *  Most of these top level namespaces hold:
 *      - a sub namespace 'api' containing interface definitions
 *      - a sub namespace 'impl' containing one or several implementations of the interfaces defined in 'api'
 *
 * The top level 'algorithm' is a little bit special. It is there that the PLAST algorithm is actually
 * implemented. It relies on all the other top level namespaces. It is itself composed of the following
 * sub namespaces:
 *      - core   : configuration and running entry point of the PLAST algorithm
 *      - hits   : hits iterators management; PLAST algorithm parts are implemented there
 *      - result : alignments management (dumping alignments result into a file for instance)
 *
 *
 ****************************************************************************************************
 *
 * \section headers Including header files
 *
 * The header path files reflect the namespaces organization.
 *
 * For instance, accessing to the ISequenceDatabase interface can be done with:
 * \code
#include <database/api/ISequenceDatabase.hpp>
 * \endcode
 *
 * Accessing to a specific implementation of this interface can be done with:
 * \code
#include <database/impl/BufferedSequenceDatabase.hpp>
 * \endcode
 *
 * It has two benefits:
 *      - the include directive gives a clear indication about the nature of the definition included
 *        (likely to related to database management in the example above)
 *      - compilation command only have to provide the path of the global source directory, ie.
 *        something like -I/somepath/PlastLibrary/src
 *
 ****************************************************************************************************
 *
 * \section compile_plast  How to compile the PLAST library ?
 *
 * The PLAST library can be compiled in a terminal with the cmake tool.
 * 
 * As a result of source code compilation, one should get a dynamic library (in 'build/lib'
 * directory) and a commend-line tool (in 'build/bin' directory).
 * 
 * Requirements:
 * 
 *    * Linux:
 *             kernel 2.6+
 *             gcc 4.4+ 
 *             cmake 2.8+
 * 
 *    * MacOS: 
 *             OS 10.7+  
 *             cmake 2.8+  
 *             XCode 4+ and its command-line development tools
 * 
 *    * Windows: 
 *             Seven+
 *             cmake 2.8+
 *             MinGW-64 environment (since gcc is required to compile PLAST)
 * 
 * Be aware that the more recent OS version you use, the more faster PLAST will be.
 * 
 * Once the source archive has been retrieved and unzipped, one just has to do:
 * 
 * Linux:
 *     mkdir build
 *     cd build
 *     cmake ..
 *     make
 * 
 * MacOS: 
 *     set CC=gcc
 *     set CXX=g++
 *     mkdir build
 *     cd build
 *     cmake ..
 *     make
 * 
 * Windows: (use MinGW-MSYS to run a real Unix shell)
 *     mkdir build
 *     cd build
 *     cmake -G "MSYS Makefiles" ..
 *     make
 *
 * Note that the target architecture must support SSE instructions set (at least SSE2).
 * For instance, most of Intel recent processors support SSE2.
 *
 ****************************************************************************************************
 *
 * \section compilation Compiling with the PlastLibrary
 *
 * Here is a small example of use of the PlastLibrary; it merely iterates the sequences of a FASTA file and print a few information
 * about each sequence. We put this code sample in the test.cpp file.
 * \code
#include <database/impl/FastaSequenceIterator.hpp>

using namespace database::impl;

int main (int argc, char* argv[])
{
    const char* filename = (argc >= 2 ? argv[1] : "/tmp/tursiops.fa");

    // we create a sequence iterator on a FASTA file
    FastaSequenceIterator it (filename);

    // we display each sequence comment
    for (it.first(); !it.isDone(); it.next())   {  printf ("%s\n", it.currentItem()->comment);  }

    return 0;
}
 * \endcode
 *
 * Now, for generating the executable (on Linux), one should have to do something like this:
 * \code
 * g++ test.cpp -I/somepath/PlastLibrary/src -L/somepath/PlastLibrary/lib  -lPlastLibrary -lpthread -lrt -o plasttest
 * \endcode
 *
 * Note that you need (on Linux) to use the posix thread library (pthread) and the real time library (rt).
 */
