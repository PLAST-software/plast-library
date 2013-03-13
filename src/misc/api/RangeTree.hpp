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

#if 0

#ifndef _RANGE_TREE_HPP_
#define _RANGE_TREE_HPP_

/********************************************************************************/

#include <misc/api/types.hpp>
#include <misc/api/macros.hpp>
#include <os/api/IThread.hpp>

#include <misc/api/ObjectPool.hpp>

#include <vector>
#include <list>
#include <assert.h>

/********************************************************************************/
/** \brief Miscellaneous definitions */
namespace misc {
/********************************************************************************/

class RangeTree
{
public:

    /** We define a type that holds the alpha values. */
    typedef u_int64_t  Value;

    /** We define a type for the coordinates. */
    typedef u_int32_t Coord;

    /** */
    typedef int8_t BALANCE;

    /** */
    enum DIRECTION { L=0, R=1, DIRECTION_MAX};

    /** Constructor. */
    RangeTree (Coord sizeX, Coord sizeY, size_t nbHeadsLog2=0);

    /** Destructor. */
    ~RangeTree ();

    /** Insert a range given an initial [x,y] point and its length. */
    bool insert (Coord x, Coord y, size_t length)
    {
        /** We check that the point [x,y] lies in the XY space. */
        assert (x < _sizeX  &&  y < _sizeY);

        /** We insert the point [x,y] in the A space. */
        return insert (alpha (x,y), length);
    }

    /** Insert a range given the [alpha,length] couple. */
    bool insert (Value alpha, size_t length);

    /** Tells whether a [x,y] point lies into some ranges. */
    bool exists (Coord x, Coord y)
    {
        /** We check that the point [x,y] lies in the XY space. */
        assert (x < _sizeX  &&  y < _sizeY);

        /** We check whether the x,y point belongs to some range. */
        return exists (alpha(x,y));
    }

    /** Return the number of ranges in the tree. */
    size_t getNbItems () { return _nbItems; }

    /** Accessors to the dimensions of the grid. */
    size_t getSizeX ()  { return _sizeX; 		}
    size_t getSizeY ()  { return _sizeY; 		}
    size_t getSizeA ()  { return _maxSizeAlpha; }

    /** Return the depth of the inner tree. */
    size_t getDepth ();

    /** */
    bool checkBalance ();

private:

    /********************************************************************************/
    class Node
    {
    public:

        /** Constructor. */
        Node () : alpha(0), length(0), balance(0)   { child[L] = child[R] = 0; }

        Value    getAlpha  ()  const  { return alpha;   }
        size_t   getLength ()  const  { return length;  }
        BALANCE  getBalance()  const  { return balance; }

    private:
        /** Constructor. */
        Node (Value a, size_t l) : alpha(a), length(l), balance(0)   { child[L] = child[R] = 0;  }

        /** */
        DIRECTION getDirection (Value alpha)  {  return alpha >= this->alpha ? R : L; }

        /** Tells whether a value belongs to the range defined by the current node. */
        bool include (size_t val)  { return (alpha <= val) &&  (val < (alpha+length));  }

        /** */
        size_t getDepth ()  {  size_t d=0, maxd=0;  return getDepth (d, maxd); }

    private:

        /** Attributes of a node => sizeof(Node)=40 bytes */
        Node*    child[DIRECTION_MAX];
        Value    alpha;
        size_t   length;
        BALANCE  balance;

        /** */
        Node& set (Value a, size_t l)
        {
            alpha  = a;
            length = l;
            return *this;
        }

        /** */
        Value getA ()  { return alpha;              }
        Value getB ()  { return alpha + length - 1; }

        /** */
        size_t getDepth (size_t& depth, size_t& maxd)
        {
            depth++;

            maxd = MAX (maxd, depth);

            if (child[L])   { child[L]->getDepth (depth, maxd);  }
            if (child[R])   { child[R]->getDepth (depth, maxd);  }

            depth--;

            return maxd;
        }

        /** a                    b            - b becomes the new root
         *   \       <--        / \           - a takes ownership of b's left child as right child
         *    b        |       a   c          - b takes ownership of a as its left child
         *     \
         *      c
         */
        Node* SingleRotationLeft ()
        {
            Node* a = this;
            Node* b = a->child[R];
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
         *    b       |      c   a          - b takes ownership of a as its right child
         *   /
         *  c
         */
        Node* SingleRotationRight ()
        {
            Node* a = this;
            Node* b = a->child[L];
            a->child[L] = b->child[R];
            b->child[R] = a;

            /** We update the balances. */
            a->balance = a->balance + 1 - MIN (b->balance,0);
            b->balance = b->balance + 1 + MAX (a->balance,0);

            /** We return the new node. */
            return b;
        }

        /** */
        bool checkBalance ();

        friend class RangeTree;

    };  /*  class Node */

    /** Shortcut */
    typedef Node* NodePtr;

    /** Compute the alpha value from a [x,y] point. */
    Value alpha (Coord x, Coord y)  { return 0 + (_maxSizeAlpha+1)*x + (_maxSizeAlpha)*(_sizeY-1 - y); }

    /** Tells whether a alpha value is in some range. */
    bool exists (Value alpha);

    /** */
    Coord _sizeX;
    Coord _sizeY;
    Coord _maxSizeAlpha;

    size_t _maxSizeAlphaLog2;
    size_t _nbHeads;
    size_t _nbHeadsLog2;
    size_t _headsMask;

    NodePtr* _heads;

    size_t _nbItems;

    /** We need a synchronizer. */
    os::ISynchronizer* _synchro;

    /** */
    void setHead (NodePtr node, Value alpha=0)
    {
        _heads [((alpha >> _maxSizeAlphaLog2) & _headsMask)] = node;
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
        return _heads [((alpha >> _maxSizeAlphaLog2) & _headsMask)];
    }


    /** Retrieve the two nodes that interleaves the provided alpha value. */
    void getBounds (Value alpha, NodePtr& left, NodePtr& right, NodePtr*& criticalRangeNodePtr, NodePtr*& parentChildPtr);

    /** We use an object pool for getting instances of Node class. */
    misc::ObjectPool<Node> _nodePool;

    static Node  _zeroNode;
    static Node  _inftyNode;
};

/********************************************************************************/

class RangeTreeTools
{
public:

    /** */
    static RangeTreeTools singleton ()  { static RangeTreeTools instance; return instance; }
};

/********************************************************************************/
}  /* end of namespaces. */
/********************************************************************************/

#endif /* _RANGE_TREE_HPP_ */
#endif
