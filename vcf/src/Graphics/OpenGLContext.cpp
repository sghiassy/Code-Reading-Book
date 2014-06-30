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
#include "OpenGLContext.h"



using namespace VCF;


OpenGLContext::OpenGLContext( const unsigned long& width, const unsigned long& height ):
	GraphicsContext( width, height )
{
	init();
}

/*
OpenGLContext::OpenGLContext( Control* control ):
	GraphicsContext( control )	
{
	init();
}
*/

OpenGLContext::OpenGLContext( const long& contextID ):
	GraphicsContext( contextID )
{
	init();
}

OpenGLContext::~OpenGLContext()
{

}

void OpenGLContext::init()
{
	this->m_glPeer = NULL;
	GraphicsToolkit* toolkit = GraphicsToolkit::getDefaultGraphicsToolkit();
	if ( NULL != toolkit ){
		m_glPeer = toolkit->createOpenGLPeer( this );
	}
	else {
		//throw exception - no toolkit !!!!
	}
}

void OpenGLContext::initGL()
{
	this->m_glPeer->initGL();
}

void OpenGLContext::swapBuffers()
{
	this->m_glPeer->swapBuffers();
}

void OpenGLContext::makeCurrent()
{
	this->m_glPeer->makeCurrent();
}