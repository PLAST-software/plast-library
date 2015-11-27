/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

#if 0

#include <misc/api/RangeTree.hpp>
#include <misc/api/macros.hpp>

#include <os/impl/DefaultOsFactory.hpp>

#include <algorithm>
#include <list>

using namespace std;
using namespace os::impl;

#define FOO

#ifdef FOO
	#define DEBUG(a)    //printf a
	#define VERBOSE(a)  //printf a
#else
	#define DEBUG(a)
	#define VERBOSE(a)
#endif

/********************************************************************************/
namespace misc {
/********************************************************************************/

#define BALANCED_LEFT   -1
#define BALANCED_RIGHT   1
#define BALANCED         0

/** */
static size_t log2 (size_t n)   {   size_t res = 0;   while (n >>= 1)  {  res++;  }  return res;  }

RangeTree::Node  RangeTree::_zeroNode  ( 0, 0);
RangeTree::Node  RangeTree::_inftyNode (~0, 0);

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

    /** We set the number of heads we want to use. */
    _nbHeads = 1 << _nbHeadsLog2;

    /** We will need % operator on the number of heads => use a mask because we have a power of 2 here. */
    _headsMask = _nbHeads - 1;

    /** We create an array of binary trees references. */
    _heads = new NodePtr [_nbHeads];
    for (size_t i=0; i<_nbHeads; i++)  {  _heads[i] = 0; }

	/** We create a synchronizer. */
    _synchro = DefaultFactory::thread().newSynchronizer();

    DEBUG (("RangeTree::RangeTree  _sizeX=%d  _sizeY=%d   max=%d _maxSizeXY=%d  _maxSizeXYLog2=%d  _nbHeadsLog2=%d  nbHeads=%d _headsMask=%d\n",
		_sizeX, _sizeY, MAX (_sizeX, _sizeY), _maxSizeAlpha, _maxSizeAlphaLog2, _nbHeadsLog2, (1 << _nbHeadsLog2), _headsMask
	));
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
    DEBUG (("RangeTree::~RangeTree  nbItems=%d  depth=%d\n", getNbItems(), getDepth()));

	/** We get rid of the synchronizer. */
    delete _synchro;

