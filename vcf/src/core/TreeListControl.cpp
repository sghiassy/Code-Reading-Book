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

// TreeListControl.cpp

#include "ApplicationKit.h"
#include "DefaultListItem.h"
#include "DefaultTreeItem.h"
#include "DefaultTreeModel.h"
#include "TreeListControl.h"
#include "DefaultTreeModel.h"

#include <algorithm>

using namespace VCF;



DefaultColumnHeaderItem::DefaultColumnHeaderItem( const String& caption, Control* owningControl, DefaultTreeListModel* model )
{
	m_columnWidth = 100;
	setCaption( caption );
	setControl( owningControl );
	setModel( model );
}

DefaultColumnHeaderItem::~DefaultColumnHeaderItem()
{

}

void DefaultColumnHeaderItem::paint( GraphicsContext* context, Rect* paintRect )
{
	DefaultListItem::paint( context, paintRect );
	Rect tmpRect = *paintRect;
	
	Light3DBorder bdr;
	bdr.paint( paintRect, context );
	tmpRect.inflate( -2, -1 );
	context->textBoundedBy( &tmpRect, getCaption() );	
}





DefaultTreeListModel::DefaultTreeListModel( Control* owningControl )
{
	m_owningControl = owningControl;
	m_columnContainer.initContainer( m_columnItems );	
	DefaultColumnHeaderItem* defaultItem = new DefaultColumnHeaderItem( "Column1", owningControl, this );	
	addColumnHeaderItem( defaultItem );
}

DefaultTreeListModel::~DefaultTreeListModel()
{
	std::vector<DefaultColumnHeaderItem*>::iterator it = m_columnItems.begin();
	while ( it != m_columnItems.end() ) {
		DefaultColumnHeaderItem* item = *it;
		delete item;
		it ++;
	}
}

void DefaultTreeListModel::addColumnHeaderItem( DefaultColumnHeaderItem* columnItem )
{
	columnItem->setModel( this );
	columnItem->setControl( m_owningControl );
	m_columnItems.push_back( columnItem );
}

void DefaultTreeListModel::insertColumnHeaderItem( DefaultColumnHeaderItem* columnItem, const ulong32& index )
{
	columnItem->setModel( this );
	columnItem->setControl( m_owningControl );
	m_columnItems.insert( m_columnItems.begin() + index, columnItem );
}

void DefaultTreeListModel::removeColumnHeaderItem( DefaultColumnHeaderItem* columnItem )
{
	std::vector<DefaultColumnHeaderItem*>::iterator found = std::find( m_columnItems.begin(), m_columnItems.end(), columnItem );
	if ( found != m_columnItems.end() ) {
		delete columnItem;
		m_columnItems.erase( found );
	}
}

void DefaultTreeListModel::removeColumnHeaderItem( const ulong32& index )
{
	std::vector<DefaultColumnHeaderItem*>::iterator found = m_columnItems.begin() + index;
	if ( found != m_columnItems.end() ) {
		DefaultColumnHeaderItem* columnItem = *found;
		delete columnItem;
		m_columnItems.erase( found );
	}
}

Enumerator<DefaultColumnHeaderItem*>* DefaultTreeListModel::getColumnHeaders()
{
	return m_columnContainer.getEnumerator();
}

DefaultColumnHeaderItem* DefaultTreeListModel::getColumnHeaderItem( const ulong32& index )
{
	return m_columnItems[index];
}

void DefaultTreeListModel::setControl( Control* owningControl )
{
	std::vector<DefaultColumnHeaderItem*>::iterator it = m_columnItems.begin();
	while ( it != m_columnItems.end() ) {
		DefaultColumnHeaderItem* item = *it;
		item->setControl( owningControl );
		it ++;
	}

	Enumerator<TreeItem*>* rootItems = this->getRootItems();
	if ( NULL != rootItems ) {
		while ( true == rootItems->hasMoreElements() ) {
			TreeItem* item = rootItems->nextElement();
			item->setControl( owningControl );
		}
	}
}

DefaultTreeListItem::DefaultTreeListItem( const String& caption, Control* owningControl, TreeModel* model ):
	DefaultTreeItem( caption, owningControl, model )
{

}

DefaultTreeListItem::~DefaultTreeListItem()
{

}

