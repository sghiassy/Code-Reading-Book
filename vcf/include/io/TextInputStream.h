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

// TextInputStream.h

#ifndef _TEXTINPUTSTREAM_H__
#define _TEXTINPUTSTREAM_H__



namespace VCF
{

/**
*TextInputStream is used for interpreting data from another stream as text.
*This assumes the data in the stream was originally written as text. For example,
*a FileStream that has been used to write a binary file, and is then passed into
*a new instance of TextInputStream to read the same file, will cause unpredicatable
*results in the decoding of the stream, since the TextInputStream is taking advantage of
*certain layout assumptions in how the source stream is organized.
*So consider yourself warned !
*/
class FRAMEWORK_API TextInputStream : public InputStream  
{
public:	
	TextInputStream( InputStream* inStream );

	virtual ~TextInputStream();

	virtual void seek(const unsigned long& offset, const SeekType& offsetFrom);

	virtual unsigned long getSize();

	virtual char* getBuffer();

	virtual void read( char* bytesToRead, unsigned long sizeOfBytes );
	
	/**
	*all numbers are written out as the number converted to text, followed by a 
	*space character ( ' ' ), so the value 2 becomes "2 "
	*/

	TextInputStream& operator>>( short& val );

	TextInputStream& operator>>( long& val );

	TextInputStream& operator>>( int& val );

	TextInputStream& operator>>( bool& val );

	TextInputStream& operator>>( char& val );

	TextInputStream& operator>>( float& val );

	TextInputStream& operator>>( double& val );

	virtual void read( short& val );
	
	virtual void read( long& val );

	virtual void read( int& val );

	virtual void read( bool& val );	

	virtual void read( float& val );

	virtual void read( double& val );

	virtual void read( String& val );

	void init();

	void readLine( String& line );
private:
	InputStream* m_inStream;
	unsigned long m_size;
	unsigned long m_totCharRead;
	String readTillWhiteSpace();

	/**
	*reads to the starting point of the token and then accumalates anything in the buffer
	*till the next occurence of the token
	*@return String the accumalted text
	*/
	String readTillTokenPair( const char& token );
};


};

#endif 

