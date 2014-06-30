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

#ifndef _WIN32REGISTRY_H__
#define _WIN32REGISTRY_H__

#include "RegistryPeer.h"


namespace VCF {

class FRAMEWORK_API Win32Registry : public Object, public RegistryPeer{

public:
	Win32Registry();

	virtual ~Win32Registry();

	virtual void setRoot( const RegistryKeyType& rootKey );
	
	virtual RegistryKeyType getRoot();

	virtual bool openKey( const String& keyname, const bool& createIfNonExistant );

	virtual bool setValue( const String& value, const String& valuename ); 

	virtual bool setValue( const uint32& value, const String& valuename ); 
	
	virtual bool setValue( const bool& value, const String& valuename ); 

	virtual bool setValue( void* dataBuffer, const uint32& dataBufferSize, const String& valuename ); 

	virtual String getStringValue( const String& valuename );

	virtual uint32 getIntValue( const String& valuename );

	virtual bool getBoolValue( const String& valuename );

	virtual void getDataBufValue( const String& valuename, uint32& dataBufferSize, void** dataBuffer );

	virtual Enumerator<String>* getKeyNames();

	virtual Enumerator<RegistryValueInfo*>* getValues();

	virtual String getCurrentKey();
private:
	std::vector<RegistryValueInfo*> m_values;
	std::vector<String> m_keys;
	EnumeratorContainer<std::vector<RegistryValueInfo*>,RegistryValueInfo*> m_valuesContainer;
	EnumeratorContainer<std::vector<String>,String> m_keysContainer;
	RegistryKeyType m_rootKeyType;
	String m_currentRegKey;

	HKEY m_currentKeyHandle;
	HKEY m_rootKeyHandle;
};

}; //end of namespace VCF

#endif //_WIN32REGISTRY_H__