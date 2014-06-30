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

// Win32FontDialog.cpp
#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"


using namespace VCF;


Win32FontDialog::Win32FontDialog( Control* owner )
{
	m_owner = owner;
}

Win32FontDialog::~Win32FontDialog()
{

}

void Win32FontDialog::setTitle( const String& title )
{

}

bool Win32FontDialog::execute()
{
	bool result = false;
	CHOOSEFONT chooseFont = {0};
	chooseFont.lStructSize = sizeof(CHOOSEFONT);
	if ( NULL != m_owner ){
		chooseFont.hwndOwner = (HWND)m_owner->getPeer()->getHandleID();
	}
	else {
		chooseFont.hwndOwner = GetActiveWindow();
	}
	LOGFONT logFont = {0};
	memcpy( &logFont, (LOGFONT*)m_font.getFontPeer()->getFontHandleID(), sizeof(LOGFONT) ); 
	
	chooseFont.lpLogFont = &logFont;
	chooseFont.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;
	Color* c = m_font.getColor();
	unsigned char r;
	unsigned char g;
	unsigned char b;
	c->getRGB( r, g, b );
	chooseFont.rgbColors = RGB( c->getRed()*255, c->getGreen()*255, c->getBlue()*255 );

	if ( TRUE == ChooseFont( &chooseFont ) ){
		r = GetRValue( chooseFont.rgbColors );
		g = GetGValue( chooseFont.rgbColors );
		b = GetBValue( chooseFont.rgbColors );
		c->setRGB( r,g,b);
		LOGFONT* fontLogFont = (LOGFONT*)m_font.getFontPeer()->getFontHandleID();
		memcpy( fontLogFont, chooseFont.lpLogFont, sizeof(LOGFONT) );
		result = true;
	}
	return result;
}

Font* Win32FontDialog::getSelectedFont()
{
	return &m_font;
}

void Win32FontDialog::setSelectedFont( Font* selectedFont )
{
	m_font.copy( selectedFont );
}
