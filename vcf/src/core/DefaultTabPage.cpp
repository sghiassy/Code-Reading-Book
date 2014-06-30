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
#include "DefaultTabPage.h"


using namespace VCF;

DefaultTabPage::DefaultTabPage( Control* component )
{
	INIT_EVENT_HANDLER_LIST(ItemPaint);
	INIT_EVENT_HANDLER_LIST(ItemChanged);
	INIT_EVENT_HANDLER_LIST(ItemSelected);
	INIT_EVENT_HANDLER_LIST(ItemAdded);
	INIT_EVENT_HANDLER_LIST(ItemDeleted);

	m_preferredHeight = 8;
	m_imageIndex = 0;
	m_component = NULL;
	m_owningControl = NULL;
	setPageComponent( component );
}

DefaultTabPage::~DefaultTabPage()
{

}

bool DefaultTabPage::containsPoint( Point * pt )
{
	return m_bounds.containsPt( pt );
}

Image* DefaultTabPage::getImage()
{
	return NULL;
}

unsigned long DefaultTabPage::getIndex()
{
	return m_index;
}

void DefaultTabPage::setIndex( const unsigned long& index )
{
	m_index = index;
}

void* DefaultTabPage::getData()
{
	return NULL;
}

void DefaultTabPage::setData( void* data )
{
	//do nothing for now;
}

Model* DefaultTabPage::getModel()
{
	return m_model;
}

void DefaultTabPage::setModel( Model* model )
{
	m_model = model;
}

void DefaultTabPage::setPageName( const String& name )
{
	m_pageName = name;
}

String DefaultTabPage::getPageName()
{
	return m_pageName;
}

Control* DefaultTabPage::getPageComponent()
{
	return m_component;
}

void DefaultTabPage::setPageComponent( Control* component )
{
	if ( NULL != m_component ){
		delete m_component;
		m_component = NULL;
	}
	m_component = component;
	
}

bool DefaultTabPage::isSelected()
{
	return m_selected;
}

void DefaultTabPage::setSelected( const bool& selected )
{
	m_selected = selected;
	ItemEvent event( this, ITEM_EVENT_SELECTED );
	fireOnItemSelected( &event );	
}

void DefaultTabPage::paint( GraphicsContext* context, Rect* paintRect )
{
	Color* hilite = GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor(SYSCOLOR_HIGHLIGHT);
	Color* shadow = GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor(SYSCOLOR_SHADOW);
	Color* face = GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor(SYSCOLOR_FACE);
	Color* textColor = GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor( SYSCOLOR_WINDOW_TEXT );
	Color* selectedTextColor = GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor( SYSCOLOR_SELECTION_TEXT );
	context->setColor( shadow );
	context->rectangle( paintRect );
	context->fillPath();
	Color oldFontColor;
	
	oldFontColor = *getPageComponent()->getFont()->getColor();

	context->getCurrentFont()->setColor( selectedTextColor );	

	if ( true == isSelected() ) {
		context->setColor( face );
		context->rectangle( paintRect );
		context->fillPath();

		context->getCurrentFont()->setColor( textColor );
		
		context->setColor( Color::getColor( "black" ) );		
		context->moveTo( paintRect->m_right-1, paintRect->m_top );
		context->lineTo( paintRect->m_right-1, paintRect->m_bottom );
		context->strokePath();

		context->setColor( hilite );
		context->moveTo( paintRect->m_left, paintRect->m_top );
		context->lineTo( paintRect->m_right-1, paintRect->m_top );
		context->moveTo( paintRect->m_left, paintRect->m_top+1 );
		context->lineTo( paintRect->m_left, paintRect->m_bottom );
		context->strokePath();

		context->setColor( shadow );

		context->moveTo( paintRect->m_right-2, paintRect->m_top+1 );
		context->lineTo( paintRect->m_right-2, paintRect->m_bottom );
		context->strokePath();
	}
	else {
		context->setColor( face );
		context->moveTo( paintRect->m_left, paintRect->m_top );
		context->lineTo( paintRect->m_left, paintRect->m_bottom );

		context->moveTo( paintRect->m_right, paintRect->m_top );
		context->lineTo( paintRect->m_right, paintRect->m_bottom );

		context->strokePath();
	}

	Rect tmpR = *paintRect;
	
	tmpR.m_left = tmpR.m_left + (tmpR.getWidth()/2.0 - context->getTextWidth( m_pageName )/2.0);

	tmpR.m_top = tmpR.m_top + (tmpR.getHeight()/2.0 - context->getTextHeight( "EM" )/2.0);

	context->textBoundedBy( &tmpR, m_pageName, false );

	context->getCurrentFont()->setColor( &oldFontColor );

	m_bounds.setRect( paintRect->m_left, paintRect->m_top, paintRect->m_right, paintRect->m_bottom );
}

ulong32 DefaultTabPage::getPreferredHeight()
{
	ulong32 result = m_preferredHeight;
	Control* control = getPageComponent();
	if ( NULL != control ) {
		result = control->getContext()->getTextHeight( "EM" );
	}
	return result;	
}


void DefaultTabPage::setImageIndex( const ulong32& imageIndex )
{
	m_imageIndex = imageIndex;
}