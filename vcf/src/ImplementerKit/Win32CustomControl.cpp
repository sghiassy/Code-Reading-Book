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

// Win32CustomControl.cpp: 

#include "ApplicationKit.h"
#include "Win32CustomControl.h"


using namespace VCF;


bool Win32CustomControl::m_windowClassRegistered = false;


LRESULT CALLBACK Win32CustomControl_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

Win32CustomControl::Win32CustomControl( Control* component )
{
	this->registerWndClass();
}

Win32CustomControl::~Win32CustomControl()
{

}

void Win32CustomControl::registerWndClass() 
{
	if ( false == Win32CustomControl::m_windowClassRegistered ){
		WNDCLASSEX wcex;
		
		wcex.cbSize = sizeof(WNDCLASSEX); 
		
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= (WNDPROC)Win32CustomControl_WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= 0;//?????
		wcex.hIcon			= NULL;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= this->getClassName().c_str();
		wcex.hIconSm		= NULL;
		
		if ( 0 != RegisterClassEx(&wcex) ){
			Win32CustomControl::m_windowClassRegistered = true;
		}
	}
}

long Win32CustomControl::getHandleID()
{
	long result = 0;
	result = (long)this->m_wndHandle;
	return result;
}

String Win32CustomControl::getText()
{
	String result = "";

	return result;
}

void Win32CustomControl::setText( const String& text )
{

}

void Win32CustomControl::setBounds( Rect* rect )
{
	::SetWindowPos( this->m_wndHandle, NULL, 
		            rect->m_left, rect->m_top, 
		            rect->getWidth(), rect->getHeight(), 
					SWP_NOACTIVATE );
}

Rect* Win32CustomControl::getBounds()
{
	Rect* result = NULL;
	RECT r = {0};
	::GetWindowRect( this->m_wndHandle, &r );	

	HWND parentHandle = ::GetParent( this->m_wndHandle );

	if ( NULL != parentHandle ){ 		
		POINT pt = {0};
		pt.x = r.left;
		pt.y = r.top;
		ScreenToClient( parentHandle, &pt );
		r.left = pt.x;
		r.top = pt.y;

		pt.x = r.right;
		pt.y = r.bottom;
		ScreenToClient( parentHandle, &pt );
		r.right = pt.x;
		r.bottom = pt.y;

		m_bounds.setRect( r.left, r.top, r.right, r.bottom );
		result = &m_bounds;
	}
	return result;
}

void Win32CustomControl::setVisible( const bool& visible )
{
	if ( true == visible ){
		::ShowWindow( this->m_wndHandle, SW_SHOW );
	}
	else{
		::ShowWindow( this->m_wndHandle, SW_HIDE );
	}
}

bool Win32CustomControl::getVisible()
{
	bool result = false;
	DWORD style = GetWindowLong( this->m_wndHandle, GWL_STYLE );
	result =  (style & WS_VISIBLE ) != 0;
	return result;
}

unsigned long Win32CustomControl::getStyleMask()
{
	unsigned long result = 0;
	result = GetWindowLong( this->m_wndHandle, GWL_STYLE );
	return result;
}

void Win32CustomControl::setStyleMask( const unsigned long& styleMask )
{

}

Control* Win32CustomControl::getControl()
{
	return m_component;
}

void Win32CustomControl::setControl( Control* component )
{
	this->m_component = component;
}

void Win32CustomControl::getCursor()
{

}

void Win32CustomControl::setCursor()
{

}

void Win32CustomControl::setParent( Control* parent )
{
	ControlPeer* Peer = parent->getPeer();
	HWND parentHandle = (HWND)Peer->getHandleID();
	
	if ( NULL == parentHandle ){
		//throw exception !!!
	}

	::SetParent( this->m_wndHandle, parentHandle );
}

Control* Win32CustomControl::getParent()
{
	return NULL;
}

bool Win32CustomControl::isFocused()
{
	HWND focusedHandle = ::GetFocus();
	bool result = (NULL != focusedHandle) && (focusedHandle == this->m_wndHandle);
	return result;
}

void Win32CustomControl::setFocus( const bool& focused )
{
	::SetFocus( this->m_wndHandle );
}

bool Win32CustomControl::isEnabled()
{	
	return (::IsWindowEnabled( this->m_wndHandle ) != 0);
}

void Win32CustomControl::setEnabled( const bool& enabled )
{
	::EnableWindow( this->m_wndHandle, enabled );
}

void Win32CustomControl::repaint( Rect* repaintRect )
{
	if ( NULL == repaintRect ){
		::InvalidateRect( this->m_wndHandle, NULL, TRUE );
	}
	else {
		RECT rect = {0};
		rect.left = repaintRect->m_left;
		rect.top = repaintRect->m_top;
		rect.right = repaintRect->m_right;
		rect.bottom = repaintRect->m_bottom;
		::InvalidateRect( this->m_wndHandle, &rect, TRUE );
	}
}	

void Win32CustomControl::keepMouseEvents()
{
	::SetCapture( this->m_wndHandle );
}

void Win32CustomControl::releaseMouseEvents()
{
	::ReleaseCapture();
}