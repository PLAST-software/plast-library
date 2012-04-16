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

#include <database/impl/CompositeSequenceDatabase.hpp>
#include <designpattern/impl/Property.hpp>
#include <misc/api/macros.hpp>

#include <stdlib.h>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace os;
using namespace os::impl;

/********************************************************************************/
namespace database { namespace impl  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
CompositeSequenceDatabase::CompositeSequenceDatabase (std::list<ISequenceDatabase*> children)
    : _sequencesTotalNb(0),_totalSize(0)
{
    _sequencesOffsets.push_back (0);
    _databasesOffsets.push_back (0);

    /** We use each child. */
    for (std::list<ISequenceDatabase*>::iterator it = children.begin(); it != children.end(); it++)
    {
        /** Shortcut. */
        ISequenceDatabase* current = *it;

        DEBUG (("CompositeSequenceDatabase::CompositeSequenceDatabase: orf=%p\n", current));

        _children.push_back (current);

        /** We use the instance. */
        current->use();

        _sequencesTotalNb += current->getSequencesNumber();
        _totalSize        += current->getSize();

        _sequencesOffsets.push_back (_sequencesTotalNb);
        _databasesOffsets.push_back (_totalSize);
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
CompositeSequenceDatabase::~CompositeSequenceDatabase ()
{
    DEBUG (("CompositeSequenceDatabase::~CompositeSequenceDatabase  this=%p\n", this));

    /** We forget each child. */
    for (std::vector<ISequenceDatabase*>::iterator it = _children.begin(); it != _children.end(); it++)
    {
        (*it)->forget();
    }

    _children.clear ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool CompositeSequenceDatabase::getSequenceByIndex (size_t index, ISequence& sequence)
{
    bool result = false;

    size_t imax = _sequencesOffsets.size() - 1;

    size_t i=0;
    for (i=0; i<imax; i++)
    {
        if ( (_sequencesOffsets[i] <= index) &&  (index <_sequencesOffsets[i+1]) )  { break; }
    }

    if (i<imax)
    {
        /** We look for the result in the actual database. */
        result = _children[i]->getSequenceByIndex (index - _sequencesOffsets[i], sequence);
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
bool CompositeSequenceDatabase::getSequenceByName (
    const std::string& id,
    ISequence& sequence
)
{
    bool result = false;

    for (size_t i=0; !result && i<_children.size(); i++)
    {
        result = _children[i]->getSequenceByName (id, sequence);
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
bool CompositeSequenceDatabase::getSequenceByOffset (
    u_int64_t  offset,
    ISequence& sequence,
    u_int32_t& offsetInSequence,
    u_int64_t& offsetInDatabase
)
{
    bool result = false;

    size_t imax = _databasesOffsets.size() - 1;

    size_t i=0;
    for (i=0; i<imax; i++)
    {
        if ( (_databasesOffsets[i] <= offset) &&  (offset <_databasesOffsets[i+1]) )  { break; }
    }

    if (i<imax)
    {
        /** We look for the result in the actual database. */
        result = _children[i]->getSequenceByOffset (
            offset - _databasesOffsets[i],
            sequence,
            offsetInSequence,
            offsetInDatabase
        );
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
ISequenceIterator* CompositeSequenceDatabase::createSequenceIterator ()
{
    ISequenceIterator* result = 0;

    list<ISequenceIterator*> itList;

    for (std::vector<ISequenceDatabase*>::iterator it = _children.begin(); it != _children.end(); it++)
    {
        itList.push_back ((*it)->createSequenceIterator());
    }

    result = new CompositeSequenceIterator (itList);

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
std::vector<ISequenceDatabase*> CompositeSequenceDatabase::split (size_t nbSplit)
{
    vector<ISequenceDatabase*> result;

    for (std::vector<ISequenceDatabase*>::iterator it = _children.begin(); it != _children.end(); it++)
    {
        vector<ISequenceDatabase*> tmp;
        tmp = (*it)->split (nbSplit);

        result.insert (result.end(), tmp.begin(), tmp.end());
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
IProperties* CompositeSequenceDatabase::getProperties (const std::string& root)
{
    IProperties* props = new Properties();

    props->add (0, root, "%lld bytes, %ld sequences", getSize(), getSequencesNumber());

    props->add (1, "size",           "%lld", getSize());
    props->add (1, "nb_sequences",   "%ld",  getSequencesNumber());

    return props;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
string CompositeSequenceDatabase::getId ()
{
    stringstream ss;

    for (std::vector<ISequenceDatabase*>::iterator it = _children.begin(); it != _children.end(); it++)
    {
        ss << (*it)->getId() << " ";
    }
    return ss.str();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void CompositeSequenceDatabase::retrieveSequencesIdentifiers (std::set<std::string>& ids)
{
    for (std::vector<ISequenceDatabase*>::iterator it = _children.begin(); it != _children.end(); it++)
    {
        (*it)->retrieveSequencesIdentifiers (ids);
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
CompositeSequenceIterator::CompositeSequenceIterator (std::list<ISequenceIterator*>& itList)
    : _itList (itList)
{
    /** We use each provided sequence iterator. */
    for (list<ISequenceIterator*>::iterator it = _itList.begin() ; it != _itList.end(); it++)
    {
        (*it)->use();
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
CompositeSequenceIterator::~CompositeSequenceIterator ()
{
    /** We release each provided sequence iterator. */
    for (list<ISequenceIterator*>::iterator it = _itList.begin() ; it != _itList.end(); it++)
    {
        (*it)->forget();
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
void CompositeSequenceIterator::iterate (void* aClient, Method method)
{
    for (list<ISequenceIterator*>::iterator it = _itList.begin(); it != _itList.end(); it++)
    {
        (*it)->iterate (aClient, method);
    }
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

