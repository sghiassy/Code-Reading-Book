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
#include "FoundationKit.h"
#include "LibraryPeer.h"
#include "Win32Peer.h"
#include "Win32LibraryPeer.h"
#include "RuntimeException.h"
#include "InvalidPointerException.h"

using namespace VCF;

Win32LibraryPeer::Win32LibraryPeer()
{
	m_libHandle = NULL;
}

Win32LibraryPeer::~Win32LibraryPeer()
{
	m_libHandle = NULL;
}


void Win32LibraryPeer::load( const String& libraryFilename )
{
	m_libHandle = LoadLibrary( libraryFilename.c_str() );
	
	if ( NULL == m_libHandle ){
		String error = VCFWin32::Win32Utils::getErrorString( GetLastError() );
		throw RuntimeException( error );
	}
}

void* Win32LibraryPeer::getFunction( const String& functionName )
{
	void* result = NULL;
	
	if ( NULL == m_libHandle ){
		throw InvalidPointerException(MAKE_ERROR_MSG(INVALID_POINTER_ERROR), __LINE__);
	}

	result = GetProcAddress( m_libHandle, functionName.c_str() );

	if ( NULL == result ){
		String error = VCFWin32::Win32Utils::getErrorString( GetLastError() );
		throw RuntimeException( error );
	}
	
	return result;
}

void Win32LibraryPeer::unload()
{
	if ( NULL == m_libHandle ){
		throw InvalidPointerException(MAKE_ERROR_MSG(INVALID_POINTER_ERROR), __LINE__);
	}

	BOOL freeLibResult = FreeLibrary( m_libHandle );

	if ( FALSE == freeLibResult ){
		String error = VCFWin32::Win32Utils::getErrorString( GetLastError() );
		throw RuntimeException( error );
	}
}
