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



using namespace VCF;

#define EPSILON 1e-6

void PixelBufferAffineRun( int *p_x0, int *p_x1, int y, int src_width, int src_height, const double affine[6] )
{
  int x0, x1;
  double z;
  double x_intercept;
  int xi;

  x0 = *p_x0;
  x1 = *p_x1;

  /* do left and right edges */
  if (affine[0] > EPSILON)
    {
      z = affine[2] * (y + 0.5) + affine[4];
      x_intercept = -z / affine[0];
      xi = ceil (x_intercept + EPSILON - 0.5);
      if (xi > x0)
	x0 = xi;
      x_intercept = (-z + src_width) / affine[0];
      xi = ceil (x_intercept - EPSILON - 0.5);
      if (xi < x1)
	x1 = xi;
    }
  else if (affine[0] < -EPSILON)
    {
      z = affine[2] * (y + 0.5) + affine[4];
      x_intercept = (-z + src_width) / affine[0];
      xi = ceil (x_intercept + EPSILON - 0.5);
      if (xi > x0)
	x0 = xi;
      x_intercept = -z / affine[0];
      xi = ceil (x_intercept - EPSILON - 0.5);
      if (xi < x1)
	x1 = xi;
    }
  else
    {
      z = affine[2] * (y + 0.5) + affine[4];
      if (z < 0 || z >= src_width)
	{
	  *p_x1 = *p_x0;
	  return;
	}
    }

  /* do top and bottom edges */
  if (affine[1] > EPSILON)
    {
      z = affine[3] * (y + 0.5) + affine[5];
      x_intercept = -z / affine[1];
      xi = ceil (x_intercept + EPSILON - 0.5);
      if (xi > x0)
	x0 = xi;
      x_intercept = (-z + src_height) / affine[1];
      xi = ceil (x_intercept - EPSILON - 0.5);
      if (xi < x1)
	x1 = xi;
    }
  else if (affine[1] < -EPSILON)
    {
      z = affine[3] * (y + 0.5) + affine[5];
      x_intercept = (-z + src_height) / affine[1];
      xi = ceil (x_intercept + EPSILON - 0.5);
      if (xi > x0)
	x0 = xi;
      x_intercept = -z / affine[1];
      xi = ceil (x_intercept - EPSILON - 0.5);
      if (xi < x1)
	x1 = xi;
    }
  else
    {
      z = affine[3] * (y + 0.5) + affine[5];
      if (z < 0 || z >= src_height)
	{
	  *p_x1 = *p_x0;
	  return;
	}
    }

  *p_x0 = x0;
  *p_x1 = x1;
}

PixelBuffer::PixelBuffer( const int32& width, const int32& height, 
		                  const int32& rowstride, uchar* pixels, const bool hasAlpha )
{
	this->m_bitsPerSample = 8;
	this->m_format = PF_RGB;
	this->m_hasAlpha = hasAlpha;
	this->m_height = height;
	this->m_numChannels = (hasAlpha) ? 4 : 3;
	this->m_pixels = NULL;
	this->m_rowStride = rowstride;
	this->m_width = width;

	int size = (m_height - 1) * m_rowStride + m_width * ((m_numChannels * m_bitsPerSample + 7) >> 3);

	m_pixels = new uchar[ size ];
	if ( NULL != pixels ){
		memcpy( m_pixels, pixels, size ); 
	}
	else {
		memset( m_pixels, 0, size );
	}
}

PixelBuffer::PixelBuffer( const PixelBuffer& pixBufferToCopy )
{
	this->m_bitsPerSample = pixBufferToCopy.m_bitsPerSample;
	this->m_format = pixBufferToCopy.m_format;
	this->m_hasAlpha = pixBufferToCopy.m_hasAlpha;
	this->m_height = pixBufferToCopy.m_height;
	this->m_numChannels = pixBufferToCopy.m_numChannels;
	this->m_pixels = NULL;
	this->m_rowStride = pixBufferToCopy.m_rowStride;
	this->m_width = pixBufferToCopy.m_width;

	int size = (m_height - 1) * m_rowStride + m_width * ((m_numChannels * m_bitsPerSample + 7) >> 3);

	m_pixels = new uchar[ size ];

	memcpy( m_pixels, pixBufferToCopy.m_pixels, size );
}

PixelBuffer::~PixelBuffer()
{
	delete [] m_pixels;
}

void PixelBuffer::destroyNotify( void* funcData, void* data )
{

}

void PixelBuffer::affineTransform( PixelBuffer* dest, Rect* destRect, const double affine[6] )
{
	if ( true == this->m_hasAlpha ){
		this->affineTransformRGBA( dest, destRect, affine );
	}
	else {
		this->affineTransformRGB( dest, destRect, affine );
	}
}

