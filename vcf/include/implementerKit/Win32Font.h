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

#ifndef _WIN32FONT_H__
#define _WIN32FONT_H__


namespace VCF {

class GRAPHICSKIT_API Win32Font : public FontPeer , public Object {

public:
	Win32Font( const String& fontName );
	
	Win32Font( const String& fontName, const double& pointSize );

	virtual ~Win32Font();

	void init();

	/**
	*returns a integer representing some native handle to a font structure.
	*What this actually is depends on the Windowing system implementation.
	*Under Win32 this represents a pointer to a LOGFONT structure.
	*/
	virtual ulong32 getFontHandleID();

	virtual String getName();

	virtual void setName( const String& name );

	/**
	*is this font a TrueType font ?
	*/
	virtual bool isTrueType();

	/**
	*returns the Color the Font will be rendered in
	*/
	virtual Color* getColor();

	/**
	*sets the Color to render the FontImplmenter in
	*/
    virtual void setColor( Color* color );

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
    virtual double getPointSize();

	/**
	*sets the point size of the FontPeer
	*/
	virtual void setPointSize( const double pointSize );

	virtual double getPixelSize();

	virtual void setPixelSize( const double pixelSize );

    virtual void setBold( const bool& bold );    

    virtual bool getBold();

    virtual bool getItalic();

    virtual void setItalic( const bool& italic );

    virtual bool getUnderlined();

    virtual void setUnderlined( const bool& underlined );

    virtual bool getStrikeOut();

    virtual void setStrikeOut( const bool& strikeout );

    virtual double getShear();

    virtual void setShear(const double& shear );

    virtual double getAngle();

    virtual void setAngle( const double& angle );

	/**
	*returns a GlyphCollection that represents the set of shapes 
	*for the specified text string.
	*@param String text - the group of characters to put in the GlyphCollection
	*@return GlyphCollection the collection of Glyphs, or vectorized shapes that
	*represent the text string.
	*/
    virtual GlyphCollection* getGlyphCollection( const String& text );

	virtual bool isEqual( Object* object );
private:
	double m_shear;
	Color m_color;
	LOGFONT m_logFont;
	double m_pointSize;
	String m_fontName;
};

}; //end of namespace VCF

#endif //_WIN32FONT_H__