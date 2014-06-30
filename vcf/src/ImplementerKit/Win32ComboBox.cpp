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

// Win32ComboBox.cpp

#include "ApplicationKit.h"

using namespace VCF;


Win32ComboBox::Win32ComboBox( ComboBoxControl* component ):
	AbstractWin32Component( component )
{
	this->createParams();	

	Win32ToolKit* toolkit = (Win32ToolKit*)UIToolkit::getDefaultUIToolkit();
	HWND parent = toolkit->getDummyParent();	

	String className = this->getClassName();
	
	this->m_hwnd = ::CreateWindowEx( this->m_exStyleMask, 
		                             "COMBOBOX",
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
		m_oldComboBoxWndProc = (WNDPROC)::SetWindowLong( this->m_hwnd, GWL_WNDPROC, (LONG)m_wndProc ); 		
		this->m_defaultWndProc = NULL;
				
	}
	else {
		//throw exception
	}

	this->setCreated( true );
}

Win32ComboBox::~Win32ComboBox()
{

}

void Win32ComboBox::createParams()
{
	this->m_styleMask = SIMPLE_VIEW | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL | CBS_HASSTRINGS;
}

void Win32ComboBox::addItem( ListItem * item )
{
	String caption = item->getCaption();
	char* tmp = new char[caption.size()];
	memset( tmp, 0, caption.size() );
	caption.copy( tmp, caption.size() );
	int index = ::SendMessage( this->m_hwnd, CB_ADDSTRING, 0, (LPARAM)caption.c_str() );
	item->setIndex( index );
	delete [] tmp;
}

void Win32ComboBox::insertItem( const unsigned long& index, ListItem * item )
{

}

LRESULT Win32ComboBox::handleEventMessages( UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultWndProc )
{
	LRESULT result = 0;

	switch ( message ) {
		case WM_PAINT:{
			result = CallWindowProc( m_oldComboBoxWndProc, m_hwnd, message, wParam, lParam );
		}
		break;
		
		case WM_COMMAND: {
			AbstractWin32Component::handleEventMessages( message, wParam, lParam );
			result = CallWindowProc( m_oldComboBoxWndProc, m_hwnd, message, wParam, lParam );
		}
		break;
		default: {
			AbstractWin32Component::handleEventMessages( message, wParam, lParam );
			result = CallWindowProc( m_oldComboBoxWndProc, m_hwnd, message, wParam, lParam );			
		}
	}
	return result;
}

void Win32ComboBox::setBounds( VCF::Rect* rect )
{
	double h = rect->getHeight() * 4;
	::SetWindowPos( this->m_hwnd, NULL, rect->m_left, rect->m_top, 
                    rect->getWidth(), h, SWP_NOACTIVATE );
}