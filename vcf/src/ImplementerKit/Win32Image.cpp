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

// Win32Image.cpp

#include "GraphicsKit.h"
#include "Win32Image.h"
#include "Win32Context.h"

using namespace VCF;




Win32Image::Win32Image()
{
	this->init();
}

Win32Image::Win32Image( const unsigned long& width, const unsigned long& height ):
	AbstractImage(false)	
{
	this->setWidth( width );
	this->setHeight( height );
	this->init();
	this->createBMP();
}

Win32Image::Win32Image( const String& fileName ):
	AbstractImage(false)	
{
	this->init();
	loadFromFile( fileName );
}

Win32Image::Win32Image( GraphicsContext* context, Rect* rect  ):
	AbstractImage(false)	
{
	if ( (NULL != context) && (NULL != rect) ){
		Win32Context* ctx = dynamic_cast<Win32Context*>(context->getPeer() );
		if ( NULL != ctx ){
			//set up the bitmap data
			this->setWidth( rect->getWidth() );
			this->setHeight( rect->getHeight() );
			this->init();
			this->createBMP();
			
			HDC srcDC = (HDC)ctx->getContextID();
			::BitBlt( this->m_dc, 0, 0, getWidth(), getHeight(), srcDC, rect->m_left, rect->m_top, SRCCOPY ); 
			//m_context->copyContext( rect->m_left, rect->m_top, context );
		}
		else {
			//throw exception !!!	
		}
	}
	else {
		//throw exception !!!
	}
}

Win32Image::Win32Image( HBITMAP bitmap )
	:AbstractImage(false)
{
	init();
	
	loadFromBMPHandle( bitmap );
}

Win32Image::~Win32Image()
{
	if ( NULL != m_hBitmap ){
		//SelectObject( m_dc, m_hOldBitmap );
		int r = DeleteObject( m_hBitmap );

		//this->m_imageBits->m_bits = NULL;
	}	
}

void Win32Image::init()
{
	this->m_palette = NULL;

	m_dc = ::CreateCompatibleDC( NULL );

	//the deletion of the m_context should delete the 
	//m_dc handle
	this->m_context = new GraphicsContext( (long)m_dc );
}

void Win32Image::setWidth( const unsigned long & width )
{
	AbstractImage::setWidth( width );	
}

void Win32Image::setHeight( const unsigned long & height )
{
	AbstractImage::setHeight( height );	
}

void Win32Image::createBMP()
{	
	
	memset( &m_bmpInfo, 0, sizeof (BITMAPINFOHEADER) ); 
	this->m_bmpInfo.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
	this->m_bmpInfo.bmiHeader.biWidth = this->getWidth();
	this->m_bmpInfo.bmiHeader.biHeight = -this->getHeight(); // Win32 DIB are upside down - do this to filp it over
	this->m_bmpInfo.bmiHeader.biPlanes = 1;
	this->m_bmpInfo.bmiHeader.biBitCount = 32; 
	this->m_bmpInfo.bmiHeader.biCompression = BI_RGB;
	this->m_bmpInfo.bmiHeader.biSizeImage = m_bmpInfo.bmiHeader.biWidth * m_bmpInfo.bmiHeader.biHeight * 4;
	m_imageBits->m_bits = NULL;
	this->m_hBitmap = CreateDIBSection ( NULL, &this->m_bmpInfo, DIB_RGB_COLORS, (void **)&m_imageBits->m_bits, NULL, NULL ); 
	if ( NULL != m_hBitmap ) {
		HGDIOBJ m_hOldBitmap = ::SelectObject( this->m_dc, m_hBitmap );
		//::DeleteObject( oldObj );//clear out the old object
	}
}

HBITMAP Win32Image::getBitmap()
{
	return this->m_hBitmap;
}
	
HDC Win32Image::getDC()
{
	return this->m_dc;
}

void Win32Image::loadFromFile( const String& fileName )
{	
	HBITMAP hBMP = (HBITMAP)LoadImage( NULL, fileName.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	loadFromBMPHandle( hBMP );
}

void Win32Image::loadFromBMPHandle( HBITMAP bitmap )
{
	m_hBitmap = bitmap;
	BITMAP bmp = {0};
	GetObject( bitmap, sizeof(bmp), &bmp );
	
	this->setHeight( bmp.bmHeight );
	this->setWidth( bmp.bmWidth );	
	
	HDC tmpBMPDC = CreateCompatibleDC( NULL );
	SelectObject(  tmpBMPDC, bitmap );
	this->createBMP();
	BitBlt( m_dc, 0, 0, bmp.bmWidth, bmp.bmHeight, tmpBMPDC, 0, 0, SRCCOPY );
	DeleteDC( tmpBMPDC );
	DeleteObject( bitmap );
}


void Win32Image::saveToFile( const String& fileName )
{

}

BMPLoader::BMPLoader()
{
	
}
	
BMPLoader::~BMPLoader()
{

}

Image* BMPLoader::loadImageFromFile( const String& fileName )
{
	return new Win32Image( fileName );
}
	
void BMPLoader::saveImageToFile( const String& fileName, Image* image )
{

}
