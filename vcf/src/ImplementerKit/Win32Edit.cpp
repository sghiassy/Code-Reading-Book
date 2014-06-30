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

// Win32Edit.cpp
#include "ApplicationKit.h"
#include <richedit.h>
#include "TextControl.h"
#include "AppKitPeerConfig.h"

using namespace VCFWin32;
using namespace VCF;

static bool Win32RicheditLibraryLoaded = false;

Win32Edit::Win32Edit( TextControl* component, const bool& isMultiLineControl ):
	AbstractWin32Component( component ),
	m_oldEditWndProc(NULL)
{
	m_numCharsRemainingToStreamIn = 0;
	m_OKToResetControlText = true;
	m_backgroundBrush = NULL;
	m_textControl = component;
	if ( NULL == m_textControl ){
		//throw exception !!!!!!
	}
	this->createParams();

	Win32ToolKit* toolkit = (Win32ToolKit*)UIToolkit::getDefaultUIToolkit();
	HWND parent = toolkit->getDummyParent();	

	String className = this->getClassName();
	if ( false == Win32RicheditLibraryLoaded ) {
		if  ( NULL != LoadLibrary("RichEd32.Dll") ) {
			Win32RicheditLibraryLoaded = true;
		}
	}

	DWORD style = m_styleMask | ES_AUTOHSCROLL | ES_SAVESEL;
	if ( true == isMultiLineControl ) {
		style |= ES_MULTILINE | WS_HSCROLL | WS_VSCROLL | ES_WANTRETURN;
	}
	this->m_hwnd = ::CreateWindowEx( this->m_exStyleMask, 
		                             RICHEDIT_CLASS, 
									 NULL,	
									 style, 
		                             this->m_bounds.m_left, 
									 this->m_bounds.m_top, 
									 this->m_bounds.getWidth(), 
									 CW_USEDEFAULT, 
									 parent, 
									 NULL, 
									 ::GetModuleHandle(NULL), 
									 NULL );


	if ( NULL != this->m_hwnd ){
		
		Win32Object::registerWin32Object( this );
		m_oldEditWndProc = (WNDPROC)::SetWindowLong( this->m_hwnd, GWL_WNDPROC, (LONG)m_wndProc ); 
		this->m_defaultWndProc = NULL;//m_oldEditWndProc;
		
		TextModelEventHandler<Win32Edit>* tml = 
			new TextModelEventHandler<Win32Edit>( this, Win32Edit::onTextModelTextChanged, "Win32TextModelHandler" );
		
		
		TextModel* tm = m_textControl->getTextModel();
		tm->addTextModelChangedHandler( tml );

	}
	else {
		//throw exception
	}

	this->setCreated( true );
}

Win32Edit::~Win32Edit()
{
	if ( NULL != m_backgroundBrush ) {
		DeleteObject( m_backgroundBrush );
	}
}

void Win32Edit::setRightMargin( const double & rightMargin )
{
	::SendMessage( this->m_hwnd, EM_SETMARGINS, EC_RIGHTMARGIN, (long)rightMargin );
}

void Win32Edit::setLeftMargin( const double & leftMargin )
{
	::SendMessage( this->m_hwnd, EM_SETMARGINS, EC_LEFTMARGIN, (long)leftMargin );
}

unsigned long Win32Edit::getLineCount()
{
	return ::SendMessage( this->m_hwnd, EM_GETLINECOUNT, 0, 0 );
}

void Win32Edit::getCurrentLinePosition()
{

}

double Win32Edit::getLeftMargin()
{
	double result = 0.0;
	
	DWORD margin = ::SendMessage( this->m_hwnd, EM_GETMARGINS, 0, 0 ); 
	result = LOWORD(margin);
	return result;
}

double Win32Edit::getRightMargin()
{
	double result = 0.0;
	
	DWORD margin = ::SendMessage( this->m_hwnd, EM_GETMARGINS, 0, 0 ); 
	result = HIWORD(margin);
	return result;
}

VCF::Point* Win32Edit::getPositionFromCharIndex( const unsigned long& index )
{
	DWORD pos = ::SendMessage( this->m_hwnd, EM_POSFROMCHAR, index, 0 );
	m_posAtChar.m_x = LOWORD(pos);
	m_posAtChar.m_y = HIWORD(pos);
	return &m_posAtChar;
}

unsigned long Win32Edit::getCharIndexFromPosition( VCF::Point* point )
{
	DWORD pos = 0;
	
	pos = MAKEWORD( (long)point->m_x, (long)point->m_y );

	unsigned long result = ::SendMessage( this->m_hwnd, EM_CHARFROMPOS, 0, pos );

	return result;
}

unsigned long Win32Edit::getCaretPosition()
{
	unsigned long result = 0;
	DWORD pos = 0;
	::SendMessage( this->m_hwnd, EM_GETSEL, (LPARAM)&pos, 0 );
	result = pos;
	return result;
}

void Win32Edit::createParams()
{
	this->m_exStyleMask = WS_EX_CLIENTEDGE;
	this->m_styleMask = SIMPLE_VIEW;
}

