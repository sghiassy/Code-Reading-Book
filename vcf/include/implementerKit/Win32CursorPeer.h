//Win32CursorPeer.h

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

#ifndef _WIN32CURSORPEER_H__
#define _WIN32CURSORPEER_H__


namespace VCF  {

class Cursor;
/**
*Class Win32CursorPeer documentation
*/
class APPKIT_API Win32CursorPeer : public Object, public CursorPeer { 
public:
	Win32CursorPeer( Cursor* cursor = NULL);

	virtual ~Win32CursorPeer();

	virtual ulong32 getCursorHandleID(){
		return (ulong32)m_hCursor;
	}

	virtual void createFromImage( Image* cursorImage, Point* hotSpot );

	virtual void createSystemCursor( const Cursor::SystemCursorType& systemCursor );

	virtual void createFromResourceName( const String& cursorName, const ulong32& instanceHandle=0 );

	virtual long getCursorID() {
		return m_cursorID;
	}

	virtual void setCursorID( const long& cursorID ) {
		m_cursorID = cursorID;
	}	

	virtual Cursor* getCursor() {
		return  m_cursor;
	}

	virtual void setCursor( Cursor* cursor ) {
		m_cursor = cursor;
	}
protected:
	static HBITMAP colorDDBToMonoDDB( HBITMAP hbm, DWORD biStyle, WORD biBits, HPALETTE hpal );
	static WORD paletteSize( void* pv );
	static WORD dibNumColors ( void* pv );

	bool m_isSharedCursor;
	HCURSOR m_hCursor;
	long m_cursorID;
	Cursor* m_cursor;

};


}; //end of namespace VCF

#endif //_WIN32CURSORPEER_H__


