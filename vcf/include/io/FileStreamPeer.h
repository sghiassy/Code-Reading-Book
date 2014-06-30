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

//FileStreamPeer.h


#ifndef _FILESTREAMPEER_H__
#define _FILESTREAMPEER_H__


namespace VCF
{

/**
*FileStreamPeer is a basic file stream interface to wrap native file IO OS calls.
*Under Win32 this will involve calls to ReadFile() and WriteFile,
*among others.
*
*@author Jim Crafton
*@version 1.0
*/
class FRAMEWORK_API FileStreamPeer {
public:
	virtual ~FileStreamPeer(){};

	/**
	*moves the seek pointer offset number of bytes in the 
	*direction specified by the offsetFrom argument.
	*@param unsigned long the number of bytes of offset the 
	*seek pointer
	*@param SeekType the direction to offset the seek pointer, which
	*may be one of the following values <br>
	*SEEK_FROM_START - offsets from the beginning of the file <br>
	*SEEK_FROM_RELATIVE - offsets relative to the current seek position <br>
	*SEEK_FROM_END - offsets backwards, starting at the end of the file <br>
	*/
	virtual void seek(const unsigned long& offset, const SeekType& offsetFrom) = 0;

	/**
	*returns the size of the file stream in bytes
	*/
	virtual unsigned long getSize() = 0;

	/**
	*reads sizeOfBytes bytes from the stream into the buffer bytesToRead
	*@param char* a buffer of at least sizeOfBytes long. This must be 
	*allocated by the caller. 
	*@param unsigned long the number of bytes to read from the file, starting
	*at the current seek position.
	*/
	virtual void read( char* bytesToRead, unsigned long sizeOfBytes ) = 0;

	/**
	*writes sizeOfBytes bytes to the file stream from the buffer bytesToWrite.
	*@param char* a buffer that must be at least sizeOfBytes long, that contains
	*the data that will written out to the file stream
	*@param @ unsigned long the number of bytes to write 
	*/
	virtual void write( char* bytesToWrite, unsigned long sizeOfBytes ) = 0;

	/**
	*returns a pointer to the buffer that represents the file contents
	*/
	virtual char* getBuffer() = 0;
};


};

#endif
