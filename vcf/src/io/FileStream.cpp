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

//FileStream.cpp
#include "FoundationKit.h"


#ifdef WIN32
#include "Win32FileStream.h"
#endif

using namespace VCF;

FileStream::FileStream( const String& filename, const FileStreamAccessType& accessType )
{
	m_filename = filename;
	m_access = accessType;
	try {
		this->init();
	}
	catch ( FileIOError& e ) {
		throw e;
	}
	catch (...){

	}
}

FileStream::FileStream( const String& filename )
{
	m_filename = filename;
	m_access = FS_READ_WRITE;
	try {
		this->init();
	}
	catch (...){

	}
}

FileStream::FileStream( const String& filename, OutputStream* outStream )
{
	if ( NULL != outStream ){
		m_filename = filename;
		m_access = FS_WRITE;
		try {
			this->init();
			this->m_outputStream = outStream;
		}
		catch (...){
			
		}
	}
	//else throw exception ?
}

FileStream::FileStream( const String& filename, InputStream* inStream )
{
	if ( NULL != inStream ){
		m_filename = filename;
		m_access = FS_READ;
		try {
			this->init();
			this->m_inputStream = inStream;
		}
		catch (...){
			
		}
	}
}

FileStream::~FileStream()
{
	close();
}

void FileStream::init()
{
	m_fsPeer = NULL;
	this->m_inputStream = NULL;
	this->m_outputStream = NULL;

#ifdef WIN32
	m_fsPeer = new Win32FileStream( m_filename, m_access ); 
#endif	
}

void FileStream::close()
{
	delete m_fsPeer;
	m_fsPeer = NULL;
}

void FileStream::open( const String& filename, const FileStreamAccessType& accessType )
{
	m_filename = filename;
	m_access = accessType;
	if ( NULL != m_fsPeer ){
		delete m_fsPeer;
	}
	m_fsPeer = NULL;

	this->init();
}

void FileStream::open( const String& filename )
{
	this->open( filename, FS_READ_WRITE );
}

void FileStream::seek(const unsigned long& offset, const SeekType& offsetFrom )
{
	if ( NULL == this->m_fsPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	

	m_fsPeer->seek( offset, offsetFrom );
}

unsigned long FileStream::getSize()
{
	if ( NULL == this->m_fsPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	

	return m_fsPeer->getSize();
}

char* FileStream::getBuffer()
{
	char* result = NULL;

	return result;
}

void FileStream::read( char* bytesToRead, unsigned long sizeOfBytes )
{
	if ( NULL != this->m_inputStream ){
		m_inputStream->read( bytesToRead, sizeOfBytes );
	}
	else {
		if ( NULL == this->m_fsPeer ){
			throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
		};	
		
		m_fsPeer->read( bytesToRead, sizeOfBytes );
	}
}

void FileStream::write( char* bytesToWrite, unsigned long sizeOfBytes )
{
	if ( NULL != this->m_outputStream ){
		m_outputStream->write( bytesToWrite, sizeOfBytes );
	}
	else {
		if ( NULL == this->m_fsPeer ){
			throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
		};	
		
		m_fsPeer->write( bytesToWrite, sizeOfBytes );
	}
}
