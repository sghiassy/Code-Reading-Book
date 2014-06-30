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

// Win32Context.h: interface for the Win32Context class.

#ifndef _WIN32CONTEXT_H__
#define _WIN32CONTEXT_H__


#include <deque>

namespace VCF
{

/*
#define MAX_WIN32_CTXT_CACHE_COUNT		4

enum ContextState {
	CTXS_NULL=0,	
	CTXS_HANDLE_VALID=1	
};
*/

enum PrimitiveType{
	PRIMITIVE_MOVE=0,
	PRIMITIVE_LINE,
	PRIMITIVE_POLYLINE,
	PRIMITIVE_BEZIER,
	PRIMITIVE_RECT,
	PRIMITIVE_ELLIPSE,
	PRIMITIVE_ARC,
	PRIMITIVE_CLOSE
};


struct PointOperation {
	PointOperation( const double& ax=0.0, const double& ay=0.0 ) {
		x = ax;
		y = ay;
		primitive = PRIMITIVE_MOVE;
	}

	double x;
	double y;	
	PrimitiveType primitive;
};

struct TextOperation {
	TextOperation( const double& ax=0.0, const double& ay=0.0 ) {
		x = ax;
		y = ay;
	};

	long x;
	long y;
	String text;
};

class GRAPHICSKIT_API Win32Image;
/**
*A Context provides the lowest level graphics interface to the
*native systems 2D drawing operations. Based loosely on PostScript,
*a Context takes a series of of drawing operations, or commands 
*(lineTo, moveTo, etc), and then executes them by either filling or
*strokeing the path(s) that result from the commands. Thus calling 
*ellipse() will not draw anything till the strokePath() or fillPath()
*methods have been called. All path commands should add their information
*to a stack of some sort that is then looped through and executed
*using the appropriate native graphics calls. At each path command
*a test should be made as to whether the buffer should be cleared.
*The buffer should be cleared only after the stroke of fill methods
*have been  called. For example:
*
*<p><pre>
*	ellipse(23,23,45,67) //added to buffer 
*	moveTo(89,100) //add to buffer
*	lineTo(300,40) //add to buffer
*	strokePath()
*	fillPath()
*	rectangle(200,300,400,400)//buffer cleared, then add to buffer
*</pre></p>
*/
class GRAPHICSKIT_API Win32Context  : public ContextPeer, public Object
{
public:
	Win32Context();
	/**
	*Creates a new HDC from scratch
	*/
	Win32Context( const unsigned long& width, const unsigned long& height );	

	Win32Context( const long& contextID );	

	virtual ~Win32Context();

	virtual void setContext( GraphicsContext* context );

	virtual GraphicsContext* getContext();	

	virtual VCF::FontState* getCurrentFontState();

	virtual long getContextID();

	virtual void setContextID( const long& handle );

	virtual void setCurrentFontState(VCF::FontState * state);

	virtual void drawImage( const double & x, const double & y, Image* image );

	virtual void drawImageWithinBounds( Rect* bounds, Image* image );

	virtual void drawPartialImage( const double& x, const double& y, Rect* imageBounds, Image* image );

    virtual void textAt(const double & x, const double & y, const String & text);

	virtual void textBoundedBy( Rect* bounds, const String& text, const bool& wordWrap );

	virtual double getTextWidth( const String& text );

	virtual double getTextHeight( const String& text );

    virtual void rectangle(const double & x1, const double & y1, const double & x2, const double & y2);

    virtual void circle(const double & x, const double & y, const double & radius);

    virtual void ellipse(const double & x1, const double & y1, const double & x2, const double & y2, const double & angle=0.0);

    virtual void setCurrentStrokeState( StrokeState * state );

    virtual void setCurrentFillState( FillState * state );

	virtual StrokeState* getCurrentStrokeState();

    virtual FillState* getCurrentFillState();

    virtual void polyline(std::vector<Point*>& pts);

    virtual void curve(const double & x1, const double & y1, const double & x2, const double & y2,
                         const double & x3, const double & y3, const double & x4, const double & y4);

    virtual void lineTo(const double & x, const double & y);

    virtual void moveTo(const double & x, const double & y);

    virtual void fillPath();

    virtual void strokePath();

	void init();

	virtual void setOrigin( const double& x, const double& y );

	virtual Point* getOrigin();

	virtual void copyContext( const double& x, const double& y, ContextPeer* context );

	virtual bool isMemoryContext();

	//utility functions
	void copyToImage( Win32Image* image );	

	virtual void checkHandle(){};
	
	virtual void releaseHandle();

	virtual bool isXORModeOn();

	virtual void setXORModeOn( const bool& XORModeOn );

	virtual void setTextAlignment( const bool& alignTobaseline );

	virtual bool isTextAlignedToBaseline();
protected:

	HDC getDC();

	bool m_pathStarted;
	std::vector<PointOperation*> m_pathOperations;
	std::vector<TextOperation*> m_textOperations;
	Point m_currentMoveTo;
	Point m_oldOrigin;
	Point m_origin;
	bool m_inFillPath;
	HDC m_dc;
	HBITMAP m_memBitmap;
	FillState m_currentFill;
	StrokeState m_currentStroke;
	FontState m_currentFontState;

	void clearBuffer();
	void clearPathBuffer();
	void clearTextBuffer();
	void testBuffer();

	long translateFillStyle( const FillStyle& fillState );
	long translateStrokeStyle( const StrokeStyle& strokeStyle );
	long translateHatch( const FillStyle& fillState );
	void execPathOperations();

	bool m_isMemoryCtx;
	GraphicsContext* m_context;
	bool m_isXORModeOn;

	/**
	*Utility function to draw a transparent bitmap
	*/
	static void drawTransparentBitmap(HDC hdc, HBITMAP hBitmap, short xStart,
                           short yStart, COLORREF cTransparentColor);

	bool m_alignToBaseline;
};

};

#endif 
