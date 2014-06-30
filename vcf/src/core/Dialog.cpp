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

//Dialog.cpp
#include "ApplicationKit.h"



using namespace VCF;

Dialog::Dialog( Frame* owner )
{		
	m_owner = owner;
	this->m_Peer = UIToolkit::getDefaultUIToolkit()->createDialogPeer( m_owner, this );
	if ( NULL == m_Peer ){
		throw InvalidPeer( MAKE_ERROR_MSG(NO_PEER), __LINE__ );
	}
	m_Peer->setControl( this );
	this->m_dialogPeer = (DialogPeer*)m_Peer;
}


Dialog::~Dialog() 
{

}

void Dialog::paint(GraphicsContext * context)
{
	Frame::paint( context );
}

void Dialog::setCaption( const String& caption )
{
	if ( NULL == m_dialogPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);	
	}
	this->m_caption = caption;
	this->m_dialogPeer->setCaption( m_caption );
}

Rect* Dialog::getClientBounds()
{
	if ( NULL == m_dialogPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);	
	}
	return this->m_dialogPeer->getClientBounds();
}

void  Dialog::setClientBounds( Rect* bounds )
{
	if ( NULL == m_dialogPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);	
	}
	m_dialogPeer->setClientBounds( bounds );
}

void Dialog::resizeChildren()
{
	Rect* bounds = getClientBounds();
	Rect rect( 0.0, 0.0, bounds->getWidth(), bounds->getHeight() );

	this->resizeChildrenUsingBounds( &rect );
}

ModalReturnType Dialog::showModal()
{
	this->setVisible( true );
	if ( NULL == m_dialogPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);	
	}
	Control* owningControl = m_owner;
	if ( NULL == owningControl ) {
		Application* app = Application::getRunningInstance();
		if ( NULL != app ) {
			owningControl = app->getMainWindow();
		}		
	}
	if ( NULL != owningControl ) {
		Rect bounds;
		bounds.m_left = owningControl->getLeft() + ( owningControl->getWidth()/2.0 - getWidth()/2.0 );
		bounds.m_top = owningControl->getTop() + ( owningControl->getHeight()/2.0 - getHeight()/2.0 );
		bounds.m_right = bounds.m_left + getWidth();
		bounds.m_bottom = bounds.m_top + getHeight();
		setBounds( &bounds );
	}
	return m_dialogPeer->showModal();
}

void Dialog::show()
{
	if ( NULL == m_dialogPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);	
	}

	m_dialogPeer->show();
}


void Dialog::showMessage( const String& message, const String& caption )
{
	DialogPeer* dialogPeer = UIToolkit::getDefaultUIToolkit()->createDialogPeer();// owner, this );
	if ( NULL != dialogPeer ){
		dialogPeer->showMessage( message, caption );
	}

	delete dialogPeer;
	dialogPeer = NULL;
}

ModalReturnType Dialog::showMessage( const String& message, const MessageStyle& messageStyle, const String& caption )
{
	ModalReturnType result = MRT_OK;
	
	DialogPeer* dialogPeer = UIToolkit::getDefaultUIToolkit()->createDialogPeer();// owner, this );
	if ( NULL != dialogPeer ){
		result = dialogPeer->showMessage( message, caption, messageStyle );
	}

	delete dialogPeer;
	dialogPeer = NULL;

	return result;
}

void Dialog::close()
{
	if ( NULL == m_dialogPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);	
	}

	m_dialogPeer->close();
}

void Dialog::setFrameStyle( const FrameStyleType& frameStyle )
{
		if ( NULL == m_dialogPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);	
	}
	m_frameStyle = frameStyle;

	if ( CS_DESIGNING != this->getComponentState() ){
		m_dialogPeer->setFrameStyle( m_frameStyle );
	}
	
}