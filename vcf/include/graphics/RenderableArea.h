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

/**
*borrowed from 
*art_render.h & Libart_LGPL - library of basic graphic primitives
* Copyright (C) 2000 Raph Levien
*/

#ifndef _RENDERABLEAREA_H__
#define _RENDERABLEAREA_H__

#include "VCF.h"

namespace VCF {


class AlphaGamma;


#if RENDERABLE_MAX_DEPTH == 16
	typedef ushort RenderPixelMaxDepthType;
	#define RENDER_PIX_MAX_FROM_8(x) ((x) | ((x) << 8))
	#define RENDER_PIX_8_FROM_MAX(x) (((x) + 0x80 - (((x) + 0x80) >> 8)) >> 8)
#else
	#if RENDERABLE_MAX_DEPTH == 8
		typedef uchar RenderPixelMaxDepthType;
		#define RENDER_PIX_MAX_FROM_8(x) (x)
		#define RENDER_PIX_8_FROM_MAX(x) (x)
	#else
		#error RENDERABLE_MAX_DEPTH must be either 8 or 16
	#endif //RENDERABLE_MAX_DEPTH == 8
#endif //RENDERABLE_MAX_DEPTH == 16


#define RENDERABLE_MAX_CHAN			16


enum RenderAlphaType{
	RA_NONE=0,
	RA_SEPERATE,
	RA_PRE_MULTIPLY
};

enum RenderCompositeMode{
	RCM_NONE=0,
	RCM_NORMAL,
	RCM_ADDITIVE,
	RCM_SUBBTRACTIVE,
	RCM_MULTIPLY,
	RCM_DIVIDE
	//more to come
};

class GRAPHICSKIT_API RenderMaskRun {
public:
	int32 m_x;
	int32 m_alpha;
};

class GRAPHICSKIT_API RenderableArea {

public:
	RenderableArea( const int32& x0, const int32& y0, const int32& x1, const int32& y1,
					const int32& rowstride, uchar *pixels=NULL,
					int n_chan=4, const int32& depth, 
					const RenderAlphaType& alpha_type=RA_NONE,
					AlphaGamma *alphaGamma=NULL ){};
	
	virtual ~RenderableArea(){};
	
	virtual void invoke();

	virtual void clear();

	virtual void clearRGB( const uint32& rgb );

	virtual void maskSolid( const int32& opacity );


protected:
	int32 m_x0;
	int32 m_y0;
	int32 m_x1;
	int32 m_y1;
	uchar* m_pixels;
	int32 m_rowstride;
	int32	m_numberOfChannels;
	int32	m_depth;
	RenderAlphaType m_alphaType;
	
	bool m_isClear;

	long m_clearColors[RENDERABLE_MAX_CHAN + 1];
	uint32 m_opacity; /* [0..0x10000] */
	
	RenderCompositeMode compositing_mode;
	
	AlphaGamma* m_alphaGamma;
	
	uchar* alphaBuffer;
	
	/* parameters of intermediate buffer */
	int m_bufferDepth;
	RenderAlphaType m_bufferAlpha;
	uchar* m_imageBuffer;
	
	/* driving alpha scanline data */
	/* A "run" is a contiguous sequence of x values with the same alpha value. */
	int m_runNumber;
	
	RenderMaskRun* m_run;
	
	/* A "span" is a contiguous sequence of x values with non-zero alpha. */
	int m_spanNumber;
	int* m_spanX;
	
	bool m_needSpan;

private:
	
	
};

}; //end of namespace VCF

#endif //_RENDERABLEAREA_H__