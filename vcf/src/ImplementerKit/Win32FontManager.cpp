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
#include "Win32FontManager.h"
#include "Win32Font.h"

using namespace VCF;

Win32FontManager* Win32FontManager::create()
{
	if ( NULL == Win32FontManager::win32FontMgr ) {
		Win32FontManager::win32FontMgr = new Win32FontManager();
		Win32FontManager::win32FontMgr->init();
	}
	return Win32FontManager::win32FontMgr;
}

/**
*Win32FontHandle code
*/
Win32FontHandle::Win32FontHandle( HFONT font )
{
	

	this->m_refCount = 0;
	this->m_fontHandle = NULL;
	this->m_fontHandle = font;
	this->incRef();
}

Win32FontHandle::~Win32FontHandle()
{
	if ( this->m_fontHandle != NULL ){
		DeleteObject( this->m_fontHandle );
	}
}

void Win32FontHandle::incRef()
{
	this->m_refCount ++;
}

void Win32FontHandle::decRef()
{
	if ( m_refCount > 0 ){
		this->m_refCount --;
		if ( (this->m_refCount == 0) && (this->m_fontHandle != NULL) ){
			DeleteObject( this->m_fontHandle );
			m_fontHandle = NULL;
		}
	}
}

HFONT Win32FontHandle::getFontHandle()
{
	return this->m_fontHandle;
}

bool Win32FontHandle::isRefCountZero()
{
	return 0 == m_refCount;
}


/**
*Win32FontManager code
*/
Win32FontManager::Win32FontManager()
{	
	m_defaultFont = NULL;		
}

Win32FontManager::~Win32FontManager()
{
	delete m_defaultFont;
	m_defaultFont = NULL;

	std::map<String,Win32FontHandle*>::iterator it = m_fontMap.begin();
	
	while ( it != m_fontMap.end() ){
		Win32FontHandle* fh = it->second;
		if ( NULL != fh ){
			delete fh;
		}	
		fh = NULL;
		it ++;
	}
	
	m_fontMap.clear();	
}

void Win32FontManager::init()
{
	m_defaultFont = new Win32Font( DEFAULT_VCF_FONT_NAME );	
}

void Win32FontManager::addFont( Win32Font* font )
{	
	String fontID = makeStringID( font );
	if ( fontID.size() > 0 ){
		
		std::map<String,Win32FontHandle*>::iterator found = this->m_fontMap.find( fontID );
		if ( found != m_fontMap.end() ){ 
			Win32FontHandle* win32FontHandle = found->second;
			win32FontHandle->incRef();
		}
		else {// only add if no entry
			/**
			*no entry found so go ahead and create a new HFONT object
			*and store the entry
			*/
			HFONT fontHandle = CreateFontIndirect( (LOGFONT*)font->getFontHandleID() );
			if ( NULL != fontHandle ){
				m_fontMap[fontID] = new Win32FontHandle( fontHandle );
			}//otherwise throw exception ?????			
		}			
	}
}

String Win32FontManager::makeStringID(  Win32Font* font )
{
	String result = "";
	if ( NULL != font ){
		result = font->getName() + font->getColor()->toString() + 
			StringUtils::toString( (int)font->getPixelSize() ) + 
			StringUtils::toString( (int)font->getAngle() ) +
			StringUtils::toString( font->getBold() ) +
			StringUtils::toString( font->getUnderlined() ) +
			StringUtils::toString( font->getStrikeOut() ) +
			StringUtils::toString( font->getItalic() );
	}
	return result;
}

void Win32FontManager::initializeFont( Win32Font* font )
{
	Win32FontManager::win32FontMgr->addFont( font );
}

HFONT Win32FontManager::getFontHandleFromFontPeer( Win32Font* font )
{
	HFONT result = NULL;

	String fontID = Win32FontManager::win32FontMgr->makeStringID( font );

	std::map<String,Win32FontHandle*>::iterator found = 
		Win32FontManager::win32FontMgr->m_fontMap.find( fontID );
	
	Win32FontHandle* win32FontHandle = NULL;

	if ( found != Win32FontManager::win32FontMgr->m_fontMap.end() ){
		win32FontHandle = found->second;
	}
	else { 
	/**
	*this font should have called Win32FontManager::initializeFont()
	*but somehow it didn't get added - let's add it now
	*/
		Win32FontManager::win32FontMgr->addFont( font );
		found = Win32FontManager::win32FontMgr->m_fontMap.find( fontID );
		if ( found != Win32FontManager::win32FontMgr->m_fontMap.end() ){
			win32FontHandle = found->second;
		}
		//otherwise we're fucked - throw an exception !!!!!
	}

	if ( NULL != win32FontHandle ){
		result = win32FontHandle->getFontHandle();
	}
	return result;
}

void Win32FontManager::removeFont( Win32Font* font )
{
	String fontID = Win32FontManager::win32FontMgr->makeStringID( font );
	
	if ( ! Win32FontManager::win32FontMgr->m_fontMap.empty() ) {
		std::map<String,Win32FontHandle*>::iterator found = 
			Win32FontManager::win32FontMgr->m_fontMap.find( fontID );
		
		if ( found != Win32FontManager::win32FontMgr->m_fontMap.end() ){
			Win32FontHandle* fontHandle = found->second;
			fontHandle->decRef();
			
			if ( true == fontHandle->isRefCountZero() ){
				delete fontHandle;
				fontHandle = NULL;
				Win32FontManager::win32FontMgr->m_fontMap.erase( found );
			}
		}
	}
}