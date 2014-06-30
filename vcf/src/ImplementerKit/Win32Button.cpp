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

// Win32Button.cpp

#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"

using namespace VCF;


Win32Button::Win32Button( CommandButton* component ):
	AbstractWin32Component( component )
{	
	this->init();

	this->createParams();
	/*
	if ( true != this->isRegistered() ){
		registerWin32Class( this->getClassName(), m_wndProc  );
	}
	*/
	this->m_commandButton = component;
	Win32ToolKit* toolkit = (Win32ToolKit*)UIToolkit::getDefaultUIToolkit();
	HWND parent = toolkit->getDummyParent();	

	String className = this->getClassName();

	this->m_hwnd = ::CreateWindowEx( this->m_exStyleMask, 
		                             "BUTTON",
									 this->m_windowCaption.c_str(),	
									 this->m_styleMask, 
		                             this->m_bounds.m_left, 
									 this->m_bounds.m_top, 
									 this->m_bounds.getWidth(), 
									 this->m_bounds.getHeight(), 
									 parent, 
									 NULL, 
									 ::GetModuleHandle(NULL), 
									 NULL );


	if ( NULL != this->m_hwnd ){	
		Win32Object::registerWin32Object( this );
		m_oldButtonWndProc = (WNDPROC)::SetWindowLong( this->m_hwnd, GWL_WNDPROC, (LONG)m_wndProc ); 
		this->m_defaultWndProc = m_oldButtonWndProc;
				
	}
	else {
		//throw exception
	}

	this->setCreated( true );
}

Win32Button::~Win32Button()
{

}

void Win32Button::createParams()
{
	this->m_styleMask = SIMPLE_VIEW | BS_PUSHBUTTON;
}

Image* Win32Button::getImage()
{
	return NULL;
}


void Win32Button::setImage( Image* image )
{

}

void Win32Button::getState()
{

}

void Win32Button::setState()
{

}

LRESULT Win32Button::handleEventMessages( UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultWndProc )
{
	LRESULT result = 0;

	switch ( message ) {
		case WM_PAINT:{
			result = CallWindowProc( m_oldButtonWndProc, m_hwnd, message, wParam, lParam );
		}
		break;
		
		case BN_CLICKED :{

			m_commandButton->click();

			HWND wnd = (HWND)this->m_peerControl->getParent()->getPeer()->getHandleID();
			WPARAM msgWParam = 0;
			switch ( this->m_commandButton->getCommandType() ){
				case BC_NONE : {
					
				}
				break;

				case BC_OK : {
					msgWParam = MAKEWPARAM(0,IDOK);
				}
				break;

				case BC_CANCEL : {
					msgWParam = MAKEWPARAM(0,IDCANCEL);
				}
				break;

				case BC_YES : {
					msgWParam = MAKEWPARAM(0,IDYES);
				}
				break;

				case BC_NO : {
					msgWParam = MAKEWPARAM(0,IDNO);
				}
				break;

				case BC_MAYBE : {
					
				}
				break;
			}
			PostMessage( wnd, WM_COMMAND, msgWParam, NULL );
		}
		break;

		case WM_COMMAND: {
			result = AbstractWin32Component::handleEventMessages( message, wParam, lParam );
			//result = CallWindowProc( m_oldButtonWndProc, m_hwnd, message, wParam, lParam );
			
		}

		default: {
			result = AbstractWin32Component::handleEventMessages( message, wParam, lParam );
			//result = CallWindowProc( m_oldButtonWndProc, m_hwnd, message, wParam, lParam );
			
		}
	}
	return result;
}
