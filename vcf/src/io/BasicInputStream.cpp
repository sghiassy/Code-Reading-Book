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

// BasicInputStream.cpp

#include "FoundationKit.h"



using namespace VCF;

BasicInputStream::BasicInputStream()
{
	this->init();
}

BasicInputStream::BasicInputStream( const String& textBuffer )
{
	this->init();
	this->m_inStream.write( textBuffer.c_str(), textBuffer.size() );
	m_size = textBuffer.size();
}

BasicInputStream::BasicInputStream( char* dataBuffer, const unsigned long& dataBufferSize )
{	
	this->init();
	this->m_inStream.write( dataBuffer, dataBufferSize );
	m_size = dataBufferSize;
}

BasicInputStream::BasicInputStream( InputStream* inStream )
{
	this->init();
	m_inputStream = inStream;	
}


void BasicInputStream::init()
{
	m_size = 0;
	this->m_inputStream = NULL;
}

BasicInputStream::~BasicInputStream()
{
	
}

void BasicInputStream::seek(const unsigned long& offset, const SeekType& offsetFrom)
{
	
}

unsigned long BasicInputStream::getSize()
{
	return m_size;
}

char* BasicInputStream::getBuffer()
{
	return m_inStream.getBuffer();
}

void BasicInputStream::read( char* bytesToRead, unsigned long sizeOfBytes )
{
	if ( NULL != m_inputStream ){
		m_inputStream->read( bytesToRead, sizeOfBytes );
	}
	else {
		this->m_inStream.read( bytesToRead, sizeOfBytes );
	}
	m_size += sizeOfBytes;
}


