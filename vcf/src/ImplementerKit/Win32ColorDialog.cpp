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
//Win32ColorDialog.cpp
#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"

using namespace VCF;



Win32ColorDialog::Win32ColorDialog( Control* owner )
{
	m_owner = owner;
}

Win32ColorDialog::~Win32ColorDialog()
{

}

Color* Win32ColorDialog::getSelectedColor()
{
	return &this->m_color;
}

void Win32ColorDialog::setSelectedColor( Color* selectedColor )
{
	this->m_color.copy( selectedColor );
}

void Win32ColorDialog::setTitle( const String& title )
{
	this->m_title = title;
}

bool Win32ColorDialog::execute()
{
	bool result = false;

	CHOOSECOLOR color = {0};
	color.lStructSize = sizeof(CHOOSECOLOR);
	COLORREF customColors[16];
	color.lpCustColors = 
		customColors;
	color.hInstance = NULL;
	color.hwndOwner = NULL;

	if ( NULL != m_owner ){
		ControlPeer* impl = m_owner->getPeer();
		color.hwndOwner = (HWND) impl->getHandleID();
	}

	color.rgbResult = this->m_color.getRGB();

	color.Flags = CC_ANYCOLOR | CC_FULLOPEN;

	if ( TRUE == ChooseColor( &color ) ){
		result = true;
		uchar r = GetRValue( color.rgbResult );
		uchar g = GetGValue( color.rgbResult );
		uchar b = GetBValue( color.rgbResult );
		this->m_color.setRGB( r, g, b );
	}

	return result;
}