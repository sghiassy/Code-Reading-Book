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

//Win32Peer.h
/**
*Main header for implementation object that Win32 headers
*/


#ifndef WIN32_PEER_H__
#define WIN32_PEER_H__


/*
these include have now been moved to FrameworkConfig.h
#include <windows.h>
#include <commctrl.h> //make sure to link with comctl32.lib
#include <shlobj.h>
*/



namespace VCFWin32 {

struct KeyboardData {
	int repeatCount;
	int scanCode;
	bool isExtendedKey;
	bool altKeyDown;
	unsigned short character;
	int VKeyCode;
	UINT keyMask;
};

#define KB_CONTEXT_CODE			29
#define KB_PREVIOUS_STATE		30
#define KB_IS_EXTENDED_KEY		24

#define SHIFT_KEY_DOWN \
	((GetKeyState( VK_SHIFT) & 15 ) == 1)

#define VCF_MESSAGE				WM_USER	+ 100
#define VCF_CONTROL_CREATE		VCF_MESSAGE + 1


/**
*what follows are a set of standard utility functions for Win32
*/
class FRAMEWORK_API Win32Utils {

public:
	static unsigned long translateKeyMask( UINT win32KeyMask );

	static unsigned long translateButtonMask( UINT win32ButtonMask );

	static KeyboardData translateKeyData( HWND wndHandle, LPARAM keyData );

	static DWORD translateStyle( unsigned long style );

	static DWORD translateExStyle( unsigned long style );

	static void trace( const VCF::String& text );

	static WORD	getWin32LangID( VCF::Locale* locale );

	static int getXFromLParam( LPARAM lParam );
	
	static int getYFromLParam( LPARAM lParam );

	static VCF::ulong32 translateVKCode( UINT vkCode );

	static bool fileExists( const VCF::String& fileName );

	static VCF::String getErrorString( const DWORD& errorCode );
};


};


#endif