void PixelBuffer::affineTransformRGB( PixelBuffer* dest, Rect* destRect, const double affine[6] )
{
	double inv[6];
	uchar* dst_p = NULL;
	uchar* dst_linestart = NULL;
	const uchar* src_p = NULL;
	Point pt;
	Point src_pt;
	int src_x = 0;
	int src_y = 0;
	int run_x0 = 0;
	int run_x1 = 0;
	
	dst_linestart = dest->m_pixels;

	affineInvert(inv, affine);
	int x0 = destRect->m_left;
	int x1 = destRect->m_right;
	int w = destRect->getWidth();
	int h = destRect->getHeight();
	for (int y = destRect->m_top; y < destRect->m_bottom; y++) {
		pt.m_y = y + 0.5;
		run_x0 = x0;
		run_x1 = x1;
		
		PixelBufferAffineRun (&run_x0, &run_x1, y, this->m_width, this->m_height, inv);

		dst_p = dst_linestart + (run_x0 - x0) * 3;
		for (int x = run_x0; x < run_x1; x++){
			pt.m_x = x + 0.5;
			//art_affine_point (&src_pt, &pt, inv);
			src_x = floor (src_pt.m_x);
			src_y = floor (src_pt.m_y);
			src_p = this->m_pixels + (src_y * this->m_rowStride) + src_x * 3;
			dst_p[0] = src_p[0];
			dst_p[1] = src_p[1];
			dst_p[2] = src_p[2];
			dst_p += 3;
		}
		dst_linestart += dest->m_rowStride;
    }
}

void PixelBuffer::affineTransformRGBA( PixelBuffer* dest, Rect* destRect, const double affine[6] )
{

}

void PixelBuffer::render( SortedVectorPath* svp, Rect* clipRect, Color* lineColor, Color* fillColor, Color* backColor, AlphaGamma* gamma )
{
	if ( (NULL != svp) && (NULL != clipRect) && (NULL != lineColor) && (NULL != fillColor) && (NULL != backColor) ){
		int r_fg = 0;
		int g_fg = 0;
		int b_fg = 0;
		int r_bg = 0;
		int g_bg = 0;
		int b_bg = 0;
		int r = 0;
		int g = 0;
		int b = 0;
		int dr = 0;
		int dg = 0;
		int db = 0;
		
		uint32 rgbtab[256];

		uint32 fg_color = fillColor->getRGB();
		uint32 bg_color = backColor->getRGB();

		/**
		*set up the colors for the Gamma table
		*/
		if ( NULL == gamma ){
			r_fg = fg_color >> 16;
			g_fg = (fg_color >> 8) & 0xff;
			b_fg = fg_color & 0xff;
			
			r_bg = bg_color >> 16;
			g_bg = (bg_color >> 8) & 0xff;
			b_bg = bg_color & 0xff;
			
			r = (r_bg << 16) + 0x8000;
			g = (g_bg << 16) + 0x8000;
			b = (b_bg << 16) + 0x8000;
			dr = ((r_fg - r_bg) << 16) / 255;
			dg = ((g_fg - g_bg) << 16) / 255;
			db = ((b_fg - b_bg) << 16) / 255;
			
			for (int i = 0; i < 256; i++) {
				rgbtab[i] = (r & 0xff0000) | ((g & 0xff0000) >> 8) | (b >> 16);
				r += dr;
				g += dg;
				b += db;
			}
		}
		else {			
			int32 *table = gamma->getTable();
			
			r_fg = table[fg_color >> 16];
			g_fg = table[(fg_color >> 8) & 0xff];
			b_fg = table[fg_color & 0xff];
			
			r_bg = table[bg_color >> 16];
			g_bg = table[(bg_color >> 8) & 0xff];
			b_bg = table[bg_color & 0xff];
			
			r = (r_bg << 16) + 0x8000;
			g = (g_bg << 16) + 0x8000;
			b = (b_bg << 16) + 0x8000;
			dr = ((r_fg - r_bg) << 16) / 255;
			dg = ((g_fg - g_bg) << 16) / 255;
			db = ((b_fg - b_bg) << 16) / 255;
			
			uchar *invtab = gamma->getInvTable();
			for (int i = 0; i < 256; i++) {
				rgbtab[i] = (invtab[r >> 16] << 16) |
					(invtab[g >> 16] << 8) |
					invtab[b >> 16];
				r += dr;
				g += dg;
				b += db;
			}
		}
		
		int x0 = clipRect->m_top;
		int y0 = clipRect->m_left;
		int x1 = clipRect->m_right;
		int y1 = clipRect->m_bottom;
		for (int y = y0;y<y1;y++) {
			
		}

	}
}