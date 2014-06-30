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
#include "Win32ProcessPeer.h"


using namespace VCF;

Win32ProcessPeer::Win32ProcessPeer()
{
	memset( &m_win32ProcessInfo, 0, sizeof( m_win32ProcessInfo ) );
	m_win32ProcessInfo.dwProcessId = ::GetCurrentProcessId();
	m_win32ProcessInfo.dwThreadId = ::GetCurrentThreadId();
	m_win32ProcessInfo.hProcess = ::GetCurrentProcess();
	m_win32ProcessInfo.hThread = ::GetCurrentThread();
	
	TCHAR moduleFileName[MAX_PATH];
	memset( moduleFileName, 0, MAX_PATH );
	//retreive the current running file name
	::GetModuleFileName( NULL, moduleFileName, MAX_PATH );
	m_processFileName = moduleFileName;
}

Win32ProcessPeer::~Win32ProcessPeer()
{

}

int Win32ProcessPeer::getProcessID()
{
	return m_win32ProcessInfo.dwProcessId;
}

int Win32ProcessPeer::getProcessThreadID() 
{
	return m_win32ProcessInfo.dwThreadId;
}

bool Win32ProcessPeer::createProcess( const String& processName )
{
	bool result = false;
	m_processFileName = "";

	String commandLine = "";
	BOOL bInheritHandles = FALSE;
	DWORD dwCreationFlags = 0;
	STARTUPINFO si = {0};
	si.cb = sizeof(STARTUPINFO);
	memset( &m_win32ProcessInfo, 0, sizeof( m_win32ProcessInfo ) );

	result = (TRUE == ::CreateProcess( processName.c_str(), NULL, NULL, NULL, 
										bInheritHandles, dwCreationFlags,
										NULL, NULL, &si, &m_win32ProcessInfo ) );
	
	if ( true == result ) {
		m_processFileName = processName;
	}
	return result;
}

String Win32ProcessPeer::getProcessFileName()
{
	return m_processFileName;
}