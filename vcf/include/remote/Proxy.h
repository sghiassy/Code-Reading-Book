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

#ifndef _PROXY_H__
#define _PROXY_H__

#include "VCF.h"
#include "RemoteCommon.h"
#include "InstanceID.h"


namespace VCFRemote {

class VCF::Object;

class REMOTEKIT_API Proxy : public VCF::Persistable {
public:
	Proxy( VCF::Object* localObjectInstance = NULL );

	virtual ~Proxy();
	
	virtual VCF::VariantData* invoke( const VCF::String& methodName,
		                              const VCF::uint32& argCount, 
									  VCF::VariantData** arguments );
    virtual bool isRemote() {
		return m_isRemote;	
	}

    virtual VCF::String getClassName();

    virtual VCF::String getClassID();

	InstanceID* getInstanceID() {
		return &m_instanceID;
	}

	virtual void saveToStream( VCF::OutputStream * stream );

	virtual void loadFromStream( VCF::InputStream * stream );

    static Proxy* createInstance( const VCF::String& className );

	static Proxy* createInstanceByClassID( const VCF::String& classID );

protected:
	void marshallArguments( const VCF::uint32& argCount, 
							VCF::VariantData** arguments,
							VCF::OutputStream* marshallingStream );
private:
	/**
	*this represents a pointer to the local Object that the 
	*Proxy might represent. It is only non-NULL if the Object
	*instance requested from Proxy::createInstance() or createInstanceByClassID()
	*exists on the Proxy process's ClassRegistry. Otherwise the Proxy represents
	*an Object instance from either another process on the same machine, or 
	*an Object instance from some remote machine.
	*/
	VCF::Object* m_localObjectInstance;

	/**
	*indicates whether the Proxy represents a local in process Object, or 
	*an Object from outside the process.
	*/
	bool m_isRemote;

	VCF::String m_className;
	
	VCF::String m_classID;

    InstanceID m_instanceID;
};

}; //end of namespace VCFRemote

#endif //_PROXY_H__