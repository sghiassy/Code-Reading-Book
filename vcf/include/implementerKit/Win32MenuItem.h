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

// Win32MenuItem.h: interface for the Win32MenuItem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _WIN32MENUITEM_H__
#define _WIN32MENUITEM_H__


namespace VCF
{

class APPKIT_API Win32MenuItem : public MenuItemPeer  
{
public:
	Win32MenuItem();

	Win32MenuItem( MenuItem* item );

	virtual ~Win32MenuItem();

	void init();

	virtual ulong32 getMenuID();

	virtual void addChild( MenuItem* child );

	virtual void insertChild( const unsigned long& index, MenuItem* child );
	
	virtual void deleteChild( MenuItem* child );

	virtual void deleteChild( const unsigned long& index );

	virtual void clearChildren();

	virtual bool isChecked();

	virtual void setChecked( const bool& checked );

	virtual MenuItem* getParent();

	virtual bool hasParent();

	virtual MenuItem* getChildAt( const unsigned long& index );

	virtual bool isEnabled();

	virtual void setEnabled( const bool& enabled );

	virtual bool isVisible();

	virtual void setVisible( const bool& visible );

	virtual bool getRadioItem();

	virtual void setRadioItem( const bool& value );

	virtual void setMenuItem( MenuItem* item );

	virtual void setCaption( const String& caption ); 

	virtual void setAsSeparator( const bool& isSeperator );

	static MenuItem* getMenuItemFromID( const uint32 id );	

	void fillInMeasureItemInfo( MEASUREITEMSTRUCT& measureItemInfo );

	void drawDefaultMenuItem( const UINT& idCtl, DRAWITEMSTRUCT& drawItemStruct );
private:
	HMENU m_itemHandle;
	uint32 m_itemId;
	MenuItem* m_menuItem;
	
	void insertSimpleMenuItem( MenuItem* child, HMENU menu );

	bool draw3DCheckmark( HDC dc, const RECT& rc, const bool& bSelected, HBITMAP hbmCheck );

	void fillMenuItemRect( HDC dc, const RECT& rc, COLORREF color );

	void drawMenuItemText( HDC dc, RECT rc, COLORREF color );

	static std::map<uint32,MenuItem*> menuItemMap;
};

};
#endif // !defined(AFX_WIN32MENUITEM_H__080448FD_2C01_11D4_B53C_00C04F0196DA__INCLUDED_)
