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

//Window.cpp
#include "ApplicationKit.h"




using namespace VCF;

Window::Window()
{
	INIT_EVENT_HANDLER_LIST(WindowClose)
	INIT_EVENT_HANDLER_LIST(WindowRestore)
	INIT_EVENT_HANDLER_LIST(WindowMaximize)
	INIT_EVENT_HANDLER_LIST(WindowMinimize)
	
	this->m_Peer = UIToolkit::getDefaultUIToolkit()->createWindowPeer( this );
	if ( NULL == m_Peer ){
		throw InvalidPeer( MAKE_ERROR_MSG(NO_PEER), __LINE__ );
	}
	
	m_Peer->setControl( this );
	m_windowPeer = (WindowPeer*)m_Peer;
	
	m_menuBar = NULL;

	setHeight( getPreferredHeight() );

	setWidth( getPreferredWidth() );
}


Window::~Window()
{
	StringUtils::traceWithArgs( "Window::~Window() @%p", this );	
}

void Window::paint(GraphicsContext * context)
{
	Frame::paint( context );
}

void Window::setCaption( const String& caption )
{
	Frame::setCaption( caption );
	this->m_Peer->setText( caption );
}

Rect* Window::getClientBounds()
{
	if ( NULL == m_windowPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);	
	}
	return this->m_windowPeer->getClientBounds();
}

void  Window::setClientBounds( Rect* bounds )
{
	if ( NULL == m_windowPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);	
	}
	m_windowPeer->setClientBounds( bounds );
}

void Window::resizeChildren()
{
	Rect* bounds = getClientBounds();
	Rect rect( 0.0, 0.0, bounds->getWidth(), bounds->getHeight() );
	this->resizeChildrenUsingBounds( &rect );
}

void Window::beforeDestroy( ComponentEvent* event )
{

	//remove me from my parent control if neccessary
	Control* parent = this->getParent();
	if ( NULL != parent ) {
		Container* parentContainer = dynamic_cast<Container*>(parent);
		if ( NULL != parentContainer ) {
			parentContainer->remove( this );
		}
	}
	
	if ( NULL != m_menuBar ){
		delete m_menuBar;
		m_menuBar = NULL;
	}
}

MenuBar* Window::getMenuBar()
{
	return this->m_menuBar;
}

void Window::setMenuBar( MenuBar* menuBar )
{
	this->m_menuBar = menuBar;
	if ( NULL != m_menuBar ){
		m_menuBar->setFrame( this );
	}
}

void Window::close() 
{
	WindowEvent event( this, WINDOW_EVENT_CLOSE );
	fireOnWindowClose( &event );

	if ( NULL == m_windowPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);	
	}
	m_windowPeer->close();
}

void Window::setFrameStyle( const FrameStyleType& frameStyle )
{
	if ( NULL == m_windowPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);	
	}
	
	
	m_frameStyle = frameStyle;

	if ( CS_DESIGNING != this->getComponentState() ){
		m_windowPeer->setFrameStyle( m_frameStyle );
	}
}

void Window::setFrameTopmost( const bool& isTopmost )
{
	Frame::setFrameTopmost( isTopmost );
	
	if ( CS_DESIGNING != this->getComponentState() ){ 
		if ( NULL == m_windowPeer ){
			throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);	
		}
		
		m_windowPeer->setFrameTopmost( isTopmost );
	}
}

double Window::getPreferredWidth()
{
	return 500.0;
}

double Window::getPreferredHeight()
{
	return 500.0;
}