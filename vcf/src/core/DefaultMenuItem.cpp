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
#include "DefaultMenuItem.h"
#include <algorithm>

using namespace VCF;

static DefaultMenuItem* previousSelectedItem = NULL;

DefaultMenuItem::DefaultMenuItem()
{
	init();
}

DefaultMenuItem::DefaultMenuItem( const String& caption, MenuItem* parent, Menu* menuOwner )
{
	init();
	setCaption( caption );
	if ( NULL != parent ){
		parent->addChild( this );
	}
	setMenuOwner( menuOwner );
}

DefaultMenuItem::~DefaultMenuItem()
{
	if ( this == previousSelectedItem )	{
		previousSelectedItem = NULL;
	}
	else if ( NULL == getParent() ) {
		previousSelectedItem = NULL;
	}

	std::vector<MenuItem*>::iterator it = m_menuItems.begin();
	while ( it != m_menuItems.end() ){
		MenuItem* item = *it;
		delete item;
		item = NULL;
		it ++;
	}
	m_menuItems.clear();
	delete m_Peer;
	m_Peer = NULL;
}

void DefaultMenuItem::init() 
{
	INIT_EVENT_HANDLER_LIST(ItemPaint);
	INIT_EVENT_HANDLER_LIST(ItemChanged);
	INIT_EVENT_HANDLER_LIST(ItemSelected);
	INIT_EVENT_HANDLER_LIST(ItemAdded);
	INIT_EVENT_HANDLER_LIST(ItemDeleted);

	INIT_EVENT_HANDLER_LIST(MenuItemClicked);
	INIT_EVENT_HANDLER_LIST(MenuItemUpdate);

	m_selected = false;
	m_imageIndex = 0;
	m_parent = NULL;
	m_radioItem = false;
	m_visible = true;
	m_data = NULL;
	m_separator = false;
	m_isEnabled = true;
	m_menuOwner = NULL;
	UIToolkit* toolkit = UIToolkit::getDefaultUIToolkit();
	if ( NULL != toolkit ){
		m_Peer = toolkit->createMenuItemPeer( this );
	}
	else {
		//throw exception
	}
	if ( NULL == m_Peer ){
		//throw exception
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	
	m_Peer->setMenuItem( this );

	m_container.initContainer( m_menuItems );
}

bool DefaultMenuItem::containsPoint( Point * pt )
{
	return true;
}

Image* DefaultMenuItem::getImage()
{
	return NULL;
}

unsigned long DefaultMenuItem::getIndex()
{
	return m_index;
}

void DefaultMenuItem::setIndex( const unsigned long& index )
{
	m_index = index;
}

void* DefaultMenuItem::getData()
{
	return m_data;
}

void DefaultMenuItem::setData( void* data )
{
	m_data = data;
}

Model* DefaultMenuItem::getModel()
{
	return NULL;
}

void DefaultMenuItem::setModel( Model* model )
{

}

void DefaultMenuItem::paint( GraphicsContext* context, Rect* paintRect )
{
	StringUtils::trace( "DefaultMenuItem::paint " + paintRect->toString() +"\n" );
	context->textAt( paintRect->m_left, paintRect->m_top, m_caption );
}
	
bool DefaultMenuItem::isSelected()
{
	return m_selected;
}

void DefaultMenuItem::setSelected( const bool& selected )
{
	if ( (NULL != previousSelectedItem) && (previousSelectedItem != this) ) {		
		previousSelectedItem->setSelected( false );
	}
	m_selected = selected;
	if ( true == m_selected ) {
		ItemEvent event( this, ITEM_EVENT_SELECTED );
		fireOnItemSelected( &event );
	}
	previousSelectedItem = this;
}

Enumerator<MenuItem*>* DefaultMenuItem::getChildren()
{
	return m_container.getEnumerator();
}

void DefaultMenuItem::addChild( MenuItem* child )
{
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	
	m_menuItems.push_back( child );
	
	child->setParent( this );

	child->setIndex( m_menuItems.size() - 1 );

	child->setMenuOwner( getMenuOwner() );
	
	m_Peer->addChild( child );

	ItemEvent event( this, ITEM_EVENT_ADDED );
	fireOnItemAdded( &event );
}

void DefaultMenuItem::insertChild( const unsigned long& index, MenuItem* child )
{
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}

	m_menuItems.insert( m_menuItems.begin() + index, child );

	child->setIndex( index );

	child->setParent( this );

	child->setMenuOwner( getMenuOwner() );

	m_Peer->insertChild( index, child );

	ItemEvent event( this, ITEM_EVENT_ADDED );
	fireOnItemAdded( &event );
}
	
