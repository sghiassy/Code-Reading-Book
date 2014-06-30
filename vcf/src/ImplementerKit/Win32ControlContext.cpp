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
#include "RuntimeException.h"


using namespace VCF;

Win32ControlContext::Win32ControlContext( ControlPeer* controlPeer )
{
	Win32Context::init();
	if ( NULL == controlPeer ){
		//throw exception 
	}

	//this->m_dc = GetDC( (HWND)controlPeer->getHandleID() );	
}

Win32ControlContext::~Win32ControlContext()
{

}

void Win32ControlContext::setContext( GraphicsContext* context )
{
	Win32Context::setContext ( context );
	this->m_owningControlCtx  = dynamic_cast<ControlGraphicsContext*>( context );
}

Control* Win32ControlContext::getOwningControl()
{
	Control* result = NULL;

	if ( NULL != this->m_owningControlCtx ){
		result = m_owningControlCtx->getOwningControl();
	}
	return result;
}

void Win32ControlContext::releaseHandle()
{
	Win32Context::releaseHandle();

	if ( (NULL != this->m_owningControlCtx->getOwningControl()) && (NULL != m_dc) ){
		Control* owningControl = m_owningControlCtx->getOwningControl();
		ControlPeer* peer = owningControl->getPeer();
		if ( true == owningControl->isLightWeight() ) {
			BOOL result = ::SetViewportOrgEx( m_dc, m_oldOrigin.m_x, m_oldOrigin.m_y, NULL );
			if ( FALSE == result ) {
				throw RuntimeException( MAKE_ERROR_MSG_2("SetViewportOrgEx() failed for win32 Context") );
			}
		}
		

		::ReleaseDC( (HWND)peer->getHandleID(), m_dc );
	}
	
}

void Win32ControlContext::checkHandle()
{	
	if ( NULL != this->m_owningControlCtx->getOwningControl() ){
		Control* owningControl = m_owningControlCtx->getOwningControl();
		ControlPeer* peer = owningControl->getPeer();
		m_dc = ::GetDC( (HWND)peer->getHandleID() );

		if ( NULL == this->m_dc ){
			//throw exception !!!!!!
			throw RuntimeException( MAKE_ERROR_MSG_2("Win32 Context has NULL Device Context") );
		}

		//check to see if we need to offset the origin
		if ( true == owningControl->isLightWeight() ) {
			Rect* bounds = owningControl->getBounds();						
			double originX = bounds->m_left;
			double originY = bounds->m_top;
			POINT oldOrigin = {0};
			BOOL result = ::SetViewportOrgEx( m_dc, originX, originY, &oldOrigin );	
			if ( FALSE == result ) {
				throw RuntimeException( MAKE_ERROR_MSG_2("SetViewportOrgEx() failed for win32 Context") );
			}
			m_oldOrigin.m_x = oldOrigin.x;
			m_oldOrigin.m_y = oldOrigin.y;
		}
	}	
}

