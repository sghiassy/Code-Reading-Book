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

// Win32FileStream.cpp

#include "FoundationKit.h"
#include "Win32FileStream.h"

using namespace VCF;


Win32FileStream::Win32FileStream( const String& filename, const FileStreamAccessType& accessType )
{
	m_filename = filename;
	m_fileHandle = CreateFile( filename.c_str(), 
		                       this->translateAccessType( accessType ),
                               0, //for now
							   NULL, //for now
							   translateAccessTypeToCreationType( accessType ),
							   FILE_ATTRIBUTE_NORMAL,
							   NULL );
	
	if ( INVALID_HANDLE_VALUE == m_fileHandle ){

		throw FileIOError( CANT_ACCESS_FILE + filename );
	}
}

Win32FileStream::~Win32FileStream()
{
	if ( INVALID_HANDLE_VALUE != m_fileHandle ){
		CloseHandle( m_fileHandle );
	}
}

void Win32FileStream::seek(const unsigned long& offset, const SeekType& offsetFrom)
{
	SetFilePointer( m_fileHandle, offset, NULL, translateSeekTypeToMoveType( offsetFrom ) );
}

unsigned long Win32FileStream::getSize()
{
	return GetFileSize( m_fileHandle, NULL );
}

void Win32FileStream::read( char* bytesToRead, unsigned long sizeOfBytes )
{
	DWORD bytesRead = 0;
	ReadFile( m_fileHandle, bytesToRead, sizeOfBytes, &bytesRead, NULL );
	if ( bytesRead != sizeOfBytes ){ //error if we are not read /writing asynchronously !
		//throw exception ?
	}
}
 
void Win32FileStream::write( char* bytesToWrite, unsigned long sizeOfBytes )
{
	DWORD bytesWritten = 0;
	WriteFile( m_fileHandle, bytesToWrite, sizeOfBytes, &bytesWritten, NULL );
	if ( bytesWritten != sizeOfBytes ){//error if we are not read /writing asynchronously !
		//throw exception ?
		throw new FileIOError( CANT_WRITE_TO_FILE + m_filename );
	}
}

char* Win32FileStream::getBuffer()
{
	return NULL;
}

DWORD Win32FileStream::translateAccessType( const FileStreamAccessType& accessType )
{
	DWORD result = 0;
	switch ( accessType ){
		case FS_READ : {
			result = GENERIC_READ;
		}
		break;

		case FS_WRITE : {
			result = GENERIC_WRITE;
		}
		break;

		case FS_READ_WRITE : case FS_DONT_CARE : {
			result = GENERIC_WRITE | GENERIC_READ;
		}
		break;
	}
	return result;
}

DWORD Win32FileStream::translateAccessTypeToCreationType( const FileStreamAccessType& accessType )
{
	DWORD result = 0;
	switch ( accessType ){
		case FS_READ : {
			result = OPEN_EXISTING;
		}
		break;

		case FS_WRITE : {
			result = CREATE_ALWAYS;//TRUNCATE_EXISTING | OPEN_ALWAYS;
		}
		break;

		case FS_READ_WRITE : case FS_DONT_CARE : {
			result = OPEN_ALWAYS;
		}
		break;
	}
	return result;
}

DWORD Win32FileStream::translateSeekTypeToMoveType( const SeekType& offsetFrom )
{
	DWORD result = 0;
	switch ( offsetFrom ){
		case SEEK_FROM_START : {
			result = FILE_BEGIN;
		}
		break;

		case SEEK_FROM_RELATIVE : {
			result = FILE_CURRENT;
		}
		break;

		case SEEK_FROM_END : {
			result = FILE_END;
		}
		break;
	}
	return result;
}