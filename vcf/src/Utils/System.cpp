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

//System.cpp

//#include <varargs.h>

#include "FoundationKit.h"
#include "SystemPeer.h"
#include "SystemToolkit.h"
#include "ErrorLog.h"
#include <stdarg.h>
#include <stdio.h>

using namespace VCF;

System* System::create()
{
	if ( NULL == System::systemInstance ) {
		System::systemInstance = new System();
		System::systemInstance->init();
	}

	return System::systemInstance;
}


System::System()
{	
	m_systemPeer = SystemToolkit::getSystemToolkit()->createSystemPeer();	

	m_errorLogInstance = NULL;
}

System::~System()
{
	delete m_systemPeer;
}

unsigned long System::getTickCount()
{
	if ( NULL != System::systemInstance->m_systemPeer ) {
		return System::systemInstance->m_systemPeer->getTickCount();
	}
	else return 0;	
}

void System::sleep( const uint32& milliseconds )
{
	System::systemInstance->m_systemPeer->sleep( milliseconds );
}

void System::setErrorLog( ErrorLog* errorLog )
{
	System::systemInstance->m_errorLogInstance = errorLog;
}

void System::print( String text, ... )
{	
	va_list args;
	va_start( args, text );
	int charRequired = 1024;
	VCFChar* tmpChar = new VCFChar[charRequired];
	memset( tmpChar, 0, charRequired );
	_vsnprintf( tmpChar, charRequired, text.c_str(), args );
	
	va_end( args ); 
	printf( tmpChar );	
	
	if ( (NULL != System::systemInstance->m_errorLogInstance) && (charRequired>0) ) {
		String tmp(tmpChar);

		System::systemInstance->m_errorLogInstance->toLog( tmp );		
	}
	delete [] tmpChar;	
}

void System::errorPrint( BasicException* exception )
{
	System::print( "Exception occured ! Error string: %s\n", exception->getMessage().c_str() );
}

