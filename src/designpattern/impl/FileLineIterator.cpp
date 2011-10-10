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

#include "FileLineIterator.hpp"
#include <string.h>

using namespace std;

#include <stdio.h>
#define DEBUG(a)  //printf a

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
FileLineIterator::FileLineIterator (
    const char* filename,
    size_t lineMaxSize,
    u_int64_t offset0,
    u_int64_t offset1
)
    : _filename(filename), _lineMaxSize(lineMaxSize), _file(0), _line(0),
      _offset0(offset0), _offset1(offset1), _range(~0),
      _readTotalSize(0), _readCurrentSize(0),
      _eof(false)
{
    DEBUG (("FileLineIterator::FileLineIterator: filename='%s'\n", filename));

    _file = fopen (_filename.c_str(), "r");

	_line = new char[_lineMaxSize];

	_eof = (_file == 0);

	/** We check that we have a non empty range. */
	if (_offset0 < _offset1)  {  _range = _offset1 - _offset0 + 1;  }
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
    if (_file)  { fclose (_file);  }
    if (_line)  { delete[] _line;  }
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
	if (_file)
	{
		/** We reset the end of file attribute. */
		_eof = false;

		/** We go to the beginning of the file. */
        fseeko64 (_file, _offset0, SEEK_SET);

		/** We force retrieval of the first line. */
		next ();
	}
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
