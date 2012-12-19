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

#include <designpattern/impl/FileLineIterator.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>
#include <string.h>

using namespace std;
using namespace os;
using namespace os::impl;

#include <iostream>
#define DEBUG(a)  //a

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
FileLineIterator::FileLineIterator (
    const char* filename,
    size_t lineMaxSize,
    u_int64_t offset0,
    u_int64_t offset1
)
    : _filename(filename), _lineMaxSize(lineMaxSize), _line(0),
      _offset0(offset0), _offset1(offset1), _range(~0),
      _readTotalSize(0), _readCurrentSize(0), _cummulatedFilesLength(0),
      _eof(false), _currentFile(0)
{
    DEBUG (cout << "FileLineIterator::FileLineIterator: filename=" << filename << endl);

    /** The provided filename may be a (comma separated) list of uri.
     *  Hence, we split this potential list and create a IFile instance for
     *  each uri.
     */
    TokenizerIterator tokenizer (filename, ",");
    for (tokenizer.first (); !tokenizer.isDone(); tokenizer.next())
    {
        /** IMPORTANT ! We open the file with binary option, otherwise Windows may provide
         * bad physical offsets with the ftell function. */
        _files.push_back (DefaultFactory::file().newFile (tokenizer.currentItem(), "rb"));
    }

    /** We create a buffer for reading lines from the file(s). */
	_line = new char[_lineMaxSize];

	/** We check that we have a non empty range. */
	if (_offset0 < _offset1)  {  _range = _offset1 - _offset0 + 1;  }

    DEBUG (cout << "FileLineIterator::FileLineIterator   _offset0=" << _offset0 << "  _offset1=" << _offset1 << "  _range=" << _range << endl);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
FileLineIterator::~FileLineIterator ()
{
    if (_line)  { delete[] _line;  }

    /** We delete each IFile instance of the list of files. */
    for (list<IFile*>::iterator it= _files.begin(); it != _files.end(); it++)
    {
        delete (*it);
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
void FileLineIterator::first()
{
	if (_files.empty() == false)
	{
		/** We reset some attributes. */
	    _cummulatedFilesLength = 0;
		_readTotalSize         = 0;
		_readCurrentSize       = 0;

	    /** We have to find the file that matches the provided beginning offset. */
        for (_filesIterator = _files.begin(); _filesIterator != _files.end();  _filesIterator++)
        {
            /** We set the current file shortcut. */
            _currentFile = *_filesIterator;

            /** We go to the end of the file to get its size. */
            _currentFile->seeko (0, SEEK_END);
            u_int64_t len  = _currentFile->tell();

            DEBUG (cout << "FileLineIterator::first  filelen=" << len << "  offset=" << _offset0 << "  cummulatedFilesLength=" << _cummulatedFilesLength << endl);

            if (_cummulatedFilesLength + len >= _offset0)
            {
                /** The current iterated files are large enough to contain the beginning offset. */

                /** We can shift to the correct location into the file. */
                _currentFile->seeko (_offset0 - _cummulatedFilesLength, SEEK_SET);

                DEBUG (cout << "FileLineIterator::first  currentSeek=" << (_offset0 - _cummulatedFilesLength) << endl);

                /** We leave the loop over the files. */
                break;
            }

            /** We increase the cumulated sizes of files. */
            _cummulatedFilesLength += len;

        } /* for (_filesIterator = _files.begin()... */

        /** We can set the end of file attribute. */
        _eof = (_filesIterator == _files.end());

        DEBUG (cout << "FileLineIterator::first  eof=" << _eof << "  cummulatedFilesLength=" << _cummulatedFilesLength << endl);

		/** We force retrieval of the first line. */
		next ();
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
bool FileLineIterator::retrieveNextFile ()
{
    /** We go to the next file. */
    _filesIterator++;

    if (_filesIterator != _files.end())
    {
        /** We go to the end of the current file in order to get its size. */
        _currentFile->seeko (0, SEEK_END);

        /** We add this size to the current aggregated files length. */
        _cummulatedFilesLength += _currentFile->tell();

        /** We update the current file. */
        _currentFile = *_filesIterator;

        /** We go to the beginning of the file. */
        _currentFile->seeko (0, SEEK_SET);
    }
    else
    {
        _eof = true;
    }

    DEBUG (cout << "FileLineIterator::retrieveNextFile  eof=" << _eof << "  _readTotalSize=" << _readTotalSize << endl);

    /** we return the current eof status. */
    return !_eof;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
