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

#include <database/impl/FastaDatabaseQuickReader.hpp>
#include <misc/api/macros.hpp>
#include <designpattern/impl/Property.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>
#include <libgen.h>

#define DEBUG(a)  //printf a

using namespace dp;
using namespace dp::impl;

/********************************************************************************/
namespace database { namespace impl {
/********************************************************************************/

static const u_int64_t MAGIC_NUMBER = 0x12345678;

/********************************************************************************/

static int getDirBaseNames (const std::string& uri, std::string& dname, std::string& bname)
{
    char* dirc  = strdup (uri.c_str());      if (!dirc)   { return -1; }
    char* basec = strdup (uri.c_str());      if (!basec)  { return -1; }

    dname = std::string (dirname(dirc));
    bname = std::string (basename(basec));

    /** Some cleanup. */
    free (dirc);  free (basec);

    return 0;
}

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
      _maxblocksize(0),
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
    _totalSize    = 0;
    _dataSize     = 0;
    _nbSequences  = 0;
    _maxblocksize = maxblocksize;
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

    /** We set the kind of bank. */
    if (_nbAminoAcids>0 || _nbNucleotids>0)
    {
        float ratio = (float) _nbNucleotids / (float) (_nbAminoAcids + _nbNucleotids);

        DEBUG (("FastaDatabaseQuickReader::getKind : _nbAminoAcids=%d  _nbNucleotids=%d => ratio=%.1f\n",
            _nbAminoAcids, _nbNucleotids, ratio
        ));

        _dbKind = ratio > 0.8 ? ENUM_NUCLOTID : ENUM_AMINO_ACID;
    }

