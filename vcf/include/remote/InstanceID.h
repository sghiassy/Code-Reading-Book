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

#ifndef _INSTANCEID_H__
#define _INSTANCEID_H__

#include "RemoteCommon.h"
#include "StringUtils.h"

namespace VCFRemote {

class REMOTEKIT_API InstanceID : public VCF::Object, public VCF::Persistable {

public:
	InstanceID();

	InstanceID( const int& memAddress );

	InstanceID( const VCF::String& machineName, const int& processID,
		        const int& threadID, const int& memAddress );

	virtual ~InstanceID(){};

	virtual VCF::String toString(){
		return 	m_machineName + "." + StringUtils::toString(m_processID) + "." +
			    StringUtils::toString(m_threadID) + "@" + 
				StringUtils::toString(m_memAddress);
	}

	VCF::String getMachinName() {
		return m_machineName;
	}

	void setMachineName( const VCF::String& machineName ) {
		m_machineName = machineName;
	}

	int getProcessID() {
		return m_processID;
	}

	void setProcessID( const int& processID ) {
		m_processID = processID;
	}

	int getThreadID() {
		return m_processID;
	}

	void setThreadID( const int& threadID ) {
		m_threadID = threadID;
	}

	int getMemAddress() {
		return m_memAddress;
	}

	void setMemAddress( const int& memAddress ) {
		m_memAddress = memAddress;
	}

	virtual void saveToStream( VCF::OutputStream * stream );

	virtual void loadFromStream( VCF::InputStream * stream );
private:
	VCF::String m_machineName;
	VCF::uint32 m_IPAddress;
    int m_processID;
    int m_threadID;
    int m_memAddress;
};

}; //end of namespace VCFRemote

#endif //_INSTANCEID_H__