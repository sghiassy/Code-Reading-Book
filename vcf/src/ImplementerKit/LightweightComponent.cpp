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

// LightweightComponent.cpp

#include "ApplicationKit.h"
#include "LightweightComponent.h"


using namespace VCF;

LightweightComponent::LightweightComponent( Control* component )
{
	this->m_component = component;
}

LightweightComponent::~LightweightComponent()
{

}

long LightweightComponent::getHandleID()
{	
	long result = -1;
	VCF::Control* parent = getHeavyWeightParent();
	if ( NULL != parent ) {
		result = parent->getPeer()->getHandleID();
	}
	return result;
}

VCF::String LightweightComponent::getText()
{
	VCF::String result = "";

	return result;
}

void LightweightComponent::setText( const VCF::String& text )
{
	
}

void LightweightComponent::setBounds( VCF::Rect* rect )
{
	m_bounds.inflate(1,1);

	VCF::Control* parent = getHeavyWeightParent();
	if ( NULL != parent ){
		parent->repaint( &m_bounds );
	}
	this->m_bounds.setRect( rect->m_left, rect->m_top, rect->m_right, rect->m_bottom );
	VCF::Size sz( m_bounds.getWidth(), m_bounds.getHeight() );
	VCF::ControlEvent event( m_component, sz );
	m_component->processControlEvent( &event );					
}

VCF::Rect* LightweightComponent::getBounds()
{	
	return &m_bounds;
}

void LightweightComponent::setVisible( const bool& visible )
{
	this->m_visible = visible;
}

bool LightweightComponent::getVisible()
{	
	return this->m_visible;
}

unsigned long LightweightComponent::getStyleMask()
{	
	return 0;
}

void LightweightComponent::setStyleMask( const unsigned long& styleMask )
{

}

VCF::Control* LightweightComponent::getControl()
{
	return m_component;
}

void LightweightComponent::setControl( VCF::Control* component )
{
	this->m_component = component;
}

void LightweightComponent::setParent( VCF::Control* parent )
{
	
}

VCF::Control* LightweightComponent::getParent()
{
	return m_component->getParent();
}

bool LightweightComponent::isFocused()
{	
	return false;
}

void LightweightComponent::setFocus( const bool& focused )
{
	
}

bool LightweightComponent::isEnabled()
{	
	return false;
}

void LightweightComponent::setEnabled( const bool& enabled )
{
	
}

void LightweightComponent::setFont( Font* font )
{
	
}

VCF::Control* LightweightComponent::getHeavyWeightParent()
{
	Control* result = NULL;

	VCF::Control* parent = this->getParent();
	if ( NULL != parent ){
		bool heavyWeightParent = !parent->isLightWeight();
		result = parent;
		while ( (false == heavyWeightParent) && (parent!=NULL) ){
			parent = result->getParent();
			if ( NULL != parent ){
				heavyWeightParent = !parent->isLightWeight();
				if ( false == heavyWeightParent ){
					result = parent;
					parent = NULL;
				}
			}
		}
	}
	result = parent;
	return result;
}

void LightweightComponent::repaint( Rect* repaintRect )
{
	VCF::Control* tmp = NULL;
	VCF::Control* parent = getHeavyWeightParent();
	if ( NULL != parent ){
		Rect tmpRect;
		if ( NULL == repaintRect ){
			tmpRect = *this->m_component->getBounds();
			tmpRect.inflate( 2, 2 );
			repaintRect = &tmpRect;
		}
		parent->repaint( repaintRect );
	}
}

void LightweightComponent::keepMouseEvents()
{
	Control* parent = this->getHeavyWeightParent();
	if ( NULL != parent ){
		parent->keepMouseEvents();
		Control::setCapturedMouseControl( this->m_component );
	}
}

void LightweightComponent::releaseMouseEvents()
{
	Control* parent = this->getHeavyWeightParent();
	if ( NULL != parent ){
		parent->releaseMouseEvents();
	}
}

void LightweightComponent::setCursor( Cursor* cursor )
{
	Control* parent = this->getHeavyWeightParent();
	if ( NULL != parent ){
		parent->getPeer()->setCursor( cursor );
	}
}