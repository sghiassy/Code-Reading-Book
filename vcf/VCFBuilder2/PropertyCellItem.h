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

#ifndef _PROPERTYCELLITEM_H__
#define _PROPERTYCELLITEM_H__

#include "DefaultTableCellItem.h"

namespace VCF {

class PropertyCellItemEditor;

class PropertyCellItem : public DefaultTableCellItem{

public:
	PropertyCellItem();

	virtual ~PropertyCellItem();

	virtual void paint( GraphicsContext* context, Rect* paintRect );

	void setProperty( Property* property ){
		m_property = property;	
	};

	Property* getProperty(){
		return m_property;
	}

	virtual TableItemEditor* getItemEditor();

private:
	Property* m_property;
	PropertyCellItemEditor* m_propertyItemEditor;
};

}; //end of namespace VCF

#endif //_PROPERTYCELLITEM_H__