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

// Win32Image.h

#ifndef _WIN32IMAGE_H__
#define _WIN32IMAGE_H__


namespace VCF
{

class GRAPHICSKIT_API Win32Image : public AbstractImage  
{
public:
	Win32Image();
	
	Win32Image( const String& fileName );
	
	Win32Image( const unsigned long& width, const unsigned long& height );
	
	Win32Image( GraphicsContext* context, Rect* rect );

	Win32Image( HBITMAP bitmap );

	virtual ~Win32Image();

	void init();

	virtual void setWidth( const unsigned long & width );

	virtual void setHeight( const unsigned long & height );

	void createBMP();

	HBITMAP getBitmap();
	
	HDC getDC();

	BITMAPINFO m_bmpInfo;
	HPALETTE m_palette;//for 256 or fewer color images
protected:
	void loadFromFile( const String& fileName );

	void loadFromBMPHandle( HBITMAP bitmap );

	void saveToFile( const String& fileName );

private:
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
	HDC m_dc;
	
};

class GRAPHICSKIT_API BMPLoader : public Object, public ImageLoader {
public:
	BMPLoader();
	
	virtual ~BMPLoader();

	virtual Image* loadImageFromFile( const String& fileName );
	

	virtual void saveImageToFile( const String& fileName, Image* image );
	
private:
	
};

};

#endif 
