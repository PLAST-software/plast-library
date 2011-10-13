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

#include "FastaDatabaseQuickReader.hpp"
#include "macros.hpp"

#define DEBUG(a)  //printf a
#define INFO(a)   printf a

/********************************************************************************/
namespace database {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
FastaDatabaseQuickReader::FastaDatabaseQuickReader (const std::string& uri, bool shouldInferType)
    : _iterator (uri.c_str(), 1024),
      _uri(uri),
      _totalSize(0), _dataSize(0), _nbSequences(0),
      _readThreshold (shouldInferType ? 100 : 0),
      _nbNucleotids(0), _nbAminoAcids(0),
      _dbKind (ENUM_UNKNOWN)
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
FastaDatabaseQuickReader::~FastaDatabaseQuickReader  ()
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
void FastaDatabaseQuickReader::read (u_int64_t  maxblocksize = 0)
{
    size_t oldIdx     = ~0;
    size_t currentIdx =  0;

    /** We clear the provided arguments. */
    _totalSize   = 0;
    _dataSize    = 0;
    _nbSequences = 0;
    _offsets.clear ();

    for (_iterator.first(); !_iterator.isDone(); _iterator.next())
    {
        char* buffer = _iterator.currentItem();

        if (*buffer == '>')
        {
            /** This is a new sequence, so we increase the number of found sequences. */
            _nbSequences ++;

            if (maxblocksize > 0)
            {
                currentIdx = _totalSize / maxblocksize;

                if (currentIdx != oldIdx)
                {
                    _offsets.push_back (_totalSize);
                    oldIdx = currentIdx;
                }
            }
        }

        else
        {
            u_int64_t currentSize = _iterator.getCurrentReadSize();

            if (_readThreshold > 0)
            {
                /** We read at most 20 characters in the current buffer. */
                size_t imax = MIN (currentSize, 10);
                for (size_t i=0; i<imax; i++)
                {
                    char c = buffer[i];

                    bool isNucleotid =
                        (c=='A') || (c=='C') || (c=='G') || (c=='T') ||
                        (c=='a') || (c=='c') || (c=='g') || (c=='t');

                    if (isNucleotid)  { _nbNucleotids++;  }
                    else              { _nbAminoAcids++;  }
                }

                _readThreshold -= imax;
            }

            /** This is pure data, we increase the total data size with the current read line. */
            _dataSize += currentSize;
        }

        /** We need +1 for counting the '\n' that has been removed by the file line iterator. */
        _totalSize += _iterator.getCurrentReadSize() + 1;
    }

    _offsets.push_back (_totalSize);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IDatabaseQuickReader::DatabaseKind_e FastaDatabaseQuickReader::getKind ()
{
    IDatabaseQuickReader::DatabaseKind_e result = ENUM_UNKNOWN;

    if (_nbAminoAcids>0 || _nbNucleotids>0)
    {
        float ratio = (float) _nbNucleotids / (float) (_nbAminoAcids + _nbNucleotids);

        result = ratio > 0.8 ? ENUM_NUCLOTID : ENUM_AMINO_ACID;
    }

    return result;
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

