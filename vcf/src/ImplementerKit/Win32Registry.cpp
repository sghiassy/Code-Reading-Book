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
#include "Registry.h"

#include "Win32Registry.h"

using namespace VCF;

Win32Registry::Win32Registry()
{
	m_rootKeyType = RKT_ROOT;
	m_currentKeyHandle = NULL;
	setRoot( m_rootKeyType );
}

Win32Registry::~Win32Registry()
{
	RegCloseKey( m_currentKeyHandle );	
}

void Win32Registry::setRoot( const RegistryKeyType& rootKey )
{
	m_rootKeyType = rootKey;
	switch ( rootKey ){
		case RKT_ROOT: {
			m_rootKeyHandle = HKEY_CLASSES_ROOT;
		}
		break;

		case RKT_CURRENT_USER: {
			m_rootKeyHandle = HKEY_CURRENT_USER;
		}
		break;

		case RKT_LOCAL_MACHINE: {
			m_rootKeyHandle = HKEY_LOCAL_MACHINE;
		}
		break;
	}

	if ( ERROR_SUCCESS != RegOpenKeyEx( m_rootKeyHandle, NULL, 0, KEY_ALL_ACCESS, &m_currentKeyHandle ) ){
		//throw exception - operation failed !
	}
}

RegistryKeyType Win32Registry::getRoot()
{
	return m_rootKeyType;
}

bool Win32Registry::openKey( const String& keyname, const bool& createIfNonExistant )
{
	bool result = false;
	HKEY tmp = m_currentKeyHandle;
	if ( true == createIfNonExistant ){
		if ( true== keyname.empty() ){
			//throw exception !!!
		}
		DWORD disposition = 0;
		result = ( ERROR_SUCCESS == RegCreateKeyEx( tmp, keyname.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_currentKeyHandle, &disposition ) );
	}
	else {
		result = ( ERROR_SUCCESS == RegOpenKeyEx( tmp, keyname.c_str(), 0, KEY_ALL_ACCESS, &m_currentKeyHandle ) );		
	}

	return result;
	
}

bool Win32Registry::setValue( const String& value, const String& valuename )
{
	const unsigned char* val = (const unsigned char*)value.c_str();
	LONG resVal = RegSetValueEx( m_currentKeyHandle, valuename.c_str(), 0, REG_SZ, val, value.size() );
	return (resVal == ERROR_SUCCESS);
}

bool Win32Registry::setValue( const uint32& value, const String& valuename )
{	
	LONG resVal = RegSetValueEx( m_currentKeyHandle, valuename.c_str(), 0, REG_DWORD, (BYTE*)&value, sizeof(value) );
	return (resVal == ERROR_SUCCESS);
}

bool Win32Registry::setValue( const bool& value, const String& valuename )
{
	DWORD boolval = (DWORD)value;
	LONG resVal = RegSetValueEx( m_currentKeyHandle, valuename.c_str(), 0, REG_DWORD, (BYTE*)&boolval, sizeof(boolval) );
	return (resVal == ERROR_SUCCESS);
}

bool Win32Registry::setValue( void* dataBuffer, const uint32& dataBufferSize, const String& valuename )
{
	LONG resVal = RegSetValueEx( m_currentKeyHandle, valuename.c_str(), 0, REG_BINARY, (BYTE*)dataBuffer, dataBufferSize );
	return (resVal == ERROR_SUCCESS);
}

String Win32Registry::getStringValue( const String& valuename )
{

	DWORD type = 0;
	BYTE* buf = NULL;
	DWORD size = 0;
	String result;
	LONG resVal = RegQueryValueEx( m_currentKeyHandle, valuename.c_str(), 0, &type, NULL, &size );
	if ( resVal == ERROR_SUCCESS ){
		if ( (type == REG_SZ) && (size > 0) ){
			buf = new BYTE[size];
			memset( buf, 0, size );
			resVal = RegQueryValueEx( m_currentKeyHandle, valuename.c_str(), 0, &type, buf, &size );
			if ( resVal == ERROR_SUCCESS ){
				
				result = (char*)buf;
			}
			else {	
				//throw exception 
			}

			delete [] buf;
		}
	}
	else {
		//throw exception 
	}

	return result;
}

