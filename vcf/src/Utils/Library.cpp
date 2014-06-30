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
#include "Library.h"
#ifdef WIN32
#include "Win32LibraryPeer.h"
#endif




using namespace VCF;

Library::Library( const String& libraryName, const bool& autoUnloadLib )
{
	m_currentLibName = libraryName;
	m_autoUnloadLib = autoUnloadLib;
	init();
}

Library::~Library()
{
	if ( NULL != m_libPeer ){
		if ( true == m_autoUnloadLib ){
			try {
				m_libPeer->unload();
			}
			catch (...){
				StringUtils::trace( "failed to unload the libary - probably alread unloaded" ); 
			}
		}		
		delete m_libPeer;
		m_libPeer = NULL;
	}
}


void Library::init()
{
	m_libPeer = NULL;

#ifdef WIN32 
	m_libPeer = new Win32LibraryPeer();
#endif

	if ( "" != m_currentLibName ){
		m_libPeer->load( m_currentLibName ); 
	}
}

void Library::load( const String& libraryFilename )
{
	m_libPeer->load( libraryFilename );
	m_currentLibName = libraryFilename;
}

void Library::unload()
{
	m_libPeer->unload();
}

void* Library::getFunction( const String& functionName )
{
	return m_libPeer->getFunction( functionName );
}