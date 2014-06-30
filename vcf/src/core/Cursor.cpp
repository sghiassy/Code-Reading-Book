//Cursor.h
/**
Copyright (c) 2000-2001, Jim Crafton
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
	Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in 
	the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

NB: This software will not save the world. 
*/

#include "ApplicationKit.h"


Cursor::Cursor( const SystemCursorType& systemCursor )
{
	m_peer = UIToolkit::getDefaultUIToolkit()->createCursorPeer( this );
	
	m_peer->createSystemCursor( systemCursor );
	CursorManager::getCursorManager()->registerCursor( this );
}

Cursor::Cursor( Image* cursorImage, Point* hotSpot )
{
	m_peer = UIToolkit::getDefaultUIToolkit()->createCursorPeer( this );

	m_peer->createFromImage( cursorImage, hotSpot );
	CursorManager::getCursorManager()->registerCursor( this );
}

Cursor::Cursor( const String& cursorName, AbstractApplication* application )
{ 
	m_peer = UIToolkit::getDefaultUIToolkit()->createCursorPeer( this );

	if ( NULL == application ) {
		m_peer->createFromResourceName( cursorName );
	}
	else {
		m_peer->createFromResourceName( cursorName, application->getPeer()->getHandleID() );
	}

	CursorManager::getCursorManager()->registerCursor( this );
}

Cursor::~Cursor()
{
	delete m_peer;
}

long Cursor::getCursorID()
{
	return m_peer->getCursorID();
}

void Cursor::setCursorFromID( const long& cursorID )
{

}

CursorPeer* Cursor::getPeer()
{
	return m_peer;
}