void DefaultMenuItem::deleteChild( MenuItem* child )
{
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}

	m_Peer->deleteChild( child );

	std::vector<MenuItem*>::iterator found = std::find( m_menuItems.begin(), m_menuItems.end(), child );
	if ( found != m_menuItems.end() ){
		ItemEvent event( this, ITEM_EVENT_DELETED );
		fireOnItemDeleted( &event );

		m_menuItems.erase( found );
		delete child;
		child = NULL;
	}
}

void DefaultMenuItem::deleteChild( const unsigned long& index )
{
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}

	m_Peer->deleteChild( index );

	std::vector<MenuItem*>::iterator found = m_menuItems.begin() + index;
	if ( found != m_menuItems.end() ){
		
		ItemEvent event( this, ITEM_EVENT_DELETED );
		fireOnItemDeleted( &event );

		m_menuItems.erase( found );
		delete *found;
	}		
}

void DefaultMenuItem::clearChildren()
{
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	std::vector<MenuItem*>::iterator it = m_menuItems.begin();
	while ( it != m_menuItems.end() ){
		delete *it;
		it++;
	}
	m_menuItems.clear();

	m_Peer->clearChildren();
}

bool DefaultMenuItem::isChecked()
{
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	return m_Peer->isChecked();
}

void DefaultMenuItem::setChecked( const bool& checked )
{
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	m_Peer->setChecked( checked );
}

bool DefaultMenuItem::hasParent()
{
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	return m_Peer->hasParent();
}

MenuItem* DefaultMenuItem::getParent()
{
	return m_parent;
}

void DefaultMenuItem::setParent( MenuItem* parent )
{
	m_parent = parent;
}

MenuItem* DefaultMenuItem::getChildAt( const unsigned long& index )
{
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	
	if ( index >= m_menuItems.size() ) {
		throw OutOfBoundsException(MAKE_ERROR_MSG(OUT_OF_BOUNDS_EXCEPTION), __LINE__);
	}
	return m_menuItems[index];
}

bool DefaultMenuItem::isEnabled()
{
	return m_isEnabled;//m_Peer->isEnabled();
}

void DefaultMenuItem::setEnabled( const bool& enabled )
{
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	m_isEnabled = enabled;
	m_Peer->setEnabled( enabled );

	ItemEvent event( this, ITEM_EVENT_CHANGED );
	fireOnItemChanged( &event );
}

bool DefaultMenuItem::isVisible()
{
	return m_visible;
}

void DefaultMenuItem::setVisible( const bool& visible )
{
	m_visible = visible;
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	m_Peer->setVisible( visible );

	ItemEvent event( this, ITEM_EVENT_CHANGED );
	fireOnItemChanged( &event );
}

bool DefaultMenuItem::getRadioItem()
{
	return m_radioItem;
}

void DefaultMenuItem::setRadioItem( const bool& value )
{
	m_radioItem = value;
	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}

	m_Peer->setRadioItem( m_radioItem );

	ItemEvent event( this, ITEM_EVENT_CHANGED );
	fireOnItemChanged( &event );
}

void DefaultMenuItem::setCaption( const String& caption )
{
	m_caption = caption;

	if ( NULL == m_Peer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}

	m_Peer->setCaption( m_caption );

	ItemEvent event( this, ITEM_EVENT_TEXT_CHANGED );
	fireOnItemChanged( &event );
}

String DefaultMenuItem::getCaption() 
{
	return m_caption;
}

MenuItemPeer* DefaultMenuItem::getPeer()
{
	return m_Peer;
}

bool DefaultMenuItem::hasChildren()
{
	return ! m_menuItems.empty();
}

void DefaultMenuItem::click()
{
	MenuItemEvent event( this, MENU_ITEM_EVENT_CLICKED );
	fireOnMenuItemClicked( &event );
}

void DefaultMenuItem::update()
{
	MenuItemEvent event( this, MENU_ITEM_EVENT_UPDATED );
	fireOnMenuItemUpdate( &event );
}

Menu* DefaultMenuItem::getMenuOwner()
{
	return m_menuOwner;
}

void DefaultMenuItem::setMenuOwner( Menu* menuOwner )
{
	m_menuOwner = menuOwner;
	std::vector<MenuItem*>::iterator it = m_menuItems.begin();
	while ( it != m_menuItems.end() ){
		MenuItem* child = *it;
		if ( NULL != child ) {
			child->setMenuOwner( menuOwner );
		}
		it++;
	}
}

bool DefaultMenuItem::isSeparator()
{
	return m_separator;
}

void DefaultMenuItem::setSeparator( const bool& separator )
{
	m_separator = separator;
	m_Peer->setAsSeparator( m_separator );
}

void DefaultMenuItem::setImageIndex( const ulong32& imageIndex )
{
	m_imageIndex = imageIndex;
}