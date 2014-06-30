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

// Win32Application.cpp
#include "ApplicationKit.h"
#include "Win32Application.h"
#include "Win32ResourceBundle.h"
#include "LibraryApplication.h"

using namespace VCF;

Win32Application::Win32Application()
{
	m_resBundle = NULL;
	m_resBundle = new Win32ResourceBundle();
	m_resBundle->setApplicationPeer( this );
	m_instanceHandle = NULL;
}

Win32Application::~Win32Application()
{
	delete m_resBundle;
	m_resBundle = NULL;
}

bool Win32Application::initApp( const std::vector<VCF::String>& appCmdLine )
{
	/**
	*set the HandleID if it is still NULL
	*Dy default it should be set in the DLLMain() 
	*function if this is a LibraryApplication
	*/
	if ( NULL == m_instanceHandle ) {
		m_instanceHandle = ::GetModuleHandle( NULL );
	}

	INITCOMMONCONTROLSEX controlsToInit = {0};
	controlsToInit.dwSize = sizeof(controlsToInit);
	controlsToInit.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx( &controlsToInit );
	
	HRESULT hr = OleInitialize(NULL);
	if ( !(SUCCEEDED(hr)) ){
		throw BasicException("OleInitialize failed");
	}
	
	return true;
}

void Win32Application::terminateApp()
{
	OleFlushClipboard();
	OleUninitialize();	
}

VCF::AbstractApplication* Win32Application::getApplication()
{
	return this->m_app;
}

void Win32Application::setApplication( VCF::AbstractApplication* application )
{
	this->m_app = application;
}

ResourceBundle* Win32Application::getResourceBundle()
{
	return m_resBundle;
}	

String Win32Application::getFileName()
{
	VCFChar fileName[MAX_PATH];
	memset( fileName, 0, MAX_PATH );
	::GetModuleFileName( m_instanceHandle, fileName, MAX_PATH );

	return String( fileName );
}

void Win32Application::run()
{
	MSG msg = {0};
	HACCEL hAccelTable = NULL;

	bool isIdle = true;
	while (true) {
		// phase1: check to see if we can do idle work
		while ((true == isIdle) && (FALSE == ::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE)) ) {
			
			m_app->idleTime();

			//check library apps;
			Enumerator<LibraryApplication*>* registeredLibs = LibraryApplication::getRegisteredLibraries();
			while ( true == registeredLibs->hasMoreElements() ) {
				LibraryApplication* libraryApp = registeredLibs->nextElement();
				libraryApp->idleTime();
			}
			isIdle = false;				
		}

		do	{
			//dispatch message, but quit on WM_QUIT			
			if ( GetMessage( &msg, NULL, 0, 0 ) ) {
				if (!TranslateAccelerator( msg.hwnd, hAccelTable, &msg ) ) {
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
			}
			else {
				return; //we're outta here - nothing more to do 
			}			

			// WM_PAINT and WM_SYSTIMER (caret blink)
			bool isIdleMessage = ( (msg.message != WM_PAINT) && (msg.message != 0x0118) );
			if ( true == isIdleMessage ) {
				if ( (msg.message == WM_MOUSEMOVE) || (msg.message == WM_NCMOUSEMOVE) ) {
					//check the prev mouse pt;
				}
			}

			if ( true == isIdleMessage ) {
				isIdle = true;
			}

		} 
		while ( FALSE != ::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE) );
	}
}

String Win32Application::getCurrentDirectory()
{
	String result = "";
	VCFChar	dirPath[MAX_PATH+1];
	memset( dirPath, 0, MAX_PATH+1 );
	if ( 0 != ::GetCurrentDirectory( MAX_PATH, dirPath ) ){
		result = dirPath;
	}
	return result;
}

void Win32Application::setCurrentDirectory( const String& currentDirectory )
{
	::SetCurrentDirectory( currentDirectory.c_str() );
}

long Win32Application::getHandleID()
{
	return (long)m_instanceHandle;
}

void Win32Application::setHandleID( const long& handleID )
{
	m_instanceHandle = (HINSTANCE)handleID;
}