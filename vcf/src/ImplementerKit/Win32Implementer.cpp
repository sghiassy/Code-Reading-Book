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

//Win32Implementer.cpp

#include "Win32Implementer.h"
#include "CommonDefines.h"
#include <bitset>

using namespace VCFWin32;


unsigned long Win32Utils::translateKeyMask( UINT win32KeyMask )
{	
	unsigned long result = KEY_UNDEFINED;

	if ( (win32KeyMask & MK_CONTROL) != 0 ){
		result |= KEY_CTRL;
	}

	if ( (win32KeyMask & MK_SHIFT) != 0 ){
		result |= KEY_SHIFT;
	}
	
	if ( ::GetAsyncKeyState( VK_MENU ) < 0 ) {
		result |= KEY_ALT;
	}
	
	return result;
}

unsigned long Win32Utils::translateButtonMask( UINT win32ButtonMask )
{
	unsigned long result = MOUSE_UNDEFINED;

	if ( (win32ButtonMask & MK_LBUTTON) != 0 ){
		result |= MOUSE_LEFT_BUTTON;
	}

	if ( (win32ButtonMask & MK_MBUTTON) != 0 ){
		result |= MOUSE_MIDDLE_BUTTON;
	}

	if ( (win32ButtonMask & MK_RBUTTON) != 0 ){
		result |= MOUSE_RIGHT_BUTTON;
	}

	return result;
}

DWORD Win32Utils::translateStyle( unsigned long style )
{
	DWORD result = 0;
	//if ( (style & ) > 0 ){
		
	//};
	return result;
}

DWORD Win32Utils::translateExStyle( unsigned long style )
{
	DWORD result = 0;

	return result;
}

/**
*translates the Win32 key code into an actually usable 
*structure.
*Specifies the repeat count, scan code, extended-key flag, context code, previous key-state flag, 
*and transition-state flag, as shown in the following table. 
*Value Description 
*	0–15 Specifies the repeat count for the current message. 
*	     The value is the number of times the keystroke is autorepeated 
*		 as a result of the user holding down the key. If the keystroke is held 
*		 long enough, multiple messages are sent. However, the repeat count is not cumulative. 
*
*	16–23 Specifies the scan code. The value depends on the original equipment manufacturer (OEM). 
*
*	24   Specifies whether the key is an extended key, such as the right-hand ALT and CTRL keys that 
*	     appear on an enhanced 101- or 102-key keyboard. The value is 1 if it is an extended key; 
*		 otherwise, it is 0. 
*
*	25–28 Reserved; do not use. 
*
*	29   Specifies the context code. The value is 1 if the ALT key is held down while the 
*        key is pressed; otherwise, the value is 0. 
*
*	30   Specifies the previous key state. The value is 1 if the key is down before
*        the message is sent, or it is 0 if the key is up. 
*	31   Specifies the transition state. The value is 1 if the key is being released, or it 
*	     is 0 if the key is being pressed. 
*/
KeyboardData Win32Utils::translateKeyData( HWND wndHandle, LPARAM keyData )
{
	KeyboardData result = {0};	
	int sum = 0;

	sum = ((BYTE*)&keyData)[2]; //gets bits 16-23
	
	BYTE keyState[256];
	if ( TRUE == GetKeyboardState( &keyState[0] ) ){
		result.scanCode = sum;
		result.repeatCount = keyData & 0xFFFF; //mask out the upper 16 bits
		result.altKeyDown = ( keyData & KB_CONTEXT_CODE ) != 0;//replace with #define
		result.isExtendedKey = ( keyData & KB_IS_EXTENDED_KEY ) != 0;
		result.character = 0;
		result.VKeyCode = MapVirtualKey( result.scanCode, 1);
		HKL keyboardLayout = GetKeyboardLayout( GetWindowThreadProcessId( wndHandle, NULL ) );

		ToAsciiEx( result.VKeyCode, result.scanCode, &keyState[0], &result.character, 1, keyboardLayout );
		
		std::bitset<16> keyBits;
		keyBits = GetAsyncKeyState( VK_SHIFT );		
		if ( keyBits[15] == 1 ){
			result.keyMask |= MK_SHIFT;
		}

		keyBits = GetAsyncKeyState( VK_CONTROL );
		if ( keyBits[15] == 1 ){
			result.keyMask |= MK_CONTROL;
		}

		keyBits = GetAsyncKeyState( VK_MENU );
		if ( keyBits[15] == 1 ){
			result.altKeyDown = true;
		}
	}
	return result;
}

void Win32Utils::trace( const VCF::String& text )
{
#ifdef _DEBUG
	OutputDebugString( TEXT( text.c_str() ) );
#endif
}