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

//AbstractTreeModel.cpp
#include "ApplicationKit.h"
#include <algorithm>

using namespace VCF;

AbstractTreeModel::AbstractTreeModel()
{
	INIT_EVENT_HANDLER_LIST(ModelEmptied)
	INIT_EVENT_HANDLER_LIST(ModelValidate)

	INIT_EVENT_HANDLER_LIST(RootNodeChanged)
	INIT_EVENT_HANDLER_LIST(NodeAdded)
	INIT_EVENT_HANDLER_LIST(NodeDeleted)

	this->init();	
}

AbstractTreeModel::~AbstractTreeModel()
{
	
}

void AbstractTreeModel::init()
{	
	m_treeItemContainer.initContainer( m_rootNodes );
}

void AbstractTreeModel::validate()
{

}

void AbstractTreeModel::empty()
{
	std::vector<TreeItem*>::iterator it = m_rootNodes.begin();
	while ( it != m_rootNodes.end() ) {
		TreeItem* rootItem = *it;
		if ( NULL != rootItem ) {
			rootItem->clearChildren();
			delete rootItem;
		}
		rootItem = NULL;
		it ++;
	}
	m_rootNodes.clear();
	ModelEvent event( this );
	fireOnModelEmptied( &event );
}

void AbstractTreeModel::insertNodeItem(TreeItem * node, TreeItem * nodeToInsertAfter)
{
	TreeItem* parent = nodeToInsertAfter->getParent();
	if ( NULL != parent ) {
		parent->insertChild( nodeToInsertAfter->getIndex(), node );
		TreeModelEvent event(this, node );
		this->fireOnNodeAdded( &event );	
	}
	else {
		//bad call throw exception
	}
}

void AbstractTreeModel::deleteNodeItem(TreeItem * nodeToDelete)
{
	TreeItem* parent = nodeToDelete->getParent();

	TreeModelEvent event(this, nodeToDelete );			
	fireOnNodeDeleted( &event );	

	if ( NULL != parent ){
		parent->deleteChild( nodeToDelete );
	}
	else {
		std::vector<TreeItem*>::iterator found = std::find( m_rootNodes.begin(), m_rootNodes.end(), nodeToDelete );
		if ( found != m_rootNodes.end() ) {
			TreeItem* item = *found;
			m_rootNodes.erase( found );
			delete item;
			item = NULL;
		}
	}	
}

void AbstractTreeModel::addNodeItem( TreeItem * node, TreeItem * nodeParent )
{
	node->setModel( this );
	if ( NULL == nodeParent ){
		m_rootNodes.push_back( node );
		TreeModelEvent event(this, node );
		this->fireOnRootNodeChanged( &event );	
	}
	else {
		nodeParent->addChild( node );
	}
	
	TreeModelEvent event(this, node );
	this->fireOnNodeAdded( &event );	
}

void AbstractTreeModel::sort()
{

}

void AbstractTreeModel::onItemPaint( ItemEvent* event )
{

}

void AbstractTreeModel::onItemChanged( ItemEvent* event )
{
	if ( NULL != event ){
		switch ( event->getType() ){
			case ITEM_EVENT_TEXT_CHANGED:{
				
			}
			break;
		}
	}
}

void AbstractTreeModel::onItemSelected( ItemEvent* event )
{

}

void AbstractTreeModel::onItemAdded( ItemEvent* event )
{

}

	
void AbstractTreeModel::onItemDeleted( ItemEvent* event )
{

}

Enumerator<TreeItem*>* AbstractTreeModel::getRootItems()
{
	return m_treeItemContainer.getEnumerator();
}