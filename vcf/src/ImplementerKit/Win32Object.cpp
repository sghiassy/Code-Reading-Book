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

// Win32Object.cpp

#include "ApplicationKit.h"
#include "Win32Object.h"
#include "Win32Peer.h"
#include <algorithm>

using namespace VCF;

std::vector< std::string > Win32Object::m_registeredWindowClasses;

std::map<HWND, Win32Object*> Win32Object::m_windowMap;


LRESULT CALLBACK Win32Object_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){

	Win32Object* win32Obj = Win32Object::getWin32ObjectFromHWND( hWnd );

	if ( NULL != win32Obj ){
		return  win32Obj->handleEventMessages( message, wParam, lParam );
	}
	else {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


Win32Object::Win32Object()
{	
	this->init();
	this->m_wndProc = Win32Object_WndProc;
	this->m_defaultWndProc = ::DefWindowProc;	
}

Win32Object::~Win32Object()
{
	
}

void Win32Object::destroyWindowHandle()
{
	std::map<HWND, Win32Object*>::iterator found =  Win32Object::m_windowMap.find( this->m_hwnd ); 		
	if ( found != Win32Object::m_windowMap.end() ){
		Win32Object::m_windowMap.erase( found );
	}	
}


void Win32Object::addRegisterWin32Class( const String& className )
{
	Win32Object::m_registeredWindowClasses.push_back( className );
}

void Win32Object::registerWin32Class( const String& className, WNDPROC wndProc )
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)wndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= (HINSTANCE)::GetModuleHandle(NULL);
	wcex.hIcon			= LoadIcon( NULL, IDI_APPLICATION );
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= className.c_str();
	wcex.hIconSm		= NULL;

	if ( 0 != RegisterClassEx(&wcex) ){
		addRegisterWin32Class( className );
	}
}

bool Win32Object::isRegistered()
{
	bool result = false;

	std::vector< std::string >::iterator it = std::find( Win32Object::m_registeredWindowClasses.begin(),
													     Win32Object::m_registeredWindowClasses.end(),
					                					 this->getClassName() );

	result = ( it != Win32Object::m_registeredWindowClasses.end() );
	return result;
}

void Win32Object::init()
{		
	this->m_exStyleMask = 0;
	this->m_styleMask = SIMPLE_VIEW;
	
	this->m_hwnd = NULL;
	
	m_wndProc = NULL;
	m_defaultWndProc = NULL;
	m_windowCaption = "";

	m_created = false;

	m_peerControl = NULL;
}

void Win32Object::createParams()
{
	this->m_exStyleMask = 0;
	this->m_styleMask = SIMPLE_VIEW;
}

LRESULT Win32Object::handleEventMessages( UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultWndProc )
{
	return ::DefWindowProc( this->m_hwnd, message, wParam, lParam);	
}

Win32Object* Win32Object::getWin32ObjectFromHWND( HWND hwnd )
{
	Win32Object* result = NULL;
	std::map<HWND,Win32Object*>::iterator found = 
		Win32Object::m_windowMap.find( hwnd );
	if ( found != Win32Object::m_windowMap.end() ){
		result = found->second;
	}
	return 	result;
}

HWND Win32Object::getHwnd()
{
	return this->m_hwnd;
}

void Win32Object::registerWin32Object( Win32Object* wndObj )
{
	Win32Object::m_windowMap[ wndObj->getHwnd() ] = wndObj;
	::PostMessage( wndObj->getHwnd(), VCF_CONTROL_CREATE, 0, 0 );
}

LRESULT Win32Object::defaultWndProcedure( UINT message, WPARAM wParam, LPARAM lParam )
{
	LRESULT result = 0;
	if ( NULL != m_defaultWndProc ){
		result = CallWindowProc( m_defaultWndProc, m_hwnd, message, wParam, lParam );				
	}

	return result;
}

void Win32Object::setCreated( const bool& creationComplete )
{
	m_created = creationComplete;
}

bool Win32Object::isCreated()
{
	return m_created;
}

void Win32Object::setPeerControl( Control* control )
{
	this->m_peerControl = control;	
}

Control* Win32Object::getPeerControl()
{
	return m_peerControl;
}