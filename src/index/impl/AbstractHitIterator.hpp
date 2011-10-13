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

#ifndef _ABSTRACT_HITITERATOR_HPP_
#define _ABSTRACT_HITITERATOR_HPP_

/********************************************************************************/

#include "IHitIterator.hpp"

#include <stdio.h>

/********************************************************************************/
namespace indexation  {
/********************************************************************************/

/** Iterates over hits.
 */
class AbstractHitIterator : public IHitIterator
{
public:

    AbstractHitIterator (size_t nbOccurs1, size_t nbOccurs2)
        : _hit(nbOccurs1,nbOccurs2),
          _inputHitsNumber(0), _outputHitsNumber(0),
          _maxHitsPerIteration (1000*1000),
          _totalTime(0) {}

    virtual ~AbstractHitIterator ()
    {
        for (size_t i=0; i<_splitIterators.size(); i++)  {  _splitIterators[i]->forget(); }
    }

    Hit* currentItem() {  return &_hit;  }

    u_int64_t getInputHitsNumber  ();
    u_int64_t getOutputHitsNumber ();

    /** */
    void update (dp::EventInfo* evt, dp::ISubject* subject)
    {
        /** We forward the event to potential clients. */
        if (this != subject)  {   notify (evt);  }
    }

    /** */
    IHitIterator* getSourceIterator ()  { return 0; }

    /**  */
    dp::IProperties* getProperties ();

protected:

    /** The 'current' item to be provided to the client through the 'currentItem' method. */
    Hit _hit;

    u_int64_t _inputHitsNumber;
    u_int64_t _outputHitsNumber;

    void setSplitIterators (const std::vector<IHitIterator*>& split);

    std::vector<IHitIterator*> _splitIterators;

    /** Threshold for controlling memory usage. */
    u_int32_t _maxHitsPerIteration;

    /** Statistics. */
    u_int32_t _totalTime;
};


/********************************************************************************/

class AbstractHitIteratorFactory : public IHitIteratorFactory
{
public:

    AbstractHitIteratorFactory () : _nextFactory(0) {}
    virtual ~AbstractHitIteratorFactory ()  { setNextFactory(0); }

    IHitIteratorFactory* getNextFactory ()  { return _nextFactory;  }

    void setNextFactory (IHitIteratorFactory* nextFactory)   {  SP_SETATTR (nextFactory);  }

private:
    IHitIteratorFactory* _nextFactory;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_HITITERATOR_HPP_ */
