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
#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"

using namespace VCF;

Win32PopupMenu::Win32PopupMenu( PopupMenu* popupMenu )
{
	m_control = NULL;
	m_popupMenu = popupMenu;	
}

Win32PopupMenu::~Win32PopupMenu()
{

}

void Win32PopupMenu::setControl( Control* control )
{
	m_control = control;
}

void Win32PopupMenu::popup( Point* pt )
{
	if ( (NULL != m_control) && (NULL != pt) ){
		MenuItem* rooItem = m_popupMenu->getRootMenuItem();
		if  ( NULL != rooItem )  {
			MenuItemPeer* peer = rooItem->getPeer();
			ControlPeer* controlPeer = m_control->getPeer();
			if ( (NULL == peer) || (NULL == controlPeer) ){
				//throw exception
			}
			HMENU menuHandle = (HMENU)peer->getMenuID();
			UINT flags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY;
			HWND wnd = (HWND)controlPeer->getHandleID();
			POINT tmpPt = {0};
			tmpPt.x = pt->m_x;
			tmpPt.y = pt->m_y;
			ClientToScreen( wnd, &tmpPt );
			UINT retVal = ::TrackPopupMenu( menuHandle, flags, tmpPt.x, tmpPt.y, 0, wnd, NULL );
			if ( 0 != retVal ){
				MenuItem* item = Win32MenuItem::getMenuItemFromID( retVal );
				if ( NULL != item ){						
					item->click();
				}
			}
		}
	}
}
