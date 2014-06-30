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

// Win32Window.cpp
#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"


using namespace VCF;

Win32Window::Win32Window( Control* component ):
	AbstractWin32Component( component )
{
	StringUtils::trace( "start of Create For Win32Window\n" );
	m_internalClose = false;
	//this->createParams();
	
	m_styleMask = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
	
	String className = getClassName();

	if ( true != this->isRegistered() ){		
		if ( className.empty() ) {
			className = "Win32Window::Win32Window";
		}
		registerWin32Class( className, m_wndProc  );
	}

	Win32ToolKit* toolkit = (Win32ToolKit*)UIToolkit::getDefaultUIToolkit();
	HWND parent = NULL;	

	
	
	Application* app = Application::getRunningInstance();
	if ( NULL != app ){
		Window* mainWnd = app->getMainWindow();
		if ( NULL != mainWnd ){
			StringUtils::traceWithArgs( "mainWnd = %x\n", mainWnd );
			parent = (HWND)mainWnd->getPeer()->getHandleID();
		}
	}
	this->m_hwnd = ::CreateWindowEx( this->m_exStyleMask, 
		                             className.c_str(), 
									 this->m_windowCaption.c_str(),	
									 this->m_styleMask, 
		                             0, 
									 0, 
									 0, 
									 0, 
									 parent, 
									 NULL, ::GetModuleHandle(NULL), NULL );

	
	if ( NULL != this->m_hwnd ){
		/*
		StringUtils::traceWithArgs( "m_hwnd = %x\n", m_hwnd );
		if ( IsWindow( m_hwnd ) ) {
			StringUtils::trace( "IsWindow() == true\n" );
		}
		*/
		::ShowWindow( this->m_hwnd, SW_HIDE );
		Win32Object::registerWin32Object( this );		
	}
	else {
		//throw exception
	}

	this->setCreated( true );

	StringUtils::trace( "Created Win32Window\n" );
}

Win32Window::~Win32Window()
{
	StringUtils::traceWithArgs( "Win32Window::~Win32Window() @%p, m_hwnd: %p", this, m_hwnd );	
}

void Win32Window::createParams()
{
	this->m_styleMask = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
	m_styleMask &= ~WS_VISIBLE;
}

Rect* Win32Window::getClientBounds()
{
	RECT r;
	::GetWindowRect( this->m_hwnd, &r );
	::GetClientRect( this->m_hwnd, &r );
	m_clientBounds.setRect(r.left, r.top, r.right, r.bottom);

	return &this->m_clientBounds;
}
void  Win32Window::setClientBounds( Rect* bounds )
{
	long style = ::GetWindowLong( this->m_hwnd, GWL_STYLE );
	long exStyle = ::GetWindowLong( this->m_hwnd, GWL_EXSTYLE );

	if ( (style & WS_CAPTION) != 0 ) {
		int cy = GetSystemMetrics( SM_CYCAPTION );
		if ( (exStyle & WS_EX_TOOLWINDOW) != 0 ) {
			cy = GetSystemMetrics( SM_CYSMCAPTION );			
		}
		else {
			cy = GetSystemMetrics( SM_CYCAPTION );			
		}
		RECT r = {0};
		r.left = bounds->m_left - GetSystemMetrics(SM_CXFRAME);	
		r.top = bounds->m_top - GetSystemMetrics(SM_CYFRAME);
		r.right = bounds->m_right + GetSystemMetrics(SM_CXFRAME);
		r.bottom = bounds->m_bottom + GetSystemMetrics(SM_CYFRAME);
		r.top -= cy;
		::MoveWindow( this->m_hwnd, r.left, r.top,	
			r.right - r.left, r.bottom - r.top, TRUE );
	}
}

