/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.0, released July  2011                                        *
 *   Copyright (c) 2011                                                      *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the CECILL version 2 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   CECILL version 2 License for more details.                              *
 *****************************************************************************/

#if 0

#include <misc/api/RangeTree.hpp>
#include <misc/api/macros.hpp>

#include <os/impl/DefaultOsFactory.hpp>

#include <algorithm>
#include <list>

using namespace std;
using namespace os::impl;

//#define FOO

#ifdef FOO
	#define DEBUG(a)    printf a
	#define VERBOSE(a)  printf a
#else
	#define DEBUG(a)
	#define VERBOSE(a)
#endif

/********************************************************************************/
namespace misc {
/********************************************************************************/

/** We define an iterator over a range of integers. */
template<typename Functor> class IntegerIterator
{
public:
    IntegerIterator (size_t n0, size_t n1, Functor& f) : _n0(n0), _n1(n1), _f(f) {}
    void iterate () {  iterate (_n0, _n1);  }
private:
    void iterate (int n0, int n1)  { if (n0 <= n1)  {  int m0 = (n0+n1) /2; _f (m0);   iterate (n0, m0-1);  iterate (m0+1, n1);  } }
    size_t _n0;  size_t _n1;  Functor& _f;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
RangeTree::RangeTree (Coord sizeX, Coord sizeY, size_t nbHeadsLog2)
    : _sizeX(sizeX),  _sizeY(sizeY), _nbHeadsLog2(nbHeadsLog2),
      _head (0),
      _nbItems(0), _nodePool(10*1000)
{
    _maxSizeAlpha = MAX (_sizeX, _sizeY);

    /** We take the nearest power of two plus one. */
    _maxSizeAlphaLog2 = 1 + log2 (_maxSizeAlpha);

    /** We set the max size for the alpha-space. */
    _maxSizeAlpha = 1 << _maxSizeAlphaLog2;

    /** We may set the nb of subtrees in an optimal way. */
    if (_nbHeadsLog2 == 0)
    {
    	/** The following has been found empirically. */
    	_nbHeadsLog2 = MIN (32 - _maxSizeAlphaLog2, 18);  // 18 => don't accept too huge memory usage
    }

    /** We will need % operator on the number of heads => use a mask because we have a power of 2 here. */
    _headsMask = (1 << _nbHeadsLog2) - 1;

#if 1
    /** We create an array of binary trees references. */
    _heads = new NodePtr [1 << _nbHeadsLog2];
#else
    _head = 0; //_nodePool.createNode (0, ~0, 0);

#endif
    /** We set the zero and infinity nodes. */
    _zeroNode.set  (0,  0,  0);
    _inftyNode.set (0, ~0, 0);

	/** We create a synchronizer. */
    _synchro = DefaultFactory::thread().newSynchronizer();

    DEBUG (("RangeTree::RangeTree  _sizeX=%d  _sizeY=%d   max=%d _maxSizeXY=%d  _maxSizeXYLog2=%d  _nbHeadsLog2=%d  nbHeads=%d _headsMask=%d\n",
		_sizeX, _sizeY, MAX (_sizeX, _sizeY), _maxSizeAlpha, _maxSizeAlphaLog2, _nbHeadsLog2, (1 << _nbHeadsLog2), _headsMask
	));

    printf ("RangeTree::RangeTree  _sizeX=%d  _sizeY=%d   max=%d _maxSizeXY=%d  _maxSizeXYLog2=%d  _nbHeadsLog2=%d  nbHeads=%d _headsMask=%d\n",
		_sizeX, _sizeY, MAX (_sizeX, _sizeY), _maxSizeAlpha, _maxSizeAlphaLog2, _nbHeadsLog2, (1 << _nbHeadsLog2), _headsMask
	);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
RangeTree::~RangeTree ()
{
	/** We loop each head of the binary tree. */
	int dmin=10000, dmax=0;
	FILE* file = fopen ("/tmp/depth.out", "w");
	for (size_t i=0; i<(1<<_nbHeadsLog2); i++)
	{
		int d = _heads[i]->getDepth();
		if (d<dmin)  { dmin = d; }
		if (d>dmax)  { dmax = d; }

		fprintf (file, "%ld %ld %ld %ld\n", i, d, dmin, dmax);
	}
	fclose (file);

	printf ("mindepth=%ld  maxdepth=%ld\n", dmin, dmax);

#if 0
	/** We delete the table of heads itself. */
	delete [] _heads;
#endif

	/** We get rid of the synchronizer. */
    delete _synchro;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void RangeTree::clean ()
{
	_nodePool.clean ();

#if 0
	/** We loop each head of the binary tree. */
	for (size_t i=0; i<(1<<_nbHeadsLog2); i++)  {  _heads[i].child[L] = _heads[i].child[R] = 0;  }
#else
	_head = 0;
#endif

	_nbItems = 0;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void RangeTree::insert (Value alpha, size_t length)
{
VERBOSE (("---------------------------------------------------- INSERT [%ld,%d] -------------------------------------------\n", alpha, length));
//printf ("---------------------------------------------------- INSERT [%ld,%d] -------------------------------------------\n", alpha, length);

	if (getHead(alpha) == 0)
	{
		setHead (_nodePool.createNode (0, alpha, length), alpha);
//		_head = _nodePool.createNode (0, alpha, length);
		_nbItems ++;
		return;
	}

    /** We compute the [A,B] interval from the provided arguments. */
    Value A = alpha;
    Value B = A + length - 1;

    Node*  minNode          = 0;
    Node*  maxNode          = 0;
    Node*  parentNode       = 0;
    Node** criticalNodePtr  = 0;
    Node** parentChildPtr   = 0;

    /** We retrieve the range [minNode,maxNode] where alpha is. */
    getBounds (alpha, minNode, maxNode, parentNode, criticalNodePtr, parentChildPtr);

    NodePtr& criticalNode = *criticalNodePtr;

    /** We compute the new range. */
    A = MAX (A, minNode->getB() + 1);
    B = MIN (B, maxNode->getA() - 1);

    DEBUG (("RangeTree::insert   [%ld,%ld]  minNode=[%ld,%ld]  maxNode=[%ld,%ld] => update  [%ld,%ld] \n",
        alpha, length, minNode->alpha, minNode->length, maxNode->alpha, maxNode->length, A, B
    ));

    /** We actually insert the range only if it is valid. */
    if (A <= B)
    {
		/** We create a new node. */
		Node* newNode = _nodePool.createNode (0, A, B - A + 1);

		/** We link the new node to the tree. */
		_synchro->lock();

#if 0
		bool isRight = A > parentNodeRef->alpha;

		for (Node* node=parentNode; node != 0;  )
		{
			parentNode = node;

			bool isRight = A > parentNode->alpha;

			if (isRight)  { node = node->right; }
			else          { node = node->left;  }
		}
#endif

		/****************************************************************/
		/** We link the new node to the tree.                           */
		/****************************************************************/
		/** We set the parent node. */
		newNode->parent = parentNode;

		/** We update the parent link with the created node. */
		*parentChildPtr= newNode;

		/****************************************************************/
		/** We adjust the balance factors from the critical node child. */
		/****************************************************************/
		DIRECTION dir = getDirection (alpha, criticalNode);
		NodePtr criticalNodeChild = criticalNode->child[dir];

VERBOSE (("critical [%ld,%d]   dir=%d   child=%p\n",
	criticalNode->alpha, criticalNode->balance,
	dir, criticalNodeChild
));

//	criticalNode->balance += (dir==R ? 1 : -1);

#if 1

		for (NodePtr node = criticalNodeChild; node && node != newNode; node = node->child[dir])
		{
			/** We compute the next direction. */
			dir = getDirection (alpha, node);

			BALANCE oldBalance = node->balance;

			/** We update the height of the current node. */
			node->balance = (dir == R ? BALANCED_RIGHT : BALANCED_LEFT);

			VERBOSE (("update balances ==> node=[%ld,%d,%p]  =>  newBalance=%d\n", node->alpha, oldBalance, node, node->balance));
		}

		/****************************************************************/
		/** We adjust the balance factor of the critical node. 			*/
		/****************************************************************/

#ifdef FOO
VERBOSE (("BEFORE =======> "));  _head->dump ();
#endif

		/** We get the direction of the critical node and its child. */
		DIRECTION dirCritical = getDirection (alpha, criticalNode);

		if (criticalNode->balance == BALANCED)
		{
VERBOSE (("CASE 1\n"));
			criticalNode->balance = (dirCritical == R ? BALANCED_RIGHT : BALANCED_LEFT);
		}

		else if (criticalNode->balance == BALANCED_RIGHT)
		{
VERBOSE (("CASE 2\n"));
criticalNode->balance += (dirCritical==R ? 1 : -1);

			if (dirCritical == L)  {  criticalNode->balance = BALANCED; VERBOSE (("CASE 2.1\n"));  }
			else
			{
VERBOSE (("CASE 2.2  criticalNodeChild=%p\n", criticalNodeChild));

				/** We have to re-balance the tree since we go to the right. */
				if (criticalNodeChild->balance == BALANCED_RIGHT)
				{
VERBOSE (("CASE 2.2.1\n"));
					/** single rotation. */
					criticalNode = Node::SingleRotationLeft (criticalNode);
				}
				else
				{
VERBOSE (("CASE 2.2.2\n"));
					/** double rotation. */
					criticalNode->child[R] = Node::SingleRotationRight (criticalNode->child[R]);
					criticalNode           = Node::SingleRotationLeft  (criticalNode);
				}
			}
		}

		else if (criticalNode->balance == BALANCED_LEFT)
		{
criticalNode->balance += (dirCritical==R ? 1 : -1);

VERBOSE (("CASE 3\n"));
			if (dirCritical == R)  {  criticalNode->balance = BALANCED;  VERBOSE (("CASE 3.1\n")); }
			else
			{
VERBOSE (("CASE 3.2  criticalNodeChild=%p\n", criticalNodeChild));
				/** We have to re-balance the tree since we go to the right. */
				if (criticalNodeChild->balance == BALANCED_LEFT)
				{
VERBOSE (("CASE 3.2.1\n"));
					/** single rotation. */
					criticalNode = Node::SingleRotationRight (criticalNode);
				}
				else
				{
VERBOSE (("CASE 3.2.2\n"));
					/** double rotation. */
					criticalNode->child[L] = Node::SingleRotationLeft  (criticalNode->child[L]);
					criticalNode           = Node::SingleRotationRight (criticalNode);
				}
			}
		}
#endif

#ifdef FOO
VERBOSE (("AFTER =======> "));  _head->dump ();
#endif

#if 0
checkBalance ();
#endif
		_synchro->unlock();

		/** We increase the number of items in the tree. */
		_nbItems ++;
    }

    /** Since we have reduce the initial range, we may have to insert another part in a recursive way. */
    int remaining = (alpha + length - 1) - B;

    if (remaining > 0)
    {
        DEBUG (("RangeTree::insert   recursion => [%3d,%3d] \n", B+1, remaining));
        insert (B+1, remaining);
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool RangeTree::exists (Value alpha)
{
    Node*  minNode = 0;

    /** We traverse the tree. */
    for (NodePtr node = getHead(alpha);  node != 0;  )
    {
    	/** NOTE: we could have less code below by writing  node=node->child[dir].
    	 *  BUT it leads to worse performance since the compiler can't statically optimizes
    	 *  the 'child' index to be used. (raw tests show that it would take about 20% more time
    	 *  for 4 millions call to 'exists').*/

        if (getDirection (alpha, node) == R)
        {
            /** Since we go to the right, we are sure that the current node will be smaller than
             * all the nodes to be traversed, so we keep it as a lowest bound. */
            minNode = node;

            /** We go to the right subtree. */
            node = node->child[R];
        }

        /** Otherwise, we go to left subtree  (ie. our value is lesser than the current node). */
        else
        {
            /** We go to the left subtree. */
            node = node->child[L];
        }
    }

    /** We check whether the provided argument is inside the left node. */
    bool result =  (minNode != 0) &&  minNode->include (alpha);

    DEBUG (("RangeTree::exists   alpha=%3d  => result=%d \n", alpha, result ));

    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void RangeTree::getBounds (Value val, NodePtr& minNode, NodePtr& maxNode, NodePtr& parent, NodePtr*& criticalNodePtr, NodePtr*& parentChildPtr)
{
    /** We initialize the result with the tree head. */
    minNode      = 0;
    maxNode      = 0;

    parentChildPtr  = &getHead(val);
    criticalNodePtr = &getHead(val);

    /** We traverse the tree. */
    for (NodePtr node = getHead(val);  node != 0;  )
    {
        /** We keep the current node as the parent. */
    	parent = node;

    	if ((*parentChildPtr)->balance != 0)  { criticalNodePtr = parentChildPtr; }

        /** If our value is greater or equals than the current node, we go to right subtree. */
        if (val >= node->alpha)
        {
            /** Since we go to the right, we are sure that the current node will be smaller than
             * all the nodes to be traversed, so we keep it as a lowest bound. */
            minNode = node;

            parentChildPtr = &(node->child[R]);

            /** We go to the right subtree. */
            node = node->child[R];
        }

        /** Otherwise, we go to left subtree  (ie. our value is lesser than the current node). */
        else
        {
            maxNode = node;

            parentChildPtr = &(node->child[L]);

            /** We go to the left subtree. */
            node = node->child[L];
        }
    }

    DEBUG (("RangeTree::getBounds   parentChildPtr=%p  *parentChildPtr=%p   criticalNodePtr=%p  *criticalNodePtr=%p\n",
		parentChildPtr, *parentChildPtr, criticalNodePtr, *criticalNodePtr
	));

    if (minNode == 0)  { minNode = &_zeroNode; }
    if (maxNode == 0)  { maxNode = &_inftyNode; }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t RangeTree::getDepth ()
{
	size_t result = 0;

#if 1
	for (size_t i=0; i<(1<<_nbHeadsLog2); i++)
	{
		size_t d = _heads[i]->getDepth();

		result = MAX (result, d);
	}
#else
	result = _head->getDepth();
#endif

	return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
class InsertFunctor
{
public:
	InsertFunctor (RangeTree& tree, const vector<RangeTree::Data>& data) : _tree(tree), _data(data)  {}

	void operator()  (size_t i)  {  _tree.insert (_data[i].alpha, _data[i].length);  }

private:
	RangeTree& _tree;
	const vector<RangeTree::Data>& _data;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void RangeTree::reorder ()
{
	GetDataFunctor getData;

	/** We retrieve the data from the nodes. */
#if 0
	for (size_t i=0; i<(1<<_nbHeadsLog2); i++)
	{
		if (_heads[i].child[L])  { _heads[i].child[L]->recurseInOrder(getData); }
		if (_heads[i].child[R])  { _heads[i].child[R]->recurseInOrder(getData); }
	}
#else
	if (_head->child[L])  { _head->child[L]->recurseInOrder(getData); }
	if (_head->child[R])  { _head->child[R]->recurseInOrder(getData); }

#endif

	vector<Data> dataVector = getData.getDataVector();

	/** We sort the list. */
	std::sort (dataVector.begin(), dataVector.end(), Data::compare);

	/** We clean the trees. */
	clean ();

	/** Now we have to iterate all integers in [0..nb[ in a way to put the middle item in the first location. */
	InsertFunctor functor (*this, dataVector);
	IntegerIterator<InsertFunctor> it (0, dataVector.size()-1, functor);
	it.iterate();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
RangeTree::NodePool::NodePool (size_t arraySize) : _synchro(0), _arraySize(arraySize), _currentIdx(0), _currentArray(0)
{
	_synchro = DefaultFactory::thread().newSynchronizer();

	/** We create a new array of nodes. */
	_currentArray = getNewArray ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
RangeTree::NodePool::~NodePool ()
{
	clean ();

	if (_synchro)  { delete _synchro; }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
RangeTree::Node* RangeTree::NodePool::createNode (Node* p, Value a, size_t l)
{
	RangeTree::Node* result = 0;

	_synchro->lock ();

	if (_currentIdx >= _arraySize)  { _currentArray = getNewArray ();  }

	result = _currentArray + (_currentIdx ++);

	result->set (p,a,l);

	_synchro->unlock ();

	return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
RangeTree::Node* RangeTree::NodePool::getNewArray ()
{
	RangeTree::Node* result = new Node [_arraySize];
	_currentIdx = 0;
	_arrayList.push_back (result);
	return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void RangeTree::NodePool::clean ()
{
	_synchro->lock ();

	for (list<Node*>::iterator it = _arrayList.begin(); it != _arrayList.end(); it++)  {  delete []  (*it);  }

	_arrayList.clear();

	_currentIdx = _arraySize;

	_synchro->unlock ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool RangeTree::Node::checkBalance (Node* node)
{
	bool result = true;

	if (node != 0)
	{
		int balL = (node->child[L] ? node->child[L]->getDepth() : 0);
		int balR = (node->child[R] ? node->child[R]->getDepth() : 0);

		int diff = balR - balL;

		result = ((node->balance == BALANCED)  && diff == 0)  ||  ((node->balance == BALANCED_RIGHT)  && diff > 0)  || ((node->balance == BALANCED_LEFT)  && diff < 0);

if (result==false ||  diff>1  || diff<-1)
{
printf ("BAD BALANCE for node [%ld,%d,%p,%p] : balR=%d  balL=%d  diff=%d\n", node->alpha, node->balance, node->child[L], node->child[R], balL, balR, diff);
node->dump ();
exit (1);
}

		result = result & checkBalance (node->child[L]) & checkBalance (node->child[R]);
	}

	return result;
}

/********************************************************************************/
}  /* end of namespaces. */
/********************************************************************************/

#endif
