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

//Frame.cpp
#include "ApplicationKit.h"


using namespace VCF;


Frame::Frame()
{
	this->setContainer( this );
	m_isTopmostFrame = false;
	m_frameStyle = FST_SIZEABLE;
}

void Frame::paint( GraphicsContext * context )
{
	Rect paintRect( 0.0, 0.0, this->getWidth(), this->getHeight() );
	Color* fillColor = this->getColor();
	context->setColor( fillColor );
	context->rectangle( &paintRect );
	context->fillPath();
	
	this->paintChildren( context );
}

void Frame::show()
{
	this->setVisible( true );
}

void Frame::hide()
{
	this->setVisible( false );
}

void Frame::setCaption( const String& caption )
{
	this->m_caption = caption;
}

String Frame::getCaption()
{
	return this->m_caption;
}

String Frame::toString()
{
	String result = "";
	ComponentDTD dtd;
	result += dtd.toString() + "\n";
	result += "<object class=\"" + this->getClassName() + "\"> \n";

	Class* clazz = this->getClass();
	if ( NULL != clazz ){
		Enumerator<Property*>* props =  clazz->getProperties();		
		
		while ( props->hasMoreElements() ){
			Property* p = props->nextElement();
			if ( NULL != p ){
				result += "\t" + p->toString() + "\n";
			}
		}
	}

	//now dump out all the components
	result += "<children> \n";
	std::vector<Control*>::iterator it = m_controls.begin();
	while ( it != m_controls.end() ){
		Control* component = *it;
		if ( NULL != component ){
			result += component->toString() + "\n";
		}
		it++;
	}
	result += "</children> \n";
	result += "</object>";
	//m_components
	return result;
}

bool Frame::isFrameTopmost()
{
	return m_isTopmostFrame;
}

void Frame::setFrameTopmost( const bool& isTopmost )
{
	m_isTopmostFrame = isTopmost;
}
