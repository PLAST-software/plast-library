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

/** \file RangeTree.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Tree of ranges management.
 */

#ifndef _RANGE_TREE_HPP_
#define _RANGE_TREE_HPP_

/********************************************************************************/

#include <misc/api/types.hpp>
#include <misc/api/macros.hpp>
#include <os/api/IThread.hpp>

#include <vector>
#include <list>
#include <assert.h>

/********************************************************************************/
/** \brief Miscellanous definitions */
namespace misc {
/********************************************************************************/

class RangeTree
{
public:

    /** We define a type for the coordinates. */
    typedef u_int32_t Coord;

    /** We define a type that holds the alpha values. */
    typedef u_int64_t  Value;

    /** We define a Range structure. */
    class Range
    {
    public:
    	void set (Coord xx, Coord yy, size_t ll)  { x=xx; y=yy; length=ll; }
    	Coord x; Coord y; size_t length;
    };

    /** Constructor. */
    RangeTree (Coord sizeX, Coord sizeY, size_t nbHeadsLog2=0);

    /** Destructor. */
    ~RangeTree ();

    /** Insert a range given an initial [x,y] point and its length. */
    void insert (Coord x, Coord y, size_t length)
    {
    	/** We check that the point [x,y] lies in the XY space. */
    	assert (x < _sizeX  &&  y < _sizeY);

    	/** We insert the point [x,y] in the A space. */
    	insert (alpha (x,y), length);
    }

    /** Insert a range given the [alpha,length] couple. */
    void insert (Value alpha, size_t length);

    /** Insert a range given an initial [x,y] point and its length. */
    void insert (Range r)  { insert (r.x, r.y, r.length); }

    /** Tells whether a [x,y] point lies into some ranges. */
    bool exists (Coord x, Coord y)  {  return exists (alpha(x,y));  }

    /** Return the number of ranges in the tree. */
    size_t getNbItems () { return _nbItems; }

    /** Accessors to the dimensions of the grid. */
    size_t getSizeX ()  { return _sizeX; 		}
    size_t getSizeY ()  { return _sizeY; 		}
    size_t getSizeA ()  { return _maxSizeAlpha; }

    /** Return the depth of the inner tree. */
    size_t getDepth ();

    /** Compute the alpha value from a [x,y] point. */
    Value alpha (Coord x, Coord y)  { return 0 + (_maxSizeAlpha+1)*x + (_maxSizeAlpha)*(_sizeY-1 - y); }
    //Value alpha (Coord x, Coord y) { return x + _maxSizeAlpha* (x + _sizeY-1 - y); }

    /** */
    void reorder ();

    /** */
    void dump () {  getHead()->dump (); }

    /** */
    bool checkBalance ()  { return Node::checkBalance (getHead()); }

private:

    /** */
    void clean ();

    /** */
	struct Entry
	{
    	Value alpha;  size_t idx;
    	static bool compare (const Entry& a, const Entry& b)  { return a.alpha < b.alpha; }
	};

    /** Tells whether a alpha value is in some range. */
    bool exists (Value val);

    /********************************************************************************/
    class Data
    {
    public:
    	Data (Value  a, size_t l) : alpha(a), length(l) {}
        Value    alpha;
        size_t   length;
        static bool compare (const Data& i, const Data& j) { return i.alpha < j.alpha;}
    };

    /********************************************************************************/
    struct Node;  typedef Node* NodePtr;

    /********************************************************************************/
    typedef int8_t BALANCE;
    static const BALANCE BALANCED_LEFT  = -1;
    static const BALANCE BALANCED_RIGHT =  1;
    static const BALANCE BALANCED       =  0;

    /********************************************************************************/
    enum DIRECTION { L=0, R=1, DIRECTION_MAX};

    DIRECTION getDirection (Value alpha, NodePtr node)  {  return alpha >= node->alpha ? R : L; }

    /********************************************************************************/
    struct Node : public Data
    {
        Node () : Data(~0,0), parent(0), balance(BALANCED)   { child[L] = child[R] = 0; }

        static BALANCE opposite (BALANCE in)  {  return (BALANCE)  (-(int)in);  }

        /** */
        Node* set (Node* p, Value a, size_t l)
        {
        	parent = p;
        	alpha  = a;
        	length = l;
        	return this;
        }

        bool include (size_t val)  { return (alpha <= val) &&  (val < (alpha+length));  }

        Node*   parent;
        Node*   child[DIRECTION_MAX];
        BALANCE balance;


        /** */
        Value getA ()  { return alpha;              }
        Value getB ()  { return alpha + length - 1; }

        /** */
        template <typename Functor>  void recursePreOrder (Functor& f)
        {
            f (this);
            if (child[L]) { child[L]->recurse  (f);  }
            if (child[R]) { child[R]->recurse (f);   }
        }

        /** */
        template <typename Functor>  void recurse (Functor& f)
        {
            if (child[L]) { child[L]->recurse  (f);  }
            if (child[R]) { child[R]->recurse (f);   }
            f (this);
        }

        /** */
        template <typename Functor>  void recurseInOrder (Functor& f)
        {
            if (child[L])  { child[L]->recurse  (f);  }
            f (this);
            if (child[R])  { child[R]->recurse (f);   }
        }

		/** */
        void cleanSubTree ()
        {
        	CleanFunctor f;
        	if (child[L])   {  child[L]->recurse  (f); }
        	if (child[R])   {  child[R]->recurse (f); }
        }