void DefaultTreeListItem::paint( GraphicsContext* context, Rect* paintRect )
{	
	DefaultTreeItem::paint( context, paintRect );

	context->rectangle( paintRect );
	context->strokePath();
	
	if ( true == this->isSelected() ) {
		Color oldColor = *(context->getColor());
		Color* selectedColor = GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor( SYSCOLOR_SELECTION );
		context->setColor( selectedColor );
		Rect tmp = *paintRect;
		tmp.inflate( -2, -2 );
		context->rectangle( &tmp );
		context->fillPath();
		context->setColor( &oldColor );
	}

	TreeListControl* treeList = (TreeListControl*)this->getControl();
	if ( NULL == treeList ) {
		//throw exception
	}

	double indent = treeList->getItemIndent() * getLevel();

	m_expanderRect.m_left = paintRect->m_left + indent;	
	m_expanderRect.m_top = paintRect->m_top;
	m_expanderRect.m_bottom = paintRect->m_bottom;
	m_expanderRect.m_right = m_expanderRect.m_left + paintRect->getHeight();

	bool isALeaf = this->isLeaf();
	Rect captionRect = *paintRect;
	captionRect.m_left = m_expanderRect.m_right;
	DefaultTreeListModel* model = (DefaultTreeListModel*)this->getModel();

	if ( false == isALeaf ) {
		paintExpander( context );
		if ( true == this->isExpanded() ) {
			Enumerator<TreeItem*>* children = getChildren();
			Rect tmpPaintRect = *paintRect;
			tmpPaintRect.m_top = paintRect->m_bottom;
			tmpPaintRect.m_bottom = tmpPaintRect.m_top + paintRect->getHeight();
			double totalHeight = 0;
			while ( true == children->hasMoreElements() ) {
				TreeItem* child = children->nextElement();				
				child->paint( context, &tmpPaintRect );
				double h = tmpPaintRect.getHeight();
				tmpPaintRect.m_top = tmpPaintRect.m_bottom;
				tmpPaintRect.m_bottom = tmpPaintRect.m_top + h;
				totalHeight += h;
			}
			if ( totalHeight > 0.0 ) {
				paintRect->m_bottom = paintRect->m_top + totalHeight;
			}
		}
	}
	else {		
		if ( model->getColumnHeaderCount() > 0 ) {
			DefaultColumnHeaderItem* col = model->getColumnHeaderItem( 0 );
			Rect* colBounds = col->getBounds();
			captionRect.m_right = colBounds->m_right;			
		}		
	}
	
	double textH = context->getTextHeight( getCaption() );
	captionRect.m_top = captionRect.m_top + ((captionRect.getHeight() / 2.0) - textH/2.0);
	captionRect.m_bottom = captionRect.m_top + textH;

	context->textBoundedBy( &captionRect, getCaption(), false );

	Enumerator<DefaultColumnHeaderItem*>* columns =  model->getColumnHeaders();
	int columnIndex = 0;
	if ( NULL != columns ) {
		while ( true == columns->hasMoreElements() ) {
			DefaultColumnHeaderItem* col = columns->nextElement();
			if ( columnIndex > 0 ) {
				paintSubItem( context, columnIndex );
			}
			columnIndex++;
		}
	}
}

void DefaultTreeListItem::paintExpander( GraphicsContext* context )
{
	std::vector<Point*> triangle;	
	double spacer = 5.0;
	if ( true == this->isExpanded() ) {
		triangle.push_back( &Point( m_expanderRect.m_left+spacer, (m_expanderRect.m_top + (m_expanderRect.getHeight()/2.0)) - spacer ) );
		triangle.push_back( &Point( triangle[0]->m_x + (m_expanderRect.getHeight()-spacer*2.0), triangle[0]->m_y ) );
		triangle.push_back( &Point( triangle[0]->m_x + ((m_expanderRect.getHeight()-spacer*2.0)/2.0), triangle[0]->m_y + (m_expanderRect.getWidth()/2.0 - spacer) ) );
		triangle.push_back( &Point( triangle[0]->m_x, triangle[0]->m_y ) );
	}
	else {
		triangle.push_back( &Point( m_expanderRect.m_left+spacer, m_expanderRect.m_top+spacer ) );
		triangle.push_back( &Point( m_expanderRect.m_left+spacer, m_expanderRect.m_bottom-spacer ) );
		triangle.push_back( &Point( m_expanderRect.m_left + (m_expanderRect.getWidth()/2.0), m_expanderRect.m_top + (m_expanderRect.getHeight()/2.0) ) );
		triangle.push_back( &Point( m_expanderRect.m_left+spacer, m_expanderRect.m_top+spacer ) );
	}

	context->setColor( Color::getColor( "black" ) );
	context->polyline( triangle );
	context->fillPath();
}
	
void DefaultTreeListItem::paintSubItem( GraphicsContext* context, const ulong32& subItemIndex )
{

}


TreeListControl::TreeListControl():
	CustomControl()
{
	init();
}	

TreeListControl::~TreeListControl()	
{

}

void TreeListControl::setTreeModel(TreeModel * model)
{
    
}

TreeModel* TreeListControl::getTreeModel()
{
	return m_treeModel;
}

void TreeListControl::getImageList()
{

}

