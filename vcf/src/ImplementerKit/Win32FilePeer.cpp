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
#include "FoundationKit.h"
#include "Win32Peer.h"

#include "Win32FilePeer.h"
#include "BasicFileError.h"



using namespace VCF;

Win32FilePeer::Win32FilePeer( File* file, const String& fileName )
{
	m_fileHandle = NULL;
	
	m_file = file;

	m_searchStarted = false;

	setName( fileName );
}

Win32FilePeer::~Win32FilePeer()
{
	if ( NULL != m_fileHandle ){
		::CloseHandle( m_fileHandle );
	}

	std::map<Directory::Finder*,FileSearchData*>::iterator it = m_findFileData.begin();
	while ( it != m_findFileData.end() ) {
		FileSearchData* findData = it->second;
		BOOL err = FindClose( findData->m_searchHandle );
		findData->m_searchHandle = NULL;
		it ++;
	}

	m_findFileData.clear();
}

void Win32FilePeer::setFile( File* file )
{
	m_file = file;
}

bool Win32FilePeer::beginFileSearch( Directory::Finder* finder )
{
	bool result = false;
	FileSearchData* findData = new FileSearchData();	
	
	m_searchFilters.clear();
	buildSearchFilters( finder->getSearchFilter() );
	
	m_searchFilterIterator = m_searchFilters.begin();
	String searchFilter = *m_searchFilterIterator;

	String searchDirFilter = m_filename + searchFilter;

	findData->m_searchHandle = ::FindFirstFile( searchDirFilter.c_str(), &findData->m_findData );
	if ( INVALID_HANDLE_VALUE != findData->m_searchHandle ) {
		result = true;
		m_findFileData[finder] = findData;
		m_searchStarted = true;
	}
	else {
		//try other filters		
		if ( m_searchFilterIterator != m_searchFilters.end() ) {
			m_searchFilterIterator++;
			while ( (INVALID_HANDLE_VALUE == findData->m_searchHandle) && (m_searchFilterIterator != m_searchFilters.end()) ) {		
				
				searchFilter = *m_searchFilterIterator;
				searchDirFilter = m_filename + searchFilter;
				findData->m_searchHandle = ::FindFirstFile( searchDirFilter.c_str(), &findData->m_findData );
				result = (INVALID_HANDLE_VALUE != findData->m_searchHandle);
				
				m_searchFilterIterator++;
			}
		}
		if ( true == result ) {
			m_findFileData[finder] = findData;
			m_searchStarted = true;
		}
		else {
			delete findData;
			findData = NULL;
		}
	}
	return result;
}

String Win32FilePeer::findNextFileInSearch( Directory::Finder* finder )
{
	String result;

	std::map<Directory::Finder*,FileSearchData*>::iterator found = m_findFileData.find( finder );
	if ( found != m_findFileData.end() ) {
		FileSearchData* findData = found->second;

		if( true == m_searchStarted ) {
			result = findData->m_findData.cFileName;
		}
		else {
			int err = ::FindNextFile( findData->m_searchHandle, &findData->m_findData );
			if ( TRUE != err ) {
				findData->m_findData.cFileName[0] = '\0';

				err = ::GetLastError();
				if ( ERROR_NO_MORE_FILES == err ) {
					BOOL err = ::FindClose( findData->m_searchHandle );
					findData->m_searchHandle = INVALID_HANDLE_VALUE;
					//try the next search filter
					if ( m_searchFilterIterator != m_searchFilters.end() ) {
						m_searchFilterIterator++;
						
						while ( (INVALID_HANDLE_VALUE == findData->m_searchHandle) && 
							(m_searchFilterIterator != m_searchFilters.end()) ) {							
							
							String searchFilter = *m_searchFilterIterator;
							String searchDirFilter = m_filename + searchFilter;
							//reset back to the start with a new filter
							findData->m_searchHandle = ::FindFirstFile( searchDirFilter.c_str(), &findData->m_findData );					
							
							m_searchFilterIterator++;
						}
						
						if ( INVALID_HANDLE_VALUE == findData->m_searchHandle ) {
							findData->m_findData.cFileName[0] = '\0';
						}
					}
				}
			}

			result = findData->m_findData.cFileName;			
		}		
	}

	m_searchStarted = false;
	return result;
}

