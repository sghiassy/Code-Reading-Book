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

// DefaultTreeItem.cpp

#include "ApplicationKit.h"
#include "DefaultTreeItem.h"
#include "StringUtils.h"
#include <algorithm>


using namespace VCF;


DefaultTreeItem::DefaultTreeItem( const String& caption, Control* owningControl, TreeModel* model )
{
	this->init();
	m_owningControl = owningControl;
	this->m_treeModel = model;
	setCaption( caption );
}

DefaultTreeItem::~DefaultTreeItem()
{
	std::vector<TreeItem*>::iterator it = m_childNodeItems.begin();
	while ( it != m_childNodeItems.end() ){
		delete *it;
		it++;
	}
}

void DefaultTreeItem::init()
{
	INIT_EVENT_HANDLER_LIST(ItemPaint);
	INIT_EVENT_HANDLER_LIST(ItemChanged);
	INIT_EVENT_HANDLER_LIST(ItemSelected);
	INIT_EVENT_HANDLER_LIST(ItemAdded);
	INIT_EVENT_HANDLER_LIST(ItemDeleted);

	m_imageIndex = 0;
	m_selectedImageIndex = 0;
	m_expandedImageIndex = 0;
	m_bounds.setRect(0.0,0.0,0.0,0.0);
	m_owningControl = NULL;
	this->m_caption = "";
	this->m_childNodeItems.clear();
	this->m_image = NULL;
	this->m_parent = NULL;
	this->m_textBold = false;
	this->m_TextColor.setRGB( 0.0, 0.0, 0.0 );
	this->m_userData = NULL;
	m_enumContainer.initContainer( m_childNodeItems );
}

bool DefaultTreeItem::containsPoint( Point * pt )
{
	return m_bounds.containsPt( pt );
}

Image* DefaultTreeItem::getImage()
{
	return m_image;
}

unsigned long DefaultTreeItem::getIndex()
{
	return this->m_index;
}

void DefaultTreeItem::setIndex( const unsigned long& index )
{
	m_index = index;
}

void* DefaultTreeItem::getData()
{
	return m_userData;
}

void DefaultTreeItem::setData( void* data )
{
	m_userData = data;
	ItemEvent event( this );
	fireOnItemChanged( &event );
}

Color* DefaultTreeItem::getTextColor()
{
	return &this->m_TextColor;
}

void DefaultTreeItem::setTextColor(Color* color)
{
	this->m_TextColor = *color;
	ItemEvent event( this );
	fireOnItemChanged( &event );
}

void DefaultTreeItem::setTextBold(const bool& bold)
{
	this->m_textBold = bold;
	ItemEvent event( this );
	fireOnItemChanged( &event );
}

bool DefaultTreeItem::getTextBold()
{
	return this->m_textBold;
}

bool DefaultTreeItem::isRoot()
{
	return ( NULL == m_parent );
}

bool DefaultTreeItem::isLeaf()
{
	return  m_childNodeItems.empty();
}

TreeItem* DefaultTreeItem::getParent()
{
	return m_parent;
}

void DefaultTreeItem::setParent( TreeItem* parent )
{
	this->m_parent = parent;
	ItemEvent event( this );
	fireOnItemChanged( &event );
}

TreeItem* DefaultTreeItem::getNextChildNodeItem()
{	
	if ( NULL != m_parent ) {
		DefaultTreeItem* dtParent = dynamic_cast<DefaultTreeItem*>(m_parent);
		if ( NULL != dtParent ) {
			std::vector<TreeItem*>::iterator it = dtParent->m_childNodeItems.begin() + this->m_index + 1;
			if ( it != dtParent->m_childNodeItems.end() ) {
				return *it;
			}
		}
	}
	return NULL;
}

TreeItem* DefaultTreeItem::getPrevChildNodeItem()
{
	if ( NULL != m_parent ) {
		DefaultTreeItem* dtParent = dynamic_cast<DefaultTreeItem*>(m_parent);
		if ( (NULL != dtParent) && (m_index > 0) ) {
			std::vector<TreeItem*>::iterator it = dtParent->m_childNodeItems.begin() + this->m_index - 1;
			if ( it != dtParent->m_childNodeItems.end() ) {
				return *it;
			}
		}
	}
	return NULL;
}

String DefaultTreeItem::getCaption()
{
	return this->m_caption;	
}

void DefaultTreeItem::setCaption( const String& caption )
{
	StringUtils::trace( "caption = " + caption + "\n" );
	this->m_caption = caption;
	ItemEvent event( this, ITEM_EVENT_TEXT_CHANGED );
	fireOnItemChanged( &event );
}

Enumerator<TreeItem*>* DefaultTreeItem::getChildren()
{
	return this->m_enumContainer.getEnumerator();
}

