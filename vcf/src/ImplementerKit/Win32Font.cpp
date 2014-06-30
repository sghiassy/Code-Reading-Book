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
#include "GraphicsKit.h"
#include "Win32Font.h"
#include "Win32FontManager.h"


using namespace VCF;

Win32Font::Win32Font( const String& fontName )
{
	m_pointSize = DEFAULT_FONT_PT_SIZE;
	m_fontName = fontName;
	init();
}

Win32Font::Win32Font( const String& fontName, const double& pointSize )
{
	m_pointSize = pointSize;
	m_fontName = fontName;
	init();
}

Win32Font::~Win32Font()
{
	Win32FontManager::removeFont( this );
}

void Win32Font::init()
{
	memset( &m_logFont, 0, sizeof(m_logFont) );	
	
	HFONT defFont = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
	defFont = NULL;
	if ( NULL != defFont ){
		LOGFONT tmpLogFont = {0};
		GetObject( defFont, sizeof(LOGFONT), &tmpLogFont );
		memcpy( &m_logFont, &tmpLogFont, sizeof(m_logFont));
	}
	else {
		
		m_logFont.lfCharSet = ANSI_CHARSET;//DEFAULT_CHARSET might be better ?
		m_logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		m_logFont.lfEscapement = 0;		
		m_logFont.lfItalic = FALSE;
		m_logFont.lfOrientation = 0;
		m_logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		m_logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		m_logFont.lfQuality = DEFAULT_QUALITY;
		m_logFont.lfStrikeOut = FALSE;
		m_logFont.lfUnderline = FALSE;
		m_logFont.lfWeight = FW_NORMAL;
		m_logFont.lfWidth = 0;//let font mapper choose closest match
	}
	HDC dc = GetDC( NULL );//gets screen DC
	int fontHeight = -MulDiv( m_pointSize, GetDeviceCaps( dc, LOGPIXELSY), 72 );
	ReleaseDC( NULL, dc );
	m_logFont.lfHeight = fontHeight;
	m_fontName.copy( m_logFont.lfFaceName, LF_FACESIZE );

	Win32FontManager::initializeFont( this );
}

ulong32 Win32Font::getFontHandleID()
{
	return (ulong32)(&this->m_logFont);
}

bool Win32Font::isTrueType()
{
	HDC dc = GetDC(NULL);
	HFONT tmpFont = CreateFontIndirect( &this->m_logFont );
	HFONT oldFont = (HFONT)SelectObject( dc, tmpFont );
	
	TEXTMETRIC tm = {0};	
	GetTextMetrics( dc, &tm ); 

	bool result = false;
	if ( (tm.tmPitchAndFamily & TMPF_TRUETYPE) != 0 ){
		result = true;
	}

	SelectObject( dc, oldFont );
	DeleteObject( tmpFont );
	ReleaseDC( NULL, dc );

	return result;
}

Color* Win32Font::getColor()
{
	return &m_color;
}

void Win32Font::setColor( Color* color )
{
	m_color.copy( color );
}

double Win32Font::getPointSize()
{
	HDC dc = GetDC( NULL );
	double ppi = (double)GetDeviceCaps( dc, LOGPIXELSY);
	double result = ((double)m_logFont.lfHeight / ppi) * 72.0;
	ReleaseDC( NULL, dc );

	return result;
}

void Win32Font::setPointSize( const double pointSize )
{	
	HDC dc = GetDC( NULL );
	double ppi = (double)GetDeviceCaps( dc, LOGPIXELSY);	
	long lfHeight = (pointSize / 72) * ppi;
	bool needsUpdate = (lfHeight != m_logFont.lfHeight);
	
	
	ReleaseDC( NULL, dc );

	if ( true == needsUpdate ) {
		Win32FontManager::removeFont( this );
	}

	m_logFont.lfHeight = lfHeight;

	if ( true == needsUpdate ) {
		Win32FontManager::initializeFont( this );
	}
}

double Win32Font::getPixelSize()
{
	return abs(m_logFont.lfHeight);
}

void Win32Font::setPixelSize( const double pixelSize )
{
	bool needsUpdate = (pixelSize != m_logFont.lfHeight);

	if ( true == needsUpdate ) {
		Win32FontManager::removeFont( this );
	}

	m_logFont.lfHeight = pixelSize;

	if ( true == needsUpdate ) {
		Win32FontManager::initializeFont( this );
	}
}

