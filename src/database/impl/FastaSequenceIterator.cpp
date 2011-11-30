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

#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/BasicSequenceBuilder.hpp>

#include <string.h>
#include <stdlib.h>

#define DEBUG(a)  //printf a
#define INFO(a)   printf a

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
FastaSequenceIterator::FastaSequenceIterator (
    const char* filename,
    size_t commentMaxSize,
    u_int64_t offset0,
    u_int64_t offset1
)
    : _commentMaxSize(commentMaxSize),  _fileIterator (filename, 1024, offset0, offset1), _isDone(false)
{
    DEBUG (("FastaSequenceIterator::FastaSequenceIterator:  filename='%s'  range=[%ld,%ld] \n",
        filename, offset0, offset1
    ));

    setBuilder (new BasicSequenceBuilder(SUBSEED));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
FastaSequenceIterator::~FastaSequenceIterator ()
{
    setBuilder (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void FastaSequenceIterator::first()
{
    _isDone = false;

    _fileIterator.first ();

    next ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
dp::IteratorStatus FastaSequenceIterator::next()
{
    char* buffer = 0;

    /** We retrieve the builder => shortcut and optimization (avoid method call) */
    ISequenceBuilder* builder = getBuilder();

    /** We set the isDone status. */
    _isDone = _fileIterator.isDone();

    /** We look for a comment. */
    for ( ; !_fileIterator.isDone(); _fileIterator.next())
    {
        /** We retrieve the current file line. */
        buffer = _fileIterator.currentItem();

        if  (buffer && buffer[0]=='>')  { break; }
    }

    /** We may have found a comment. */
    if (buffer != 0)
    {
        if (builder)  {  builder->setComment (buffer + 1, _commentMaxSize);  }

        /** We reset the data size. */
        if (builder)  {  builder->resetData ();  }

        /** We read the next line (the current one still points to the comment line). */
        _fileIterator.next();

        for ( ; !_fileIterator.isDone(); _fileIterator.next())
        {
            /** We retrieve the current file line. */
            buffer = _fileIterator.currentItem();

            if (buffer && buffer[0]=='>')  { break; }

            /** We add the character to the sequence buffer. */
            if (builder)  {  builder->addData ((LETTER*)buffer, strlen(buffer), ASCII);  }
        }
    }

    return dp::ITER_UNKNOWN;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

