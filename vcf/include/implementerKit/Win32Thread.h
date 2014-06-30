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

// Win32Peer.h

#ifndef _WIN32THREAD_H__
#define _WIN32THREAD_H__


#include "ThreadPeer.h"


namespace VCF
{

class Thread;

class FRAMEWORK_API Win32Thread : public ThreadPeer  {
public:
	Win32Thread( Thread* thread, bool runInLoop=false );	

	virtual ~Win32Thread();

	virtual bool start();

	virtual void pause();
	
	static unsigned __stdcall threadProc( void* param );

	static unsigned __stdcall loopThreadProc( void* param );

	virtual void sleep( unsigned int milliseconds );

	virtual uint32 getThreadID(){
		return m_threadID;
	}

	virtual unsigned int getOwningProcessID()	{
		return m_owningProcessID;
	}
private:
	unsigned m_threadID;
	unsigned m_owningProcessID;
	unsigned m_initFlags;
	void* m_security;
	unsigned m_stackSize;
	uint32 m_threadHandle;
	Thread* m_thread;
	bool m_runInLoop;
};


};
#endif //_WIN32THREAD_H__
