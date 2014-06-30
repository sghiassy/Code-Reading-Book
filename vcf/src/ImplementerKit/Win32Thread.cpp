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

// Win32Thread.cpp

#include "FoundationKit.h"
#include "Thread.h"
#include "Win32Thread.h"
#include "Win32Peer.h"
#include <process.h>

using namespace VCF;

Win32Thread::Win32Thread( Thread* thread, bool runInLoop )
{
	m_security = NULL;
	m_threadID = 0;
	m_initFlags = CREATE_SUSPENDED;
	m_stackSize = 0;
	m_threadHandle = 0;
	m_thread = thread;
	m_runInLoop = runInLoop;
	m_owningProcessID = 0;
}

Win32Thread::~Win32Thread()
{
	//_endthreadex( m_threadID );
	if ( 0 != m_threadHandle )	{
		int waitResult = WaitForSingleObject( (HANDLE)m_threadHandle, 5000 );
		
		switch ( waitResult ) {
			case WAIT_ABANDONED:
			case WAIT_FAILED :{
				StringUtils::trace( "WaitForSingleObject failed for thread " +StringUtils::toString(m_threadID) + "\n" );
			}
			break;

			case WAIT_TIMEOUT :{
				StringUtils::trace( "WaitForSingleObject timed out for thread " +StringUtils::toString(m_threadID) + "\n" );
			}
			break;
			case WAIT_OBJECT_0 : {
				StringUtils::trace( "WaitForSingleObject succeeded for thread " +StringUtils::toString(m_threadID) + "\n" );
			}
			break;
		}
		int err = CloseHandle( (HANDLE)m_threadHandle );
		if ( !err ) {
			err = GetLastError();
		}
	}
	m_threadHandle = 0;
}

bool Win32Thread::start()
{
	if ( false == m_runInLoop ){
		m_threadHandle = _beginthreadex( m_security, m_stackSize, Win32Thread::threadProc, (void*)this, m_initFlags, &m_threadID );
	}
	else {
		m_threadHandle = _beginthreadex( m_security, m_stackSize, Win32Thread::loopThreadProc, (void*)this, m_initFlags, &m_threadID );
	}
	this->m_owningProcessID = GetCurrentProcessId();
	SetThreadPriority( (HANDLE)m_threadHandle, THREAD_PRIORITY_LOWEST );
	::ResumeThread( (HANDLE)m_threadHandle );
	return false;
}

void Win32Thread::pause()
{

}

unsigned Win32Thread::threadProc( void* param )
{
	Win32Thread* peer = (Win32Thread*)param;
	if ( NULL != peer->m_thread ){
		
		peer->m_thread->run();			
	
		if ( true == peer->m_thread->canAutoDelete() )	{
			delete peer->m_thread;
		}	
	}
	return 0;
}

unsigned Win32Thread::loopThreadProc( void* param )
{
	Win32Thread* peer = (Win32Thread*)param;
	if ( NULL != peer->m_thread ){
		bool runResult = true;
		while ( (peer->m_thread->canContinue()) && (true == runResult) ){
			runResult = peer->m_thread->run();
			Sleep( 0 );
		}
		if ( (true == peer->m_thread->canAutoDelete()) && peer->m_thread->canContinue() )	{
			delete peer->m_thread;
		}
	}
	return 0;
}

void Win32Thread::sleep( unsigned int milliseconds ) 
{
	//::Sleep( milliseconds );
	HANDLE handleArray[1];
	handleArray[0] = (HANDLE)this->m_threadHandle;

	int waitResult = MsgWaitForMultipleObjects( 1, (HANDLE*)&handleArray, TRUE, milliseconds, QS_ALLEVENTS );
	switch ( waitResult ) {
		case WAIT_ABANDONED:
		case WAIT_FAILED :{
			//StringUtils::trace( "MsgWaitForMultipleObjects failed for thread ID " + StringUtils::toString(m_threadID) + "\n" );
		}
		break;

		case WAIT_TIMEOUT :{
			//StringUtils::trace( "MsgWaitForMultipleObjects timed out for thread ID " + StringUtils::toString(m_threadID) + "\n" );
		}
		break;
		case WAIT_OBJECT_0 : {
			//StringUtils::trace( "MsgWaitForMultipleObjects succeeded for thread ID " + StringUtils::toString(m_threadID) + "\n" );
		}
		break;
	}
}