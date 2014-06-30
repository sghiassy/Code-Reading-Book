//Win32GraphicsToolkit.h

/**
Copyright (c) 2000-2001, Jim Crafton
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
	Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in 
	the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

NB: This software will not save the world. 
*/

#ifndef _WIN32GRAPHICSTOOLKIT_H__
#define _WIN32GRAPHICSTOOLKIT_H__




namespace VCF  {

/**
*Class Win32GraphicsToolkit documentation
*/
class Win32GraphicsToolkit : public GraphicsToolkit { 
public:
	Win32GraphicsToolkit();

	virtual ~Win32GraphicsToolkit();

	virtual ContextPeer* createContextPeer( const unsigned long& width, const unsigned long& height );	

	virtual ContextPeer* createContextPeer( const long& contextID );

	virtual FontPeer* createFontPeer( const String& fontName );

	virtual FontPeer* createFontPeer( const String& fontName, const double& pointSize );
	
	virtual OpenGLPeer* createOpenGLPeer( GraphicsContext* glContext );
	
	virtual Image* createImage( const unsigned long& width, const unsigned long& height );

	virtual Image* createImage( GraphicsContext* context, Rect* rect );	

protected:
	static Win32GraphicsToolkit win32GraphicsToolkitInstance;
	void loadSystemColors();
private:
};


}; //end of namespace VCF

#endif //_WIN32GRAPHICSTOOLKIT_H__


