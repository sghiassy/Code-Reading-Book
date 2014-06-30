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

// Win32Component.cpp

#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"


using namespace VCF;

using namespace VCFWin32;


Win32Component::Win32Component( Control* component ):
	AbstractWin32Component( component )
{
	String className = "VCF::Win32Component";
	if ( true != this->isRegistered() ){
		registerWin32Class( className, m_wndProc  );
	}

	Win32ToolKit* toolkit = (Win32ToolKit*)UIToolkit::getDefaultUIToolkit();
	HWND parent = toolkit->getDummyParent();		

	this->m_hwnd = ::CreateWindowEx( this->m_exStyleMask, className.c_str(), 
									 this->m_windowCaption.c_str(),	this->m_styleMask, 
		                             this->m_bounds.m_left, this->m_bounds.m_top, 
									 this->m_bounds.getWidth(), this->m_bounds.getHeight(), parent, 
									 NULL, ::GetModuleHandle(NULL), NULL );

	if ( NULL != this->m_hwnd ){
		Win32Object::registerWin32Object( this );	
	}
	else {
		//throw exception
	}

	this->setCreated( true );
}

Win32Component::~Win32Component()
{

}

