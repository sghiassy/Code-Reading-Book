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

// TextOutputStream.cpp

#include "FoundationKit.h"


using namespace VCF;

#define NUMBER_TXT_SIZE		25


TextOutputStream::TextOutputStream( OutputStream* outStream )
{
	this->init();
	m_outStream = outStream;
}

TextOutputStream::TextOutputStream()
{
	this->init();
}

TextOutputStream::~TextOutputStream()
{

}

void TextOutputStream::init()
{
	m_size = 0;
	m_outStream = NULL;	
}

void TextOutputStream::seek(const unsigned long& offset, const SeekType& offsetFrom )
{
	if ( NULL != m_outStream ){
		m_outStream->seek( offset, offsetFrom );
	}
}

unsigned long TextOutputStream::getSize()
{
	unsigned long result = 0;
	if ( NULL != m_outStream ){
		result = m_outStream->getSize();
	}
	else {
		result = this->m_textBuffer.size();
	}
	return result;
}

char* TextOutputStream::getBuffer()
{
	char* buffer = NULL;
	if ( NULL != m_outStream ){
		buffer = m_outStream->getBuffer();
	}
	return buffer;
}

void TextOutputStream::write( char* bytesToRead, unsigned long sizeOfBytes )
{
	if ( NULL != this->m_outStream ){
		m_outStream->write( bytesToRead, sizeOfBytes );
	}
	m_textBuffer += bytesToRead;
}

void TextOutputStream::write( const short& val )
{
	char tmpText[NUMBER_TXT_SIZE];
	memset( tmpText, 0, NUMBER_TXT_SIZE );
	sprintf( tmpText, SHORT_STR_CONVERSION, val );
	this->write( tmpText, strlen(tmpText) );
}
	
void TextOutputStream::write( const long& val )
{
	char tmpText[NUMBER_TXT_SIZE];
	memset( tmpText, 0, NUMBER_TXT_SIZE );
	sprintf( tmpText, LONG_STR_CONVERSION, val );
	this->write( tmpText, strlen(tmpText) );
}

void TextOutputStream::write( const int& val )
{
	char tmpText[NUMBER_TXT_SIZE];
	memset( tmpText, 0, NUMBER_TXT_SIZE );
	sprintf( tmpText, INT_STR_CONVERSION, val );
	this->write( tmpText, strlen(tmpText) );
}

void TextOutputStream::write( const bool& val )
{
	char* tmpText = (char*)(val ? BOOL_STR_CONVERSION_TRUE : BOOL_STR_CONVERSION_FALSE);
	this->write( tmpText, strlen(tmpText) );
}

void TextOutputStream::write( const float& val )
{
	char tmpText[NUMBER_TXT_SIZE];
	memset( tmpText, 0, NUMBER_TXT_SIZE );
	sprintf( tmpText, FLOAT_STR_CONVERSION, val );
	this->write( tmpText, strlen(tmpText) );
}

void TextOutputStream::write( const double& val )
{
	char tmpText[NUMBER_TXT_SIZE];
	memset( tmpText, 0, NUMBER_TXT_SIZE );
	sprintf( tmpText, DOUBLE_STR_CONVERSION, val );
	this->write( tmpText, strlen(tmpText) );
}

void TextOutputStream::write( const String& val )
{
	String tmp = val + "\n";
	int size = tmp.size();
	char* buf = new char[size];
	memset(buf, 0, size );
	val.copy( buf, size-1 );	
	this->write( buf, size-1 );
	delete [] buf;
}

TextOutputStream& TextOutputStream::operator<<( const short& val )
{
	this->write( val );	
	return *(this);
}

TextOutputStream& TextOutputStream::operator<<( const long& val )
{
	this->write( val );	
	return *(this);
}

TextOutputStream& TextOutputStream::operator<<( const int& val )
{
	this->write( val );	
	return *(this);
}

TextOutputStream& TextOutputStream::operator<<( const bool& val ){	
	this->write( val );
	return *(this);
}

TextOutputStream& TextOutputStream::operator<<( const char& val )
{
	this->write( (char*)&val, sizeof(val) );
	return *(this);
}

TextOutputStream& TextOutputStream::operator<<( const float& val )
{
	this->write( val );	
	return *(this);
}

TextOutputStream& TextOutputStream::operator<<( const double& val )
{
	this->write( val );	
	return *(this);
}

void TextOutputStream::writeLine( const String& line )
{
	*((OutputStream*)this) << line;
	*this << '\n';
}

String TextOutputStream::getTextBuffer()
{
	return this->m_textBuffer;
}

String TextOutputStream::toString()
{
	return m_textBuffer;	
}