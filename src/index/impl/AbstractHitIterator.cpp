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

#include "AbstractHitIterator.hpp"
#include "macros.hpp"
#include "Property.hpp"
#include <queue>
#include "macros.hpp"

using namespace std;
using namespace dp;
using namespace database;
using namespace seed;

#include <stdio.h>
#define DEBUG(a) // printf a

/********************************************************************************/
namespace indexation {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
u_int64_t AbstractHitIterator::getInputHitsNumber ()
{
    u_int64_t result = 0;

    for (size_t i=0; i<_splitIterators.size(); i++)
    {
        result += _splitIterators[i]->getInputHitsNumber();
    }

    if (result==0)  { result += _inputHitsNumber; }

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
u_int64_t AbstractHitIterator::getOutputHitsNumber ()
{
    u_int64_t result = 0;

    for (size_t i=0; i<_splitIterators.size(); i++)
    {
        result += _splitIterators[i]->getOutputHitsNumber();
    }

    if (result==0)  { result += _outputHitsNumber; }

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
dp::IProperties* AbstractHitIterator::getProperties ()
{
    dp::IProperties* props = new dp::Properties ();

    if (getSourceIterator() != 0)
    {
        props->add (0, getSourceIterator()->getProperties ());
    }

    u_int64_t input  = getInputHitsNumber();
    u_int64_t output = getOutputHitsNumber();
    double ratio = (input > 0 ? ((double) output / (double)input) :  0.0);

    props->add (0, getName());

    props->add (1, "in",    "%ld", input);
    props->add (1, "out",   "%ld", output);
    props->add (1, "ratio", "%g",  ratio);
    //props->add (1, "time",  "%ld", _totalTime);

    return props;
}


//void AbstractHitIterator::getProperties (list<HitProperty>& info)
//{
//    for (size_t i=0; i<_splitIterators.size(); i++)
//    {
//        /** We retrieve the information for the current split. */
//        list<HitProperty> currentList;
//        _splitIterators[i]->getProperties (currentList);
//
//        /** We loop theses properties. */
//        list<HitProperty>::iterator currentIt;
//        for (currentIt = currentList.begin();  currentIt != currentList.end(); currentIt++)
//        {
//            /** We loop the result list. */
//            list<HitProperty>::iterator resultIt;
//            for (resultIt = info.begin();  resultIt != info.end(); resultIt++)
//            {
//                if (resultIt->key.compare (currentIt->key) == 0)
//                {
//                    resultIt->value += currentIt->value;
//                }
//            }
//        }
//    }
//}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AbstractHitIterator::setSplitIterators (const std::vector<IHitIterator*>& split)
{
    for (size_t i=0; i<_splitIterators.size(); i++)
    {
        /** We detach ouself as listener. */
        _splitIterators[i]->removeObserver(this);

        /** We forget the split. */
        _splitIterators[i]->forget();
    }

    _splitIterators = split;

    for (size_t i=0; i<_splitIterators.size(); i++)
    {
        /** We use this split. */
        _splitIterators[i]->use ();

        /** We attach ouself as listener. */
        _splitIterators[i]->addObserver (this);
    }
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
