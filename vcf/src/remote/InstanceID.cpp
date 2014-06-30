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

#include "InstanceID.h"
#include "NetToolkit.h"
#include "VCFProcess.h"

using namespace VCF;
using namespace VCFRemote;
using namespace VCFNet;

InstanceID::InstanceID()
{
	Process thisProcess;
	m_machineName = NetToolkit::getDefaultNetToolkit()->getLocalMachineName();
	m_processID = thisProcess.getID();
	m_threadID = thisProcess.getThreadID();
	m_memAddress = 0;
}

InstanceID::InstanceID( const int& memAddress )
{
	Process thisProcess;
	m_machineName = NetToolkit::getDefaultNetToolkit()->getLocalMachineName();
	m_processID = thisProcess.getID();
	m_threadID = thisProcess.getThreadID();
	m_memAddress = memAddress;
}

InstanceID::InstanceID( const VCF::String& machineName, const int& processID,
					     const int& threadID, const int& memAddress )
{
	this->m_machineName = machineName;
	m_processID = processID;
	m_threadID = threadID;
	m_memAddress = memAddress;
}

void InstanceID::saveToStream( VCF::OutputStream * stream )
{
	stream->write( m_machineName );
	stream->write( m_processID );
	stream->write( m_threadID );
	stream->write( m_memAddress );
}

void InstanceID::loadFromStream( VCF::InputStream * stream )
{
	stream->read( m_machineName );
	stream->read( m_processID );
	stream->read( m_threadID );
	stream->read( m_memAddress );
}