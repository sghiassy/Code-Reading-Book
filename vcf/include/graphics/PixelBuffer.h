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

#ifndef _PIXELBUFFER_H__
#define _PIXELBUFFER_H__



namespace VCF {

enum PixelFormat{
	PF_RGB = 0
};


class Rect;

class SortedVectorPath;

class Color;

class AlphaGamma;


class GRAPHICSKIT_API PixelBuffer {

public:
	PixelBuffer( const int32& width, const int32& height, 
		         const int32& rowstride, uchar* pixels=NULL, const bool hasAlpha=true );
	
	PixelBuffer( const PixelBuffer& pixBufferToCopy );

	virtual ~PixelBuffer();

	virtual void destroyNotify( void* funcData, void* data );

	void affineTransform( PixelBuffer* dest, Rect* destRect, const double affine[6] );

	void render( SortedVectorPath* svp, Rect* clipRect, Color* lineColor, Color* fillColor, Color* backColor, AlphaGamma* gamma=NULL );
protected:

	void affineTransformRGB( PixelBuffer* dest, Rect* destRect, const double affine[6] );

	void affineTransformRGBA( PixelBuffer* dest, Rect* destRect, const double affine[6] );

private:

	void affineInvert (double dst[6], const double src[6]){
		double r_det = 0.0;
		
		r_det = 1.0 / (src[0] * src[3] - src[1] * src[2]);
		dst[0] = src[3] * r_det;
		dst[1] = -src[1] * r_det;
		dst[2] = -src[2] * r_det;
		dst[3] = src[0] * r_det;
		dst[4] = -src[4] * dst[0] - src[5] * dst[2];
		dst[5] = -src[4] * dst[1] - src[5] * dst[3];
	}

	PixelFormat m_format;
	int32 m_numChannels;
	bool m_hasAlpha;
	int32 m_bitsPerSample;
	
	uchar* m_pixels;

	int32 m_width;
	int32 m_height;
	int32 m_rowStride;
	void* m_destroyData;
	
};

}; //end of namespace VCF

#endif //_PIXELBUFFER_H__