        /** */
        size_t getDepth ()
        {
            size_t currentDepth=0, maxDepth=0;

            getDepth_aux (this, currentDepth, maxDepth);

            return 1 + maxDepth;
        }

        /** */
        static void rebalance (Node*& node);

        /** */
        void dump ()  {  dump_aux (this); printf ("\n"); }

        void dump_aux (NodePtr node)
        {
        	if (node)
        	{
        		printf ("(%ld,%d)", node->alpha, node->balance);
        		if (node->child[L])  {  printf ("L["); dump_aux (node->child[L]);  printf ("]");  }
        		if (node->child[R])  {  printf ("R["); dump_aux (node->child[R]);  printf ("]");  }
        	}
        }


        void getDepth_aux (Node* node, size_t& currentDepth, size_t& maxDepth)
        {
            if (node != 0)
            {
                if (node->child[L])   {  currentDepth++;  if (currentDepth>maxDepth) { maxDepth=currentDepth; }  getDepth_aux (node->child[L], currentDepth, maxDepth);  currentDepth--; }
                if (node->child[R])   {  currentDepth++;  if (currentDepth>maxDepth) { maxDepth=currentDepth; }  getDepth_aux (node->child[R], currentDepth, maxDepth); currentDepth--; }
            }
        }

        /** a                    b            - b becomes the new root
         *   \       <--        / \           - a takes ownership of b's left child as right child
         *    b	       |       a   c          - b takes ownership of a as its left child
         *     \
         *      c
         */
        static NodePtr SingleRotationLeft   (NodePtr a)
        {
        	NodePtr b = a->child[R];
        	a->child[R] = b->child[L];
        	b->child[L] = a;

        	/** We update the balances. */
        	a->balance = a->balance - 1 - MAX (b->balance,0);
        	b->balance = b->balance - 1 + MIN (a->balance,0);

        	/** We return the new node. */
        	return b;
        }

        /**     a              b            - b becomes the new root
         *     /      -->     / \           - a takes ownership of b's right child as left child
         *    b	      |      c   a          - b takes ownership of a as its right child
         *   /
         *  c
         */
        static NodePtr SingleRotationRight (NodePtr a)
        {
        	NodePtr b = a->child[L];
			a->child[L] = b->child[R];
        	b->child[R] = a;

        	/** We update the balances. */
        	a->balance = a->balance + 1 - MIN (b->balance,0);
        	b->balance = b->balance + 1 + MAX (a->balance,0);

        	/** We return the new node. */
        	return b;
        }

        /** */
        static bool checkBalance (Node* node);

    };  /*  struct Node */

    /** */
    class CleanFunctor  {  public:   void operator () (Node* node)  {  if (node)  { delete node; } }  };

    /** */
    class GetDataFunctor
    {
	public:
    	GetDataFunctor () {}
    	void operator () (Node* node)  {  _data.push_back (Data (node->alpha, node->length));  }
    	std::vector<Data>& getDataVector ()  { return _data; }
	private:
    	std::vector<RangeTree::Data> _data;
    };

    /** */
    Coord _sizeX;
    Coord _sizeY;
    Coord _maxSizeAlpha;

    size_t _maxSizeAlphaLog2;
    size_t _nbHeadsLog2;
    size_t _headsMask;

Node* _head;
    NodePtr* _heads;
    Node  _zeroNode;
    Node  _inftyNode;

    size_t _nbItems;

    /** We need a synchronizer. */
    os::ISynchronizer* _synchro;

    /** */
    void setHead (NodePtr node, Value alpha=0)
    {
    	_heads [((alpha >> _maxSizeAlphaLog2) & _headsMask)] = node;
//printf ("setHead(%ld)  idx=%d  => %p \n", alpha, ((alpha >> _maxSizeAlphaLog2) & _headsMask), _heads [((alpha >> _maxSizeAlphaLog2) & _headsMask)] );
    }

    /** */
    NodePtr& getHead (Value alpha=0)
    {
    	/** We look for the head node for the underlying diagonal.
    	 * 		1) we compute the diagonal with alpha >> _maxSizeAlphaLog2
    	 * 		2) we compute the diagonal index in the heads table
    	 * 			=> the modulo operation is done with a & operator
    	 * 			   since the number of heads is a power of 2
    	 */
#if 1
//printf ("getHead(%ld)  idx=%d  => %p \n", alpha, ((alpha >> _maxSizeAlphaLog2) & _headsMask), _heads [((alpha >> _maxSizeAlphaLog2) & _headsMask)] );
    	return _heads [((alpha >> _maxSizeAlphaLog2) & _headsMask)];
#else
    	return _head;
#endif
    }

    /** */
    void getBounds (Value alpha, NodePtr& left, NodePtr& right, NodePtr& parent, NodePtr*& criticalNodePtr, NodePtr*& parentChildPtr);

    /** */
    size_t log2 (size_t n)   {   size_t res = 0;   while (n >>= 1)  {  res++;  }  return res;  }

    /** */
    class NodePool
    {
    public:

    	NodePool (size_t arraySize);
    	~NodePool ();

    	Node* createNode (Node* p, Value a, size_t l);

    	void clean ();

    private:
    	os::ISynchronizer* _synchro;

    	size_t _arraySize;
    	size_t _currentIdx;
    	Node*  _currentArray;

    	Node* getNewArray ();
    	std::list<Node*> _arrayList;
    };

    NodePool _nodePool;

    friend class InsertFunctor;
    friend class NodePool;
};


/********************************************************************************/
}  /* end of namespaces. */
/********************************************************************************/

#endif /* _RANGE_TREE_HPP_ */
