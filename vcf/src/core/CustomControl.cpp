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

//CustomControl.cpp

#include "ApplicationKit.h"



using namespace VCF;


CustomControl::CustomControl():
	Control()
{	
	m_Peer =	UIToolkit::getDefaultUIToolkit()->createControlPeer( this );
	m_isLightweight = false;
	//this->m_customControlPeer = (CustomControlPeer*)m_Peer;
	this->init();
}

CustomControl::CustomControl( const bool& useHeavyweightPeer ):
	Control()
{
	if ( true == useHeavyweightPeer ){
		m_Peer = UIToolkit::getDefaultUIToolkit()->createControlPeer( this, CT_HEAVYWEIGHT );
	}
	else{
		m_Peer = UIToolkit::getDefaultUIToolkit()->createControlPeer( this, CT_LIGHTWEIGHT );
	}
	this->m_customControlPeer = (CustomControlPeer*)m_Peer;
	m_isLightweight = !useHeavyweightPeer;
	this->init();
}

CustomControl::~CustomControl()
{
}

void CustomControl::paint(GraphicsContext * context)
{
	Border* border = this->getBorder();
	
	Rect innerBounds(0,0,this->getWidth(), this->getHeight() );

	if ( NULL != border ){
		border->paint( this, context );
		Rect* tmpBounds = border->getClientRect( this );
		innerBounds.setRect( tmpBounds->m_left, tmpBounds->m_top, tmpBounds->m_right, tmpBounds->m_bottom );
		innerBounds.inflate( -1, -1 );
	}
	if ( true != m_isTransparent){
		FillState fill;
		fill.m_Color = *(this->getColor());
		context->rectangle( innerBounds.m_left, innerBounds.m_top, innerBounds.m_right, innerBounds.m_bottom );
		context->setCurrentFillState( &fill );
		context->fillPath();
	}

	context->setCurrentFont( getFont() );
}

void CustomControl::init()
{	
	this->setEnabled( true );
	this->setVisible( true );
	this->setFocus( true );
}

bool CustomControl::isLightWeight()
{
	return m_isLightweight;
}

bool CustomControl::isTransparent()
{
	return m_isTransparent;
}

void CustomControl::setTransparent( const bool& transparent )
{
	this->m_isTransparent = transparent;
}