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

//Stream.cpp
#include "FoundationKit.h"

using namespace VCF;


void InputStream::read( Persistable* persistableObject ){
	if ( NULL != persistableObject ){
		persistableObject->loadFromStream( this );
	}	
}

void InputStream::read( short& val )
{
	this->read( (char*)&val, sizeof(val) );
}
	
void InputStream::read( long& val )
{	
	this->read( (char*)&val, sizeof(val) );
}	

void InputStream::read( int& val )
{
	this->read( (char*)&val, sizeof(val) );
}

void InputStream::read( bool& val )
{
	this->read( (char*)&val, sizeof(val) );
}

void InputStream::read( char& val )
{
	this->read( (char*)&val, sizeof(val) );
}

void InputStream::read( float& val )
{
	this->read( (char*)&val, sizeof(val) );
}

void InputStream::read( double& val )
{
	this->read( (char*)&val, sizeof(val) );
}

void InputStream::read( String& val )
{
	val = "";
	char c = '\0';
	unsigned long count = 0;
	unsigned long size = this->getSize();

	this->read( &c, sizeof(c) );
	while ( (c != '\0') && (count < size) ){
		val += c;
		this->read( &c, sizeof(c) );
		count ++;
	}
}

InputStream& InputStream::operator>>( short& val ){
	this->read( val );
	return *this;
}

InputStream& InputStream::operator>>( long& val ){
	this->read( val );
	return *this;	
}

InputStream& InputStream::operator>>( int& val ){
	this->read( val );
	return *this;
}

InputStream& InputStream::operator>>( bool& val ){
	this->read( val );
	return *this;
}

InputStream& InputStream::operator>>( char& val ){
	this->read( val );
	return *this;
}

InputStream& InputStream::operator>>( float& val ){
	this->read( val );
	return *this;
}

InputStream& InputStream::operator>>( double& val ){
	this->read( val );
	return *this;
}	

InputStream& InputStream::operator>>( String& val )
{
	this->read( val );
	return *this;
}

InputStream& InputStream::operator>>( Persistable* val ){
	this->read( val );
	return *this;
}


void OutputStream::write( Persistable* persistableObject ){
	if ( NULL != persistableObject ){
		persistableObject->saveToStream( this );
	}
}

OutputStream& OutputStream::operator<<( const short& val ){
	this->write( val );
	return *this;
}

OutputStream& OutputStream::operator<<( const long& val ){
	this->write( val );
	return *this;
}

OutputStream& OutputStream::operator<<( const int& val ){
	this->write( val );
	return *this;
}

OutputStream& OutputStream::operator<<( const bool& val ){
	this->write( val );
	return *this;
}

OutputStream& OutputStream::operator<<( const char& val ){
	this->write( val );
	return *this;
}

OutputStream& OutputStream::operator<<( const float& val ){
	this->write( val );
	return *this;
}

OutputStream& OutputStream::operator<<( const double& val ){
	this->write( val );
	return *this;
}

OutputStream& OutputStream::operator<<( const String& val )
{
	this->write( val );
	return *this;
}

OutputStream& OutputStream::operator<<( Persistable* val ){
	this->write( val );
	return *this;
}

void OutputStream::write( const short& val )
{
	this->write( (char*)&val, sizeof(val) );
}
	
void OutputStream::write( const long& val )
{
	this->write( (char*)&val, sizeof(val) );
}

void OutputStream::write( const int& val )
{
	this->write( (char*)&val, sizeof(val) );
}

void OutputStream::write( const bool& val )
{
	this->write( (char*)&val, sizeof(val) );
}

void OutputStream::write( const char& val )
{
	this->write( (char*)&val, sizeof(val) );
}

void OutputStream::write( const float& val )
{
	this->write( (char*)&val, sizeof(val) );
}

void OutputStream::write( const double& val )
{
	this->write( (char*)&val, sizeof(val) );
}

void OutputStream::write( const String& val )
{
	//size of the string is val.size() - 1 to avoid the null term
	int size = val.size();
	char* buf = new char[size+1];
	memset(buf, 0, size+1 );
	val.copy( buf, size );	
	this->write( buf, size );
	delete [] buf;
}
