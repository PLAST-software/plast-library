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

/**\page howto_page How to
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
 * \ref indexation
 *
 * \ref environment
 *
 * We provide here some code snippets to see how to use some concepts of the PlastLibrary.
 *
 * Note that, for readibility reasons, these snippets don't take care about some aspects
 * (memory leak for instance).
 *
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
 * \code
#include <designpattern/api/ICommand.hpp>
#include <designpattern/impl/CommandDispatcher.hpp>

#include <stdio.h>
#include <unistd.h>

using namespace dp;
using namespace dp::impl;

// we implement our job in the execute() method: just sleep a few seconds.
class MyCommand : public ICommand
{
public:

    MyCommand (int value) : _value (value) {}

    void execute ()
    {
        printf ("[BEGIN] wait for %d seconds\n", _value);
        sleep (_value);
        printf ("[END] wait for %d seconds\n", _value);
    }

private:
    int _value;
};

int main (int argc, char* argv[])
{
    // We create a list of commands
    std::list<ICommand*> commands;
    commands.push_back (new MyCommand(2));
    commands.push_back (new MyCommand(5));
    commands.push_back (new MyCommand(3));

    // We create a commands dispatcher that parallelizes the execution of the commands.
    ICommandDispatcher* dispatcher = new ParallelCommandDispatcher ();

    // We launch the 3 commands.
    dispatcher->dispatchCommands (commands, 0);

    // Here, we don't go further until all the commands are finished (the second one that waits for 5 seconds is the last to be finished)

    return 0;
}
 * \endcode
 *
 ***************************************************************************************************
 *
 * \section dp_synchronizer How to protect data against concurrent accesses ?
 *
 * It may happen that the same piece of data have to be read and writen by several threads at the same
 * time. Doing it carelessely may lead to program crashes.
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
 * at the begining of the for-loop block and unlocked at its end (so lock/unlock will happen 2000
 * times in our example).
 *
 * If one tries to comment the line where the LocalSynchronizer object is declared, the result
 * is very unlikely to be 8000 because the shared variable would be wrongly accessed by the different
 * commands.
 *
 * \code

#include <designpattern/api/ICommand.hpp>
#include <designpattern/impl/CommandDispatcher.hpp>

#include <os/impl/DefaultOsFactory.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace dp;
using namespace dp::impl;

using namespace os;
using namespace os::impl;

/////////////////////////////////////////////////////////////////////
class MyCommand : public ICommand
{
public:

    MyCommand (int& val, ISynchronizer* synchro) : _val(val),  _synchro(synchro) {}

    void execute ()
    {
        for (size_t i=1; i<=2000; i++)
        {
            // We protect the englobing statements block (here the for-block) against concurrent access.
            LocalSynchronizer s (_synchro);

            // We modify the shared data
            _val ++;

            // We mimic real time behaviour by waiting a random time (helps to show the potential real time issue)
            usleep ( (100*random()) / RAND_MAX);
        }
    }

private:
    int& _val;
    ISynchronizer* _synchro;
};

/////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[])
{
    int val = 0;

    // We create a synchronizer
    ISynchronizer* synchro = DefaultFactory::thread().newSynchronizer();

    // We create a list of commands; Note that they will share the same synchronizer
    std::list<ICommand*> commands;
    for (size_t i=1; i<=4; i++)  {   commands.push_back (new MyCommand (val, synchro));  }

    // We create a commands dispatcher that parallelizes the execution of the commands.
    ICommandDispatcher* dispatcher = new ParallelCommandDispatcher ();

    // We launch the 3 commands.
    dispatcher->dispatchCommands (commands, 0);

    // We print the final value
    printf ("val=%d\n", val);

    return 0;
}
 * \endcode
 *
 * This sample hence shows how data protection can be done with few lines of code in a parallel
 * execution context.
 *
 *
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
 *  \code
#include <stdio.h>

#include <designpattern/impl/FileLineIterator.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>
#include <designpattern/impl/ProductIterator.hpp>
#include <designpattern/impl/ListIterator.hpp>

using namespace dp;
using namespace dp::impl;

int main (int argc, char* argv[])
{
    ///////////////////////////////////////////////////
    // Iteration of a file (line by line)
    ///////////////////////////////////////////////////
    FileLineIterator it1 ("/tmp/afile.txt", 256);
    for (it1.first(); !it1.isDone(); it1.next())
    {
        // We get the current read line
        char* line = it1.currentItem ();
        printf ("line '%s'\n", line);
    }

    ///////////////////////////////////////////////////
    // Iteration of a string parts (aka tokenizer)
    ///////////////////////////////////////////////////
    TokenizerIterator it2 ("this is the text to tokenize", " ");
    for (it2.first(); !it2.isDone(); it2.next())
    {
        // We get the current token
        char* token = it2.currentItem ();
        printf ("token '%s'\n", token);
    }

    ///////////////////////////////////////////////////
    // Product iteration of two iterators (ie. iterates each possible couple)
    ///////////////////////////////////////////////////
    ProductIterator<char*,char*> it3 (&it1, &it2);
    for (it3.first(); !it3.isDone(); it3.next())
    {
        // We get the current item
        std::pair<char*,char*>& item = it3.currentItem ();
        printf ("< '%s', '%s' >\n", item.first, item.second);
    }

    ///////////////////////////////////////////////////
    // Iteration of a std::list
    ///////////////////////////////////////////////////
    std::list<int> l;   l.push_back(1);  l.push_back(3);   l.push_back(7);
    ListIterator<int> it4 (l);
    for (it4.first(); !it4.isDone(); it4.next())
    {
        // We get the current item
        int val = it4.currentItem ();
        printf ("value %d\n", val);
    }

    return 0;
}
 *  \endcode
 *
 * One can see in this sample the uniform way to iterate the underlying set, whatever the kind
 * of iterated object can be.
 *
 * If you have a specific data structure T holding some items, you can easily implement the Iterator
 * interface for your T structure. Here is an example:
 * \code
#include <stdio.h>

#include <designpattern/api/Iterator.hpp>

using namespace dp;

/////////////////////////////////////////////////////////////////////
// Here is my specific structure: a linked list of integers
/////////////////////////////////////////////////////////////////////
struct MyList
{
    MyList (int value, struct MyList* next) : _value(value), _next(next) {}

    int _value;
    struct MyList* _next;
};

/////////////////////////////////////////////////////////////////////
// We implement the Iterator interface for the MyList structure
/////////////////////////////////////////////////////////////////////
class MyIterator : public Iterator<int>
{
public:

    MyIterator (MyList* alist) : _list(alist), _loop(0) {}

    void first() { _loop = _list; }

    IteratorStatus next()  { _loop = _loop->_next;  return ITER_UNKNOWN; }

    bool isDone()  { return _loop == 0; }

    int currentItem()  { return _loop->_value; }

private:
    MyList* _list;
    MyList* _loop;
};

/////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[])
{
    // We create a list instance with some items
    MyList* l1 = new MyList (7,NULL);
    MyList* l2 = new MyList (3,l1);
    MyList* l3 = new MyList (1,l2);

    // We create an iterator for l3
    MyIterator it (l3);
    for (it.first(); !it.isDone(); it.next())
    {
        printf ("val=%d\n", it.currentItem());
    }

    return 0;
}
 * \endcode
 *
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
 *  - the Subect implementation provides methods for adding/removing observers to itself.
 *
 * \code
#include <designpattern/api/IObserver.hpp>
#include <designpattern/impl/Observer.hpp>

#include <iostream>

using namespace std;

using namespace dp;
using namespace dp::impl;

// We define some notification information class.
class MyEventInfo : public EventInfo
{
public:
    MyEventInfo (const std::string& message) : EventInfo(0), _message(message) {}
    const std::string& getMessage ()  { return _message; }
private:
    std::string _message;
};


// We define some Observer class.
class MyObserver : public IObserver
{
public:
    void update (EventInfo* evt, ISubject* subject)
    {
        MyEventInfo* info = dynamic_cast<MyEventInfo*> (evt);
        if (info != 0)  {  cout << "Receiving: " << info->getMessage() << endl;  }
    }
};


int main (int argc, char* argv[])
{
    // we define a subject instance
    ISubject* subject = new Subject ();

    // we create a specific observer
    IObserver* observer = new MyObserver ();

    // we attach the observer to the subject
    subject->addObserver (observer);

    // the subject sends some notification => should be received by our observer
    subject->notify (new MyEventInfo ("Message that should be received"));

    // we detach the observer from the subject
    subject->removeObserver (observer);

    // the subject sends some notification => should not be received by our observer
    subject->notify (new MyEventInfo ("Message that should NOT be received"));

    return 0;
}
 * \endcode
 *
 *
 ***************************************************************************************************
 *
 * \section database How to read a genomic database ?
 *
 * Databases files contain a list of sequences, one sequence being identified by some textual
 * comment and by the actual genomic letters data  (nucleotids or amino acids).

 * Given the location of the genomic database in the filesystem, clients may want to parse
 * the database (FASTA format for instance), ie. they want to have access to the sequence information.
 *
 * They have two possibilies:
 *      - 1. they want to iterate the sequences, one by one
 *      - 2. they want to have more flexible access to the sequences
 *
 * In the first case, they need to use a ISequenceIterator object; such an object will only iterate
 * each sequence of the database.
 * \code
#include <database/impl/FastaSequenceIterator.hpp>

using namespace database;
using namespace database::impl;

int main (int argc, char* argv[])
{
    // we create a sequence iterator on a FASTA file
    ISequenceIterator* it = new FastaSequenceIterator ("/tmp/tursiops.fa");

    // we loop each sequence
    for (it->first(); !it->isDone(); it->next())
    {
        // we get the currently iterated sequence
        const ISequence* seq = it->currentItem();

        // we can display some information about the sequence
        printf ("comment:'%s'  data: %d %d %d ...\n",
            seq->comment,
            seq->data.letters.data[0],
            seq->data.letters.data[1],
            seq->data.letters.data[2]
        );
    }

    return 0;
}
 * \endcode
 *
 * In the second case, they need to use a ISequenceDatabase object; such an object will provide various
 * ways to access specific sequences in the database.
 * \code
#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/BufferedSequenceDatabase.hpp>

using namespace database;
using namespace database::impl;

int main (int argc, char* argv[])
{
    // we create a database object; note the database is filled from a provided sequence iterator
    ISequenceDatabase* db = new BufferedSequenceDatabase (
        new FastaSequenceIterator ("/tmp/tursiops.fa"),
        false
    );

    // we display information about the database
    printf ("sequences number is %d\n",                   db->getSequencesNumber());
    printf ("database size (excluding comments) is %d\n", db->getSize());

    // we retrieve a sequence given its index in the database;
    ISequence seq;
    if (db->getSequenceByIndex (4, seq) == true)
    {
        // we found the 5th sequence in the database.
        printf ("Found the 5th sequence...  comment:'%s'  data: %d %d %d ...\n",
            seq.comment,
            seq.data.letters.data[0],
            seq.data.letters.data[1],
            seq.data.letters.data[2]
        );
    }
}
 * \endcode
 *
 * We have so far talked about genomic databases in files. We can quickly define
 * genomic sequences "on the fly" with the StringSequenceIterator class. This iterator
 * defines as strings the genomic sequences. This may be useful for the end user
 * that wants to compare a (file) subject database with a sequence she/he wants to
 * enter in some text field.
 * \code
#include <database/impl/StringSequenceIterator.hpp>

using namespace database;
using namespace database::impl;

int main (int argc, char* argv[])
{
    // we create a sequence iterator from 3 sequences defined as strings
    ISequenceIterator* it = new StringSequenceIterator (3,
        "KAKLAEQAERYDDMAAAMKAVTEQGHELSNEERNLL",
        "FDEAIAELDTLNEESYKDSTL",
        "VHFETHEAAQNAISTMNGMLLIAELNDRKVFVGHFKPR"
    );

    // we loop each sequence
    for (it->first(); !it->isDone(); it->next())
    {
        // we get the currently iterated sequence
        const ISequence* seq = it->currentItem();

        // we can display some information about the sequence
        printf ("comment:'%s'  data: %d %d %d ...\n",
            seq->comment,
            seq->data.letters.data[0],
            seq->data.letters.data[1],
            seq->data.letters.data[2]
        );
    }

    return 0;
}
 * \endcode
 *
 *
 ***************************************************************************************************
 *
 * \section indexation How to index a genomic database ?
 *
 * It is interesting (and crucial for PLAST) to index a genomic database, which allows
 * to make quick lookups in the database for some kind of keys.
 *
 * The keys we consider here are called seeds, and represent a small word with genomic
 * letters (nucleotids or amino acids). For instance, a seed may be 4 amino acids long.
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
 * \code

#include <database/impl/StringSequenceIterator.hpp>
#include <database/impl/BufferedSequenceDatabase.hpp>

#include <seed/impl/BasicSeedModel.hpp>

#include <index/impl/DatabaseIndex.hpp>

using namespace database;
using namespace database::impl;

using namespace seed;
using namespace seed::impl;

using namespace indexation;
using namespace indexation::impl;

int main (int argc, char* argv[])
{
    // we create a database object, holding 3 sequences defined as strings of amino acids
    ISequenceDatabase* db = new BufferedSequenceDatabase (
        new StringSequenceIterator (3,
            "KIAELEQAERYDDMAAAMKAVTEQGHELSNEERNLL",
            "FDEAIAELDTLNEESYKDSTL",
            "VHFETHEAAQNAISTMNGMLLIAELNDRKVFVGHFKPR"
        ),
        false
    );

    // we create a seed model, with 4 letters long seeds
    ISeedModel* seedModel = new BasicSeedModel (SUBSEED, 3);

    // we create an indexation object
    IDatabaseIndex* index = new DatabaseIndex (db, seedModel);

    // we build the index
    index->build ();

    // we create a seeds iterator
    ISeedIterator* itSeeds = seedModel->createAllSeedsIterator ();
    LOCAL(itSeeds);

    // we iterate each seed known by the seed model
    for (itSeeds->first(); !itSeeds->isDone(); itSeeds->next())
    {
        // we retrieve the currenly iterated seed
        const ISeed* seed = itSeeds->currentItem();
        printf ("current seed is '%s'\n", seed->kmer.toString().c_str() );

        // we  create a IOccurrenceIterator instance from the IDatabaseIndex instance
        IOccurrenceIterator* itOccur = index->createOccurrenceIterator (seed);
        LOCAL(itOccur);

        // we iterate all occurrences of the current seed
        for (itOccur->first(); !itOccur->isDone(); itOccur->next())
        {
            // we retrieve the currently iterated occurrence
            const ISeedOccurrence* occur = itOccur->currentItem();

            // we display information about this occurrence
            printf ("    sequenceIndex=%d  offsetInSequence=%d  offsetInDatabase=%d \n",
                occur->sequence.index, occur->offsetInSequence, occur->offsetInDatabase
            );
        }
    }

   return 0;
}
 * \endcode
 *
 ***************************************************************************************************
 *
 * \section environment  How to run the PLAST algorithm ?
 *
 * The entry point of the PLAST algorithm is the IEnvironment interface.
 * You have to simply call the run() method with an argument gathering
 * all the options to be used for algorithm configuration.
 *
 * The minimal set of options you can provide only holds the URIs of the subject
 * and the query databases to be compared.
 *
 * Code sample
 * \code
 * #include <designpattern/impl/Property.hpp>
 * #include <algo/core/impl/DefaultAlgoEnvironment.hpp>
 *
 * using namespace dp;
 * using namespace dp::impl;
 * using namespace algo::core;
 * using namespace algo::core::impl;
 *
 * int main (int argc, char* argv[])
 * {
 *      // we create our entry point for the PLAST algorithm
 *      IEnvironment* env = new DefaultEnvironment ();
 *
 *      // we create a IProperties instance holding the subject and query databases URIs
 *      IProperties* props = new Properties ();
 *      props->add (0, "-d", "/tmp/tursiops.fa");
 *      props->add (0, "-i", "/tmp/query.fa");
 *
 *      // we launch the PLAST algorithm
 *      env->run (props);
 *
 *      return 0;
 * }
 * \endcode
 */