    DEBUG (("FastaDatabaseQuickReader::read : _nbAminoAcids=%d  _nbNucleotids=%d => result=%d\n",
        _nbAminoAcids, _nbNucleotids, _dbKind
    ));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int FastaDatabaseQuickReader::load (const std::string& uri)
{
    int res = -1;

    FILE* file = fopen (uri.c_str(), "rb");
    if (file != 0)
    {
        /** Magic number. */
        u_int64_t magic = 0;
        fread (&magic, sizeof(u_int64_t), 1, file);
        if (magic != MAGIC_NUMBER)  { return -1; }

        /** WARNING !!! See 'save' method comments. */

        /** We need the directory name of the 'info' file. */
        std::string dname, bname;
        if (getDirBaseNames (uri, dname, bname) != 0)  { return -1; }

        /** length of uri name. */
        size_t len = 0;
        fread (&len, sizeof(size_t), 1, file);

        /** Uri of the bank(s) */
        char* buffer = new char [len];
        fread (buffer, sizeof(char), len, file);
        std::string readBuffer (buffer, len);

        /** We reset the bank(s) uri. */
        _uri.clear();

        const char* sep = ",";
        bool first = true;
        TokenizerIterator it (readBuffer.c_str(), sep);
        for (it.first(); !it.isDone(); it.next(), first=false)
        {
            if (!first)  { _uri += sep; }
            _uri +=  dname + std::string("/") + it.currentItem();
        }

        /** u_int64_t   _totalSize;  */
        fread (&_totalSize, sizeof(u_int64_t), 1, file);

        /** u_int64_t   _dataSize; */
        fread (&_dataSize, sizeof(u_int64_t), 1, file);

        /** u_int32_t   _nbSequences; */
        fread (&_nbSequences, sizeof(u_int32_t), 1, file);

        /** DatabaseKind_e _dbKind; */
        fread (&_dbKind, sizeof(DatabaseKind_e), 1, file);

        /** u_int64_t  _maxblocksize; */
        fread (&_maxblocksize, sizeof(u_int64_t), 1, file);

        /** std::vector<u_int64_t> _offsets */
        size_t nbOffsets = 0;
        fread (&nbOffsets, sizeof(size_t), 1, file);

        _offsets.clear();
        for (size_t i=0; i<nbOffsets; i++)
        {
            u_int64_t offset = 0;
            fread (&offset, sizeof(u_int64_t), 1, file);
            _offsets.push_back (offset);
        }

        /** We close the file. */
        fclose (file);

        /** We set the result status. */
        res = 0;
    }

    /** We return the status. */
    return res;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int FastaDatabaseQuickReader::save (const std::string& uri)
{
    int res = -1;

    FILE* file = fopen (uri.c_str(), "wb");
    if (file != 0)
    {
        /** First, we set which uri we save into the file => only the file name, not the full path.
         *  During the load operation, this filename will be prefixed by the basename of the 'info' file.
         *  (which implies that the 'info' file should be located at the same place than the referred bank).
         *
         *  For instance:
         *      - the info file (ie 'uri' provided argument) is "/somedir/bank.info'
         *      - the bank file (ie '_uri' attribute) is "/somedir/bank.fa"
         *
         *  So, the saved uri bank will be 'bank.fa'
         *
         *  Now, say that we rename "/somedir" into "/mydir". During the 'load' operation, we will have as argument
         *  the 'info' uri as "/mydir/bank.info". So the load bank uri will be "/mydir" + "bank.fa" => "/mydir/bank.fa"
         *
         *  Moreover, we have to check that the bank is not a compound bank, ie list of fasta banks (comma separated).
         */
        _actualUri.clear();
        const char* sep = ",";
        bool first = true;
        TokenizerIterator it (_uri.c_str(), sep);
        for (it.first(); !it.isDone(); it.next(), first=false)
        {
            std::string dname, bname;
            if (getDirBaseNames (it.currentItem(), dname, bname) != 0)  { return -1; }

            if (!first)  { _actualUri += sep; }
            _actualUri += bname;
        }

        /** Magic number. */
        fwrite (&MAGIC_NUMBER, sizeof(u_int64_t), 1, file);

        /** length of uri name. */
        size_t len = _actualUri.size();
        fwrite (&len, sizeof(size_t), 1, file);

        /** Uri of the bank */
        fwrite (_actualUri.c_str(), sizeof(char), len, file);

        /** u_int64_t   _totalSize;  */
        fwrite (&_totalSize, sizeof(u_int64_t), 1, file);

        /** u_int64_t   _dataSize; */
        fwrite (&_dataSize, sizeof(u_int64_t), 1, file);

        /** u_int32_t   _nbSequences; */
        fwrite (&_nbSequences, sizeof(u_int32_t), 1, file);

        /** DatabaseKind_e _dbKind; */
        fwrite (&_dbKind, sizeof(DatabaseKind_e), 1, file);

        /** u_int64_t  _maxblocksize; */
        fwrite (&_maxblocksize, sizeof(u_int64_t), 1, file);

        /** std::vector<u_int64_t> _offsets */
        size_t nbOffsets = _offsets.size();
        fwrite (&nbOffsets, sizeof(size_t), 1, file);
        for (size_t i=0; i<nbOffsets; i++)
        {
            u_int64_t offset = _offsets[i];
            fwrite (&offset, sizeof(u_int64_t), 1, file);
        }

        /** We close the file. */
        fclose (file);

        /** We set the result status. */
        res = 0;
    }

    DEBUG (("FastaDatabaseQuickReader::save: uri='%s'  res=%d\n", uri.c_str(), res));

    /** We return the status. */
    return res;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool FastaDatabaseQuickReader::isQuickReaderFile (const std::string& filename)
{
    bool result = false;

    FILE* file = fopen (filename.c_str(), "rb");
    if (file != 0)
    {
        u_int64_t magic = 0;  fread (&magic, sizeof(u_int64_t), 1, file);

        if (magic == MAGIC_NUMBER)  { result = true; }

        fclose (file);
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
dp::IProperties* FastaDatabaseQuickReader::getProperties ()
{
    IProperties* result = new Properties ();

    result->add (0, "reader");

    result->add (1, "uri",          "%s",       _actualUri.c_str());
    result->add (1, "totalSize",    "%lld",     _totalSize);
    result->add (1, "dataSize",     "%lld",     _dataSize);
    result->add (1, "nbSequences",  "%ld",      _nbSequences);
    result->add (1, "dbKind",       "%ld",      _dbKind);
    result->add (1, "maxblocksize", "%ld",      _maxblocksize);
    result->add (1, "offsets",      "%ld",      _offsets.size());
    for (size_t i=0; i<_offsets.size(); i++)
    {
        result->add (2, "offset",  "%ld", _offsets[i]);
    }

    return result;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

