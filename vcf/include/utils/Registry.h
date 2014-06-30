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

#ifndef _REGISTRY_H__
#define _REGISTRY_H__


namespace VCF {

enum RegistryKeyType{
	RKT_ROOT=0,
	RKT_CURRENT_USER,
	RKT_LOCAL_MACHINE
};

enum RegistryDataType{
	RDT_STRING = 0,
	RDT_INTEGER,
	RDT_BOOL,
	RDT_BINARY
};


/**
*a class that wraps up a specific registry value that may
*be a string, integer, bool, or stream of binary data
*/
class FRAMEWORK_API RegistryValueInfo : public Object {
public:

	RegistryValueInfo( VariantData* data, const RegistryDataType& dataType, 
		               const String& valueName, char* dataBuffer=NULL, const uint32& dataBufferSize=0  ){		
		
		m_dataType = dataType;

		m_valueName = valueName;
		
		m_dataBufferSize = dataBufferSize;

		m_dataBuffer = NULL;

		if ( NULL != data ){
			switch ( m_dataType ){
				case RDT_STRING :{
					m_data = (String)*data;
				}
				break;

				case RDT_INTEGER :{
					m_data = (ulong32)*data;
				}
				break;

				case RDT_BOOL :{
					m_data = (bool)*data;
				}
				break;

				case RDT_BINARY :{
					m_dataBuffer = dataBuffer;
				}
				break;
			}	
		}
	}


	virtual ~RegistryValueInfo(){
		if ( NULL != m_dataBuffer ){
			delete m_dataBuffer;
		}
	}

	RegistryDataType m_dataType;

	VariantData m_data; 

	String m_valueName;

	char* m_dataBuffer;

	uint32 m_dataBufferSize;
};



class RegistryPeer;

/**
*The registry represents an external data source that applications
*may store application data in. This may be things like special file names,
*UI positions, application state data etc. The external data may be anything
*depending on the OS. In Win32, this represents a connection to the 
*Win32 registry data base. 
*/
class FRAMEWORK_API Registry  : public Object{

public:
	Registry();

	virtual ~Registry();

	void setRoot( const RegistryKeyType& rootKey );
	
	RegistryKeyType getRoot();

	bool openKey( const String& keyname, const bool& createIfNonExistant=false );

	bool setValue( const String& value, const String& valuename ); 

	bool setValue( const uint32& value, const String& valuename ); 
	
	bool setValue( const bool& value, const String& valuename ); 

	bool setValue( void* dataBuffer, const uint32& dataBufferSize, const String& valuename ); 

	String getStringValue( const String& valuename );

	uint32 getIntValue( const String& valuename );

	bool getBoolValue( const String& valuename );

	void getDataBufValue( const String& valuename, uint32& dataBufferSize, void** dataBuffer );

	Enumerator<String>* getKeyNames();

	Enumerator<RegistryValueInfo*>* getValues();

	String getCurrentKey();
private:
	RegistryPeer* m_peer;
};

}; //end of namespace VCF

#endif //_REGISTRY_H__