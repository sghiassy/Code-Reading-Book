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

#ifdef WIN32
	#include "Win32FilePeer.h"
#endif

using namespace VCF;

File::File( const String& fileName )
{
	m_filePeer = NULL;

#ifdef WIN32 
	m_filePeer = new Win32FilePeer( this, fileName );
#endif

}

File::~File()
{
	if ( NULL != m_filePeer ){
		delete m_filePeer;
	}
	m_filePeer = NULL;
}

void File::remove()
{
	m_filePeer->remove();
}

void File::create()
{
	m_filePeer->create();
}

uint32 File::getSize()
{
	return m_filePeer->getSize();
}

String File::getName()
{
	return m_filePeer->getName();
}

void File::setName( const String& fileName )
{
	m_filePeer->setName( fileName );
}

void File::copyTo( const String& copyFileName )
{
	m_filePeer->copyTo( copyFileName );
}

bool File::isDirectory()
{
	return (m_filePeer->getSize() == 0);
}