void TreeListControl::setImageList()
{

}

void TreeListControl::paint( GraphicsContext * context )
{
	CustomControl::paint( context );

	Enumerator<DefaultColumnHeaderItem*>* columnItems = m_treeModel->getColumnHeaders();
	Rect itemRect;	

	double prevTop = 0.0;

	if ( NULL != columnItems ) {		
		itemRect.m_top = prevTop;
		double prevLeft = 0.0;
		itemRect.m_bottom = itemRect.m_top + m_columnHeight;
		while ( true == columnItems->hasMoreElements() ) {
			DefaultColumnHeaderItem* columnItem = columnItems->nextElement();
			itemRect.m_left = prevLeft;			
			itemRect.m_right = itemRect.m_left + columnItem->getWidth();		

			columnItem->paint( context, &itemRect );
			
			prevLeft += columnItem->getWidth();			
		}
		prevTop = itemRect.m_bottom;
	}
	
	Enumerator<TreeItem*>* rootItems = m_treeModel->getRootItems();	
	
	if ( NULL != rootItems ) {
		itemRect.m_left = 0;
		itemRect.m_right = getWidth();
		while ( true == rootItems->hasMoreElements() ) {
			TreeItem* item = rootItems->nextElement();
			itemRect.m_top = prevTop;
			itemRect.m_bottom = itemRect.m_top + m_itemHeight;
			item->paint( context, &itemRect );
			
			prevTop += itemRect.getHeight();
		}
	}
}

void TreeListControl::init()
{
	m_treeModel = NULL;
	m_currentSelectedItem = NULL;
	m_treeModel = new DefaultTreeListModel( this );
	m_itemHeight = 25.0;
	m_itemIndent = 19;
	m_columnHeight = m_itemHeight;

	setColor( GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor( SYSCOLOR_WINDOW ) );
}	

void TreeListControl::addItem( DefaultTreeListItem* item, DefaultTreeListItem* parent )
{
	m_treeModel->addNodeItem( item, parent );
	item->setControl( this );
	if ( NULL != parent ) {
		parent->expand( true );
	}
	repaint();
}

void TreeListControl::addColumnHeaderItem( DefaultColumnHeaderItem* columnItem )
{
	m_treeModel->addColumnHeaderItem( columnItem );
	repaint();
}

void TreeListControl::insertColumnHeaderItem( DefaultColumnHeaderItem* columnItem, const ulong32& index )
{
	m_treeModel->insertColumnHeaderItem( columnItem, index );
	repaint();
}

void TreeListControl::removeColumnHeaderItem( DefaultColumnHeaderItem* columnItem )
{
	m_treeModel->removeColumnHeaderItem( columnItem );
	repaint();
}

void TreeListControl::removeColumnHeaderItem( const ulong32& index )
{
	m_treeModel->removeColumnHeaderItem( index );
	repaint();
}

DefaultTreeListItem* TreeListControl::hitTest( Point* pt, DefaultTreeListItem* itemToTest )
{
	DefaultTreeListItem* result = NULL;
	if ( true == itemToTest->containsPoint( pt ) ) {
		result = itemToTest;
	}
	else {
		if ( false == itemToTest->isLeaf() ) {
			Enumerator<TreeItem*>* children = itemToTest->getChildren();
			while ( true == children->hasMoreElements() ) {
				TreeItem* item = children->nextElement();
				result = hitTest( pt, (DefaultTreeListItem*)item );
				if ( NULL != result ) {
					break;
				}
			}
		}
	}

	return result;
}

void TreeListControl::setItemIndent( const ulong32& itemIndent )
{
	m_itemIndent = itemIndent;
	repaint();
}

void TreeListControl::mouseDown( MouseEvent* event )
{
	CustomControl::mouseDown( event );
	DefaultTreeListItem* foundItem = NULL;
	Enumerator<TreeItem*>* rootChildren = this->m_treeModel->getRootItems();
	while ( rootChildren->hasMoreElements() ) {
		TreeItem* rootItem = rootChildren->nextElement();
		foundItem = hitTest( event->getPoint(), (DefaultTreeListItem*)rootItem );
		if ( NULL != foundItem ) {
			foundItem->expand( !foundItem->isExpanded() );
			if ( m_currentSelectedItem != NULL ) {
				if ( m_currentSelectedItem != foundItem ) {
					m_currentSelectedItem->setSelected( false );
				}
			}
			m_currentSelectedItem = foundItem;
			m_currentSelectedItem->setSelected( true );
			repaint();
			break;
		}
	}	
}

void TreeListControl::mouseMove( MouseEvent* event )
{
	CustomControl::mouseMove( event );
}

void TreeListControl::mouseUp( MouseEvent* event )
{
	CustomControl::mouseUp( event );
}