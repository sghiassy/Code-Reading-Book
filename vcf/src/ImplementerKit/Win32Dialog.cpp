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
#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"
#include "LibraryApplication.h"

using namespace VCF;


Win32Dialog::Win32Dialog():
	AbstractWin32Component( NULL )
{	
	m_owner = NULL;
	m_dialogComponent = NULL;
	m_isModal = false;
}


Win32Dialog::Win32Dialog( Frame* owner, Dialog* component ):
	AbstractWin32Component( component )
{	
	m_isModal = false;
	m_owner = owner;		
	m_dialogComponent = component;
	String className = this->getClassName();
	if ( className.empty() ) {
		className = "VCF::Win32Dialog";
	}
	if ( true != this->isRegistered() ){
		registerWin32Class( className, m_wndProc  );
	}
	this->init();

	HWND parent = NULL;
	if ( NULL == m_owner ){
		Application* app = Application::getRunningInstance();
		if ( NULL != app ){
			Window* mainWnd = app->getMainWindow();
			if ( NULL != mainWnd ){
				parent = (HWND)mainWnd->getPeer()->getHandleID();
			}
		}
		else {
			parent = ::GetDesktopWindow();
		}
	}
	else {
		parent = (HWND)this->m_owner->getPeer()->getHandleID();
	}
	
	String caption = this->m_dialogComponent->getCaption();
	this->m_hwnd = ::CreateWindowEx( this->m_exStyleMask, 
		                             className.c_str(), 
									 caption.c_str(),	
									 this->m_styleMask, 
		                             0,//this->m_bounds.m_left, 
									 0,//this->m_bounds.m_top, 
									 0,//this->m_bounds.getWidth(), 
									 0,//this->m_bounds.getHeight(), 
									 parent, 
									 NULL, 
									 ::GetModuleHandle(NULL), 
									 NULL );

	if ( NULL != this->m_hwnd ){
		Win32Object::registerWin32Object( this );
		HMENU sysMenu = ::GetSystemMenu( m_hwnd, FALSE );
		if ( NULL != sysMenu ) 	{
			::RemoveMenu ( sysMenu, SC_MAXIMIZE, MF_BYCOMMAND );
			::RemoveMenu ( sysMenu, SC_MINIMIZE, MF_BYCOMMAND );
			::RemoveMenu ( sysMenu, SC_RESTORE, MF_BYCOMMAND );
			::RemoveMenu ( sysMenu, SC_SIZE, MF_BYCOMMAND );
			
		}
		::ShowWindow( this->m_hwnd, SW_HIDE );
	}
	this->setCreated( true );
}


Win32Dialog::~Win32Dialog()
{

}

void Win32Dialog::init()
{	
	this->createParams();
	this->m_bounds.setRect( 20, 20, 220, 220 );
}

void Win32Dialog::createParams()
{		
	this->m_styleMask = WS_POPUPWINDOW | WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_DLGFRAME | DS_MODALFRAME | DS_3DLOOK;
	this->m_exStyleMask = WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME | WS_EX_CONTEXTHELP;

	//m_styleMask &= ~WS_MINIMIZEBOX;
	//m_styleMask &= ~WS_MAXIMIZEBOX;
}

Rect* Win32Dialog::getClientBounds()
{
	RECT r;
	::GetClientRect( this->m_hwnd, &r );	
	
	m_clientBounds.setRect(r.left, r.top, r.right, r.bottom );

	return &this->m_clientBounds;
}

void  Win32Dialog::setClientBounds( Rect* bounds )
{

}

LRESULT Win32Dialog::handleEventMessages( UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultWndProc )
{
	LRESULT result = 0;

	switch ( message ) {		

		case WM_CLOSE:{
			//VCF::WindowEvent event( this->getComponent(), WINDOW_EVENT_CLOSE );

			//VCF::Window* window = (VCF::Window*)this->getComponent();
			//window->notify_onWindowClose( &event );
			result = AbstractWin32Component::handleEventMessages( message, wParam, lParam );
		}
		break;

		case WM_DESTROY:{
			destroyWindowHandle();
			if ( true == m_isModal ) {
				PostQuitMessage(0);
			}
		}
		break;

		default: result = AbstractWin32Component::handleEventMessages( message, wParam, lParam );
	}
	return result;
}


