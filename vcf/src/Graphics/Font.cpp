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

//Font.cpp
#include "GraphicsKit.h"


using namespace VCF;

Font::Font()
{
	GraphicsToolkit* toolkit = GraphicsToolkit::getDefaultGraphicsToolkit();
	if ( NULL == toolkit ){	
		throw NoGraphicsToolkitFoundException( MAKE_ERROR_MSG(NO_GFX_TOOLKIT_ERROR), __LINE__ );
	}
	m_Peer = NULL;
	m_Peer = toolkit->createFontPeer( DEFAULT_FONT_NAME ); 
	if ( NULL == m_Peer ){
		throw InvalidPeer( MAKE_ERROR_MSG(NO_PEER), __LINE__ );
	}
}

Font::Font( const String& fontName )
{
	GraphicsToolkit* toolkit = GraphicsToolkit::getDefaultGraphicsToolkit();
	if ( NULL == toolkit ){
		throw NoGraphicsToolkitFoundException( MAKE_ERROR_MSG(NO_GFX_TOOLKIT_ERROR), __LINE__ );
	}
	m_Peer = NULL;
	m_Peer = toolkit->createFontPeer( fontName ); 
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
}

Font::Font( const String& fontName, const double& pointSize )
{
	GraphicsToolkit* toolkit = GraphicsToolkit::getDefaultGraphicsToolkit();
	if ( NULL == toolkit ){
		throw NoGraphicsToolkitFoundException( MAKE_ERROR_MSG(NO_GFX_TOOLKIT_ERROR), __LINE__ );
	}
	m_Peer = NULL;
	m_Peer = toolkit->createFontPeer( fontName, pointSize ); 
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
}
Font::~Font()
{
	delete this->m_Peer;
}

bool Font::isTrueType()
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	return m_Peer->isTrueType();
}

Color* Font::getColor()
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	return m_Peer->getColor();
}

void Font::setColor( Color* color )
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	m_Peer->setColor( color );
}

double Font::getPointSize()
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	return m_Peer->getPointSize();
}

void Font::setPointSize( const double pointSize )
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	m_Peer->setPointSize( pointSize );
}

double Font::getPixelSize()
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	return m_Peer->getPixelSize();
}

void Font::setPixelSize( const double pixelSize )
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	m_Peer->setPixelSize( pixelSize );
}

void Font::setBold( const bool& bold )
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	m_Peer->setBold( bold );
}

bool Font::getBold()
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	return m_Peer->getBold();
}	

bool Font::getItalic()
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	return m_Peer->getItalic();
}

void Font::setItalic( const bool& italic )
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	m_Peer->setItalic( italic );
}

bool Font::getUnderlined()
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	return m_Peer->getUnderlined();
}

void Font::setUnderlined( const bool& underlined )
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	m_Peer->setUnderlined( underlined );
}

bool Font::getStrikeOut()
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	return m_Peer->getStrikeOut();
}

void Font::setStrikeOut( const bool& strikeout )
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	m_Peer->setStrikeOut( strikeout );
}

double Font::getShear()
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	return m_Peer->getShear();
}

void Font::setShear(const double& shear )
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	m_Peer->setShear( shear );
}

double Font::getAngle()
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	return m_Peer->getAngle();
}

void Font::setAngle( const double& angle )
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};	
	m_Peer->setAngle( angle );
}

GlyphCollection* Font::getGlyphCollection( const String& text )
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};
	return m_Peer->getGlyphCollection( text );
}

FontPeer* Font::getFontPeer()
{
	return this->m_Peer;
}

String Font::getName()
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};
	return m_Peer->getName();
}

void Font::setName( const String& name )
{
	if ( NULL == this->m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};
	m_Peer->setName( name );
}

void Font::copy( Object* source )
{
	if ( NULL != source ){
		Font* srcFont = dynamic_cast<Font*>( source );
		if ( NULL != srcFont ){
			this->setAngle( srcFont->getAngle() );
			this->setShear( srcFont->getShear() );
			this->setStrikeOut( srcFont->getStrikeOut() );
			this->setUnderlined( srcFont->getUnderlined() );
			this->setBold( srcFont->getBold() );
			this->setItalic( srcFont->getItalic() );
			this->setPointSize( srcFont->getPointSize() );
			this->setName( srcFont->getName() );

			Color* srcColor = srcFont->getColor();
			this->getColor()->copy( srcColor );
		}
	}
}