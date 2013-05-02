//! [snippet1]
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

		// we create a IOccurrenceIterator instance from the IDatabaseIndex instance
		IOccurrenceIterator* itOccur = index->createOccurrenceIterator (seed);
		LOCAL(itOccur);

		// we iterate all occurrences of the current seed
		for (itOccur->first(); !itOccur->isDone(); itOccur->next())
		{
			// we retrieve the currently iterated occurrence
			const ISeedOccurrence* occur = itOccur->currentItem();

			// we display information about this occurrence
			printf (" sequenceIndex=%d offsetInSequence=%d offsetInDatabase=%lld \n",
					occur->sequence.index, occur->offsetInSequence, occur->offsetInDatabase
			);
		}
	}

	return 0;
}
//! [snippet1]
