//Directory.h
/**
Copyright (c) 2000-2001, Jim Crafton
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
	Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in 
	the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

NB: This software will not save the world. 
*/
#include "FoundationKit.h"
#include "Directory.h"
#include "FilePeer.h"

using namespace VCF;


Directory::Finder::Finder( Directory* directoryToSearch, const String& filter )
{
	m_owningDirectory = directoryToSearch;
	m_searchFilter = filter;
	reset();
}

Directory::Finder::~Finder()
{
	FilePeer* dirPeer = m_owningDirectory->getPeer();
	if ( NULL != dirPeer ) {
		dirPeer->endFileSearch( this );
	}	
}

bool Directory::Finder::hasMoreElements( const bool& backward )
{
	if ( false == m_searchHasElements ) {
		FilePeer* dirPeer = m_owningDirectory->getPeer();
		if ( NULL != dirPeer ) {
			dirPeer->endFileSearch( this );
		}
		else {
			//throw exception
		}
	}
	return m_searchHasElements;
}

String Directory::Finder::nextElement()
{
	String result;
	FilePeer* dirPeer = m_owningDirectory->getPeer();
	if ( NULL != dirPeer ) {
		result = dirPeer->findNextFileInSearch( this );
		m_searchHasElements = !result.empty();
	}
	else {
		//throw exception
	}

	return result;
}

String Directory::Finder::prevElement()
{
	return nextElement();
}

void Directory::Finder::reset( const bool& backward )
{
	FilePeer* dirPeer = m_owningDirectory->getPeer();
	if ( NULL != dirPeer ) {
		m_searchHasElements = dirPeer->beginFileSearch( this );
	}
	else {
		//throw exception
	}
}


Directory::Directory( const String& fileName ):
	File( "" )
{
	String tmp = fileName;
	if ( tmp[tmp.size()-1] != '\\' ) {
		tmp += "\\";
	}
	setName( tmp );
}

Directory::~Directory()
{

}

Directory::Finder* Directory::findFiles( const String& filter )
{
	Directory::Finder* result = new Directory::Finder( this, filter );

	return result;
}