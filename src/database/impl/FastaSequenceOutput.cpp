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

#include <os/impl/DefaultOsFactory.hpp>

#include <designpattern/impl/WrapperIterator.hpp>

#include <database/impl/FastaSequenceOutput.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a
#define INFO(a)   printf a

using namespace os;
using namespace os::impl;

using namespace dp;
using namespace dp::impl;

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
FastaSequenceOutput::FastaSequenceOutput (ISequenceIterator* iterator, const char* filename, const std::list<std::string>& comments)
    :  _iterator(0), _filename (filename), _comments (comments)
{
    setIterator (iterator);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
FastaSequenceOutput::FastaSequenceOutput (ISequenceIterator* iterator, const char* filename)
    :  _iterator(0), _filename (filename)
{
    setIterator (iterator);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
FastaSequenceOutput::~FastaSequenceOutput ()
{
    setIterator (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void FastaSequenceOutput::dump ()
{
    /** We iterate the sequences. */
    if (_iterator != 0)
    {
        /** We create the output file. */
        _file = DefaultFactory::file().newFile (_filename.c_str(), "w");

        if (_file != 0)
        {
            /** We initialize the iteration on the provided comments. */
            _itComments = _comments.begin ();

            /** We launch the iteration. */
            _iterator->iterate (this, (Iterator<const ISequence*>::Method) &FastaSequenceOutput::dumpSequence);

            /** We get rid of the IFile instance. */
            delete _file;
            _file = 0;
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
void FastaSequenceOutput::dumpSequence (const ISequence* seq)
{
    if (_file != 0  &&  seq != 0)
    {
        char buffer[256];

        if (_itComments != _comments.end())
        {
            sprintf (buffer, ">%s", (*_itComments).c_str());
            _itComments ++;
        }
        else
        {
            sprintf (buffer, ">%s",seq->comment);
        }

        /** We write the comment. */
        _file->println (buffer);

        /** We split the data in lines of N characters max. */
        WrapperIterator it (seq->data.toString(), 60);

        for (it.first(); ! it.isDone(); it.next())
        {
            _file->println (it.currentItem());
        }
    }
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

