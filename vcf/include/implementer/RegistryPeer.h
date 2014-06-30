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

#ifndef _REGISTRYPEER_H__
#define _REGISTRYPEER_H__



namespace VCF {



class FRAMEWORK_API RegistryPeer {
public:
	virtual ~RegistryPeer(){};

	virtual void setRoot( const RegistryKeyType& rootKey ) = 0;
	
	virtual RegistryKeyType getRoot() = 0;

	virtual bool openKey( const String& keyname, const bool& createIfNonExistant ) = 0;

	virtual bool setValue( const String& value, const String& valuename ) = 0; 

	virtual bool setValue( const uint32& value, const String& valuename ) = 0; 
	
	virtual bool setValue( const bool& value, const String& valuename ) = 0; 

	virtual bool setValue( void* dataBuffer, const uint32& dataBufferSize, const String& valuename ) = 0; 

	virtual String getStringValue( const String& valuename ) = 0;

	virtual uint32 getIntValue( const String& valuename ) = 0;

	virtual bool getBoolValue( const String& valuename ) = 0;

	virtual void getDataBufValue( const String& valuename, uint32& dataBufferSize, void** dataBuffer ) = 0;

	virtual Enumerator<String>* getKeyNames() = 0;

	virtual Enumerator<RegistryValueInfo*>* getValues() = 0;

	virtual String getCurrentKey() = 0;
};

}; //end of namespace VCF

#endif //_REGISTRYPEER_H__