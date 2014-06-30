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

#ifndef _CURSOR_H__
#define _CURSOR_H__



namespace VCF  {

class CursorPeer;

/**
*Class Cursor documentation
*/
class APPKIT_API Cursor : public Object { 
public:
	enum SystemCursorType{
		SCT_DEFAULT=0,
		SCT_CROSSHAIRS,
		SCT_SPLIT_VERT,
		SCT_SPLIT_HORZ,
		SCT_SIZE_HORZ,
		SCT_SIZE_VERT,
		SCT_SIZE_NE_SW,
		SCT_SIZE_NW_SE,
		SCT_TEXT,
		SCT_HELP,
		SCT_NOWAYHOSER,
		SCT_WAITING,
		SCT_WAIT
	};

	Cursor( const SystemCursorType& systemCursor );

	Cursor( Image* cursorImage, Point* hotSpot );

	Cursor( const String& cursorName, AbstractApplication* application=NULL );

	virtual ~Cursor();

	long getCursorID();

	void setCursorFromID( const long& cursorID );	

	CursorPeer* getPeer();

protected:
	CursorPeer* m_peer;
};


}; //end of namespace VCF

#endif //_CURSOR_H__


