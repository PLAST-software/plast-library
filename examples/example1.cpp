//! [snippet1]
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
//! [snippet1]
