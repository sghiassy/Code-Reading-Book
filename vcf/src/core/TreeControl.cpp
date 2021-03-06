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

/* Generated by Together */
#include "ApplicationKit.h"
#include "TreeControl.h"
#include "DefaultTreeModel.h"


using namespace VCF;

TreeControl::TreeControl()
{
	m_Peer = UIToolkit::getDefaultUIToolkit()->createTreePeer( this );
	m_treePeer = dynamic_cast<TreePeer*>( m_Peer );
	
	if ( NULL == this->m_treePeer ){
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	};
	this->init();	
}
	
TreeControl::~TreeControl()
{
	delete m_treeModel;
	m_treeModel = NULL;
}

void TreeControl::init()
{
	this->m_imageList = NULL;

	m_stateImageList = NULL;

	m_treeModel = new DefaultTreeModel();	

	TreeModelEventHandler<TreeControl>* tmh = 
		new TreeModelEventHandler<TreeControl>( this, 
												TreeControl::onTreeRootNodeChanged,
												"TreeControl::onTreeRootNodeChanged" );

	m_treeModel->addTreeRootNodeChangedHandler( tmh );

	tmh = new TreeModelEventHandler<TreeControl>( this, 
													TreeControl::onTreeNodeAdded,
													"TreeControl::onTreeNodeAdded" );

	m_treeModel->addTreeNodeAddedHandler( tmh );

	ModelEventHandler<TreeControl>* mh = new ModelEventHandler<TreeControl>( this, TreeControl::onModelEmptied, "ModelHandler" );	
	m_treeModel->addModelHandler( mh );
}

void TreeControl::setTreeModel( TreeModel * model )
{
	if ( NULL != m_treeModel ){
		delete m_treeModel;
		m_treeModel = NULL;
	}
	m_treeModel = model;
	if ( NULL != m_treeModel ) {
		EventHandler* tml = getEventHandler( "TreeControl::onTreeRootNodeChanged" );
		m_treeModel->addTreeRootNodeChangedHandler ( tml );

		tml = getEventHandler( "TreeControl::onTreeNodeAdded" );
		m_treeModel->addTreeNodeAddedHandler( tml );

		EventHandler* ml = getEventHandler( "ModelHandler" );
		m_treeModel->addModelHandler( ml );
	}
}

TreeModel* TreeControl::getTreeModel()
{
	return m_treeModel;
}

ImageList* TreeControl::getImageList()
{
	return m_imageList;
}

void TreeControl::setImageList( ImageList* imageList )
{
	m_imageList = imageList;
	this->m_treePeer->setImageList( imageList );
}

void TreeControl::paint( GraphicsContext * context )
{

}

void TreeControl::onTreeRootNodeChanged( TreeModelEvent* event )
{
	TreeItem* item = event->getTreeItem();
	//this->m_treePeer->addItem( item );
}

void TreeControl::onTreeNodeAdded( TreeModelEvent* event )
{
	TreeItem* item = event->getTreeItem();
	m_treePeer->addItem( item );
	EventHandler* il = getEventHandler( "TreeItemListener" );
	if  ( il == NULL ) {
		il = new ItemEventHandler<TreeControl>( this, TreeControl::onTreeItemPaint, "TreeItemListener" );
	}
	item->addItemPaintHandler( il );
}

void TreeControl::onTreeNodeDeleted( TreeModelEvent* event )
{
	
}

void TreeControl::onModelEmptied( ModelEvent* event )
{
	m_treePeer->clear();
}

TreeItem* TreeControl::hitTestForItem( Point* pt, TreeItem* item )
{
	TreeItem* result = NULL;
	if ( true == item->containsPoint( pt ) ) {
		result = item;
	}
	else if ( false == item->isLeaf() ){
		Enumerator<TreeItem*>* children = item->getChildren();
		while ( true == children->hasMoreElements() ) {
			TreeItem* childItem = children->nextElement();
			result = hitTestForItem( pt, childItem );
			if ( result != NULL ) {
				break;
			}
		}
	}
	
	return result;
}

TreeItem* TreeControl::findItem( Point* pt )
{
	TreeItem* result = NULL;

	Enumerator<TreeItem*>* rootItems = this->m_treeModel->getRootItems();
	while ( true == rootItems->hasMoreElements() ) {
		TreeItem* item = rootItems->nextElement();
		result = hitTestForItem( pt, item );
		if ( NULL != result ) {
			break;
		}
	}

	return result;
}

double TreeControl::getItemIndent()
{
	return this->m_treePeer->getItemIndent();
}

void TreeControl::setItemIndent( const double& indent )
{
	this->m_treePeer->setItemIndent( indent );
}

void TreeControl::onTreeItemPaint( ItemEvent* event )
{	
	TreeItem* item = (TreeItem*)event->getSource();
	GraphicsContext* context = event->getContext();
	if ( NULL != context ) {
		ImageList* il = this->getImageList();
		if ( NULL != il ) {
			Rect imgRect = *(item->getBounds());
			imgRect.m_left += (item->getLevel()+1) * getItemIndent();
			imgRect.m_right = imgRect.m_left + il->getImageWidth();
			context->setColor( GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor( SYSCOLOR_WINDOW ) );
			context->rectangle( &imgRect );
			context->fillPath();

			if ( true == item->isSelected() ) {
				if ( NULL != m_stateImageList ) {
					m_stateImageList->draw( context, item->getSelectedImageIndex(), &imgRect );
				}
				else {
					il->draw( context, item->getImageIndex(), &imgRect );
				}	
			}
			else {
				if ( true == item->isExpanded() ) {
					if ( NULL != m_stateImageList ) {
						m_stateImageList->draw( context, item->getExpandedImageIndex(), &imgRect );
					}
					else {
						il->draw( context, item->getImageIndex(), &imgRect );
					}
				}
				else {
					il->draw( context, item->getImageIndex(), &imgRect );
				}
			}			
		}
	}
}

void TreeControl::setStateImageList( ImageList* imageList )
{
	m_stateImageList = imageList;
}

ImageList* TreeControl::getStateImageList()
{
	return m_stateImageList;
}