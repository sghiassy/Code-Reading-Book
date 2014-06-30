/**
*Visual Component Framework for C++
*
*Copyright © 2000 Jim Crafton
*
*This program is free software you can redistribute it and/or
*modify it under the terms of the GNU General Public License
*as published by the Free Software Foundation either version 2
*of the License, or (at your option) any later version.
*
*This program is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*GNU General Public License for more details.
*
*You should have received a copy of the GNU General Public License
*along with this program if not, write to the Free Software
*Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*
*Contact Jim Crafton at ddiego@one.net for more information about the 
*Visual Component Framework
*/ 
#include "ApplicationKit.h"
#include "TableItemEditor.h"
#include "DefaultTableCellItem.h"
#include "BasicTableItemEditor.h"

using namespace VCF;

DefaultTableCellItem::DefaultTableCellItem()
{
	this->init();
}

DefaultTableCellItem::~DefaultTableCellItem()
{
	delete m_basicItemEditor;
	m_basicItemEditor = NULL;
}

void DefaultTableCellItem::init()
{
	INIT_EVENT_HANDLER_LIST(ItemPaint);
	INIT_EVENT_HANDLER_LIST(ItemChanged);
	INIT_EVENT_HANDLER_LIST(ItemSelected);
	INIT_EVENT_HANDLER_LIST(ItemAdded);
	INIT_EVENT_HANDLER_LIST(ItemDeleted);

	this->m_col = 0;
	m_imageIndex = 0;
	this->m_data = NULL;
	this->m_image = NULL;
	this->m_model = NULL;
	m_owningControl = NULL;
	this->m_row = 0;
	this->m_selected = false;
	m_isFixedItem = false;
	m_basicItemEditor = new BasicTableItemEditor( this );
}

bool DefaultTableCellItem::containsPoint( Point * pt )
{
	return m_bounds.containsPt( pt );
}

Image* DefaultTableCellItem::getImage()
{
	return this->m_image;
}

unsigned long DefaultTableCellItem::getIndex()
{
	return -1;
}

void DefaultTableCellItem::setIndex( const unsigned long& index )
{

}

void* DefaultTableCellItem::getData()
{
	return this->m_data;
}

void DefaultTableCellItem::setData( void* data )
{
	this->m_data = data;
}

Model* DefaultTableCellItem::getModel()
{
	return this->m_model;
}

void DefaultTableCellItem::setModel( Model* model )
{
	this->m_model = model;
}

void DefaultTableCellItem::paint( GraphicsContext* context, Rect* paintRect )
{
	m_bounds.setRect( paintRect->m_left, paintRect->m_top, paintRect->m_right, paintRect->m_bottom );
	if ( true == m_isFixedItem ){
		this->m_border.paint( &m_bounds, context );
	}
	else {
		Rect tmp(m_bounds);
		context->setColor( &Color(0) );
		context->rectangle( &tmp );
		context->strokePath();

		if ( true == this->m_selected ){
			context->setColor( &Color(0xFCFCFC) );
		}
		else {
			context->setColor( &Color(0xCCCCCC) );
		}
		
		tmp.inflate(-1,-1);
		context->rectangle( &tmp );
		context->fillPath();
	}
	
	double x = paintRect->m_left + 5;
	Font* f = context->getCurrentFont();
	double y = paintRect->m_top + ((paintRect->getHeight() / 2) - (f->getPixelSize()/2));
	Rect textRect( x, y, paintRect->m_right, paintRect->m_bottom ); 
	context->textBoundedBy( &textRect, m_caption, false );
}

bool DefaultTableCellItem::isSelected()
{
	return m_selected;
}

void DefaultTableCellItem::setSelected( const bool& selected )
{
	this->m_selected = selected;
	if ( true == m_selected ){

	}
	ItemEvent e( this, ITEM_EVENT_SELECTED );
	fireOnItemSelected( &e );
}

uint32 DefaultTableCellItem::getRow()
{
	return this->m_row;
}

void DefaultTableCellItem::setRow( const uint32& currentRow )
{
	this->m_row = currentRow;
}

uint32 DefaultTableCellItem::getColumn()
{
	return this->m_col;
}

void DefaultTableCellItem::setColumn( const uint32& currentColumn )
{
	this->m_col = currentColumn;
}

TableItemEditor* DefaultTableCellItem::getItemEditor()
{
	return m_basicItemEditor;
}

bool DefaultTableCellItem::isItemEditable(){
	bool result = false;
	if ( false == m_isFixedItem ) {
		TableItemEditor* editor = getItemEditor();
		if ( NULL != editor ){
			result = editor->isCellEditable();
		}
	}
	return result;
}	

bool DefaultTableCellItem::isFixed()
{
	return m_isFixedItem;
}

void DefaultTableCellItem::setFixed( const bool& isFixed )
{
	m_isFixedItem = isFixed;
}

String DefaultTableCellItem::getCaption()
{
	return m_caption;
}

void DefaultTableCellItem::setCaption( const String& caption )
{
	m_caption = caption;
}

void DefaultTableCellItem::setImageIndex( const ulong32& imageIndex )
{
	m_imageIndex = imageIndex;
}