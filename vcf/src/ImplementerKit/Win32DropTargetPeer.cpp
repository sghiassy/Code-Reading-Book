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
//Win32DropTargetPeer.cpp
#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"


using namespace VCF;


Win32DropTargetPeer::Win32DropTargetPeer()
{
	m_dropTarget = NULL;
	m_comDropTarget = new VCFCOM::DropTarget();
	m_comDropTarget->setPeer( this );
}

Win32DropTargetPeer::~Win32DropTargetPeer()
{
	delete m_comDropTarget;
}

void Win32DropTargetPeer::registerTarget( Control* control )
{
	

	if ( NULL != control ){
		ControlPeer* peer = control->getPeer();
		if ( NULL != peer ){
			HWND hwnd = (HWND)peer->getHandleID();
			if ( NULL != hwnd ){
				IDropTarget* dt = static_cast<IDropTarget*>(this->m_comDropTarget);
				HRESULT result = RegisterDragDrop( hwnd, dt );
				switch ( result ){
					case S_OK :{
						StringUtils::trace("RegisterDragDrop returned S_OK\n");
					}
					break;

					case DRAGDROP_E_INVALIDHWND :{
						StringUtils::trace("RegisterDragDrop returned DRAGDROP_E_INVALIDHWND\n");
					}
					break;

					case DRAGDROP_E_ALREADYREGISTERED:{
						StringUtils::trace("RegisterDragDrop returned DRAGDROP_E_ALREADYREGISTERED\n");
					}
					break;

					default :{
						StringUtils::trace("RegisterDragDrop returned some other error\n");
					}
					break;
				}
			}
		}
	}
}

void Win32DropTargetPeer::unregisterTarget( Control* control )
{
	if ( NULL != control ){
		ControlPeer* peer = control->getPeer();
		if ( NULL != peer ){
			HWND hwnd = (HWND)peer->getHandleID();
			if ( NULL != hwnd ){
				RevokeDragDrop( hwnd );
			}
		}
	}
}

DropTarget* Win32DropTargetPeer::getDropTarget()
{
	return this->m_dropTarget;
}

void Win32DropTargetPeer::setDropTarget( DropTarget* dropTarget )
{
	this->m_dropTarget = dropTarget;
}
