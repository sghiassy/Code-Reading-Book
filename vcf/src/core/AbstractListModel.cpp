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

//AbstractListModel.cpp
#include "ApplicationKit.h"

#include <algorithm>

using namespace VCF;

AbstractListModel::AbstractListModel()
{
	INIT_EVENT_HANDLER_LIST(ModelEmptied)
	INIT_EVENT_HANDLER_LIST(ModelValidate)
	INIT_EVENT_HANDLER_LIST(ContentsChanged)
	INIT_EVENT_HANDLER_LIST(ItemAdded)
	INIT_EVENT_HANDLER_LIST(ItemDeleted)

	m_listContainer.initContainer( m_listItems );
}

AbstractListModel::~AbstractListModel()
{
	std::vector<ListItem*>::iterator it = m_listItems.begin();	
	while ( it != m_listItems.end() ){		
		delete *it;
		it ++;
	}
	m_listItems.clear();
}

void AbstractListModel::addItem( ListItem* item )
{
	this->m_listItems.push_back( item );
	item->setIndex( m_listItems.size() - 1 );
	ListModelEvent event( this, item );
	fireOnItemAdded( &event );
}

void AbstractListModel::deleteItem( const unsigned long& index )
{
	ListItem* item = m_listItems[index];	
	ListModelEvent event( this, item );	
	fireOnItemDeleted( &event );	

	delete item;
	m_listItems.erase( m_listItems.begin() + index );
}

void AbstractListModel::deleteItem( ListItem* item )
{
	std::vector<ListItem*>::iterator it = std::find( m_listItems.begin(), m_listItems.end(), item );
	if ( it != m_listItems.end() ){		
		ListModelEvent event( this, item );
		fireOnItemDeleted( &event );		
		delete *it;
		m_listItems.erase( it );
	}
}

void AbstractListModel::empty()
{
	std::vector<ListItem*>::iterator it = m_listItems.begin();
	ListModelEvent itemEvent( this, LIST_MODEL_ITEM_DELETED );
	while ( it != m_listItems.end() ){
		itemEvent.setListItem( *it );
		fireOnItemDeleted( &itemEvent );
		delete *it;
		it ++;
	}
	m_listItems.clear();

	ListModelEvent event( this, LIST_MODEL_CONTENTS_DELETED );
	fireOnContentsChanged( &event );
	
	ModelEvent modelEvent( this );
	fireOnModelEmptied( &modelEvent );
}

void AbstractListModel::insertItem( const unsigned long& index, ListItem* item )
{
	m_listItems.insert( m_listItems.begin() + index, item );
	item->setIndex( index );
	ListModelEvent event( this, item );
	fireOnItemAdded( &event );
}

void AbstractListModel::validate()
{

}

ListItem* AbstractListModel::getItemFromIndex( const unsigned long& index )
{
	ListItem* result = NULL;
	if ( index < m_listItems.size() ){
		result = m_listItems[index];
	}
	return result;
}

Enumerator<ListItem*>* AbstractListModel::getItems()
{
	return m_listContainer.getEnumerator();	
}

unsigned long AbstractListModel::getCount()
{
	return this->m_listItems.size();
}

void AbstractListModel::saveToStream( OutputStream * stream )
{
	Enumerator<ListItem*>* items = this->getItems();
	long count = this->getCount();
	stream->write( count );
	while ( items->hasMoreElements() ){
		ListItem* item = items->nextElement();
		if ( NULL != item ){
			Persistable* persistableItem = dynamic_cast<Persistable*>(item);
			if ( NULL != persistableItem ){
				stream->write( persistableItem );
			}
			else {
				String caption = item->getCaption();
				stream->write( caption );
			}
		}	
	}
}

void AbstractListModel::loadFromStream( InputStream * stream )
{
	long count = 0;
	stream->read( count );
	for (int i=0;i<count;i++){
		String s = "";
		stream->read( s );
	}
}