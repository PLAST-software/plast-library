//! [snippet1]
#include <stdio.h>
#include <designpattern/impl/FileLineIterator.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>
#include <designpattern/impl/ProductIterator.hpp>
#include <designpattern/impl/ListIterator.hpp>

using namespace dp;
using namespace dp::impl;

int main (int argc, char* argv[])
{
    // Iteration of a file (line by line)
    FileLineIterator it1 ("/tmp/afile.txt", 256);
    for (it1.first(); !it1.isDone(); it1.next())
    {
        // We get the current read line
        char* line = it1.currentItem ();
        printf ("line '%s'\n", line);
    }

    // Iteration of a string parts (aka tokenizer)
    TokenizerIterator it2 ("this is the text to tokenize", " ");
    for (it2.first(); !it2.isDone(); it2.next())
    {
        // We get the current token
        char* token = it2.currentItem ();
        printf ("token '%s'\n", token);
    }

    // Product iteration of two iterators (ie. iterates each possible couple)
    ProductIterator<char*,char*> it3 (&it1, &it2);
    for (it3.first(); !it3.isDone(); it3.next())
    {
        // We get the current item
        std::pair<char*,char*>& item = it3.currentItem ();
        printf ("< '%s', '%s' >\n", item.first, item.second);
    }

    // Iteration of a std::list
    std::list<int> l; l.push_back(1); l.push_back(3); l.push_back(7);
    ListIterator<int> it4 (l);
    for (it4.first(); !it4.isDone(); it4.next())
    {
        // We get the current item
        int val = it4.currentItem ();
        printf ("value %d\n", val);
    }

    return 0;
}
//! [snippet1]
