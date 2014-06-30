//CursorManager.h
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

using namespace VCF;



CursorManager* CursorManager::create()
{
	if ( NULL == CursorManager::cursorMgrInstance ) {
		CursorManager::cursorMgrInstance = new CursorManager();
		CursorManager::cursorMgrInstance->init();
	}

	return CursorManager::cursorMgrInstance;
}


CursorManager* CursorManager::getCursorManager()
{
	return CursorManager::cursorMgrInstance;
}

CursorManager::CursorManager()
{
	m_cursorIDCount = (long)(Cursor::SCT_WAIT) + 1;	
}

CursorManager::~CursorManager()
{
	std::map<long,Cursor*>::iterator it = m_cursorMap.begin();
	while ( it != m_cursorMap.end() ) {
		Cursor* cursor = it->second;
		delete cursor;
		cursor = NULL;
		it ++;
	}
	m_cursorMap.clear();
}

Cursor* CursorManager::getCursor( const long& cursorID )
{
	Cursor* result = NULL;
	std::map<long,Cursor*>::iterator found = m_cursorMap.find( cursorID );
	if ( found != m_cursorMap.end() ) {
		result = found->second;
	}
	else {
		
		if ( (cursorID >= (long)Cursor::SCT_DEFAULT) && (cursorID <= (long)Cursor::SCT_WAIT) ) {
			result = new Cursor( (Cursor::SystemCursorType)cursorID );
		}
	}

	return result;
}

void CursorManager::registerCursor( Cursor* cursor )
{
	long cursorID = cursor->getCursorID();
	std::map<long,Cursor*>::iterator found = m_cursorMap.find( cursorID );
	if ( found == m_cursorMap.end() ) {
		//only add the cursor to the map if it isn't already there
		if ( (cursorID == CursorManager::UNREGISTERED_ID) || 
			((cursorID >= Cursor::SCT_DEFAULT) && (cursorID <= Cursor::SCT_WAIT)) ) {
			if ( cursorID == CursorManager::UNREGISTERED_ID ) {
				cursorID = m_cursorIDCount;
				cursor->getPeer()->setCursorID( cursorID );
				m_cursorIDCount ++;
			}
			m_cursorMap[cursorID] = cursor;
		}
		else {
			//this is bad - somehow the cursor has an ID but is not present in the
			//map - possibly throw an exception ?
		}
	}
}