void Win32FilePeer::endFileSearch( Directory::Finder* finder )
{
	std::map<Directory::Finder*,FileSearchData*>::iterator found = m_findFileData.find( finder );
	if ( found != m_findFileData.end() ) {
		FileSearchData* findData = found->second;
		BOOL err = ::FindClose( findData->m_searchHandle );
		findData->m_searchHandle = NULL;
		delete findData;
		findData = NULL;

		m_findFileData.erase( found );
	}
}

void Win32FilePeer::buildSearchFilters( const String& searchFilter )
{
	String tmp = searchFilter;
	int pos = tmp.find( ";" );
	while ( String::npos != pos ) {
		m_searchFilters.push_back( tmp.substr( 0, pos ) );
		tmp.erase( 0, pos+1 );
		pos = tmp.find( ";" );
	}

	if ( !tmp.empty() ) {
		m_searchFilters.push_back( tmp );	
	}
}

void Win32FilePeer::remove()
{
	if ( NULL != m_fileHandle ){
		::CloseHandle( m_fileHandle );
		m_fileHandle = NULL;
	}
	if ( m_filename[m_filename.size()-1] == '\\' ) {
		if ( FALSE == RemoveDirectory( m_filename.c_str() ) ) {
			throw BasicFileError( "Unable to remove directory \"" + m_filename + "\",\nprobably because the directory still contains objects." );
		}
	}
	else {
		if ( FALSE == ::DeleteFile( m_filename.c_str() ) ) {
			throw BasicFileError( "Unable to remove file \"" + m_filename + "\"." );
		}
	}
}

void Win32FilePeer::create()
{
	if ( false == m_filename.empty() ){
		if ( NULL != m_fileHandle ){
			::CloseHandle( m_fileHandle );
		}
		if ( m_filename[m_filename.size()-1] == '\\' ) {
			m_fileHandle = NULL;		
			std::vector<String> dirPaths;
			String tmp = m_filename;
			int pos = tmp.find( "\\" );
			while ( pos != String::npos ) {
				dirPaths.push_back( tmp.substr( 0, pos ) );
				tmp.erase( 0, pos+1 );
				pos = tmp.find( "\\" );
			}
			if ( dirPaths.size() == 1 ) {
				String dir = dirPaths[0];
				if ( dir[dir.size()-1] != ':' ) {
					throw BasicFileError( "Unable to create directory \"" + m_filename + "\", path too short or incorrect." );	
				}
			}
			else if ( dirPaths.size() < 2 ) {
				throw BasicFileError( "Unable to create directory \"" + m_filename + "\", path too short or incorrect." );
			}
			std::vector<String>::iterator it = dirPaths.begin();
			String dirPath = *it;
			it++;			
			while ( it != dirPaths.end() ) {
				dirPath += "\\" + *it;
				if ( FALSE == ::CreateDirectory( dirPath.c_str(), NULL ) ) {
					int err = GetLastError();
					if ( ERROR_ALREADY_EXISTS != err ) {
						throw BasicFileError( "Unable to create directory \"" + m_filename + "\"" );
					}
				}
				it++;
			}			
		}
		else {
			//attach to the file
			String fileDir = StringUtils::getFileDir( m_filename );
			if ( true == fileDir.empty() ){
				TCHAR currentDir[MAX_PATH];
				memset( currentDir, 0 , MAX_PATH );
				GetCurrentDirectory( MAX_PATH, currentDir );
				m_filename = "\\" + m_filename;
				m_filename = currentDir +  m_filename;
			}
			
			m_fileHandle = ::CreateFile( m_filename.c_str(), 
											GENERIC_READ | GENERIC_WRITE, 
											FILE_SHARE_READ | FILE_SHARE_WRITE,
											NULL,
											OPEN_EXISTING,
											FILE_ATTRIBUTE_NORMAL,
											NULL );
			
			if ( (NULL == m_fileHandle) || (INVALID_HANDLE_VALUE == m_fileHandle) ){
				m_fileHandle = NULL;
				//throw exception
				throw BasicFileError("Unable to create or attach to the specified file");
			}
		}
	}
}


uint32 Win32FilePeer::getSize()
{
	uint32 result = 0;
	if ( NULL != m_fileHandle ){
		GetFileSize( m_fileHandle, (DWORD*)&result );
	}
	return result;
}

void Win32FilePeer::setName( const String& fileName )
{	
	this->m_filename = fileName;
	
	create();
}

void Win32FilePeer::copyTo( const String& copyFileName )
{
	if ( FALSE == CopyFile( m_filename.c_str(), copyFileName.c_str(), FALSE ) ) {
		throw BasicFileError("File \"" + copyFileName + "\" already exists." );
	}
}