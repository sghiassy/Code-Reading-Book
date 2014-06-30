//LibraryApplication.h
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

#include "ApplicationKit.h"
#include "LibraryApplication.h"

using namespace VCF;


 ;
 ;

LibraryApplication::LibraryApplication()
{
	initApplicationKit();

	UIToolkit* toolkit = UIToolkit::getDefaultUIToolkit();

	this->m_applicationPeer = toolkit->createApplicationPeer();
	m_applicationPeer->setApplication( this );	
	
	m_libName = "";
}

LibraryApplication::~LibraryApplication()
{
	terminateApplicationKit();
}

void LibraryApplication::setName( const String& name )
{
	m_libName = name;

	std::map<String,LibraryApplication*>::iterator found = 	LibraryApplication::namedLibraryAppMap->find( m_libName );
	if ( found != LibraryApplication::namedLibraryAppMap->end() ) {
		(*LibraryApplication::namedLibraryAppMap)[ m_libName ] = this;				
	}
}

Enumerator<LibraryApplication*>* LibraryApplication::getRegisteredLibraries()
{
	return LibraryApplication::namedLibAppContainer->getEnumerator();
}

void LibraryApplication::registerLibrary( LibraryApplication* libraryApp )
{
	(*LibraryApplication::namedLibraryAppMap)[ libraryApp->getName() ] = libraryApp;
}


void LibraryApplication::unRegisterLibrary( LibraryApplication* libraryApp )
{
	std::map<String,LibraryApplication*>::iterator found = 	LibraryApplication::namedLibraryAppMap->find( libraryApp->getName() );
	if ( found != LibraryApplication::namedLibraryAppMap->end() ) {
		LibraryApplication::namedLibraryAppMap->erase( found );
	}
}

LibraryApplication* LibraryApplication::getRegisteredLibraryApplication( const String& libName )
{
	LibraryApplication* result = NULL;

	std::map<String,LibraryApplication*>::iterator found = 	LibraryApplication::namedLibraryAppMap->find( libName );
	if ( found != LibraryApplication::namedLibraryAppMap->end() ) {
		result = found->second;
	}

	return result;
}

void LibraryApplication::load( LibraryApplication* libraryApp )
{
	
}

void LibraryApplication::unload( LibraryApplication* libraryApp )
{
	
}

void LibraryApplication::initLibraryRegistrar()
{
	LibraryApplication::namedLibraryAppMap = new std::map<String,LibraryApplication*>();
	LibraryApplication::namedLibAppContainer = new EnumeratorMapContainer<std::map<String,LibraryApplication*>, LibraryApplication*>();

	LibraryApplication::namedLibAppContainer->initContainer( *LibraryApplication::namedLibraryAppMap );
}

void LibraryApplication::clearLibraryRegistrar()
{
	delete LibraryApplication::namedLibraryAppMap;
	LibraryApplication::namedLibraryAppMap = NULL;

	delete LibraryApplication::namedLibAppContainer;
	LibraryApplication::namedLibAppContainer = NULL;
}