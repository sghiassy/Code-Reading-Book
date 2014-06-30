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

// TextInputStream.cpp

#include "FoundationKit.h"


using namespace VCF;

TextInputStream::TextInputStream( InputStream* inStream )
{
	this->init();
	this->m_inStream = inStream;
	if ( NULL != m_inStream ){
		this->m_size = this->m_inStream->getSize();
	}	
}

TextInputStream::~TextInputStream()
{

}

void TextInputStream::seek(const unsigned long& offset, const SeekType& offsetFrom )
{
	if ( NULL != m_inStream ){
		m_inStream->seek( offset, offsetFrom );
	}
}

unsigned long TextInputStream::getSize()
{
	return m_size;
}

char* TextInputStream::getBuffer()
{
	return NULL;
}

void TextInputStream::read( char* bytesToRead, unsigned long sizeOfBytes )
{
	if ( NULL != m_inStream ){
		m_inStream->read( bytesToRead, sizeOfBytes );
	}
}

void TextInputStream::read( short& val )
{
	if ( NULL != this->m_inStream ){
		String tmp = readTillWhiteSpace();		
		int i = val = 0;
		sscanf( tmp.c_str(), STR_INT_CONVERSION, &i );
		val = (short)i;		
	}
}
	
void TextInputStream::read( long& val )
{
	if ( NULL != this->m_inStream ){
		String tmp = readTillWhiteSpace();		
		int i = val = 0;
		sscanf( tmp.c_str(), STR_INT_CONVERSION, &i );
		val = i;		
	}
}

void TextInputStream::read( int& val )
{
	if ( NULL != this->m_inStream ){
		String tmp = readTillWhiteSpace();		
		int i = val = 0;
		sscanf( tmp.c_str(), STR_INT_CONVERSION, &i );
		val = i;
	}
}

void TextInputStream::read( bool& val )
{
	if ( NULL != this->m_inStream ){
		String tmp = readTillWhiteSpace();
		if ( STR_BOOL_CONVERSION_TRUE == tmp ){
			val = true;
		}
		else if ( STR_BOOL_CONVERSION_FALSE == tmp ){
			val = false;
		}
	}
}

void TextInputStream::read( float& val )
{	
	if ( NULL != this->m_inStream ){
		String tmp = readTillWhiteSpace();
		val = 0.0;
		float f = 0.0;
		sscanf( tmp.c_str(), STR_DOUBLE_CONVERSION, &f );
		val = f;
	}
}

void TextInputStream::read( double& val )
{
	if ( NULL != this->m_inStream ){
		String tmp = readTillWhiteSpace();
		val = 0.0;
		float f = 0.0;
		sscanf( tmp.c_str(), STR_DOUBLE_CONVERSION, &f );
		val = f;
	}
}

void TextInputStream::read( String& val )
{
	val = readTillTokenPair( '\n' );
}

TextInputStream& TextInputStream::operator>>( short& val )
{
	this->read( val );
	return *this;
}

TextInputStream& TextInputStream::operator>>( long& val )
{
	this->read( val );
	return *this;
}

TextInputStream& TextInputStream::operator>>( int& val )
{
	this->read( val );
	return *this;
}

TextInputStream& TextInputStream::operator>>( bool& val )
{
	this->read( val );
	return *this;
}

TextInputStream& TextInputStream::operator>>( char& val )
{
	if ( NULL != this->m_inStream ){
		m_inStream->read( &val, sizeof(val) );
	}
	return *this;
}

TextInputStream& TextInputStream::operator>>( float& val )
{
	this->read( val );
	return *this;
}

TextInputStream& TextInputStream::operator>>( double& val )
{
	this->read( val );
	return *this;
}	

void TextInputStream::init()
{
	this->m_inStream = NULL;
	this->m_size = 0;
	m_totCharRead = 0;
}

void TextInputStream::readLine( String& line )
{
	if ( NULL != this->m_inStream ){
		char c = '\0';
		m_inStream->read( &c, sizeof(c) );
		m_totCharRead++;
		while ( (c == '\n') && (m_totCharRead < m_size) ){ 
			line += c;
			m_inStream->read( &c, sizeof(c) );			
			m_totCharRead++;
		}
	}
}

String TextInputStream::readTillWhiteSpace()
{	
	String result = "";
	if ( NULL != this->m_inStream ){				
		char c = '\0';
		m_inStream->read( &c, sizeof(c) );
		m_totCharRead++;
		
		while ( ((c == ' ') || (c == '\0')) && (m_totCharRead < m_size) ){			
			m_inStream->read( &c, sizeof(c) );			
			m_totCharRead++;
		}
		//get all the nonwhitespace characters
		while ( (c != ' ') && (c != '\0')  && (m_totCharRead < m_size) ){
			result += c;
			m_inStream->read( &c, sizeof(c) );
			m_totCharRead++;
		}				
	}
	return result;
}

String TextInputStream::readTillTokenPair( const char& token )
{
	String result = "";
	if ( NULL != this->m_inStream ){				
		char c = '\0';
		m_inStream->read( &c, sizeof(c) );
		m_totCharRead++;
		
		while ( (c != token) && (c != ' ') && (m_totCharRead < m_size) ){
			m_inStream->read( &c, sizeof(c) );
			m_totCharRead++;
		}
		
		while ( (c != token) && (m_totCharRead < m_size) ){
			result += c;
			m_inStream->read( &c, sizeof(c) );
			m_totCharRead++;
		}
	}
	return result;
}