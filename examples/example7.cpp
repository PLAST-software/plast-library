//! [snippet1]
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
	printf ("sequences number is %ld\n",                    db->getSequencesNumber());
	printf ("database size (excluding comments) is %lld\n", db->getSize());

	// we retrieve a sequence given its index in the database;
	ISequence seq;
	if (db->getSequenceByIndex (4, seq) == true)
	{
		// we found the 5th sequence in the database.
		printf ("Found the 5th sequence... comment:'%s' data: %d %d %d ...\n",
				seq.comment,
				seq.data.letters.data[0],
				seq.data.letters.data[1],
				seq.data.letters.data[2]
		);
	}
}
//! [snippet1]
