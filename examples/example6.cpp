//! [snippet1]
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
        printf ("comment:'%s' data: %d %d %d ...\n",
            seq->comment,
            seq->data.letters.data[0],
            seq->data.letters.data[1],
            seq->data.letters.data[2]
        );
    }

    return 0;
}
//! [snippet1]
