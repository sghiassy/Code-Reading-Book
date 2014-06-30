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

//ListViewControl.cpp
#include "ApplicationKit.h"
#include "ListViewControl.h"
#include "DefaultListModel.h"

using namespace VCF;


ListViewControl::ListViewControl()
{
	INIT_EVENT_HANDLER_LIST(ItemSelectionChanged);

	m_Peer =	UIToolkit::getDefaultUIToolkit()->createListViewPeer( this );
	
	m_listviewPeer = dynamic_cast<ListviewPeer*>(m_Peer );	

	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}

	m_listModel = new DefaultListModel();
	ListModelEventHandler<ListViewControl>* lmh = 
		new ListModelEventHandler<ListViewControl>( this, ListViewControl::onItemAdded, "ListBoxControl::onItemAdded" );

	m_listModel->addItemAddedHandler( lmh );

	lmh = 
		new ListModelEventHandler<ListViewControl>( this, ListViewControl::onItemDeleted, "ListBoxControl::onItemDeleted" );

	m_listModel->addItemDeletedHandler( lmh );

	lmh = 
		new ListModelEventHandler<ListViewControl>( this, ListViewControl::onListModelContentsChanged, "ListBoxControl::onListModelContentsChanged" );
	
	m_listModel->addContentsChangedHandler( lmh );

	init();
}


ListViewControl::~ListViewControl()
{
	delete m_listModel;
}

void ListViewControl::init()
{
	m_iconStyle = IS_LARGE_ICON;

	setColor( GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor( SYSCOLOR_WINDOW ) );
}

ListModel* ListViewControl::getListModel()
{
	return m_listModel;
}

void ListViewControl::setListModel(ListModel * model)
{
	m_listModel = model;
}

void ListViewControl::onListModelContentsChanged( ListModelEvent* event )
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}

	if ( NULL != event ){
		switch ( event->getType() ){
			case LIST_MODEL_CONTENTS_DELETED: {
				m_listviewPeer->clear();
			}
			break;

			case LIST_MODEL_ITEM_CHANGED: {
				ListItem* item = event->getListItem();
				if ( NULL != item ){
					m_listviewPeer->deleteItem( item );
					m_listviewPeer->insertItem( item->getIndex(), item );
				}
			}
			break; 
		}
	}
}

void ListViewControl::onItemAdded( ListModelEvent* event )
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}

	m_listviewPeer->addItem( event->getListItem() );
}

void ListViewControl::onItemDeleted( ListModelEvent* event )
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	m_listviewPeer->deleteItem( event->getListItem() );
}

void ListViewControl::addHeaderColumn( const String& columnName )
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	m_listviewPeer->addHeaderColumn( columnName );
}

void ListViewControl::insertHeaderColumn( const unsigned long& index, const String& columnName )
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	m_listviewPeer->insertHeaderColumn( index, columnName );
}

void ListViewControl::deleteHeaderColumn( const unsigned long& index )
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	m_listviewPeer->deleteHeaderColumn( index );
}

IconStyleType ListViewControl::getIconStyle()
{
	//if ( NULL == this->m_listviewPeer ){
	//	throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	//};
	return m_iconStyle;// m_listviewPeer->getIconStyle();
}

void ListViewControl::setIconStyle( const IconStyleType& iconStyle )
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	m_iconStyle = iconStyle;
	m_listviewPeer->setIconStyle( m_iconStyle );
}

bool ListViewControl::getAllowsMultiSelect()
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	return m_listviewPeer->getAllowsMultiSelect();
}

void ListViewControl::setAllowsMultiSelect( const bool& allowsMultiSelect )
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	m_listviewPeer->setAllowsMultiSelect( allowsMultiSelect );
}

IconAlignType ListViewControl::getIconAlignment()
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	return m_listviewPeer->getIconAlignment();
}

void ListViewControl::setIconAlignment( const IconAlignType& iconAlignType )
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	m_listviewPeer->setIconAlignment( iconAlignType );
}

bool ListViewControl::getAllowLabelEditing()
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	return m_listviewPeer->getAllowLabelEditing();
}

void ListViewControl::setAllowLabelEditing( const bool& allowLabelEditing )
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	m_listviewPeer->setAllowLabelEditing( allowLabelEditing );
}

void ListViewControl::setItemFocused( ListItem* item )
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	m_listviewPeer->setFocusedItem( item );
}

void ListViewControl::onListModelEmptied( ModelEvent* event )
{
	if ( NULL == this->m_listviewPeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}

	m_listviewPeer->clear();
}

ListItem* ListViewControl::getSelectedItem()
{
	ListItem* result = NULL;
	
	result = m_listviewPeer->getSelectedItem();

	return result;
}