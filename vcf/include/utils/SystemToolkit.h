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

#ifndef _SYSTEMTOOLKIT_H__
#define _SYSTEMTOOLKIT_H__


namespace VCF {

class ProcessPeer;

class ThreadPeer;

class Process;

class Thread;

class System;

class SystemPeer;

/**
*The SystemToolkit is used to create or get access to 
*basic system peer instances, like threads, processes, etc.
*/
class FRAMEWORK_API SystemToolkit : public Object{

public:

	static SystemToolkit* create();

	SystemToolkit();

	virtual ~SystemToolkit();
	
	/**
	*creates a new ProcessPeer instance
	*/
	virtual ProcessPeer* createProcessPeer( Process* process );
	
	/**
	*creates a new ThreadPeer instance
	*/
	virtual ThreadPeer* createThreadPeer( Thread* thread );

	/**
	*creates a new system peer
	*/
	virtual SystemPeer* createSystemPeer();

	/**
	*returns the one and only instance of the system toolkit
	*/
	static SystemToolkit* getSystemToolkit() {
		return systemToolkitInstance;
	}
	
private:
	static SystemToolkit* systemToolkitInstance;
};

}; //end of namespace VCF

#endif //_SYSTEMTOOLKIT_H__