void Win32Font::setBold( const bool& bold )
{
	DWORD style = bold ? FW_BOLD : FW_NORMAL;
	bool needsUpdate = (style != m_logFont.lfWeight);

	if ( true == needsUpdate ) {
		Win32FontManager::removeFont( this );
	}

	this->m_logFont.lfWeight = style;

	if ( true == needsUpdate ) {
		Win32FontManager::initializeFont( this );
	}
}

bool Win32Font::getBold()
{
	return (m_logFont.lfWeight == FW_NORMAL) ? false : true;
}

bool Win32Font::getItalic()
{
	return (m_logFont.lfItalic == TRUE) ? true : false;
}

void Win32Font::setItalic( const bool& italic )
{
	BYTE lfItalic = italic ? TRUE : FALSE;
	bool needsUpdate = (lfItalic != m_logFont.lfItalic);
	if ( true == needsUpdate ) {
		Win32FontManager::removeFont( this );
	}

	m_logFont.lfItalic = italic ? TRUE : FALSE;

	if ( true == needsUpdate ) {
		Win32FontManager::initializeFont( this );
	}
}

bool Win32Font::getUnderlined()
{
	return (m_logFont.lfUnderline == TRUE) ? true : false;
}

void Win32Font::setUnderlined( const bool& underlined )
{
	BYTE lfUnderlined = underlined ? TRUE : FALSE;

	bool needsUpdate = (lfUnderlined != m_logFont.lfUnderline);
	if ( true == needsUpdate ) {
		Win32FontManager::removeFont( this );
	}

	m_logFont.lfUnderline = underlined ? TRUE : FALSE;

	if ( true == needsUpdate ) {
		Win32FontManager::initializeFont( this );
	}
}

bool Win32Font::getStrikeOut()
{
	return (m_logFont.lfStrikeOut == TRUE) ? true : false;
}

void Win32Font::setStrikeOut( const bool& strikeout )
{
	BYTE lfStrikeOut = strikeout ? TRUE : FALSE;

	bool needsUpdate = (lfStrikeOut != m_logFont.lfStrikeOut);

	if ( true == needsUpdate ) {
		Win32FontManager::removeFont( this );
	}

	m_logFont.lfStrikeOut = strikeout ? TRUE : FALSE;

	if ( true == needsUpdate ) {
		Win32FontManager::initializeFont( this );
	}
}

double Win32Font::getShear()
{
	return this->m_shear;
}

void Win32Font::setShear(const double& shear )
{
	this->m_shear = shear;
}

double Win32Font::getAngle()
{
	double tmp = (double)m_logFont.lfEscapement;
	return tmp / 10.0; //Win32 lfEscapement is in tenths of degrees i.e. 334.23° is 3342 
}

void Win32Font::setAngle( const double& angle )
{
	bool needsUpdate = ((angle*10) != m_logFont.lfEscapement);

	if ( true == needsUpdate ) {
		Win32FontManager::removeFont( this );
	}

	m_logFont.lfEscapement = angle * 10;
	m_logFont.lfOrientation = angle * 10;

	if ( true == needsUpdate ) {
		Win32FontManager::initializeFont( this );
	}
}

String Win32Font::getName()
{
	String result = "";
	result = m_logFont.lfFaceName;

	return result;
}

void Win32Font::setName( const String& name )
{
	bool needsUpdating = ( m_fontName != name );
	if ( true == needsUpdating ) {
		Win32FontManager::removeFont( this );
	}

	m_fontName = name;
	m_fontName.copy( m_logFont.lfFaceName, LF_FACESIZE );

	if ( true == needsUpdating ) {		
		Win32FontManager::initializeFont( this );
	}
}

GlyphCollection* Win32Font::getGlyphCollection( const String& text )
{
	return NULL;
}

bool Win32Font::isEqual( Object* object )
{
	bool result = false;
	Win32Font* fntObj = dynamic_cast<Win32Font*>(object);
	if ( NULL != fntObj ){
		
		if ( (this->m_fontName == fntObj->m_fontName) &&
			 (this->m_color.isEqual( &(fntObj->m_color) )) &&
			 (this->m_pointSize == fntObj->m_pointSize) ) {
			result = true;
		}
	}

	return result;
}