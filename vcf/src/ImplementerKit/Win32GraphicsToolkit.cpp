//Win32GraphicsToolkit.h
/**
Copyright (c) 2000-2001, Jim Crafton
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
	Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in 
	the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

NB: This software will not save the world. 
*/
#include "GraphicsKit.h"

#include "Win32GraphicsToolkit.h"

#include "Win32Context.h"
#include "Win32Image.h"
#include "Win32Font.h"
#include "Win32OpenGLPeer.h"
#include "Win32FontManager.h"

using namespace VCF;

Win32FontManager* Win32FontManager::win32FontMgr = NULL;


Win32GraphicsToolkit::Win32GraphicsToolkit()
{
	Win32FontManager::create();

	GraphicsToolkit::graphicsToolkitInstance = this;
	loadSystemColors();
	registerImageLoader( "image/bmp", new BMPLoader() );
}

Win32GraphicsToolkit::~Win32GraphicsToolkit()
{
	Win32FontManager::getFontManager()->free();
}

ContextPeer* Win32GraphicsToolkit::createContextPeer( const long& contextID )
{
	ContextPeer* result = NULL;
	
	result = new Win32Context( contextID );

	return result;
}

ContextPeer* Win32GraphicsToolkit::createContextPeer( const unsigned long& width, const unsigned long& height )
{
	return new Win32Context( width, height );
}

FontPeer* Win32GraphicsToolkit::createFontPeer( const String& fontName )
{
	return new Win32Font( fontName );
}

FontPeer* Win32GraphicsToolkit::createFontPeer( const String& fontName, const double& pointSize )
{
	return new Win32Font( fontName, pointSize );
}

OpenGLPeer* Win32GraphicsToolkit::createOpenGLPeer( GraphicsContext* glContext )
{
	return new Win32OpenGLPeer( glContext );
}

Image* Win32GraphicsToolkit::createImage( const unsigned long& width, const unsigned long& height )
{
	return new Win32Image( width, height );
}

Image* Win32GraphicsToolkit::createImage( GraphicsContext* context, Rect* rect  )
{	
	if ( NULL != context ){
		return new Win32Image( context, rect );
	}
	else {
		return NULL;
	}
}


void Win32GraphicsToolkit::loadSystemColors()
{
	Color* sysColor = NULL;
	sysColor = new Color( ::GetSysColor( COLOR_BTNSHADOW ) );
	m_systemColors[SYSCOLOR_SHADOW] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_3DFACE ) );
	m_systemColors[SYSCOLOR_FACE] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_3DHILIGHT ) );
	m_systemColors[SYSCOLOR_HIGHLIGHT] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_ACTIVECAPTION ) );
	m_systemColors[SYSCOLOR_ACTIVE_CAPTION] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_ACTIVEBORDER ) );
	m_systemColors[SYSCOLOR_ACTIVE_BORDER] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_DESKTOP ) );
	m_systemColors[SYSCOLOR_DESKTOP] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_CAPTIONTEXT ) );
	m_systemColors[SYSCOLOR_CAPTION_TEXT] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_HIGHLIGHT ) );
	m_systemColors[SYSCOLOR_SELECTION] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_HIGHLIGHTTEXT ) );
	m_systemColors[SYSCOLOR_SELECTION_TEXT] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_INACTIVEBORDER ) );
	m_systemColors[SYSCOLOR_INACTIVE_BORDER] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_INACTIVECAPTION ) );
	m_systemColors[SYSCOLOR_INACTIVE_CAPTION] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_INFOBK ) );
	m_systemColors[SYSCOLOR_TOOLTIP] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_INFOTEXT ) );
	m_systemColors[SYSCOLOR_TOOLTIP_TEXT] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_MENU ) );
	m_systemColors[SYSCOLOR_MENU] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_MENUTEXT ) );
	m_systemColors[SYSCOLOR_MENU_TEXT] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_WINDOW ) );
	m_systemColors[SYSCOLOR_WINDOW] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_WINDOWTEXT ) );
	m_systemColors[SYSCOLOR_WINDOW_TEXT] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_WINDOWFRAME ) );
	m_systemColors[SYSCOLOR_WINDOW_FRAME] = sysColor;

}