void DefaultTreeItem::addChild( TreeItem* child )
{
	StringUtils::trace( "Adding child " + child->getCaption() + "\n" );
	this->m_childNodeItems.push_back( child );
	child->setParent( this );
	ItemEvent event( this, ITEM_EVENT_ADDED );
	fireOnItemAdded( &event );
}

void DefaultTreeItem::insertChild( const unsigned long& index, TreeItem* child )
{
	std::vector<TreeItem*>::iterator it = m_childNodeItems.begin();
	this->m_childNodeItems.insert( it + index, child );
	child->setParent( this );
	ItemEvent event( this, ITEM_EVENT_ADDED );
	fireOnItemAdded( &event );
	it = m_childNodeItems.begin() + index;
	int i = index;
	while ( it != m_childNodeItems.end() ) {
		TreeItem* item = *it;
		item->setIndex( i );
		it ++;
		i++;
	}
}
	
void DefaultTreeItem::deleteChild( TreeItem* child )
{
	std::vector<TreeItem*>::iterator found = std::find( m_childNodeItems.begin(), m_childNodeItems.end(), child );
	if ( found != m_childNodeItems.end() ) {
		int index = found - m_childNodeItems.begin();
		TreeItem* item = *found;
		ItemEvent event( child, ITEM_EVENT_DELETED );
		fireOnItemDeleted( &event );
		delete item;
		item = NULL;
		m_childNodeItems.erase ( found );
		int i = index;
		std::vector<TreeItem*>::iterator it = m_childNodeItems.begin() + index;
		while ( it != m_childNodeItems.end() ) {
			item = *it;
			item->setIndex( i );
			it ++;
			i ++;
		}
	}
}

void DefaultTreeItem::deleteChild( const unsigned long& index )
{
	std::vector<TreeItem*>::iterator found =  m_childNodeItems.begin() + index;
	if ( found != m_childNodeItems.end() ) {
		TreeItem* item = *found;
		ItemEvent event( item, ITEM_EVENT_DELETED );
		fireOnItemDeleted( &event );
		m_childNodeItems.erase ( found );
		delete item;
		item = NULL;		
		int i = index;
		std::vector<TreeItem*>::iterator it = m_childNodeItems.begin() + index;
		while ( it != m_childNodeItems.end() ) {
			item = *it;
			item->setIndex( i );
			it ++;
			i ++;
		}
	}
}

void DefaultTreeItem::clearChildren()
{	
	for (std::vector<TreeItem*>::iterator it = m_childNodeItems.begin(); it != m_childNodeItems.end(); it ++ ){
		TreeItem* item = *it;
		item->clearChildren();		
		ItemEvent deleteEvent( item, ITEM_EVENT_DELETED );
		fireOnItemDeleted( &deleteEvent );
		delete item;
	}
	this->m_childNodeItems.clear();

	ItemEvent event( this );
	fireOnItemChanged( &event );
}

Model* DefaultTreeItem::getModel()
{
	return this->m_treeModel;
}

void DefaultTreeItem::setModel( Model* model )
{
	this->m_treeModel = dynamic_cast<TreeModel*>(model );
	for (std::vector<TreeItem*>::iterator it = m_childNodeItems.begin(); it != m_childNodeItems.end(); it ++ ){
		(*it)->setModel( model );
	}
}

void DefaultTreeItem::paint( GraphicsContext* context, Rect* paintRect )
{
	m_bounds = *paintRect;

	ItemEvent event( this, context );
	fireOnItemPaint( &event );
}
	
bool DefaultTreeItem::isSelected()
{
	return m_selected;
}

void DefaultTreeItem::setSelected( const bool& selected )
{
	m_selected = selected;
	ItemEvent event( this, ITEM_EVENT_SELECTED );
	fireOnItemSelected( &event );	
}

void DefaultTreeItem::expand( const bool& isExpanded )
{
	m_isExpanded = isExpanded;
}

ulong32 DefaultTreeItem::getLevel()
{
	ulong32 result = 0;

	if ( false == this->isRoot() ) {
		TreeItem* parent = this->getParent();
		if ( NULL != parent ) {
			result = parent->getLevel();
			result ++;
		}
	}
	return result;
}

void DefaultTreeItem::setControl( Control* control )
{
	m_owningControl = control;
	std::vector<TreeItem*>::iterator it = m_childNodeItems.begin();
	while ( it != m_childNodeItems.end() ){
		TreeItem* item = *it;
		item->setControl( m_owningControl );
		it++;
	}
}

void DefaultTreeItem::setImageIndex( const ulong32& imageIndex )
{
	m_imageIndex = imageIndex;
}

void DefaultTreeItem::setSelectedImageIndex( const ulong32& selectedImageIndex )
{
	this->m_selectedImageIndex = selectedImageIndex;
}

void DefaultTreeItem::setExpandedImageIndex( const ulong32& expandedImageIndex )
{
	m_expandedImageIndex = expandedImageIndex;
}