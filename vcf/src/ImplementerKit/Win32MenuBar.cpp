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

// Win32MenuBar.cpp

#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"

using namespace VCF;


Win32MenuBar::Win32MenuBar(  MenuBar* menuBar  )
{
	m_frame = NULL;
	this->m_menuRoot = menuBar->getRootMenuItem();
}

Win32MenuBar::~Win32MenuBar()
{

}

void Win32MenuBar::setFrame( Frame* frame )
{
	m_frame = frame;
	if ( (NULL != m_frame) && (NULL != m_menuRoot) ){		
		ControlPeer* Peer = m_frame->getPeer();
		HWND wnd = (HWND)Peer->getHandleID();
		
		MenuItemPeer* itemImpl = m_menuRoot->getPeer();
		
		HMENU menu = (HMENU) itemImpl->getMenuID();
		SetMenu( wnd, menu );
	}
}

void Win32MenuBar::update()
{
	if ( (NULL != m_frame) && (NULL != m_menuRoot) ){		
		ControlPeer* Peer = m_frame->getPeer();
		HWND wnd = (HWND)Peer->getHandleID();

		DrawMenuBar( wnd );
	}
}

void Win32MenuBar::setMenuItems( MenuItem* item )
{
	m_menuRoot = item;
}