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

#ifndef _WIN32FONTMANAGER_H__
#define _WIN32FONTMANAGER_H__




namespace VCF {

class Win32Font;

#define DEFAULT_VCF_FONT_NAME		"Arial"


/**
*used to hold a HFONT and a refcount
*/
class GRAPHICSKIT_API Win32FontHandle : public Object {
public:
	Win32FontHandle( HFONT font );
	
	virtual ~Win32FontHandle();

	void incRef();

	/**
	*decrement the ref count. If the count goes to 0 then
	*delete the HFONT object
	*/
	void decRef();

	HFONT getFontHandle();

	bool isRefCountZero();
private:
	int m_refCount;
	HFONT m_fontHandle;
};


/**
*Uses to manage fonts for controls
*/
class GRAPHICSKIT_API Win32FontManager : public Object{

public:
	static Win32FontManager* create();

	Win32FontManager();

	virtual ~Win32FontManager();
	
	virtual void init();

	/**
	*this is called by the Win32Font class to add the
	*the font peer to the font manager.
	*/
	static void initializeFont( Win32Font* font );

	static void removeFont( Win32Font* font ); 
	
	static HFONT getFontHandleFromFontPeer( Win32Font* font );

	static Win32FontManager* getFontManager() {
		return Win32FontManager::win32FontMgr;
	}
private:
	/**
	*adds the font to the manager called internally
	*/
	void addFont( Win32Font* font );

	std::map<String, Win32FontHandle*> m_fontMap;
	Win32Font* m_defaultFont;
	
	/**
	*makes a string id (like a hash code - there probably is a better way to do this.....
	*of the Win32Font.
	*the ID is the Name + Color + 
	*/
	String makeStringID( Win32Font* font );
	
	static Win32FontManager* win32FontMgr;
};

}; //end of namespace VCF

#endif //_WIN32FONTMANAGER_H__