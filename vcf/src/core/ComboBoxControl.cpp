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

//ComboBoxControl
#include "ApplicationKit.h"
#include "DefaultListModel.h"

using namespace VCF;

ComboBoxControl::ComboBoxControl():
	CustomControl( true )//make this a heavyweight control !
{		
	this->init();	
}

void ComboBoxControl::init()
{
	INIT_EVENT_HANDLER_LIST(SelectionChanged)

	this->setBorder( new Basic3DBorder() );

	m_listModel = new DefaultListModel();
	
	m_listModel->addContentsChangedHandler( 
		new ListModelEventHandler<ComboBoxControl>( this, ComboBoxControl::onListModelContentsChanged, "ComboBox_onListModelContentsChanged" ) );

	m_listModel->addItemAddedHandler( 
		new ListModelEventHandler<ComboBoxControl>( this, ComboBoxControl::onItemAdded, "ComboBox_onItemAdded" ) );

	m_listModel->addItemDeletedHandler( 
		new ListModelEventHandler<ComboBoxControl>( this, ComboBoxControl::onItemDeleted, "ComboBox_onItemDeleted" ) );	
	

	m_selectedIndex = 0;
	m_arrowPressed = false;
}

ComboBoxControl::~ComboBoxControl()
{
	if ( NULL != m_listModel ){
		delete m_listModel;
		m_listModel = NULL;
	}
}

ListModel* ComboBoxControl::getListModel()
{
	return m_listModel;
}

void ComboBoxControl::setListModel(ListModel * model)
{
	m_listModel = model;
}

void ComboBoxControl::paint( GraphicsContext* context )
{
	CustomControl::paint( context );
	GraphicsToolkit* toolkit = GraphicsToolkit::getDefaultGraphicsToolkit();
	Color* hilight = toolkit->getSystemColor( SYSCOLOR_HIGHLIGHT );	
	Color* shadow = toolkit->getSystemColor( SYSCOLOR_SHADOW );

	double h = this->getHeight();
	double x = this->getWidth() - h;
	double y = 3.0;
	m_arrowRect.setRect( x, y, (x + h) -4, (y + h) -6 );
	
	Color* cl1 = hilight;
	Color* cl2 = shadow;
	if ( true == this->m_arrowPressed ) {
		cl1 = shadow;
		cl2 = hilight;
		context->rectangle( &m_arrowRect );
		context->setColor( shadow );
		context->fillPath();

		Border* border = this->getBorder();
		Rect innerRect;
		if ( NULL != border ){
			Rect* tmp = border->getClientRect( this );
			innerRect.setRect( tmp->m_left, tmp->m_top, m_arrowRect.m_left, tmp->m_bottom );
			innerRect.inflate( -2, -2 );
			innerRect.m_bottom -=1;
		}
		else {
			innerRect.setRect( 3, 3, m_arrowRect.m_left, m_arrowRect.m_bottom-1 );
		}
		context->moveTo( innerRect.m_left, innerRect.m_bottom );	
		context->lineTo( innerRect.m_left, innerRect.m_top );
		context->lineTo( innerRect.m_right, innerRect.m_top );
		context->setColor( shadow );
		context->strokePath();

		context->moveTo( innerRect.m_right, innerRect.m_top );
		context->lineTo( innerRect.m_right, innerRect.m_bottom );
		context->lineTo( innerRect.m_left, innerRect.m_bottom );
		context->setColor( hilight );
		context->strokePath();
	}
	else {

	}

	context->moveTo( x, y );
	context->lineTo( x, h - 3 );
	context->setColor( cl1 );
	context->strokePath();

	context->moveTo( x+1, y );
	context->lineTo( x+1, h - 3 );
	context->setColor( cl2 );
	context->strokePath();

	std::vector<Point*> triangle;
	Point pt1( x + h/4, h/2 - 2 );
	Point pt2( pt1.m_x + h/2, pt1.m_y );	
	Point pt3( x + h/2, pt1.m_y + h/4 );
	triangle.push_back( &pt1 );
	triangle.push_back( &pt2 );
	triangle.push_back( &pt3 );
	
	context->polyline( triangle );
	context->setColor( Color::getColor( "black" ) );	
	context->fillPath();
	ListItem* selectedItem = getSelectedItem();
	if ( NULL != selectedItem ){
		String caption = selectedItem->getCaption();
		Font* f = context->getCurrentFont();
		double sy = h/2 - (f->getPixelSize()/2);
		context->drawString( 5, sy, caption );	
	}	
}

void ComboBoxControl::onListModelContentsChanged( ListModelEvent* event )
{
	if ( NULL != event ){
		switch ( event->getType() ){
			case LIST_MODEL_CONTENTS_DELETED: {
				
			}
			break;

			case LIST_MODEL_ITEM_CHANGED: {
				ListItem* item = event->getListItem();
				if ( NULL != item ){

				}
			}
			break;		
		}
	}
}

void ComboBoxControl::onItemAdded( ListModelEvent* event )
{
}

void ComboBoxControl::onItemDeleted( ListModelEvent* event )
{
}

void ComboBoxControl::mouseDown( MouseEvent* event )
{
	CustomControl::mouseDown( event );
	
	this->m_arrowPressed = this->m_arrowRect.containsPt( event->getPoint() );
	if ( true == m_arrowPressed ){
		this->keepMouseEvents();
	}
	this->repaint();
}

void ComboBoxControl::mouseMove( MouseEvent* event )
{
	CustomControl::mouseMove( event );
	if ( event->hasLeftButton() ){
		this->m_arrowPressed = this->m_arrowRect.containsPt( event->getPoint() );
		this->repaint();
	}
}

void ComboBoxControl::mouseUp( MouseEvent* event )
{
	CustomControl::mouseUp( event );
	m_arrowPressed = false;
	this->repaint();
	this->releaseMouseEvents();
}

void ComboBoxControl::keyPressed( KeyboardEvent* event )
{
	switch ( event->getVirtualCode() ){
		case VIRT_KEY_UP_ARROW :{
			ulong32 index = m_selectedIndex + 1;
			
			if ( index >=	this->m_listModel->getCount() ){
				index = 0;
			}

			this->setSelectedItemIndex( index );

			this->repaint();
		}
		break;

		case VIRT_KEY_DOWN_ARROW :{
			ulong32 index = m_selectedIndex - 1;
			/**
			* this is done this way because we have an UNSIGNED long so we 
			*won't get negative numbers, anything over the count needs to be wrapped
			*/
			if ( index > this->m_listModel->getCount() ){ 
				index = this->m_listModel->getCount() -1;
			}

			this->setSelectedItemIndex( index );

			this->repaint();
		}
		break;
	}
}

ListItem* ComboBoxControl::getSelectedItem()
{
	ListItem* result = NULL;
	
	if ( NULL != m_listModel ){
		if ( m_listModel->getCount() > 0 ){
			result = m_listModel->getItemFromIndex( this->m_selectedIndex );
		}
	}
		
	return result;
}

void ComboBoxControl::setSelectedItem( ListItem* selectedItem )
{

}

void ComboBoxControl::setSelectedItemIndex( const ulong32& selectedIndex )
{
	m_selectedIndex = selectedIndex;
	if ( NULL != m_listModel ){
		if ( m_listModel->getCount() > 0 ){
			ListItem* item = m_listModel->getItemFromIndex( this->m_selectedIndex );
			if ( NULL != item ){
				item->setSelected( true );
				ItemEvent event( item, ITEM_EVENT_SELECTED );
				fireOnSelectionChanged( &event );
				this->repaint();
			}
		}
	}
}