uint32 Win32Registry::getIntValue( const String& valuename )
{
	uint32 result = 0;
	
	DWORD type = 0;
	DWORD size = 0;
	LONG resVal = RegQueryValueEx( m_currentKeyHandle, valuename.c_str(), 0, &type, (BYTE*)&result, &size );
	if ( resVal == ERROR_SUCCESS ){
		if ( (type != REG_DWORD) && (size <= 0) ){
			//throw exception 
		}
	}
	else {
		//throw exception 
	}

	return result;
}

bool Win32Registry::getBoolValue( const String& valuename )
{
	bool result = false;
	DWORD type = 0;
	DWORD size = 0;
	LONG resVal = RegQueryValueEx( m_currentKeyHandle, valuename.c_str(), 0, &type, (BYTE*)&result, &size );
	if ( resVal == ERROR_SUCCESS ){
		if ( (type != REG_DWORD) && (size <= 0) ){
			//throw exception 
		}
	}
	else {
		//throw exception 
	}

	return result;
}

void Win32Registry::getDataBufValue( const String& valuename, uint32& dataBufferSize, void** dataBuffer )
{
	DWORD type = 0;
	BYTE* buf = NULL;
	DWORD size = 0;

	*dataBuffer = NULL;

	LONG resVal = RegQueryValueEx( m_currentKeyHandle, valuename.c_str(), 0, &type, NULL, &size );

	if ( resVal == ERROR_SUCCESS ){
		if ( (type == REG_BINARY) && (size > 0) ){
			buf = new BYTE[size]; //callers responsibility to clean up 
			memset( buf, 0, size );
			resVal = RegQueryValueEx( m_currentKeyHandle, valuename.c_str(), 0, &type, buf, &size );
			if ( resVal == ERROR_SUCCESS ){
				*dataBuffer = (void*)buf;				
			}
			else {	
				//throw exception 
			}
		}
	}
	else {
		//throw exception 
	}
}

Enumerator<String>* Win32Registry::getKeyNames()
{	
	DWORD index = 0;
	DWORD size = MAX_PATH + 1;
	TCHAR* keyName = new TCHAR[size];
	memset( keyName, 0, size );
	m_keys.clear();
	while ( ERROR_SUCCESS == RegEnumKey( m_currentKeyHandle, index, keyName, size ) ){
		this->m_keys.push_back( String(keyName) );
		memset( keyName, 0, size );
		index ++;
	}
	m_keysContainer.initContainer( m_keys );
	delete [] keyName;

	return m_keysContainer.getEnumerator();
}

Enumerator<RegistryValueInfo*>* Win32Registry::getValues()
{
	DWORD index = 0;
	CHAR valName[256];
	DWORD type = 0;
	DWORD valNameSize = 256;
	BYTE buffer[256];
	DWORD bufferSize = 256;

	std::vector<RegistryValueInfo*>::iterator it = m_values.begin();
	while ( it != m_values.end() ){
		RegistryValueInfo* regVal = *it;
		delete regVal;
		regVal = NULL;
		it++;
	}

	m_values.clear();
	memset( valName, 0, 256 );

	while ( ERROR_NO_MORE_ITEMS != RegEnumValue( m_currentKeyHandle, index, valName, &valNameSize, NULL, &type, (BYTE*)&buffer, &bufferSize ) ){
		String tmp = String(valName);		
		RegistryValueInfo* regVal = NULL;
		VariantData data;	
		
		switch ( type ){
			case REG_SZ:{
				String strVal( (VCFChar*)buffer );
				data = strVal;
				regVal = new RegistryValueInfo( &data, RDT_STRING, tmp );				
			}
			break;

			case REG_DWORD:{
				ulong32* intVal = (ulong32*)buffer;
				data = *intVal;
				regVal = new RegistryValueInfo( &data, RDT_INTEGER, tmp );
			}
			break;

			case REG_BINARY:{										
				void* dataBuf = NULL;				
				if ( NULL != buffer ){
					dataBuf = (void*)new char[bufferSize];
					memcpy( dataBuf, buffer, bufferSize );
				}
				regVal = new RegistryValueInfo( NULL, RDT_BINARY, tmp, (char*)dataBuf, bufferSize );
			}
			break;
		}

		m_values.push_back( regVal );
		bufferSize=256;
		valNameSize = 256;
		memset( valName, 0, 256 );
		index ++;
	}
	m_valuesContainer.initContainer( m_values );

	return m_valuesContainer.getEnumerator();
}

String Win32Registry::getCurrentKey()
{
	return m_currentRegKey;
}