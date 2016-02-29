PLAST Library
=============

**Linux:** [![Build Status](https://ci.inria.fr/plast/job/build-from-git-linux/badge/icon)](https://ci.inria.fr/plast/job/build-from-git-linux/) - **Mac OSX:** [![Build Status](https://ci.inria.fr/plast/job/build-from-git-macosx/badge/icon)](https://ci.inria.fr/plast/job/build-from-git-macosx/) - **Windows:** [![Build Status](https://ci.inria.fr/plast/job/build-from-git-win64/badge/icon)](https://ci.inria.fr/plast/job/build-from-git-win64/) - **Unit tests:** [![Build Status](https://ci.inria.fr/plast/job/test-from-git-linux/badge/icon)](https://ci.inria.fr/plast/job/test-from-git-linux/)


What is PLAST?
--------------

PLAST stands for Parallel Local Sequence Alignment Search Tool and is was published in [BMC Bioinformatics, 2009](http://www.biomedcentral.com/1471-2105/10/329).

PLAST is a fast, accurate and NGS scalable bank-to-bank sequence similarity search tool providing significant accelerations of seeds-based heuristic comparison methods, such as the Blast suite of algorithms.

Relying on unique software architecture, PLAST takes full advantage of recent multi-core personal computers without requiring any additional hardware devices.

PLAST is a general purpose sequence comparison tool providing the following benefits:

* PLAST is a high-performance sequence comparison tool designed to compare two sets of sequences (query vs. reference),
* Reduces the processing time of sequences comparisons while providing highest quality results; i.e. PLAST *is* faster than BLAST *and* provides its quality,
* Contains a fully integrated data filtering engine capable of selecting relevant hits with user-defined criteria (E-Value, identity, coverage, alignment length, etc.),
* Does not require any additional hardware, since it is a software solution. It is easy to install, cost-effective, takes full advantage of multi-core processors and uses a small RAM footprint,
* Ready to be used on desktop computer, cluster, cloud as well as within distributed system running Hadoop.


What is PLAST Library?
----------------------

PLAST Library is written in c++/11 and provides all the needed software components required for comparing two sequence databases with the PLAST algorithm.

As a library, it should not contain any 'main' function. However, to simplify the use of the library as a command-line tool the library provides:

1. an API to setup new softwares relying on PLAST;
2. a main class (tools::PlastCmd.cpp) to directly provide a ready-to-use PLAST command-line tool. 

About Github mirror
-------------------

PLAST project resides at two locations:

* [Inria Forge](https://gforge.inria.fr/): the official git repository
* Github: this is a mirror of the Inria Forge repository

The Github repository is auto-updated from Inria GForge. So all commits have to be done by authorized users/developers on the Inria Forge. Otherwise, consider using "Github/pull request" for any update to be done on PLAST.

Use **stable** release: we always advise you to use the stable release of the software; we consider the master branch as a development branch, so it could be unstable. Recommended procedure:

    git clone ...
    git checkout stable

This project contains:
----------------------

1. C++/11 source code ('src' directory);
2. CMake file to compile the project on various 64bit OS, such as Linux, MacOSX and Windows;
3. API documentation ('doc' directory);
4. sample code snippets illustrating the use of the API ('examples' directory); 
5. sample Fasta files to test the software ('test/db' directory);


Requirements
------------

The PLAST library can be compiled in a terminal with the [CMake tool](https://cmake.org/). 

Requirements: 

   * **Hardware**: Intel/AMD 64bit processor provising SSE2+ instructions set


   * **Linux**: 
            kernel 2.6+ ;
            gcc 4.4+ ;
            cmake 2.8+

   * **MacOS**: 
            OS 10.9+ ;
            cmake 2.8+ ; 
            XCode and its command-line development tools (clang)

   * **Windows**: 
            Seven+ ;
            cmake 2.8+ ;
            MinGW-64 environment (since gcc is required to compile PLAST)

Be aware that the more recent OS version you use, the more faster PLAST will be. Same for gcc/clang.


Compiling PLAST binary
----------------------

Once the source has been retrieved, one just has to do:

*Linux:*

    cd plast-library
    git checkout stable  #see comment [1] below
    mkdir build ; cd build ; cmake .. ; make -j8

*MacOS:*
    
    cd plast-library
    git checkout stable  #see comment [1] below
    set CC=gcc
    set CXX=g++
    mkdir build ; cd build ; cmake .. ; make -j8

*Windows* (use MinGW-MSYS to run a real Unix shell):

    cd plast-library
    git checkout stable  #see comment [1] below
    mkdir build ; cd build ; cmake -G "MSYS Makefiles" .. ; make -j8

As a result of source code compilation, one should get a dynamic library (in 'build/lib' directory) and a command-line tool (in 'build/bin' directory). 

PlastCmd is a self-contained command-line tool: you can move it anywhere you want, even renaming it; for instance, the PLAST team from Inria provide pre-compiled PLAST binary renamed as 'plast'.

**Comment [1]** -  We *ALWAYS* advise you to compile PLAST from its stable release:

* Github users: after the "git clone", you should run "git checkout stable" before compiling the code. Indeed, compiling PLAST from the master branch is done at your own risk, since this is the development branch of PLAST. As a result, the software may be unstable. This is the reason why PLAST is always tagged with "stable", a tag corresponding to the latest available stable release.

* "tar.gz" users: if you retrieve PLAST from its [stable archive](https://plast.inria.fr/developer-guide/) from PLAST web site, you do not have to run 'git checkout stable', since we provide you with a stable archive.

    
Running PLAST cmdline tool
--------------------------

    cd plast-library/build/bin
    ./PlastCmd -p plastp \
               -i ../../test/db/query.fa \ 
               -d ../../test/db/tursiops.fa \ 
               -o result.txt
       
* -p: the comparison method
* -i: the query
* -d: the reference databank (can be a Fasta file or a Blast databank)
* -o: the result file

More options: see documentation...


User documentation
------------------

To get PlastCmd inline help:

    ./PlastCmd -h

PLAST user documentation is [here](https://plast.inria.fr/user-guide/).

Compiling PLAST c++ snippets
---------------------------------

Developers can see how to use the PLAST c++ API through snippets provided with this project.

Snippets compilation is done as follows:

    cd plast-library/build
    make examples

Snippet binaries will be located in 'build/bin'. 

Snippets documentation: [http://plast.gforge.inria.fr/docs/cpp/snippets_page.html](http://plast.gforge.inria.fr/docs/cpp/snippets_page.html)

Compiling PLAST API documentation
---------------------------------

This documentation is available at [http://plast.gforge.inria.fr/docs/cpp/](http://plast.gforge.inria.fr/docs/cpp/).

However, you can prepare a local documentation as follows (we suppose you already have compiled PLAST binary, see above):

    cd plast-library/build
    make doc-api
    

Developer documentation
-----------------------

See [http://plast.gforge.inria.fr/docs/cpp/](http://plast.gforge.inria.fr/docs/cpp/).


More information and contact
-----------------------------

Please, visit [PLAST web site](https://plast.inria.fr/) for more information and contact (questions regarding the use of PLAST, bug report, feature request, etc.).

License
-------

PLAST is free software; you can redistribute it and/or modify it under the Affero GPL v3 
license. See http://www.gnu.org/licenses/agpl-3.0.en.html

