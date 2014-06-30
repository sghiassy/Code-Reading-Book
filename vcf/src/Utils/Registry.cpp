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
#include "RegistryPeer.h"
#ifdef WIN32
	#include "Win32Registry.h"
#endif

using namespace VCF;

Registry::Registry()
{
	m_peer = NULL;
#ifdef WIN32
	m_peer = new Win32Registry();
#endif
}

Registry::~Registry()
{
	delete m_peer;
}

void Registry::setRoot( const RegistryKeyType& rootKey )
{
	m_peer->setRoot( rootKey );
}

RegistryKeyType Registry::getRoot()
{
	return m_peer->getRoot();
}

bool Registry::openKey( const String& keyname, const bool& createIfNonExistant )
{
	return m_peer->openKey( keyname, createIfNonExistant );
}

bool Registry::setValue( const String& value, const String& valuename )
{
	return m_peer->setValue( value, valuename );
}

bool Registry::setValue( const uint32& value, const String& valuename )
{
	return m_peer->setValue( value, valuename );
}

bool Registry::setValue( const bool& value, const String& valuename )
{
	return m_peer->setValue( value, valuename );
}

bool Registry::setValue( void* dataBuffer, const uint32& dataBufferSize, const String& valuename )
{
	return m_peer->setValue( dataBuffer, dataBufferSize, valuename );
}

String Registry::getStringValue( const String& valuename )
{
	return m_peer->getStringValue( valuename );
}

uint32 Registry::getIntValue( const String& valuename )
{
	return m_peer->getIntValue( valuename );
}

bool Registry::getBoolValue( const String& valuename )
{
	return m_peer->getBoolValue( valuename );
}

void Registry::getDataBufValue( const String& valuename, uint32& dataBufferSize, void** dataBuffer )
{
	m_peer->getDataBufValue( valuename, dataBufferSize, dataBuffer );
}

Enumerator<String>* Registry::getKeyNames()
{
	return m_peer->getKeyNames();
}

Enumerator<RegistryValueInfo*>* Registry::getValues()
{
	return m_peer->getValues();
}

String Registry::getCurrentKey()
{
	return m_peer->getCurrentKey();
}