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

#include <index/impl/AbstractDatabaseIndex.hpp>
#include <designpattern/impl/Property.hpp>

#include <misc/api/macros.hpp>

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace database;
using namespace seed;

#include <stdio.h>
#define DEBUG(a)  //printf a

/********************************************************************************/
namespace indexation { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractDatabaseIndex::AbstractDatabaseIndex (ISequenceDatabase* database, ISeedModel* model)
    : _database(0), _model(0), _maxSeedsNumber(0)
{
    setDatabase         (database);
    setModel            (model);

    _maxSeedsNumber = _model->getSeedsMaxNumber();  // little optimization
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractDatabaseIndex::~AbstractDatabaseIndex ()
{
    /** We get rid of the database and the seed model. */
    setDatabase         (0);
    setModel            (0);

    /** We forget all children. */
    for (std::list<IDatabaseIndex*>::iterator it=_children.begin(); it != _children.end(); it++)
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
void AbstractDatabaseIndex::addChildIndex (IDatabaseIndex* child)
{
    if (CHECKPTR(child))
    {
        /** We use the object. */
        child->use();

        /** We add it into the vector. */
        _children.push_back (child);
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
void AbstractDatabaseIndex::removeChildIndex (IDatabaseIndex* child)
{
    /** We look for the child. */
    for (std::list<IDatabaseIndex*>::iterator it=_children.begin(); it != _children.end(); it++)
    {
        if (child == (*it))
        {
            /** We forget the object. */
            child->forget ();

            /** We remove the entry from the container. */
            _children.erase (it);

            /** Just leave. */
            break;
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
IProperties* AbstractDatabaseIndex::getProperties (const std::string& root)
{
    IProperties* props = new Properties();

    props->add (0, root);

    ISeedModel* model = this->getModel();

    /** We create a seeds iterator. */
    ISeedIterator* it = model->createAllSeedsIterator();
    LOCAL (it);

    size_t nbMax  =  0;
    size_t nbMin  = ~0;

    size_t nbPos  =  0;
    size_t nbZero =  0;

    IWord maxWord;
    IWord minWord;

    /** We loop all possible seeds. */
    for (it->first(); !it->isDone(); it->next())
    {
        const ISeed* seed = it->currentItem();

        size_t nb =  this->getOccurrenceNumber (seed);

        if (nb > nbMax)  { nbMax = nb;  maxWord = seed->kmer;  }
        if (nb < nbMin)  { nbMin = nb;  minWord = seed->kmer;  }

        if (nb > 0)  { nbPos++;     }
        else         { nbZero++;    }
    }

    props->add (1, root);

    props->add (1, "min_occur",   "%s (%ld)", minWord.toString().c_str(), nbMin);
    props->add (2, "number",      "%ld", nbMin);
    props->add (2, "kmer",        "%s",  minWord.toString().c_str());

    props->add (1, "max_occur",   "%s (%ld)", maxWord.toString().c_str(), nbMax);
    props->add (2, "number",      "%ld", nbMax);
    props->add (2, "kmer",         "%s",  maxWord.toString().c_str());

    props->add (1, "found_nb",       "%ld", nbPos);

    float ratio = (nbPos + nbZero > 0 ?  100.0 * (float)nbPos / (float)(nbPos + nbZero) : 0.0);
    props->add (1, "found_percentage",  "%.2f", ratio);

    return props;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
