/**
*Visual Control Framework for C++
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
*Visual Control Framework
*/ 
// ControlEvent.cpp
#include "ApplicationKit.h"



using namespace VCF;


ControlEvent::ControlEvent( Object* source ):
	Event( source )
{
	//this->init();
}


ControlEvent::ControlEvent( Object* source, const ulong32& eventType ):
	Event( source, eventType )
{
	this->init();
}

ControlEvent::~ControlEvent()
{

}

void ControlEvent::init() 
{
	this->m_newParent = NULL;
}
