//GraphicsKit.h

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

#ifndef _GRAPHICS_KIT_H__
#define _GRAPHICS_KIT_H__

#include "FoundationKit.h"
#include <math.h>
#include "Color.h"
#include "Matrix2D.h"
#include "ContextPeer.h"
#include "GraphicsContext.h"
#include "ImageBits.h"
#include "Image.h"
#include "ImageLoader.h"
#include "ImageTile.h"
#include "Kernel.h"
#include "Layer.h"
#include "MicroTiles.h"
#include "PathEnumerator.h"
#include "Path.h"
#include "PixelBuffer.h"
#include "Polygon.h"
#include "Curve.h"
#include "Ellipse.h"
#include "Circle.h"
#include "Glyph.h"
#include "GlyphCollection.h"
#include "Font.h"
#include "FontPeer.h"
#include "OpenGLPeer.h"
#include "OpenGLContext.h"
#include "ImageEvent.h"
#include "ImageListener.h"
#include "AbstractImage.h"
#include "PrintContext.h"
#include "Printable.h"
#include "RenderPaths.h"
#include "Stroke.h"
#include "Fill.h"
#include "BasicFill.h"
#include "StrokeState.h"
#include "FillState.h"
#include "FontState.h"
#include "BasicStroke.h"
#include "ClippingRegion.h"
#include "Composition.h"
#include "TileManager.h"
#include "GraphicsToolKit.h"
#include "UVMap.h"
#include "Vector2D.h"
#include "AlphaGamma.h"
#include "NoGraphicsToolkitFoundException.h"
#include "InvalidImage.h"



/**
*initializes the Graphics Kit runtime.
*This includes registering the basic classes in the runtime system
*This MUST be called first off, after the initFoundationKit, in the main() function of the app
*/



GRAPHICSKIT_API void initGraphicsKit();

GRAPHICSKIT_API void terminateGraphicsKit();


#endif //_GRAPHICS_KIT_H__