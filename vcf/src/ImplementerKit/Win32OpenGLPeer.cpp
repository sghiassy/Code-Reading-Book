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
//#include "Win32Peer.h"
#include "Win32OpenGLPeer.h"
#include "Win32Context.h"
#include "VCFOpenGL.h"

//gl stuff


using namespace VCF;

Win32OpenGLPeer::Win32OpenGLPeer( GraphicsContext* glContext )
{
	this->m_glContext = glContext;
	m_isInitialized = false;
}

Win32OpenGLPeer::~Win32OpenGLPeer()
{
	if ( NULL != m_glContext ){
		Win32Context* win32Ctx = dynamic_cast<Win32Context*>(m_glContext->getPeer());
		if ( NULL == win32Ctx ){
			//throw exception - we are screwed !!!!
		}
		HDC dc = (HDC)win32Ctx->getContextID();
		wglMakeCurrent( dc, NULL );
		wglDeleteContext( m_hrc );
	}
}

void Win32OpenGLPeer::initGL()
{
	if ( true == m_isInitialized ){
		return;
	}
	if ( NULL != m_glContext ){
		Win32Context* win32Ctx = dynamic_cast<Win32Context*>(m_glContext->getPeer());
		if ( NULL == win32Ctx ){
			//throw exception - we are screwed !!!!
		}
		
		win32Ctx->checkHandle();

		HDC dc = (HDC)win32Ctx->getContextID();
		if ( NULL != dc ){
			PIXELFORMATDESCRIPTOR pfd =
			{
				sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
					1,                              // version number
					PFD_DRAW_TO_WINDOW |            // support window
					PFD_SUPPORT_OPENGL |          // support OpenGL
					PFD_DOUBLEBUFFER,             // double buffered
					PFD_TYPE_RGBA,                  // RGBA type
					24,                             // 24-bit color depth
					0, 0, 0, 0, 0, 0,               // color bits ignored
					0,                              // no alpha buffer
					0,                              // shift bit ignored
					0,                              // no accumulation buffer
					0, 0, 0, 0,                     // accum bits ignored
					32,                             // 32-bit z-buffer
					0,                              // no stencil buffer
					0,                              // no auxiliary buffer
					PFD_MAIN_PLANE,                 // main layer
					0,                              // reserved
					0, 0, 0                         // layer masks ignored
			};
			
			int pixelformat = ChoosePixelFormat( dc, &pfd );
			if ( pixelformat == 0 ){
				//throw exception
				pixelformat = GetPixelFormat( dc ); 
				DescribePixelFormat( dc, pixelformat, 
					                 sizeof(PIXELFORMATDESCRIPTOR), &pfd); 

			}
			
			if ( FALSE == SetPixelFormat( dc, pixelformat, &pfd ) ){
				//throw exception
			}
			
			m_hrc = wglCreateContext( dc );
			m_isInitialized = ( TRUE == wglMakeCurrent( dc, m_hrc ) ) ? true : false;
			win32Ctx->releaseHandle();
		}
	}
}

void Win32OpenGLPeer::swapBuffers()
{
	if ( NULL != m_glContext ){
		Win32Context* win32Ctx = dynamic_cast<Win32Context*>(m_glContext->getPeer());
		if ( NULL == win32Ctx ){
			//throw exception - we are screwed !!!!
		}
		win32Ctx->checkHandle();

		HDC dc = (HDC)win32Ctx->getContextID();		

		::SwapBuffers( dc );

		win32Ctx->releaseHandle();
	}
}

void Win32OpenGLPeer::makeCurrent()
{
	if ( NULL != m_glContext ){
		Win32Context* win32Ctx = dynamic_cast<Win32Context*>(m_glContext->getPeer());
		if ( NULL == win32Ctx ){
			//throw exception - we are screwed !!!!
		}
		win32Ctx->checkHandle();

		HDC dc = (HDC)win32Ctx->getContextID();

		wglMakeCurrent( dc, m_hrc );

		win32Ctx->releaseHandle();
	}
}