void Win32Edit::processTextEvent( VCFWin32::KeyboardData keyData, WPARAM wParam, LPARAM lParam )
{
	VCF::TextModel* model = this->m_textControl->getTextModel();
	if ( NULL != model ){
		//this is the braindead way - needs to be reworked in the future
		int length = ::GetWindowTextLength( this->m_hwnd );
		VCFChar* text = new VCFChar[length+1];
		memset( text, 0, length+1);
		::GetWindowText( this->m_hwnd, text, length+1 );  
		VCF::String newText = text;
		
		m_OKToResetControlText = false;
		
		model->setText( newText );

		m_OKToResetControlText = true;

		delete[] text;

		/**
		*for the future we need to be able to insertText and deleteText from the
		*model according to the characters pressed. At first glance this is that
		*big of a deal: determine if we have a selection, delete the selected
		*text, and then insert the appropriate character. The problem comes in 
		*determing what causes a delete and what to insert. Normally the only things
		*that should cause an delete/insert are the set of characters [a..z,A..Z,0..9], 
		*back space, space, and delete. This is a valid assumption assuming en/US language
		*but for other languages this totally falls down...
		*/

		/*
		//figure out if anything is selected
		int selStart = 0;
		int selEnd = 0;
		::SendMessage( this->m_hwnd, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd );
		int delta = abs( selStart - selEnd );
		if ( 0 != delta ){
			//text is being deleted

			if ( (keyData.VKeyCode != VK_BACK) || (keyData.VKeyCode == VK_BACK) ){
				
			}		
		}

		
		unsigned long index = this->m_peerControl->getCaretPosition(); 
		VCF::String text;
		char ch[2] = {0};
		ch[0] = (VCF::VCFChar)keyData.character;
		text += ch;
		model->insertText( index, text );
		*/
	}
}

LRESULT Win32Edit::handleEventMessages( UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultWndProc)
{
	LRESULT result = 0;

	switch ( message ) {
		case WM_CHAR: case WM_KEYDOWN: case WM_KEYUP:{
			result = CallWindowProc( this->m_oldEditWndProc, m_hwnd, message, wParam, lParam );

			KeyboardData keyData = Win32Utils::translateKeyData( this->m_hwnd, lParam );
			unsigned long eventType = 0;
			switch ( message ){
				case WM_CHAR:{
					eventType = KEYBOARD_EVENT_DOWN;									
				}
				break;

				case WM_KEYDOWN:{
					eventType = KEYBOARD_EVENT_PRESSED;
				}
				break;

				case WM_KEYUP:{
					eventType = KEYBOARD_EVENT_UP;
				}
				break;
			}
			
			unsigned long keyMask = Win32Utils::translateKeyMask( keyData.keyMask );
			ulong32 virtKeyCode = Win32Utils::translateVKCode( keyData.VKeyCode );
			VCF::KeyboardEvent event( this->m_peerControl, eventType, keyData.repeatCount, 
				                 keyMask, (VCF::VCFChar)keyData.character,
								 virtKeyCode );

			processTextEvent( keyData, wParam, lParam );

			this->m_peerControl->processKeyboardEvent( &event );									
		}
		break;

		case WM_PAINT:{
			//result = (m_oldEditWndProc)(m_hwnd, message, wParam, lParam );
			result = CallWindowProc( this->m_oldEditWndProc, m_hwnd, message, wParam, lParam );
		}
		break;

		case WM_CTLCOLOREDIT : {
			HDC hdcEdit = (HDC) wParam;   // handle to display context 
			HWND hwndEdit = (HWND) lParam; // handle to static control 
			if ( NULL != m_backgroundBrush ) {
				DeleteObject( m_backgroundBrush );
			}
			Color* color = m_peerControl->getColor();
			
			COLORREF backColor = RGB(color->getRed() * 255.0,
									color->getGreen() * 255.0,
									color->getBlue() * 255.0 );

			m_backgroundBrush = CreateSolidBrush( backColor );
			SetBkColor( hdcEdit, backColor );
			result = (LRESULT)m_backgroundBrush;
		}
		break;

		default: {
			AbstractWin32Component::handleEventMessages( message, wParam, lParam );
			result = CallWindowProc( this->m_oldEditWndProc, m_hwnd, message, wParam, lParam );
		}
		break;
	}
	return result;
}

void Win32Edit::onTextModelTextChanged( TextEvent* event )
{
	if ( NULL != event ){ 
		String text = event->getChangeText();
		if ( true == m_OKToResetControlText ){
			this->setText( text );
		}
	}
}

DWORD CALLBACK Win32Edit::EditStreamCallback( DWORD dwCookie, // application-defined value
												LPBYTE pbBuff,  // pointer to a buffer
												LONG cb,        // number of bytes to read or write
												LONG *pcb       // pointer to number of bytes transferred
												)
{	
	StringUtils::traceWithArgs( "Win32Edit::EditStreamCallback cb: %d\n", cb );
	Win32Edit* thisPtr = (Win32Edit*)dwCookie;
	*pcb = thisPtr->m_numCharsRemainingToStreamIn;
	if ( *pcb > 0 ) {
		TextControl* tc = (TextControl*)thisPtr->getControl();
		String text = tc->getTextModel()->getText();
		memset( pbBuff, 0, *pcb );
		text.copy( (VCFChar*)pbBuff, *pcb );
		thisPtr->m_numCharsRemainingToStreamIn -= text.size();
	}
	
	return (*pcb>0 ? NOERROR : E_FAIL);
}
 

void Win32Edit::setText( const VCF::String& text )
{
	AbstractWin32Component::setText( text );
	/*
	EDITSTREAM editStream = {0};
	editStream.dwCookie = (DWORD)this;
	editStream.pfnCallback = Win32Edit::EditStreamCallback;
	m_numCharsRemainingToStreamIn = text.size();
	
	::SendMessage( m_hwnd, EM_STREAMIN, SF_TEXT, (LPARAM)&editStream );
	*/
}

