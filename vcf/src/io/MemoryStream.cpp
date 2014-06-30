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

//MemoryStream.cpp
#include "FoundationKit.h"
#include "MemoryStream.h"

using namespace VCF;



MemoryStream::MemoryStream()
{
	this->init();	
}

MemoryStream::MemoryStream( OutputStream* outStream )
{
	if ( NULL != outStream ){
		this->init();
		this->m_outputStream = outStream;
	}
	//else throw exception ?
}

MemoryStream::MemoryStream( InputStream* inStream )
{
	if ( NULL != inStream ){
		this->init();
		this->m_inputStream = inStream;
	}
}

MemoryStream::~MemoryStream()
{
	
}

void MemoryStream::init()
{
	this->m_inputStream = NULL;
	this->m_outputStream = NULL;
	this->m_size = 0;
}

void MemoryStream::seek(const unsigned long& offset, const SeekType& offsetFrom)
{
	
}

unsigned long MemoryStream::getSize()
{		
	return m_size;
}

void MemoryStream::write( Persistable* persistableObject )
{
	if ( NULL != this->m_outputStream ){
		m_outputStream->write( persistableObject );
	}
	else {
		persistableObject->saveToStream( this );		
	}
}

void MemoryStream::write( char* bytesToWrite, unsigned long sizeOfBytes )
{	
	if ( NULL != this->m_outputStream ){
		m_outputStream->write( bytesToWrite, sizeOfBytes );
	}
	else {
		this->m_stream.write( bytesToWrite, sizeOfBytes );
	}
	m_size += sizeOfBytes;
}

void MemoryStream::read( char* bytesToRead, unsigned long sizeOfBytes )
{
	if ( NULL != this->m_inputStream ){
		m_inputStream->read( bytesToRead, sizeOfBytes );
	}
	else {
		this->m_stream.read( bytesToRead, sizeOfBytes );
	}
}
