//! [snippet1]
#include <launcher/core/PlastCmd.hpp>
#include <designpattern/impl/CommandDispatcher.hpp>
#include <designpattern/impl/Property.hpp>
#include <list>

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace launcher::core;

int main (int argc, char* argv[])
{
	// we create a IProperties instance holding the subject and query databases URIs
	IProperties* props = new Properties ();
	props->add (0, "-d", "/tmp/tursiops.fa");
	props->add (0, "-i", "/tmp/query.fa");

	// We create the PLAST command with the arguments above
	list<ICommand*> cmds;
	cmds.push_back (new PlastCmd (props));

	// We launch the request through some dispatcher.
	SerialCommandDispatcher().dispatchCommands (cmds);

	return 0;
}
//! [snippet1]
