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

//GraphicsToolkit.cpp
#include "GraphicsKit.h"

using namespace VCF;


GraphicsToolkit::GraphicsToolkit()
{
	initContentTypes();	
	m_colorMap[WHITE]		=	new Color( 1.0f, 1.0f, 1.0f );
	m_colorMap[BLACK]		=	new Color( 0.0f, 0.0f, 0.0f );
	m_colorMap[GREY]		=	new Color( 0.5f, 0.5f, 0.5f );
	m_colorMap[YELLOW]		=	new Color( 1.0f, 1.0f, 0.0f );
	m_colorMap[GREEN]		=	new Color( 0.0f, 1.0f, 0.0f );
	m_colorMap[BLUE]		=	new Color( 0.0f, 0.0f, 1.0f );
	m_colorMap[RED]		=	new Color( 1.0f, 0.0f, 0.0f );
	m_colorMap[PURPLE]		=	new Color( 0.5f, 0.0f, 1.0f );
	m_colorMap[LT_BLUE]	=	new Color( 0.0f, 1.0f, 1.0f );
}

GraphicsToolkit::~GraphicsToolkit()
{
	std::map<unsigned long,Color*>::iterator it = m_systemColors.begin();
	while ( it != m_systemColors.end() ){
		delete it->second;
		it++;
	}
	m_systemColors.clear();

	std::map<String,Color*>::iterator it2 = m_colorMap.begin();
	while ( it2 != m_colorMap.end() ){
		Color* c = it2->second;
		delete c;
		c = NULL;
		it2 ++;
	}		
	m_colorMap.clear();

	std::map<String,ImageLoader*>::iterator it3 = m_imageLoaders.begin();
	if ( it3 != m_imageLoaders.end() ){
		delete it3->second;
		it3->second = NULL;	
		it3 ++;
	}
	m_imageLoaders.clear();
}

GraphicsToolkit* GraphicsToolkit::getDefaultGraphicsToolkit()
{
	if ( NULL != GraphicsToolkit::graphicsToolkitInstance ){
		//throw exception
	}

	return GraphicsToolkit::graphicsToolkitInstance;
}

Image* GraphicsToolkit::createImage( const String& fileName )
{
	Image* result = NULL;
	String ext = StringUtils::getFileExt( fileName );	
	ext = StringUtils::lowerCase( ext );
	std::map<String,String>::iterator it = m_contentTypes.find( ext );
	if ( it != m_contentTypes.end() ){
		ImageLoader* imageLoader = getImageLoader( it->second );
		if ( NULL != imageLoader ){
			result = imageLoader->loadImageFromFile( fileName );
		}
	}
	return result;
}

void GraphicsToolkit::registerImageLoader( const String& name, ImageLoader* imageLoader )
{
	m_imageLoaders[name] = imageLoader;
}                                   

void GraphicsToolkit::initContentTypes()
{
	m_contentTypes["jpeg"] = "image/jpeg";
	m_contentTypes["jpe"] = "image/jpeg";
	m_contentTypes["jpg"] = "image/jpeg";
	m_contentTypes["gif"] = "image/gif";
	m_contentTypes["bmp"] = "image/bmp";
	m_contentTypes["rle"] = "image/bmp";
	m_contentTypes["dib"] = "image/bmp";
	m_contentTypes["wmf"] = "image/emf";
	m_contentTypes["emf"] = "image/emf";
	m_contentTypes["tif"] = "image/tiff";
	m_contentTypes["tiff"] = "image/tiff";
	m_contentTypes["tga"] = "image/targa";
	m_contentTypes["jfif"] = "image/jpeg";
	m_contentTypes["png"] = "image/png";
	m_contentTypes["pcx"] = "image/pcx";
	m_contentTypes["sgi"] = "image/sgi";
}

ImageLoader* GraphicsToolkit::getImageLoader( const String& contentType )
{
	ImageLoader* result = NULL;
	std::map<String,ImageLoader*>::iterator it = m_imageLoaders.find( contentType );
	if ( it != m_imageLoaders.end() ){
		result = it->second;
	}
	return result;
}

Color* GraphicsToolkit::getSystemColor( const unsigned long& systemColor )
{
	Color* result = NULL;
	std::map<unsigned long,Color*>::iterator found = this->m_systemColors.find( systemColor );
	if ( found != m_systemColors.end() ){
		result = found->second;
	}
	return result;
}

Color* GraphicsToolkit::getColorFromColormap( const String& color )
{
	Color* result = NULL;
	std::map<String,Color*>::iterator found = m_colorMap.find( color );
	if ( found != m_colorMap.end() ){
		result = found->second;
	}
	return result;
}	

#ifdef WIN32	
	#include "Win32GraphicsToolkit.h"
	
#endif

void GraphicsToolkit::initGraphicsToolkit()
{
#ifdef WIN32
	
	GraphicsToolkit::graphicsToolkitInstance = new Win32GraphicsToolkit();

#endif

	GraphicsToolkit::graphicsToolkitInstance->init();
}

void GraphicsToolkit::closeGraphicsToolkit()
{
	GraphicsToolkit::graphicsToolkitInstance->free();

	GraphicsToolkit::graphicsToolkitInstance = NULL;
}