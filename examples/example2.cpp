//! [snippet1]
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

class MyCommand : public ICommand
{
public:
    MyCommand (int& val, ISynchronizer* synchro) : _val(val), _synchro(synchro) {}
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

int main (int argc, char* argv[])
{
    int val = 0;

    // We create a synchronizer
    ISynchronizer* synchro = DefaultFactory::thread().newSynchronizer();

    // We create a list of commands; Note that they will share the same synchronizer
    std::list<ICommand*> commands;
    for (size_t i=1; i<=4; i++) { commands.push_back (new MyCommand (val, synchro)); }

    // We create a commands dispatcher that parallelizes the execution of the commands.
    ICommandDispatcher* dispatcher = new ParallelCommandDispatcher ();

    // We launch the 3 commands.
    dispatcher->dispatchCommands (commands, 0);

    // We print the final value
    printf ("val=%d\n", val);
    return 0;
}
//! [snippet1]
