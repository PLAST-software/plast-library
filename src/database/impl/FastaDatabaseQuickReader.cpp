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

#include <database/impl/FastaDatabaseQuickReader.hpp>
#include <misc/api/macros.hpp>

#define DEBUG(a)  //printf a
#define INFO(a)   printf a

/********************************************************************************/
namespace database { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
FastaDatabaseQuickReader::FastaDatabaseQuickReader (const std::string& uri, bool shouldInferType, bool getOnlyType)
    : _iterator (uri.c_str()),
      _uri(uri),
      _totalSize(0), _dataSize(0), _nbSequences(0),
      _readThreshold (shouldInferType ? 100 : 0),
      _nbNucleotids(0), _nbAminoAcids(0),
      _dbKind (ENUM_UNKNOWN),
      _getOnlyType (getOnlyType)
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
void FastaDatabaseQuickReader::read (u_int64_t  maxblocksize)
{
    size_t oldIdx     = ~0;
    size_t currentIdx =  0;

    DEBUG (("FastaDatabaseQuickReader::read  maxblocksize=%ld  _getOnlyType=%d  _readThreshold=%d \n",
        maxblocksize, _getOnlyType, _readThreshold
    ));

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
            if (_readThreshold > 0)
            {
                /** We read at most 20 characters in the current buffer. */
                size_t imax = MIN (_iterator.getLineSize(), 10);
                for (size_t i=0; i<imax; i++)
                {
                    char c = buffer[i];

                    bool isNucleotid =
                        (c=='A') || (c=='C') || (c=='G') || (c=='T') || (c=='N') || (c=='U') ||
                        (c=='a') || (c=='c') || (c=='g') || (c=='t') || (c=='n') || (c=='u');

                    if (isNucleotid)  { _nbNucleotids++;  }
                    else              { _nbAminoAcids++;  }

                }

                /** We are interested only in the type; we can break since we should have got enough residues. */
                if (_getOnlyType)  { break; }

                _readThreshold -= imax;
            }

            /** This is pure data, we increase the total data size with the current read line. */
            _dataSize += _iterator.getLineSize();
        }

        /** We get the position in the stream. */
        _totalSize = _iterator.tell ();
    }

    DEBUG (("FastaDatabaseQuickReader::read  _nbSequences=%d  _nbNucleotids=%d  _nbAminoAcids=%d\n",
        _nbSequences, _nbNucleotids, _nbAminoAcids
    ));

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

        DEBUG (("FastaDatabaseQuickReader::getKind : _nbAminoAcids=%d  _nbNucleotids=%d => ratio=%.1f\n",
            _nbAminoAcids, _nbNucleotids, ratio
        ));

        result = ratio > 0.8 ? ENUM_NUCLOTID : ENUM_AMINO_ACID;
    }

    DEBUG (("FastaDatabaseQuickReader::getKind : _nbAminoAcids=%d  _nbNucleotids=%d => result=%d\n",
        _nbAminoAcids, _nbNucleotids, result
    ));

    return result;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