ModalReturnType Win32Dialog::showModal()
{
	m_isModal = true;
	ModalReturnType result = MRT_NONE;		

	
	Control* peerControl = this->getPeerControl();	
	if ( NULL != this->m_hwnd ){		
		HWND parent = NULL;
		
		if ( NULL == m_owner ){
			Control* parentControl = peerControl->getParent();
			if ( NULL != parentControl ){
				parent = (HWND)parentControl->getPeer()->getHandleID();
			}
			else{
				Application* app = Application::getRunningInstance();
				if ( NULL != app ) {
					parent = (HWND)app->getMainWindow()->getPeer()->getHandleID();				
				}				
			}
		}
		else {
			parent = (HWND)this->m_owner->getPeer()->getHandleID();
		}

		
		if ( (NULL != parent) && (TRUE == ::IsWindowEnabled(parent)) ) {
			::EnableWindow(parent, FALSE);	
		}

		int err = EnableWindow( m_hwnd, TRUE );

		MSG msg = {0};
		bool idle = true;
		bool done = false;
		while (false == done) {

			while ( (true == idle) && (FALSE ==::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE)) ) {
				err = ::ShowWindow( m_hwnd, SW_SHOWNORMAL );
				err = ::UpdateWindow( m_hwnd );
				idle = false;
				Application* app = Application::getRunningInstance();
				if ( NULL != app ) {
					app->idleTime();
				}
				
				//check library apps;
				Enumerator<LibraryApplication*>* registeredLibs = LibraryApplication::getRegisteredLibraries();
				while ( true == registeredLibs->hasMoreElements() ) {
					LibraryApplication* libraryApp = registeredLibs->nextElement();
					libraryApp->idleTime();
				}
			}
			
			// phase2: pump messages while available
			do	{		
				
				if ( GetMessage(  &msg, NULL, 0, 0 ) ) {					
					TranslateMessage( &msg );
					DispatchMessage( &msg );					
				}
				else {
					done = true;
					break;
				}				
				
				// show the window when certain special messages rec'd
				if ((msg.message == 0x118) || (msg.message == WM_SYSKEYDOWN) ) {
					err = ::ShowWindow( m_hwnd, SW_SHOWNORMAL );
					err = ::UpdateWindow( m_hwnd );					
				}				
				
				switch ( msg.message ){
				
					case WM_SYSCOMMAND: {
						if ( SC_CLOSE == msg.wParam ){
							result = MRT_CANCEL;
						}
					}
					break;

					case WM_COMMAND: {
						UINT wID = HIWORD(msg.wParam);
						switch ( wID ){
							case IDCANCEL: {
								result = MRT_CANCEL;
							}
							break;

							case IDOK: {
								result = MRT_OK;
							}
							break;

							case IDYES: {
								result = MRT_YES;
							}
							break;

							case IDNO: {
								result = MRT_NO;
							}
							break;
						}
						
						if ( (wID == IDCANCEL) || (wID == IDOK) || (wID == IDYES) || (wID == IDNO) ){
							PostMessage( msg.hwnd, WM_CLOSE, 0, 0 );
						}
					}
					break;

					case WM_QUIT:{
						done = true;	
					}
					break;
				}
				
				// WM_PAINT and WM_SYSTIMER (caret blink)
				bool isIdleMessage = ( (msg.message != WM_PAINT) && (msg.message != 0x0118) );
				if ( true == isIdleMessage ) {
					if ( (msg.message == WM_MOUSEMOVE) || (msg.message == WM_NCMOUSEMOVE) ) {
						//check the prev mouse pt;
					}
				}
				
				if ( true == isIdleMessage ) {
					idle = true;
					//lIdleCount = 0;
				}
				
			} while (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE));
		} //outer for loop

		
		DestroyWindow( m_hwnd );

		if ( (NULL != parent) && (FALSE == ::IsWindowEnabled(parent)) ) {
			::EnableWindow(parent, TRUE);
			::SetActiveWindow(parent);
		}

	}
	else {
		//throw exception
	}
	
	m_isModal = false;

	return result;
}

