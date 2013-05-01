//! [snippet1]
#include <database/impl/BufferedCachedSequenceDatabase.hpp>
#include <database/impl/FastaSequenceIterator.hpp>
#include <designpattern/impl/CommandDispatcher.hpp>
#include <designpattern/impl/IteratorGet.hpp>

using namespace std;
using namespace database;
using namespace database::impl;
using namespace dp;
using namespace dp::impl;

void callback (const ISequence*& seq, size_t& nbRetrieved, void* data)
{
	// we can do whatever we want with the current sequence
}

int main (int argc, char* argv[])
{
	// we create a database object.
	// Note that we use the BufferedCachedSequenceDatabase class that ensures that the iterated ISequence instances exist in memory
	ISequenceDatabase* db = new BufferedCachedSequenceDatabase (
		new FastaSequenceIterator ("/tmp/tursiops.fa"),
		false
	);

	// we create a special sequences iterator from the database
	IteratorGet<const ISequence*>* it = new IteratorGet<const ISequence*> (db->createSequenceIterator());

	// we create some commands that will use this iterator.
	list<ICommand*> commands;
	for (size_t i=1; i <= 8; i++)
	{
		// we create an IteratorCommand instance with a given callback
		//commands.push_back (new IteratorCommand<const ISequence*> (it, callback, NULL));
	}

	// We dispatch the commands.
	// Each sequence will be iterated through the 'callback' function, each call being done in one of the 8 created threads
	ParallelCommandDispatcher().dispatchCommands (commands,0);
}
//! [snippet1]
