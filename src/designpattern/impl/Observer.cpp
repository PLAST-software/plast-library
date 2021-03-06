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

#include <designpattern/impl/Observer.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;

/********************************************************************************/
namespace dp { namespace impl {
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
    if (event != 0)
    {
        /** We locally use the event. */
        LOCAL (event);

        if (_observers != 0)
        {
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
} } /* end of namespaces. */
/********************************************************************************/
