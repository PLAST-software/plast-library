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

#ifdef __WINDOWS__

#include <os/impl/WindowsMemoryFile.hpp>
#include <os/impl/CommonOsImpl.hpp>

#include <windows.h>
/********************************************************************************/
namespace os { namespace impl {
/********************************************************************************/

/** */
class WindowsMemoryFile : public IMemoryFile
{
public:

	WindowsMemoryFile (const char* path, bool map)
		:_data(0),
		 _size(0),
		 _file_handle(INVALID_HANDLE_VALUE),
		 _file_mapping_handle(INVALID_HANDLE_VALUE),
	     _mapOffset(0),
		 _map(map)

	{
		SYSTEM_INFO sysinfo = {0};
		::GetSystemInfo(&sysinfo);
		_pageSize = sysinfo.dwAllocationGranularity;
		_file_handle = ::CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	    if (_file_handle == INVALID_HANDLE_VALUE) return;
	    _size = ::GetFileSize(_file_handle, 0);
	    _file_mapping_handle = ::CreateFileMapping(_file_handle, 0, PAGE_READONLY, 0, 0, 0);
	    if (_file_mapping_handle == INVALID_HANDLE_VALUE) return;
	    if (_map)
	    	_data = static_cast<const char*>(::MapViewOfFile(_file_mapping_handle, FILE_MAP_READ, 0, 0, _size));
	}

	void mapFile(off_t start, size_t length)
    {
    	if (!_map)
    	{
    		DWORD dwFileMapStart = (start/_pageSize)*_pageSize;
			_mapOffset= start-dwFileMapStart;
			DWORD dwMapViewSize = _mapOffset + length; 
			if ((length+start) > _size)
				dwMapViewSize = 0;
			_data = static_cast<const char*>(::MapViewOfFile(_file_mapping_handle, FILE_MAP_READ, 0, dwFileMapStart, dwMapViewSize));
    	}
    }

    void unmapFile()
    {
    	if ((!_map)&&(_data))
    	{
    		UnmapViewOfFile(_data);
    		_data = 0;
    	}
    }

    ~WindowsMemoryFile ()
    {
       	if (_map && _data)
       	{
    		 UnmapViewOfFile(_data);
    		 _data=0;
       	}
    	CloseHandle(_file_mapping_handle);
    	_file_mapping_handle = INVALID_HANDLE_VALUE;
    	CloseHandle(_file_handle);
    	_file_handle = INVALID_HANDLE_VALUE;
    }

    const char* getData() { return _data +_mapOffset; }

    u_int64_t getSize() { return _size; }

    std::string getPath () { return _filePath; };

private :
    typedef void * HANDLE;
    HANDLE _file_handle;
    HANDLE _file_mapping_handle;
    DWORD _pageSize;

    const char* _data;
    size_t _size;
    off_t _mapOffset;
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
IMemoryFile* WindowsMemoryFileFactory::newFile (const char *path, bool map)
{
    return new WindowsMemoryFile (path, map);
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /*  __WINDOWS__  */