LRESULT Win32Window::handleEventMessages( UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultWndProc )
{
	LRESULT result = 0;
#ifdef NDEBUG
	//StringUtils::traceWithArgs( "%p(Win32Window)::handleEventMessages( %#06x, %d, %d, %x )\n",
	//	                         this, message, wParam, lParam, defaultWndProc );
#endif
	switch ( message ) {
		case WM_CLOSE:{
			result = 0;
			//check if we need to re notify the listeners of the close event
			if ( false == m_internalClose ){
				VCF::WindowEvent event( this->getControl(), WINDOW_EVENT_CLOSE );
				
				VCF::Window* window = (VCF::Window*)this->getControl();
				window->fireOnWindowClose( &event );
				
				//check if the main window is clossing - if it is
				//then close the app !

				Application* app = Application::getRunningInstance();
				if ( NULL != app ){
					Window* mainWindow = app->getMainWindow();
					if ( mainWindow == this->getControl() ){
						::PostMessage( this->m_hwnd, WM_QUIT, 0, 0 ); 
					}
					else {
						result = AbstractWin32Component::handleEventMessages( message, wParam, lParam );			
					}
				}
			}
			else {
				result = AbstractWin32Component::handleEventMessages( message, wParam, lParam );
			}
		}
		break;

		case WM_DESTROY: {			
			result = AbstractWin32Component::handleEventMessages( message, wParam, lParam );			
		}
		break;

		default: {
			result = AbstractWin32Component::handleEventMessages( message, wParam, lParam );
		}
		break;
	}
#ifdef NDEBUG
	//StringUtils::traceWithArgs( ">>%p(Win32Window) preparing to return from handleEventMessages( %#06x, %d, %d, %x )<<\n",
	//	                         this, message, wParam, lParam, defaultWndProc );
#endif
	return result;
}

void Win32Window::close()
{
	if ( false == m_internalClose ){
		m_internalClose = true;
		Application* app = Application::getRunningInstance();
		if ( NULL != app ){
			Window* mainWindow = app->getMainWindow();
			if ( mainWindow == this->getControl() ){
				::PostMessage( this->m_hwnd, WM_QUIT, 0, 0 ); 
			}
		}		
	}
	
	::PostMessage( this->m_hwnd, WM_CLOSE, 0, 0 ); 	
}

void Win32Window::setFrameStyle( const FrameStyleType& frameStyle )
{
	long style = ::GetWindowLong( this->m_hwnd, GWL_STYLE );
	long exStyle = ::GetWindowLong( this->m_hwnd, GWL_EXSTYLE );
	switch ( frameStyle ){
		case FST_SIZEABLE :{
			style |= WS_OVERLAPPEDWINDOW;			
			exStyle &= ~WS_EX_TOOLWINDOW;
		}
		break;

		case FST_NOBORDER :{
			style &= ~WS_DLGFRAME;
			style &= ~WS_OVERLAPPEDWINDOW;
			style |= WS_THICKFRAME;		
			exStyle &= ~WS_EX_TOOLWINDOW;
		}
		break;

		case FST_FIXED :{
			style |= WS_OVERLAPPEDWINDOW;
			style &= ~WS_THICKFRAME;
			exStyle &= ~WS_EX_TOOLWINDOW;
		}
		break;

		case FST_NOBORDER_FIXED :{
			style &= ~WS_OVERLAPPEDWINDOW;			
			exStyle &= ~WS_EX_TOOLWINDOW;
		}
		break;		

		case FST_TOOLBAR_BORDER :{
			style &= ~WS_OVERLAPPEDWINDOW;
			style |= WS_POPUPWINDOW | WS_DLGFRAME | WS_THICKFRAME | WS_OVERLAPPED | 0x0003B00;

			exStyle |= WS_EX_TOOLWINDOW;			
		}
		break;

		case FST_TOOLBAR_BORDER_FIXED :{
			exStyle |= WS_EX_TOOLWINDOW;
			style &= ~WS_THICKFRAME;			
		}
		break;
		
	}

	::SetWindowLong( this->m_hwnd, GWL_STYLE, style );
	::SetWindowLong( this->m_hwnd, GWL_EXSTYLE, exStyle );
	
	Rect* r = this->m_peerControl->getBounds();
	::SetWindowPos( this->m_hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );

	::UpdateWindow( this->m_hwnd );
	
	m_peerControl->setBounds( r );

}

void Win32Window::setFrameTopmost( const bool& isTopmost ) 
{
	if ( true == isTopmost ){
		::SetWindowPos( this->m_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	}
	else{
		::SetWindowPos( this->m_hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER );
	}
}

void Win32Window::setParent( VCF::Control* parent )
{
	DWORD style = ::GetWindowLong( this->m_hwnd, GWL_STYLE );
	if ( NULL == parent ) {		
		style &= ~WS_CHILD;
		style |= WS_POPUP;
		::SetParent( this->m_hwnd, NULL );
	}
	else {
		style &= ~WS_POPUP;
		style |= WS_CHILD;
		VCF::ControlPeer* peer = parent->getPeer();
		HWND wndParent = (HWND)peer->getHandleID();		
		::SetParent( this->m_hwnd, wndParent );
	}

	::SetWindowLong( this->m_hwnd, GWL_STYLE, style );	
	::SetWindowPos( this->m_hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );	
}