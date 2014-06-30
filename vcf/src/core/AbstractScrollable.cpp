//AbstractScrollable.h
/**
Copyright (c) 2000-2001, Jim Crafton
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
	Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in 
	the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

NB: This software will not save the world. 
*/
#include "ApplicationKit.h"
#include "Scrollable.h"
#include "AbstractScrollable.h"
#include "ScrollPeer.h"

using namespace VCF;

AbstractScrollable::ControlResizeHandler::ControlResizeHandler( AbstractScrollable* scrollable )
{
	m_scrollable = scrollable;
	ControlEventHandler<ControlResizeHandler>* ch = 
		new ControlEventHandler<ControlResizeHandler>( this, AbstractScrollable::ControlResizeHandler::controlResized, "ControlHandler" );
}

void AbstractScrollable::ControlResizeHandler::controlResized( ControlEvent* event )
{
	m_scrollable->onControlResized( event );
}

AbstractScrollable::AbstractScrollable()
{
	this->m_scrollableControl = NULL;
	
	
	m_resizeHandler = new ControlResizeHandler(this);

	m_scrollPeer = UIToolkit::getDefaultUIToolkit()->createScrollPeer( NULL ) ;

	m_hasVertScrollbar = false;
	m_hasHorzScrollbar = false;
	m_virtualViewHeight = 0.0;
	m_virtualViewWidth = 0.0;
	m_vertPosition = 0.0;
	m_horzPosition = 0.0;	
	m_topScrollSpace = 0.0;
	m_bottomScrollSpace = 0.0;
	m_leftScrollSpace = 0.0;
	m_rightScrollSpace = 0.0;	
}

AbstractScrollable::~AbstractScrollable()
{
	delete m_resizeHandler;
}

void AbstractScrollable::setScrollableControl( Control* scrollableControl ) {
	
	EventHandler* ch = m_resizeHandler->getEventHandler( "ControlHandler" );
	if ( NULL != m_scrollableControl ) {
		m_scrollableControl->removeControlSizedHandler( ch );
	}
	m_scrollableControl = scrollableControl;
	if ( NULL != m_scrollableControl ) {
		m_scrollableControl->addControlSizedHandler( ch );
	}
	m_scrollPeer->setScrollableControl( m_scrollableControl );
}

void AbstractScrollable::setVerticalTopScrollSpace( const double& topScrollSpace )
{
	this->m_topScrollSpace = topScrollSpace;
	this->recalcScrollPositions();
}

void AbstractScrollable::setVerticalBottomScrollSpace( const double& bottomScrollSpace )
{
	this->m_bottomScrollSpace = bottomScrollSpace;
	this->recalcScrollPositions();
}

void AbstractScrollable::setHorizontalLeftScrollSpace( const double& leftScrollSpace )
{
	this->m_leftScrollSpace = leftScrollSpace;
	this->recalcScrollPositions();
}

void AbstractScrollable::setHorizontalRightScrollSpace( const double& rightScrollSpace )
{
	this->m_rightScrollSpace = rightScrollSpace;
	this->recalcScrollPositions();
}

void AbstractScrollable::onControlResized( ControlEvent* event )
{
	Rect bounds( 0, 0, m_scrollableControl->getWidth(), m_scrollableControl->getHeight() );
	m_scrollPeer->recalcScrollPositions( this );
}

void AbstractScrollable::recalcScrollPositions()
{
	if ( NULL != this->m_scrollableControl ) {
		m_scrollPeer->recalcScrollPositions( this );
	}
}

void AbstractScrollable::setVerticalPosition( const double& vertPosition ) 
{
	m_vertPosition = vertPosition;
	if ( NULL != this->m_scrollableControl ) {
		m_scrollPeer->scrollTo( m_horzPosition, m_vertPosition );
	}
}

void AbstractScrollable::setHorizontalPosition( const double& horzPosition ) {
	m_horzPosition = horzPosition;
	if ( NULL != this->m_scrollableControl ) {
		m_scrollPeer->scrollTo( m_horzPosition, m_vertPosition );
	}
}