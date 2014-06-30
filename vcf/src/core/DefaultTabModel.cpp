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
#include "DefaultTabModel.h"
#include <algorithm>


using namespace VCF;

DefaultTabModel::DefaultTabModel()
{
	INIT_EVENT_HANDLER_LIST(ModelEmptied)
	INIT_EVENT_HANDLER_LIST(ModelValidate)	

	INIT_EVENT_HANDLER_LIST(TabPageAdded)
	INIT_EVENT_HANDLER_LIST(TabPageRemoved)
	INIT_EVENT_HANDLER_LIST(TabPageSelected)

	m_container.initContainer( this->m_pages );
}

DefaultTabModel::~DefaultTabModel()
{
	this->clearTabPages();
}
  
void DefaultTabModel::addTabPage( TabPage* page )
{
	TabModelEvent event( this, TAB_MODEL_EVENT_ITEM_ADDED, page );
	fireOnTabPageAdded( &event );
	m_pages.push_back( page );
}

void DefaultTabModel::insertTabPage( const ulong32& index, TabPage* page )
{
	TabModelEvent event( this, TAB_MODEL_EVENT_ITEM_ADDED, page );
	fireOnTabPageAdded( &event );
	m_pages.insert( m_pages.begin() + index, page );
}
	
void DefaultTabModel::deleteTabPage( TabPage* page )
{
	TabModelEvent event( this, TAB_MODEL_EVENT_ITEM_REMOVED, page );
	fireOnTabPageRemoved( &event );
	std::vector<TabPage*>::iterator found = std::find( m_pages.begin(), m_pages.end(), page );
	if ( found != m_pages.end() ){
		m_pages.erase( found );
		//clean up memory
		delete *found;
	}
}

void DefaultTabModel::deleteTabPage( const ulong32& index )
{
	std::vector<TabPage*>::iterator found = m_pages.begin() + index;
	if ( found != m_pages.end() ){
		TabModelEvent event( this, TAB_MODEL_EVENT_ITEM_REMOVED, *found );
		fireOnTabPageRemoved( &event );
		m_pages.erase( found );
		//clean up memory
		delete *found;
	}	
}

void DefaultTabModel::clearTabPages()
{
	std::vector<TabPage*>::iterator it = m_pages.begin();
	while ( it != m_pages.end() ){
		TabPage* page = *it;
		delete page;
		page = NULL;
		it++;
	}
	m_pages.clear();
}

TabPage* DefaultTabModel::getPageAt( const ulong32& index )
{
	TabPage* result = NULL;
	std::vector<TabPage*>::iterator it = m_pages.begin() + index;
	if ( it != m_pages.end() ){
		result = *it;
	}
	return result;
}

TabPage* DefaultTabModel::getSelectedPage()
{
	TabPage* result = NULL;
	std::vector<TabPage*>::iterator it = m_pages.begin();
	while ( it != m_pages.end() ){
		TabPage* page = *it;
		if ( NULL != page ){
			if ( true == page->isSelected() ){
				result = page;
				break;
			}
		}	
		it++;	
	}
	return result;
}

void DefaultTabModel::setSelectedPage( TabPage* page )
{	
	std::vector<TabPage*>::iterator it = m_pages.begin();
	while ( it != m_pages.end() ){
		TabPage* aPage = *it;
		if ( NULL != page ){
			aPage->setSelected( false );
		}	
		it++;	
	}
	page->setSelected( true );
	TabModelEvent event( this, TAB_MODEL_EVENT_ITEM_SELECTED, page );
	fireOnTabPageSelected( &event );
}

void DefaultTabModel::setSelectedPage( const ulong32& index )
{
	std::vector<TabPage*>::iterator it = m_pages.begin();
	if ( it != m_pages.end() ){
		TabPage* page = *it;
		setSelectedPage( page );
	}
}

Enumerator<TabPage*>* DefaultTabModel::getPages()
{
	return this->m_container.getEnumerator();
}