/*******************************************************************************
 * 
 * @file /vobs/th_comp/util_design_pattern/src/dp_observer.cpp
 *
 * @brief this file contains the definition of the EventInfoLocator class
 *
 * Copyright (C) 2009-2010 Thomson
 * All Rights Reserved
 *
 * This program contains proprietary information which is a trade
 * secret of THOMSON and/or its affiliates and also is protected as
 * an unpublished work under applicable Copyright laws. Recipient is
 * to retain this program in confidence and is not permitted to use or
 * make copies thereof other than as permitted in a written agreement
 * with THOMSON, unless otherwise expressly allowed by applicable laws
 *
 ******************************************************************************/

#include "Observer.hpp"

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;

/********************************************************************************/
namespace dp {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
Subject::Subject ()
:   _interface(0), _singleObserver(0), _observers(0)
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
Subject::Subject (const InterfaceId& interface)
:   _interface(interface), _singleObserver(0), _observers(0)
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
Subject::~Subject()
{
    if (_observers != 0)  { delete _observers; }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void Subject::notify (EventInfo* event)
{
    if (event !=0  &&  _observers != 0)
    {
        /** We locally use the event. */
        LOCAL (event);

        /** Optimization in case we have only one observer. */
        if (_singleObserver != 0)  {  _singleObserver->update (event, this);  }

        else
        {
            for (std::list<IObserver*>::iterator it=_observers->begin(); it!=_observers->end(); it++)
            {
                (*it)->update (event, this);
            }
        }
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
void Subject::addObserver (IObserver* observer)
{
    /** We check the list. */
    if (_observers==0)  { _observers = new std::list<IObserver*> () ; }

    /** We add the observer into the list. */
    _observers->push_back (observer);

    /** We may use the 'single observer' optimization. */
    _singleObserver = (_observers->size() == 1  ? _observers->front() : 0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void Subject::removeObserver (IObserver* observer)
{
    /** We check the list. */
    if (_observers==0)  { _observers = new std::list<IObserver*> () ; }

    /** We remove the observer from the list. */
    _observers->remove (observer);

    /** We may use the 'single observer' optimization. */
    _singleObserver = (_observers->size() == 1  ? _observers->front() : 0);
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
