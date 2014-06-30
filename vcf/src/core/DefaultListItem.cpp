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

// DefaultListItem.cpp

#include "ApplicationKit.h"
#include "DefaultListItem.h"

using namespace VCF;



DefaultListItem::DefaultListItem()
{
	init();
}

DefaultListItem::DefaultListItem( Model* model )
{
	init();
	m_model = model;
}

DefaultListItem::DefaultListItem( Model* model, const String& caption )
{
	init();
	m_model = model;
	m_caption = caption;
}

DefaultListItem::~DefaultListItem()
{

}

void DefaultListItem::init()
{
	INIT_EVENT_HANDLER_LIST(ItemPaint);
	INIT_EVENT_HANDLER_LIST(ItemChanged);
	INIT_EVENT_HANDLER_LIST(ItemSelected);
	INIT_EVENT_HANDLER_LIST(ItemAdded);
	INIT_EVENT_HANDLER_LIST(ItemDeleted);

	m_model = NULL;
	m_owningControl = NULL;
	m_imageIndex = 0;
	m_bounds.setRect( 0.0, 0.0, 0.0, 0.0 );
	
}

bool DefaultListItem::containsPoint( Point * pt )
{
	return m_bounds.containsPt( pt );
}

Image* DefaultListItem::getImage()
{
	return m_image;
}

unsigned long DefaultListItem::getIndex()
{
	return m_index;
}

void DefaultListItem::setIndex( const unsigned long& index )
{
	m_index = index;
}

void* DefaultListItem::getData()
{
	return m_data;
}

void DefaultListItem::setData( void* data )
{
	m_data = data;
}

String DefaultListItem::getCaption()
{
	return m_caption;
}

void DefaultListItem::setCaption( const String& caption )
{
	m_caption = caption;
	ItemEvent event( this, ITEM_EVENT_TEXT_CHANGED );
	fireOnItemChanged( &event );
}

Model* DefaultListItem::getModel()
{
	return this->m_model;
}

void DefaultListItem::setModel( Model* model )
{
	m_model = model;
}

void DefaultListItem::paint( GraphicsContext* context, Rect* paintRect )
{
	m_bounds = *paintRect;
}
	
bool DefaultListItem::isSelected()
{
	return m_selected;
}

void DefaultListItem::setSelected( const bool& selected )
{
	m_selected = selected;
	ItemEvent event( this, ITEM_EVENT_SELECTED );
	fireOnItemSelected( &event );
}

void DefaultListItem::setImageIndex( const ulong32& imageIndex )
{
	m_imageIndex = imageIndex;
}