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

// TextOutputStream.h

#ifndef _TEXTOUTPUTSTREAM_H__
#define _TEXTOUTPUTSTREAM_H__



namespace VCF
{


/**
*The TextOutputStream is used for writing data from another stream as text.

*/
class FRAMEWORK_API TextOutputStream : public OutputStream  
{
public:	

	TextOutputStream( OutputStream* outStream );

	TextOutputStream();

	virtual ~TextOutputStream();

	virtual void seek(const unsigned long& offset, const SeekType& offsetFrom);

	virtual unsigned long getSize();

	virtual char* getBuffer();

	virtual void write( char* bytesToRead, unsigned long sizeOfBytes );
	
	/**
	*all numbers are written out as the number converted to text, followed by a 
	*space character ( ' ' ), so the value 2 becomes "2 "
	*/
	TextOutputStream& operator<<( const short& val );

	TextOutputStream& operator<<( const long& val );

	TextOutputStream& operator<<( const int& val );

	TextOutputStream& operator<<( const bool& val );

	TextOutputStream& operator<<( const char& val );

	TextOutputStream& operator<<( const float& val );

	TextOutputStream& operator<<( const double& val );

	virtual void write( const short& val );
	
	virtual void write( const long& val );

	virtual void write( const int& val );

	virtual void write( const bool& val );	

	virtual void write( const float& val );

	virtual void write( const double& val );

	virtual void write( const String& val );

	void init();

	void writeLine( const String& line );

	String getTextBuffer();

	virtual String toString();
private:
	String m_textBuffer;
	unsigned long m_size;	
	OutputStream* m_outStream;
};


};

#endif 
