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

#ifdef __DARWIN__

#include <os/impl/MacOsMemoryFile.hpp>
#include <os/impl/CommonOsImpl.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

/********************************************************************************/
namespace os { namespace impl {
/********************************************************************************/

/** */
class MacOsMemoryFile : public IMemoryFile
{
public:

	MacOsMemoryFile (const char* path, bool map)
		:_data(0),
		 _size(0),
		 _mapLength(0),
		 _mapOffset(0),
		 _file_handle(-1),
		 _filePath(path),
		 _map(map)
	{
    	_file_handle = ::open(path, O_RDONLY);
        if (_file_handle == -1) return;
        struct stat sbuf;
        if (::fstat(_file_handle, &sbuf) == -1) return;
        if (_map)
        {
			_data = static_cast<const char*>(::mmap(0, sbuf.st_size, PROT_READ, MAP_SHARED, _file_handle, 0));
			if (_data == MAP_FAILED) _data = 0;
			else _size = sbuf.st_size;
        }
        else
        	 _size = sbuf.st_size;

	}

    void mapFile(off_t start, size_t length)
    {
    	if (!_map)
    	{
			off_t pa_offset = start & ~(sysconf(_SC_PAGE_SIZE) - 1);
			_mapOffset= start-pa_offset;
			_mapLength = length+_mapOffset;
			_data = static_cast<const char*>(::mmap(0, _mapLength, PROT_READ, MAP_SHARED, _file_handle, pa_offset));
			if (_data == MAP_FAILED) _data = 0;
    	}
    }

    void unmapFile()
    {
    	if ((!_map)&&(_data))
    	{
    		::munmap(const_cast<char*>(_data), _mapLength);
    		_data = 0;
    	}
    }

    ~MacOsMemoryFile ()
    {
    	if (_map && _data)
    	{
    		::munmap(const_cast<char*>(_data), _size);
    		_data = 0;
    	}
        ::close(_file_handle);
        _file_handle = -1;
   }

    /** As the map file need to have a start offset which is a "modulo" of the
     * virtual memory page size, we need to add the offset between the real start offset and
     *  the offset of the file memory map
     */

   const char* getData() { return _data +_mapOffset;  }

   u_int64_t getSize() { return _size; }

   std::string getPath () { return _filePath; };

private :
    const char* _data;
    size_t _size;
    size_t _mapLength;
    off_t _mapOffset;
    int _file_handle;
    std::string _filePath;
    bool _map;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IMemoryFile* MacOsMemoryFileFactory::newFile (const char *path, bool map)
{
    return new MacOsMemoryFile (path, map);
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /*  __LINUX__  */