    delete [] _heads;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool RangeTree::insert (Value alpha, size_t length)
{
	bool result = false;

//if (alpha == 296253)  { printf ("insert [%ld] with len=%ld  [%ld,%ld] \n", alpha, length, alpha/_maxSizeAlpha, 0); }

	VERBOSE (("---------------------------------------------------- INSERT [%ld,%d] -------------------------------------------\n", alpha, length));

	if (getHead(alpha) == 0)
	{
	    NodePtr node = & _nodePool.createNode().set(alpha, length);
		setHead (node, alpha);
		_nbItems ++;
		return false;
	}

    /** We compute the [A,B] interval from the provided arguments. */
	Value A = alpha;
	Value B = A + length - 1;

	NodePtr  minNode          = 0;
	NodePtr  maxNode          = 0;
	NodePtr* criticalRangeNodePtr  = 0;
	NodePtr* parentChildPtr   = 0;

    /** We retrieve the range [minNode,maxNode] where alpha is. */
    getBounds (alpha, minNode, maxNode, criticalRangeNodePtr, parentChildPtr);

    NodePtr& criticalNode = *criticalRangeNodePtr;

    /** We compute the new range. */
    A = MAX (A, minNode->getB() + 1);
    B = MIN (B, maxNode->getA() - 1);

    VERBOSE (("RangeTree::insert   [%ld,%ld]  minNode=[%ld,%ld]  maxNode=[%ld,%ld] => update  [%ld,%ld] \n",
        alpha, length, minNode->alpha, minNode->length, maxNode->alpha, maxNode->length, A, B
    ));

    /** We actually insert the range only if it is valid. */
    if (A <= B)
    {
//if (A == 296253)  { printf ("insert2 alpha=%ld with len=%ld  [A=%ld,B=%ld | %ld] minNode=[%ld:%ld | %ld]\n",
//	alpha, length, A,B, B-A+1, minNode->getA(), minNode->getB(), minNode->getLength()
//);	}

		/** We create a new node. */
		NodePtr newNode = & _nodePool.createNode().set(A,B-A+1);

		/** We link the new node to the tree. */
		_synchro->lock();

		/****************************************************************/
		/** We link the new node to the tree.                           */
		/****************************************************************/

		/** We update the parent link with the created node. */
		*parentChildPtr= newNode;

		/****************************************************************/
		/** We adjust the balance factors from the critical node child. */
		/****************************************************************/
		DIRECTION dir = criticalNode->getDirection (alpha);
		NodePtr criticalNodeChild = criticalNode->child[dir];

		VERBOSE (("critical [%ld,%d]   dir=%d   child=%p\n",
			criticalNode->alpha, criticalNode->balance,
			dir, criticalNodeChild
		));

		for (NodePtr node = criticalNodeChild; node && node != newNode; node = node->child[dir])
		{
			/** We compute the next direction. */
			dir = node->getDirection (alpha);

			/** We update the height of the current node. */
			node->balance = (dir == R ? BALANCED_RIGHT : BALANCED_LEFT);

			VERBOSE (("update balances ==> node=[%ld,%d]\n", node->alpha, node->balance));
		}

		/****************************************************************/
		/** We adjust the balance factor of the critical node. 			*/
		/****************************************************************/
		/** We get the direction of the critical node and its child. */
		DIRECTION dirCritical = criticalNode->getDirection (alpha);

		if (criticalNode->balance == BALANCED)
		{
			criticalNode->balance = (dirCritical == R ? BALANCED_RIGHT : BALANCED_LEFT);
		}

		else if (criticalNode->balance == BALANCED_RIGHT)
		{
			criticalNode->balance += (dirCritical==R ? 1 : -1);

			if (dirCritical == L)  {  criticalNode->balance = BALANCED;  }
			else
			{
				/** We have to re-balance the tree since we go to the right. */
				if (criticalNodeChild->balance == BALANCED_RIGHT)
				{
					/** single rotation. */
					criticalNode = criticalNode->SingleRotationLeft ();
				}
				else
				{
					/** double rotation. */
					criticalNode->child[R] = criticalNode->child[R]->SingleRotationRight ();
					criticalNode           = criticalNode->SingleRotationLeft  ();
				}
			}
		}

		else if (criticalNode->balance == BALANCED_LEFT)
		{
			criticalNode->balance += (dirCritical==R ? 1 : -1);

			if (dirCritical == R)  {  criticalNode->balance = BALANCED;  }
			else
			{
				/** We have to re-balance the tree since we go to the right. */
				if (criticalNodeChild->balance == BALANCED_LEFT)
				{
					/** single rotation. */
					criticalNode = criticalNode->SingleRotationRight ();
				}
				else
				{
					/** double rotation. */
					criticalNode->child[L] = criticalNode->child[L]->SingleRotationLeft  ();
					criticalNode           = criticalNode->SingleRotationRight ();
				}
			}
		}

		_synchro->unlock();

		/** We increase the number of items in the tree. */
		_nbItems ++;
    }
    else
    {
    	/** The range already exists. */
    	result = true;
    }

    /** Since we have reduce the initial range, we may have to insert another part in a recursive way. */
    int remaining = (alpha + length - 1) - B;

    if (remaining > 0)
    {
    	VERBOSE (("RangeTree::insert   recursion => [%3d,%3d] \n", B+1, remaining));
//printf ("RangeTree::insert   recursion => [%3d,%3d] \n", B+1, remaining);
        result = insert (B+1, remaining);
    }

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
bool RangeTree::exists (Value alpha)
{
    NodePtr  minNode = 0;

    /** We traverse the tree. */
    for (NodePtr node = getHead(alpha);  node != 0;  )
    {
    	/** NOTE: we could have less code below by writing  node=node->child[dir].
    	 *  BUT it leads to worse performance since the compiler can't statically optimizes
    	 *  the 'child' index to be used. (raw tests show that it would take about 20% more time
    	 *  for 4 millions call to 'exists').*/

        if (node->getDirection (alpha) == R)
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

//if (alpha == 296254)  { printf ("minNode=%p  [%ld,%ld] \n", minNode, minNode->getA(), minNode->getLength()); }


    /** We check whether the provided argument is inside the left node. */
    bool result =  (minNode != 0) &&  minNode->include (alpha);

    VERBOSE (("RangeTree::exists   alpha=%3ld  => result=%d \n", alpha, result ));

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
void RangeTree::getBounds (Value val, NodePtr& minNode, NodePtr& maxNode, NodePtr*& criticalNodePtr, NodePtr*& parentChildPtr)
{
    /** We initialize the result with the tree head. */
    minNode      = 0;
    maxNode      = 0;

    parentChildPtr  = &getHead(val);
    criticalNodePtr = &getHead(val);

    /** We traverse the tree. */
    for (NodePtr node = getHead(val);  node != 0;  )
    {
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

    VERBOSE (("RangeTree::getBounds   parentChildPtr=%p  *parentChildPtr=%p   criticalRangeNodePtr=%p  *criticalRangeNodePtr=%p\n",
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

	for (size_t i=0; i<_nbHeads; i++)
	{
		size_t d = (_heads[i] ? _heads[i]->getDepth() : 0);

		result = MAX (result, d);
	}

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
bool RangeTree::checkBalance ()
{
    return getHead()->checkBalance ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool RangeTree::Node::checkBalance ()
{
	bool result = false;

    int balL = (child[L] ? child[L]->getDepth() : 0);
    int balR = (child[R] ? child[R]->getDepth() : 0);

    int diff = balR - balL;

    result = ((balance == BALANCED)  && diff == 0)  ||  ((balance == BALANCED_RIGHT)  && diff > 0)  || ((balance == BALANCED_LEFT)  && diff < 0);

    if (result == false)
    {
        if (child[L])  { return child[L]->checkBalance(); }
        if (child[R])  { return child[R]->checkBalance(); }
    }

	return result;
}

/********************************************************************************/
}  /* end of namespaces. */
/********************************************************************************/
#endif
