/**
*Copyright (c) 2000-2001, Jim Crafton
*All rights reserved.
*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions
*are met:
*	Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*
*	Redistributions in binary form must reproduce the above copyright
*	notice, this list of conditions and the following disclaimer in 
*	the documentation and/or other materials provided with the distribution.
*
*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
*AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
*OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*NB: This software will not save the world.
*/

#ifndef _WIN32FILEPEER_H__
#define _WIN32FILEPEER_H__



#include "FilePeer.h"


namespace VCF {

class FRAMEWORK_API Win32FilePeer : public Object, public FilePeer{

public:

	class FileSearchData {
	public:
		FileSearchData() {
			m_searchHandle = NULL;
			memset( &m_findData, 0, sizeof( WIN32_FIND_DATA ) );
		}

		WIN32_FIND_DATA m_findData;
		HANDLE m_searchHandle;
	};

	Win32FilePeer( File* file, const String& fileName="" );	
	
	virtual ~Win32FilePeer();

	virtual void setFile( File* file );

	virtual void remove();

	/**
	*creates a new file
	*if the last character in the m_filename is a 
	*directory character, then a directory is created
	*instead of a file.
	*/
	virtual void create();


	/**
	*returns the size of the file in bytes
	*/
	virtual uint32 getSize();

	virtual String getName(){
		return m_filename;
	};

	virtual void setName( const String& fileName );

	virtual void copyTo( const String& copyFileName );

	virtual bool beginFileSearch( Directory::Finder* finder );

	virtual String findNextFileInSearch( Directory::Finder* finder );

	virtual void endFileSearch( Directory::Finder* finder );
private:
	String m_filename;
	HANDLE m_fileHandle;
	File* m_file;
	bool m_searchStarted;
	std::vector<String> m_searchFilters;
	std::vector<String>::iterator m_searchFilterIterator;
	std::map<Directory::Finder*,FileSearchData*> m_findFileData;

	void buildSearchFilters( const String& searchFilter );
};

}; //end of namespace VCF

#endif //_WIN32FILEPEER_H__