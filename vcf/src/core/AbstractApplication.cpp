//AbstractApplication.h
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
#include "AbstractApplication.h"
#include "VFFInputStream.h"
#include "InvalidPointerException.h"

using namespace VCF;



AbstractApplication::AbstractApplication()
{
	m_applicationPeer = NULL;
}

AbstractApplication::~AbstractApplication()
{

}

ApplicationPeer* AbstractApplication::getPeer()
{
	return this->m_applicationPeer;
}

ResourceBundle* AbstractApplication::getResourceBundle()
{
	return this->m_applicationPeer->getResourceBundle();
}

String AbstractApplication::getFileName()
{
	return this->m_applicationPeer->getFileName();
}

String AbstractApplication::getCurrentDirectory()
{
	return m_applicationPeer->getCurrentDirectory();
}

void AbstractApplication::setCurrentDirectory( const String& currentDirectory )
{
	m_applicationPeer->setCurrentDirectory( currentDirectory );
}

Frame* AbstractApplication::createFrame( const String& frameClassName )
{
	Frame* result = NULL;
	
	ResourceBundle* resBundle = this->getResourceBundle();
	if ( NULL != resBundle ){
		String vffString = resBundle->getVFF( frameClassName );
		
		if ( false == vffString.empty() ){
			BasicInputStream bis( vffString );
			VFFInputStream vis( &bis );
			vis.readComponent( (Component**)&result );
		}
		else{
			String errMsg = "VFF Resource \"" + frameClassName + "\" has no data.";
			throw InvalidPointerException( MAKE_ERROR_MSG( errMsg ) ); 
		}
	}
	else{
		String errMsg = "Resource \"" + frameClassName + "\" not found.";
		throw InvalidPointerException( MAKE_ERROR_MSG( errMsg ) ); 
	}
	
	return result;
}

void AbstractApplication::loadFrame( Frame** frame )
{
	
	String frameClassName = (*frame)->getClassName();

	ResourceBundle* resBundle = this->getResourceBundle();
	if ( NULL != resBundle ){
		String vffString = resBundle->getVFF( frameClassName );
		
		if ( false == vffString.empty() ){
			BasicInputStream bis( vffString );
			VFFInputStream vis( &bis );
			vis.readComponent( (Component**)frame );
		}
		else{
			String errMsg = "VFF Resource \"" + frameClassName + "\" has no data.";
			throw InvalidPointerException( MAKE_ERROR_MSG( errMsg ) ); 
		}
	}
	else{
		String errMsg = "Resource \"" + frameClassName + "\" not found.";
		throw InvalidPointerException( MAKE_ERROR_MSG( errMsg ) ); 
	}
}

void AbstractApplication::idleTime()
{

}