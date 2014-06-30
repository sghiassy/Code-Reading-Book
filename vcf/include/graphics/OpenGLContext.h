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

#ifndef _OPENGLCONTEXT_H__
#define _OPENGLCONTEXT_H__


namespace VCF {

class OpenGLPeer;

class GRAPHICSKIT_API OpenGLContext : public GraphicsContext{

public:
	/**
	*Creates a new blank graphics context of the specified width and height
	*/
	OpenGLContext( const unsigned long& width, const unsigned long& height );
	
	/**
	*Creates a context based on the component. The context is attached to the component
	*for the duration of it's lifetime.
	*/
	//OpenGLContext( Control* control );

	/**
	*Create a new context based on a context ID. See ContextPeer getContextID()
	*Doing this should caryy over any of the current settings of the context that the contextID represents.
	*/
	OpenGLContext( const long& contextID );

	virtual ~OpenGLContext();

	void initGL();

	void init();

	void swapBuffers();

	void makeCurrent();
private:
	OpenGLPeer* m_glPeer;
};

}; //end of namespace VCF

#endif //_OPENGLCONTEXT_H__