void Win32Dialog::show()
{
	EnableWindow( this->m_hwnd, TRUE );
	::ShowWindow( this->m_hwnd, SW_SHOWNORMAL );
	::UpdateWindow( this->m_hwnd );
}

void Win32Dialog::setCaption( const String& caption )
{
	SetWindowText( this->m_hwnd, caption.c_str() );
}

void Win32Dialog::showMessage( const String& message, const String& caption )
{
	HWND activeWnd = GetActiveWindow();
	
	String tmp = caption;
	
	if ( tmp == "" ){
		TCHAR modFilename[MAX_PATH];
		memset(modFilename, 0, MAX_PATH );
		GetModuleFileName( GetModuleHandle(NULL), modFilename, MAX_PATH );
		tmp = modFilename;
		tmp = StringUtils::getFileName( tmp );
	}

	MessageBox( activeWnd, message.c_str(), tmp.c_str(), MB_OK );
}

ModalReturnType Win32Dialog::showMessage( const String& message, const String& caption, const MessageStyle& messageStyle )
{
	ModalReturnType result = MRT_NONE;
	
	UINT mbStyle = 0;
	switch ( messageStyle ){
		case MS_DEFAULT: {
			mbStyle = MB_OK;	
		}
		break;

		case MS_ERROR: {
			mbStyle = MB_OK | MB_ICONERROR;	
		}
		break;

		case MS_INFO: {
			mbStyle = MB_OK | MB_ICONINFORMATION;	
		}
		break;

		case MS_WARNING: {
			mbStyle = MB_OK | MB_ICONWARNING;	
		}
		break;
	}

	HWND activeWnd = GetActiveWindow();
	
	String tmp = caption;
	
	if ( tmp == "" ){
		TCHAR modFilename[MAX_PATH];
		memset(modFilename, 0, MAX_PATH );
		GetModuleFileName( GetModuleHandle(NULL), modFilename, MAX_PATH );
		tmp = modFilename;
		tmp = StringUtils::getFileName( tmp );
	}

	int returnVal = MessageBox( activeWnd, message.c_str(), tmp.c_str(), mbStyle );

	switch ( returnVal ){
		case IDABORT:{
			
		}
		break;

		case IDCANCEL:{
			result = MRT_CANCEL;
		}
		break;
		/*
		case IDCONTINUE:{
			
		}
		break;
		*/
		case IDIGNORE:{
			
		}
		break;

		case IDNO:{
			result = MRT_NO;
		}
		break;

		case IDOK:{
			result = MRT_OK;
		}
		break;

		case IDRETRY:{
			
		}
		break;
		/*
		case IDTRYAGAIN:{
			
		}
		break;
		*/
		case IDYES:{
			result = MRT_YES;
		}
		break;
	}

	return result;
}

void Win32Dialog::close()
{
	::SendMessage( this->m_hwnd, WM_CLOSE, 0, 0 );
}

void Win32Dialog::setFrameStyle( const FrameStyleType& frameStyle )
{
	long style = ::GetWindowLong( this->m_hwnd, GWL_STYLE );
	switch ( frameStyle ){
		case FST_SIZEABLE :{
			style |= WS_OVERLAPPEDWINDOW;			
		}
		break;

		case FST_NOBORDER :{
			style &= ~WS_DLGFRAME;
			style &= ~WS_OVERLAPPEDWINDOW;
			style |= WS_THICKFRAME;			
		}
		break;

		case FST_FIXED :{
			style |= WS_OVERLAPPEDWINDOW;
			style &= ~WS_THICKFRAME;
		}
		break;

		case FST_NOBORDER_FIXED :{
			style &= ~WS_OVERLAPPEDWINDOW;			
		}
		break;		
	}

	::SetWindowLong( this->m_hwnd, GWL_STYLE, style );
	
	Rect* r = this->m_peerControl->getBounds();
	::SetWindowPos( this->m_hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );

	::UpdateWindow( this->m_hwnd );
	
	m_peerControl->setBounds( r );

}
