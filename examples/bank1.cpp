#include <iostream>

#include <database/impl/FastaSequenceIterator.hpp>

using namespace std;
using namespace database;
using namespace database::impl;
using namespace os;
using namespace os::impl;
using namespace dp;
using namespace dp::impl;

/********************************************************************************/
int main (int argc, char** argv)
{
	const char* filename = argv[1];

	ISequenceIterator* fastaIterator = new FastaSequenceIterator (filename, 100);
    fastaIterator->use ();

    for (fastaIterator->first(); !fastaIterator->isDone(); fastaIterator->next())
    {
    	cout << *(fastaIterator->currentItem()) << endl;
    }

	return 0;
}
