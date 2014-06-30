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

//FontPeer.h

#ifndef _FONT_PEER_H__
#define _FONT_PEER_H__

#include "VCF.h"

namespace VCF
{

class GlyphCollection;

class Color;


/**
*Represents the native windowing system's representation of a Font. 
*/
class GRAPHICSKIT_API FontPeer {
public:
	virtual ~FontPeer(){};

	/**
	*returns a integer representing some native handle to a font structure.
	*What this actually is depends on the Windowing system implementation.
	*Under Win32 this represents a pointer to a LOGFONT structure.
	*/
	virtual ulong32 getFontHandleID() = 0;

	virtual String getName() = 0;

	virtual void setName( const String& name ) = 0;

	/**
	*is this font a TrueType font ?
	*/
	virtual bool isTrueType() = 0;

	/**
	*returns the Color the Font will be rendered in
	*/
	virtual Color* getColor() = 0;

	/**
	*sets the Color to render the FontImplmenter in
	*/
    virtual void setColor( Color* color ) = 0;

	/**
	*return the point size of the Font. One point is 1/72 of an inch
	*( or 0.0352552 cm for our more civilized friends !), so to figure 
	*out the pixels involved, find out the Pixels per Inch and then apply
	*the following formula
	*<code>
	* (PointSize / 72) * PPI
	*</code>
	*where PPI represents the Pixels Per Inch
	*/
    virtual double getPointSize() = 0;

	/**
	*sets the point size of the FontPeer
	*/
	virtual void setPointSize( const double pointSize ) = 0;

	virtual double getPixelSize() = 0;

	virtual void setPixelSize( const double pixelSize ) = 0;

    virtual void setBold( const bool& bold ) = 0;    

    virtual bool getBold() = 0;

    virtual bool getItalic() = 0;

    virtual void setItalic( const bool& italic ) = 0;

    virtual bool getUnderlined() = 0;

    virtual void setUnderlined( const bool& underlined ) = 0;

    virtual bool getStrikeOut() = 0;

    virtual void setStrikeOut( const bool& strikeout ) = 0;

    virtual double getShear() = 0;

    virtual void setShear(const double& shear ) = 0;

    virtual double getAngle() = 0;

    virtual void setAngle( const double& angle ) = 0;

	/**
	*returns a GlyphCollection that represents the set of shapes 
	*for the specified text string.
	*@param String text - the group of characters to put in the GlyphCollection
	*@return GlyphCollection the collection of Glyphs, or vectorized shapes that
	*represent the text string.
	*/
    virtual GlyphCollection* getGlyphCollection( const String& text ) = 0;
};

};

#endif//_FONT_PEER_H__