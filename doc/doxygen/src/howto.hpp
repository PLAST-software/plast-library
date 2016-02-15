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

/**\page snippets_page Snippets for PlastLibrary
 *
 * \ref dp_command
 *
 * \ref dp_synchronizer
 *
 * \ref dp_iterator
 *
 * \ref dp_observer
 *
 * \ref database
 *
 * \ref database2
 *
 * \ref indexation
 *
 * \ref environment
 *
 * We provide here some code snippets to see how to use some concepts of the PlastLibrary.
 *
 * Note that, for readability reasons, these snippets don't take care about some aspects
 * (memory leak for instance).
 *
 ***************************************************************************************************
 * \section compile_plast_snippets  How to compile the PLAST example source codes (snippets) ?
 *
 * The PLAST library comes with several code snippets can be compiled in a terminal with the cmake tool.
 * 
 * As a result of source code compilation, you'll get a set of binaries in 'build/bin' directory.
 * 
 * Requirements to compile PLAST snippets:
 * 
 *    * Linux:
 *             kernel 2.6+ ; 
 *             gcc 4.4+ ; 
 *             cmake 2.8+.
 * 
 *    * MacOS: 
 *             OS 10.7+ ;  
 *             cmake 2.8+ ;   
 *             XCode 4+ and its command-line development tools.
 * 
 *    * Windows: 
 *             Seven+ ; 
 *             cmake 2.8+ ; 
 *             MinGW-64 environment (since gcc is required to compile PLAST).
 * 
 * Be aware that the more recent OS version you use, the more faster PLAST will be.
 * 
 * Once the source archive has been retrieved and unzipped, one just has to do:
 * 
 * Linux:
 *\code
 *     mkdir build ; cd build ; cmake .. ; make examples
 * \endcode
 *
 * MacOS: 
 *\code
 *     set CC=gcc
 *     set CXX=g++
 *     mkdir build
 *     cd build
 *     cmake ..
 *     make examples
 * \endcode
 *
 * Windows: (use MinGW-MSYS to run a real Unix shell)
 *\code
 *     mkdir build
 *     cd build
 *     cmake -G "MSYS Makefiles" ..
 *     make examples
 * \endcode
 *
 * Note that the target architecture must support SSE instructions set (at least SSE2).
 * For instance, most of Intel recent processors support SSE2.
 ***************************************************************************************************
 *
 * \section dp_command  How to run some work in a specific thread ?
 *
 * In a multicore architecture, clients may want to parallelize some work by creating several threads
 * and then let the operating system run each thread on a specific core.
 *
 * This may lead to tedious code where the actual job to be executed is mixed up with thread management code.
 *
 * A better approach is:
 *    - to define a ICommand interface that encapsulates the job to be executed
 *    - to define a ICommandDispatcher interface that encapsulates the thread management code and know how
 *      to execute a ICommand instance
 *
 * Clients have to implement the ICommand interface by putting the job they want to be executed in the execute()
 * method. Then, they can use the "parallelized" implementation of  ICommandDispatcher for launching their commands.
 *
 *\snippet example1.cpp  snippet1
 *
 ***************************************************************************************************
 *
 * \section dp_synchronizer How to protect data against concurrent accesses ?
 *
 * It may happen that the same piece of data have to be read and written by several threads at the same
 * time. Doing it carelessly may lead to program crashes.
 *
 * Classical solutions for this issue are known as mutex, semaphores, etc...
 *
 * Here, we propose the ISynchronizer interface that encapsulates low level aspects
 * (ie. operating system concerns) and provides lock and unlock methods.
 *
 * The following sample is similar to the one about threads and ICommand interface.
 * Here, the actual job increases a variable 2000 times. Since we launch 4 commands in parallel (on a multicore
 * plateform), the shared variable should be at the end 2000*4=8000.
 *
 * Note how we secure the way the variable is increased in the execute() method; we use a
 * LocalSynchronizer object that will lock/unlock our shared synchronizer in its visibility
 * domain, here the 'for' statements block. In other words, the _synchro attribute will be locked
 * at the beginning of the for-loop block and unlocked at its end (so lock/unlock will happen 2000
 * times in our example).
 *
 * If one tries to comment the line where the LocalSynchronizer object is declared, the result
 * is very unlikely to be 8000 because the shared variable would be wrongly accessed by the different
 * commands.
 *
 *\snippet example2.cpp  snippet1
 *
 * This sample hence shows how data protection can be done with few lines of code in a parallel
 * execution context.
 *
 ***************************************************************************************************
 *
 * \section dp_iterator How to use iterators ?
 *
 * Iterating sets are a very common pattern in every day software development. Note that iteration is
 * a more generic concept than the set itself. As a matter of fact, it is possible to have to deal with
 * sets too big for being cached into memory. On the other hand, a smart iterator may iterate all items
 * in the set without knowing all the items at the same time; it can cache in memory only a small subset
 * of the original set, iterate the items of the subset, delete the subset and create the next one.
 *
 * We have defined a Iterator interface that provides a uniform way for iterating many kinds of objects.
 * Here are some samples of different implementations of this interface:
 *\snippet example3.cpp  snippet1
 *
 * One can see in this sample the uniform way to iterate the underlying set, whatever the kind
 * of iterated object can be.
 *
 * If you have a specific data structure T holding some items, you can easily implement the Iterator
 * interface for your T structure. Here is an example:
 *
 *\snippet example4.cpp  snippet1
 *
 ***************************************************************************************************
 *
 * \section dp_observer How to send notifications to clients ?
 *
 * A very common pattern in software development is the capacity to send some information to clients
 * interested in receiving such information.
 *
 * By telling this, we define three parts:
 *  - the information to be sent
 *  - the object that sends the information (called subject)
 *  - the object that receives the information (called observer)
 *
 * At any time, an observer can register to a subject; by doing so, the observer will receive any
 * notification sent by the subject. Note that the observer can also unregister from the subject
 * when it is no longer interested in receiving the subject's notifications.
 *
 * Two interfaces (IObserver and ISubject) are defined for managing this notifications system:
 *  - the IObserver::update() method must be implemented by clients that want to behave like observers
 *  - the Subject implementation provides methods for adding/removing observers to itself.
 *
 *\snippet example5.cpp  snippet1
 *
 ***************************************************************************************************
 *
 * \section database How to read a genomic database ?
 *
 * Databases files contain a list of sequences, one sequence being identified by some textual
 * comment and by the actual genomic letters data  (nucleotides or amino acids).

 * Given the location of the genomic database in the filesystem, clients may want to parse
 * the database (FASTA format for instance), ie. they want to have access to the sequence information.
 *
 * They have two possibilities:
 *      - 1. they want to iterate the sequences, one by one
 *      - 2. they want to have more flexible access to the sequences
 *
 * In the first case, they need to use a ISequenceIterator object; such an object will only iterate
 * each sequence of the database.
 *
 *\snippet example6.cpp  snippet1
 *
 *
 * In the second case, they need to use a ISequenceDatabase object; such an object will provide various
 * ways to access specific sequences in the database.
 *
 *\snippet example7.cpp  snippet1
 *
 *
 * We have so far talked about genomic databases in files. We can quickly define
 * genomic sequences "on the fly" with the StringSequenceIterator class. This iterator
 * defines as strings the genomic sequences. This may be useful for the end user
 * that wants to compare a (file) subject database with a sequence she/he wants to
 * enter in some text field.
 *
 *\snippet example8.cpp  snippet1
 *
 ****************************************************************************************************
 *
 * \section database2  How to read sequences from a database in an parallel fashion ?
 *
 * Here, we want to iterate all sequences of a database, and we also want to take full
 * advantage of a multicore architecture in order to speed up the process.
 *
 * We need a special kind of iterator that provides items references to several clients that can be
 * present on different threads. For this, we can use the dp::impl::IteratorGet. Such an iterator encapsulates
 * an instance of dp::Iterator and ensures that it provides references on the iterated items in a safe
 * manner in regard to the potential concurrent accesses from different clients.
 *
 *\snippet example9.cpp  snippet1
 *
 ***************************************************************************************************
 *
 * \section indexation How to index a genomic database ?
 *
 * It is interesting (and crucial for PLAST) to index a genomic database, which allows
 * to make quick lookups in the database for some kind of keys.
 *
 * The keys we consider here are called seeds, and represent a small word with genomic
 * letters (nucleotides or amino acids). For instance, a seed may be 4 amino acids long.
 *
 * The index is built according to a given set of seeds. Such a set is defined by a seed
 * model, reified as the ISeedModel interface.
 *
 * Once a database is indexed, we can retrieve (for a given seed) all the occurrences
 * of the seed in the database, occurrence meaning here the offset in the buffer holding
 * all concatenated sequences data. This is achieved through the IOccurrenceIterator
 * interface, which iterates over all occurrences of a given seed by providing ISeedOccurrence
 * instances.
 *
 * Here is a sample of how to index a database, and how to use the index.
 *
 *\snippet example10.cpp  snippet1
 *
 ***************************************************************************************************
 *
 * \section environment  How to run the PLAST algorithm ?
 *
 * The entry point of the PLAST algorithm is the launcher::core::PlastCmd interface.
 *
 * The minimal set of options you can provide only holds the URIs of the subject
 * and the query databases to be compared.
 *
 *\snippet example11.cpp  snippet1
 */
