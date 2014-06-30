/**
*Visual Component Framework for C++
*
*Copyright © 2000 Jim Crafton
*
*This program is free software; you can redistribute it and/or
*modify it under the terms of the GNU General Public License
*as published by the Free Software Foundation; either version 2
*of the License, or (at your option) any later version.
*
*This program is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*GNU General Public License for more details.
*
*You should have received a copy of the GNU General Public License
*along with this program; if not, write to the Free Software
*Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*
*Contact Jim Crafton at ddiego@one.net for more information about the 
*Visual Component Framework
*/ 
#include "ApplicationKit.h"
#include "TableItemEditor.h"
#include "PropertyCellItem.h"
#include "PropertyCellItemEditor.h"
#include "PropertyEditorManager.h"

using namespace VCF;

PropertyCellItem::PropertyCellItem()
{
	m_property = NULL;
	m_propertyItemEditor = NULL;
	m_propertyItemEditor = new PropertyCellItemEditor( this );	
}

PropertyCellItem::~PropertyCellItem()
{
	if ( NULL != m_propertyItemEditor ){
		delete m_propertyItemEditor;
		m_propertyItemEditor = NULL;
	}
}

void PropertyCellItem::paint( GraphicsContext* context, Rect* paintRect )
{	
	VariantData* v = NULL;
	if ( NULL != m_property ){
		v = m_property->get();
		if ( NULL != v ){
			String propertyClassName = Class::getClassNameForProperty( m_property );
			PropertyEditor* pe = PropertyEditorManager::findEditor( propertyClassName );
			if ( NULL != pe ){				
				if ( true == pe->canPaintValue() ){
					DefaultTableCellItem::paint( context, paintRect );
					pe->setValue( v );
					pe->paintValue( context, *paintRect );
				}
				else {
					String s = v->toString();
					if ( true == s.empty() ){
						s = "( " + propertyClassName + " )";
					}
					this->setCaption( s );
					DefaultTableCellItem::paint( context, paintRect );
				}
			}
			else{
				String s = v->toString();
				if ( true == s.empty() ){
					s = "( " + propertyClassName + " )";
				}
				this->setCaption( s );
				DefaultTableCellItem::paint( context, paintRect );
			}
		}
		else {
			this->setCaption( "null" );
			DefaultTableCellItem::paint( context, paintRect );
		}
	}
	else{
		DefaultTableCellItem::paint( context, paintRect );
	}
}

TableItemEditor* PropertyCellItem::getItemEditor()
{
	return m_propertyItemEditor;
}