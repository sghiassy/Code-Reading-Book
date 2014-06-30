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
#include "Thread.h"

#ifdef WIN32 
#include "Win32Thread.h"
#endif


using namespace VCF;


Thread::Thread( Runnable* runnableObject, const bool& autoDelete, const bool& autoCreatePeer )
{
	m_canContinue = true;
	this->m_runnableObject = runnableObject;
	m_peer = NULL;
	m_autoDelete = autoDelete;
	if ( true == autoCreatePeer ) {
#ifdef WIN32
		m_peer = new Win32Thread(this);
#endif
	}
	//start();
}

Thread::~Thread()
{
	m_canContinue = false;
	if ( NULL != m_peer ){
		delete m_peer;
	}

	m_peer = NULL;
}

bool Thread::run()
{
	bool result = false;
	if ( NULL != m_runnableObject ){
		result = m_runnableObject->run();
	}
	else {

	}
	return result;	
}

void Thread::pause()
{
	if ( NULL == this->m_peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};
	m_canContinue = false;
	m_peer->pause();
}

bool Thread::start()
{
	return m_peer->start();
}

void Thread::sleep( uint32 milliseconds )
{
	if ( NULL == this->m_peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};
	m_peer->sleep( milliseconds );
}

uint32 Thread::getThreadID()
{
	if ( NULL == this->m_peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};
	return m_peer->getThreadID();
}

uint32 Thread::getOwningProcessID()
{
	if ( NULL == this->m_peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};
	return m_peer->getOwningProcessID();
}


//////////////////////////ThreadLooper///////////////////////////////////////

ThreadLooper::ThreadLooper( Runnable* runnableObject, const bool& autoDelete ):
	Thread( runnableObject, autoDelete, false )
{	
#ifdef WIN32
	m_peer = new Win32Thread(this, true);	
#endif
}

ThreadLooper::~ThreadLooper()
{

}
