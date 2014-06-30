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

// Win32FileStream.h

#ifndef _WIN32FILESTREAM_H__
#define _WIN32FILESTREAM_H__


#include "FileStreamPeer.h"


namespace VCF
{

class FRAMEWORK_API Win32FileStream : public FileStreamPeer  
{
public:
	Win32FileStream( const String& filename, const FileStreamAccessType& accessType );	

	virtual ~Win32FileStream();
	
	virtual void seek( const unsigned long& offset, const SeekType& offsetFrom );

	virtual unsigned long getSize();

	virtual void read( char* bytesToRead, unsigned long sizeOfBytes );

	virtual void write( char* bytesToWrite, unsigned long sizeOfBytes );

	virtual char* getBuffer();
private:
	HANDLE m_fileHandle;
	VCF::String m_filename;
	DWORD translateAccessType( const FileStreamAccessType& accessType );
	DWORD translateAccessTypeToCreationType( const FileStreamAccessType& accessType );
	DWORD translateSeekTypeToMoveType( const SeekType& offsetFrom );
};


};

#endif
