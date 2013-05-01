//! [snippet1]
#include <stdio.h>
#include <designpattern/api/Iterator.hpp>

using namespace dp;

// Here is my specific structure: a linked list of integers
struct MyList
{
	MyList (int value, struct MyList* next) : _value(value), _next(next) {}
	int _value;
	struct MyList* _next;
};

// We implement the Iterator interface for the MyList structure
class MyIterator : public Iterator<int>
{
public:
	MyIterator (MyList* alist) : _list(alist), _loop(0) {}
	void first() { _loop = _list; }
	IteratorStatus next() { _loop = _loop->_next; return ITER_UNKNOWN; }
	bool isDone() { return _loop == 0; }
	int currentItem() { return _loop->_value; }
private:
	MyList* _list;
	MyList* _loop;
};


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
//! [snippet1]
