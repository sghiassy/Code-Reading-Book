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

#ifndef _EVENTCELLITEM_H__
#define _EVENTCELLITEM_H__

#include "DefaultTableCellItem.h"

namespace VCF {

class EventCellItem : public DefaultTableCellItem{

public:
	EventCellItem();

	virtual ~EventCellItem();

	virtual void paint( GraphicsContext* context, Rect* paintRect );

	EventProperty* getEvent(){
		return m_event;
	}

	void setEvent( EventProperty* event ){
		m_event = event;
	}
private:
	EventProperty* m_event;
};

}; //end of namespace VCF

#endif //_EVENTCELLITEM_H__