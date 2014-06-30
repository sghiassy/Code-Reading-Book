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

// Win32MenuItem.cpp

#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"

#define BASE_MENU_ID	32778
int globalLastMenuID = BASE_MENU_ID;

#define CXGAP				1		// num pixels between button and text
#define CXTEXTMARGIN		2		// num pixels after hilite to start text
#define CXBUTTONMARGIN		2		// num pixels wider button is than bitmap
#define CYBUTTONMARGIN		2		// ditto for height


using namespace VCF;

std::map<uint32, MenuItem*> Win32MenuItem::menuItemMap;


Win32MenuItem::Win32MenuItem( MenuItem* item )
{	
	this->m_menuItem = item;	
	init();
}

Win32MenuItem::Win32MenuItem()
{	
	init();
}

Win32MenuItem::~Win32MenuItem()
{
	std::map<uint32,MenuItem*>::iterator found = menuItemMap.find( this->m_itemId );
	if ( found != menuItemMap.end() ){
		menuItemMap.erase( found );
	}
	globalLastMenuID --;
	if ( NULL != m_itemHandle ){
		::DeleteObject( m_itemHandle );
	}
}

void Win32MenuItem::init() 
{
	this->m_itemHandle = NULL;
	m_itemId = globalLastMenuID;	
	globalLastMenuID ++;
	menuItemMap[m_itemId] = m_menuItem;
}

void Win32MenuItem::insertSimpleMenuItem( MenuItem* child, HMENU menu )
{

	MenuItemPeer* itemImpl = child->getPeer();
	Win32MenuItem* win32ChildImpl = (Win32MenuItem*)itemImpl;
	MENUITEMINFO info = {0};
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_ID  | MIIM_STATE | MIIM_TYPE;
	info.fState = MFS_ENABLED;
	if ( true == child->isSeparator() ){
		info.fType |= MFT_SEPARATOR;
	}
	else {
		info.fType = MFT_STRING;
	}
	
	//put this back in later
	if ( true == child->canPaint() ) {
		info.fType |= MFT_OWNERDRAW;
	}
	info.wID = win32ChildImpl->m_itemId;
	
	if ( child->hasChildren() ){		
		info.fMask |= MIIM_SUBMENU;
		info.hSubMenu = (HMENU) itemImpl->getMenuID();
	}
	String itemName = child->getCaption();
	
	info.cch = itemName.size();

	char* tmpName = new char[info.cch+1];
	memset( tmpName, 0, info.cch+1 );
	itemName.copy( tmpName, info.cch );

	info.dwTypeData = tmpName;

	InsertMenuItem( menu, info.wID, TRUE, &info );


	MenuItem* parentItem = m_menuItem->getParent();
	
	if ( NULL != parentItem ) {
		int index = this->m_menuItem->getIndex();
		MENUITEMINFO thisInfo = {0};
		thisInfo.cbSize = sizeof(MENUITEMINFO);
		thisInfo.fMask = MIIM_SUBMENU;
		Win32MenuItem* win32ParentImpl = (Win32MenuItem*)parentItem->getPeer();
		HMENU parentMenuHandle = (HMENU)win32ParentImpl->getMenuID();
		
		if ( NULL != parentMenuHandle ){
			if ( TRUE == GetMenuItemInfo( parentMenuHandle, index, TRUE, &thisInfo ) ){
				thisInfo.fMask |= MIIM_SUBMENU;
				thisInfo.hSubMenu = (HMENU)getMenuID();
				::SetMenuItemInfo( parentMenuHandle, index, TRUE, &thisInfo );
			}
			else {
				//throw exception
			}	
		}
		
	}

	delete [] tmpName;
}

void Win32MenuItem::addChild( MenuItem* child )
{
	if ( NULL != child ){
		insertSimpleMenuItem( child, (HMENU)this->getMenuID() );
	}
}

void Win32MenuItem::insertChild( const unsigned long& index, MenuItem* child )
{
	child->setIndex( index );
	insertSimpleMenuItem( child, m_itemHandle );
}
	
void Win32MenuItem::deleteChild( MenuItem* child )
{
	int index = child->getIndex();
	DeleteMenu( (HMENU)this->getMenuID(), index, MF_BYPOSITION );
}

void Win32MenuItem::deleteChild( const unsigned long& index )
{
	DeleteMenu( (HMENU)this->getMenuID(), index, MF_BYPOSITION );
}

void Win32MenuItem::clearChildren()
{
	int count = GetMenuItemCount( m_itemHandle );
	for ( int i=0;i<count;i++ ){
		DeleteMenu( m_itemHandle, i, MF_BYPOSITION );
	}
}

bool Win32MenuItem::isChecked()
{
	if ( true == m_menuItem->isSeparator() ){
		return false;
	}

	int index = this->m_menuItem->getIndex();

	MENUITEMINFO info = {0};
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	HMENU menuHandle = (HMENU)this->getMenuID();
	if ( NULL != menuHandle ){
		if ( TRUE == GetMenuItemInfo( menuHandle, index, TRUE, &info ) ){
			return ((info.fState & MFS_CHECKED) != 0) ? true : false;
		}
	}
	return false;
}

void Win32MenuItem::setChecked( const bool& checked )
{
	if ( true == m_menuItem->isSeparator() ){
		return;
	}
	int index = this->m_menuItem->getIndex();
	MENUITEMINFO info = {0};
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	MenuItem* parent = this->getParent();
	if ( NULL != parent ){
		MenuItemPeer* parentPeer = parent->getPeer();
		HMENU menuHandle = (HMENU)parentPeer->getMenuID();
		if ( NULL != menuHandle ){
			if ( TRUE == GetMenuItemInfo( menuHandle, index, TRUE, &info ) ){
				if ( checked ){
					info.fState |= MFS_CHECKED;	
					info.fState &= ~MFS_UNCHECKED;
				}
				else {
					info.fState &= ~MFS_CHECKED;
					info.fState |= MFS_UNCHECKED;
				}
				
				SetMenuItemInfo( menuHandle, index, TRUE, &info );
			}	
		}
	}
}

bool Win32MenuItem::hasParent()
{
	return false;
}

MenuItem* Win32MenuItem::getParent()
{
	return this->m_menuItem->getParent();
}

MenuItem* Win32MenuItem::getChildAt( const unsigned long& index )
{
	return NULL;
}

bool Win32MenuItem::isEnabled()
{
	if ( true == m_menuItem->isSeparator() ){
		return false;
	}
	
	MenuItem* parent = this->getParent();
	if ( NULL != parent ) {
		int index = this->m_menuItem->getIndex();

		MENUITEMINFO info = {0};
		info.cbSize = sizeof(MENUITEMINFO);
		info.fMask = MIIM_STATE;
		MenuItemPeer* parentPeer = parent->getPeer();
		HMENU menuHandle = (HMENU)parentPeer->getMenuID();
		//HMENU menuHandle = (HMENU)this->getMenuID();
		if ( NULL != menuHandle ){
			if ( TRUE == GetMenuItemInfo( menuHandle, index, TRUE, &info ) ){
				bool res = ((info.fState == MFS_ENABLED) != 0) ? true : false;
				return res;
			}
			else {
				int err = GetLastError();
			}
		}
	}
	return false;
}

void Win32MenuItem::setEnabled( const bool& enabled )
{
	if ( true == m_menuItem->isSeparator() ){
		return;
	}
	int index = this->m_menuItem->getIndex();
	MENUITEMINFO info = {0};
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	MenuItem* parent = this->getParent();
	if ( NULL != parent ){
		MenuItemPeer* parentPeer = parent->getPeer();
		HMENU menuHandle = (HMENU)parentPeer->getMenuID();
		if ( NULL != menuHandle ){
			if ( TRUE == GetMenuItemInfo( menuHandle, index, TRUE, &info ) ){
				if ( true == enabled ){
					info.fState |= MFS_ENABLED;		
					info.fState &= ~MFS_DISABLED;
				}
				else {
					info.fState &= ~MFS_ENABLED;
					info.fState |= MFS_DISABLED;

				}
				
				SetMenuItemInfo( menuHandle, index, TRUE, &info );
			}	
		}
	}
}

bool Win32MenuItem::isVisible()
{
	return true;
}

void Win32MenuItem::setVisible( const bool& visible )
{
	int index = this->m_menuItem->getIndex();
	if ( true == visible ){
		
	}
	else {
		//RemoveMenu( this->m_itemHandle, index, MF_BYPOSITION );
	}
}

bool Win32MenuItem::getRadioItem()
{
	return false;
}

void Win32MenuItem::setRadioItem( const bool& value )
{
	if ( true == m_menuItem->isSeparator() ){
		return;
	}
	int index = this->m_menuItem->getIndex();
	MENUITEMINFO info = {0};
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_TYPE | MIIM_STATE;
	MenuItem* parent = this->getParent();
	if ( NULL != parent ){
		MenuItemPeer* parentPeer = parent->getPeer();
		HMENU menuHandle = (HMENU)parentPeer->getMenuID();
		if ( NULL != menuHandle ){
			if ( TRUE == GetMenuItemInfo( menuHandle, index, TRUE, &info ) ){
				
				info.fType |= MFT_RADIOCHECK;
				if ( true == value ){						
					info.fState |= MFS_CHECKED;	
				}
				else {
					info.fState &= ~MFS_CHECKED;					
				}
				
				SetMenuItemInfo( menuHandle, index, TRUE, &info );
			}	
		}
	}
}

void Win32MenuItem::setMenuItem( MenuItem* item )
{
	m_menuItem = item;
}

void Win32MenuItem::setCaption( const String& caption )
{
	if ( true == m_menuItem->isSeparator() ){
		return;
	}
	int index = this->m_menuItem->getIndex();
	MENUITEMINFO info = {0};
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_TYPE;
	MenuItem* parent = this->getParent();
	if ( NULL != parent ){
		MenuItemPeer* parentPeer = parent->getPeer();
		HMENU menuHandle = (HMENU)parentPeer->getMenuID();
		if ( NULL != menuHandle ){
			if ( TRUE == GetMenuItemInfo( menuHandle, index, TRUE, &info ) ){
				info.cch = caption.size();
				
				VCFChar* tmpName = new VCFChar[info.cch+1];
				memset( tmpName, 0, info.cch+1 );
				caption.copy( tmpName, info.cch );
				
				info.dwTypeData = tmpName;
				
				SetMenuItemInfo( menuHandle, index, TRUE, &info );	
				
				delete [] tmpName;				
			}	
		}
	}
}

ulong32 Win32MenuItem::getMenuID()
{
	if ( NULL != m_menuItem ){
		//throw exception !!!
		//throw InvalidPointerException();
		Menu* menuOwner = m_menuItem->getMenuOwner();
		if ( NULL == menuOwner ){
			//throw exception !!!!
			//throw InvalidPointerException();
		}
		if ( (NULL == m_itemHandle) && (NULL != menuOwner)  ){			
			PopupMenu* popupOwner = dynamic_cast<PopupMenu*>( menuOwner );
			if ( NULL != popupOwner ){
				m_itemHandle = ::CreatePopupMenu();
			}
			else {
				MenuBar* menuBarOwner = dynamic_cast<MenuBar*>( menuOwner );
				if ( NULL != menuBarOwner ){
					m_itemHandle = ::CreateMenu();
				}
				else {
					throw RuntimeException(MAKE_ERROR_MSG("Unknown or Invalid Menu Item owner"), __LINE__);
				}
			}

			if ( NULL == m_itemHandle  ){
				throw RuntimeException(MAKE_ERROR_MSG("Error allocation Resources for Menu Item Peer"), __LINE__);	
			}
		}		
	}
	return (ulong32)m_itemHandle;
}

MenuItem* Win32MenuItem::getMenuItemFromID( const uint32 id )
{
	MenuItem* result = NULL;
	
	std::map<uint32, MenuItem*>::iterator found = Win32MenuItem::menuItemMap.find( id );
	if ( found != Win32MenuItem::menuItemMap.end() ){
		result = found->second;
	}
	return result;	
}

void Win32MenuItem::setAsSeparator( const bool& isSeperator )
{
	int index = this->m_menuItem->getIndex();
	MENUITEMINFO info = {0};
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_TYPE | MIIM_STATE;
	MenuItem* parent = this->getParent();
	if ( NULL != parent ){
		MenuItemPeer* parentPeer = parent->getPeer();
		HMENU menuHandle = (HMENU)parentPeer->getMenuID();
		if ( NULL != menuHandle ){
			if ( TRUE == GetMenuItemInfo( menuHandle, index, TRUE, &info ) ){				
				if ( true == isSeperator ) {
					info.fType |= MFT_SEPARATOR;
				}
				else {
					info.fType &= ~MFT_SEPARATOR;
				}				
				
				SetMenuItemInfo( menuHandle, index, TRUE, &info );
			}	
		}
	}
}

void Win32MenuItem::fillInMeasureItemInfo( MEASUREITEMSTRUCT& measureItemInfo )
{

	int index = this->m_menuItem->getIndex();
	MENUITEMINFO info = {0};
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_TYPE;
	MenuItem* parent = this->getParent();
	if ( NULL != parent ){
		MenuItemPeer* parentPeer = parent->getPeer();
		HMENU menuHandle = (HMENU)parentPeer->getMenuID();
		if ( NULL != menuHandle ){
			if ( TRUE == GetMenuItemInfo( menuHandle, index, TRUE, &info ) ){
				if ( ODT_MENU != measureItemInfo.CtlType ) {
					return;
				}
				
				if ( (info.fType & MFT_SEPARATOR) != 0 ) {
					// separator: use half system height and zero width
					measureItemInfo.itemHeight = GetSystemMetrics(SM_CYMENU)>>1;
					measureItemInfo.itemWidth  = 0;					
				} 
				else {
					// compute size of text: use DrawText with DT_CALCRECT					
					NONCLIENTMETRICS ncInfo;
					ncInfo.cbSize = sizeof(ncInfo);
					::SystemParametersInfo (SPI_GETNONCLIENTMETRICS, sizeof(info), &ncInfo, 0);
					HFONT menuHFont = CreateFontIndirect( &ncInfo.lfMenuFont );
					if ( NULL != menuHFont ) {
						String caption = m_menuItem->getCaption();

						HDC dc = ::CreateCompatibleDC( NULL );// screen DC--I won't actually draw on it
						HFONT oldFont = (HFONT)SelectObject( dc, menuHFont );
						RECT rcText = {0,0,0,0};						
						
						::DrawText( dc, caption.c_str(), caption.size(), &rcText, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_CALCRECT);
						
						SelectObject( dc, oldFont );
						
						// height of item is just height of a standard menu item
						measureItemInfo.itemHeight = __max( ::GetSystemMetrics(SM_CYMENU), abs(rcText.bottom - rcText.top) );
						
						// width is width of text plus a bunch of stuff
						int cx = rcText.right - rcText.left;	// text width 
						cx += CXTEXTMARGIN << 1;		// L/R margin for readability
						cx += CXGAP;					// space between button and menu text
						
						//cx += m_szButton.cx<<1;		// button width (L=button; R=empty margin)
						
						// whatever value I return in lpms->itemWidth, Windows will add the
						// width of a menu checkmark, so I must subtract to defeat Windows. Argh.
						//
						cx += GetSystemMetrics(SM_CXMENUCHECK)-1;
						measureItemInfo.itemWidth = cx;		// done deal	
						
						::DeleteObject( menuHFont );
						::DeleteDC( dc );
					}					
				}
			}	
		}
	}
}

void Win32MenuItem::drawDefaultMenuItem( const UINT& idCtl, DRAWITEMSTRUCT& drawItemStruct )
{	
	if ( drawItemStruct.CtlType != ODT_MENU ) {
		return; // not handled by me
	}
	
	if ( NULL == drawItemStruct.hDC ) {
		return; //throw exception ??
	}	
	
	int index = this->m_menuItem->getIndex();
	MENUITEMINFO info = {0};
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_TYPE | MIIM_CHECKMARKS;
	MenuItem* parent = this->getParent();
	if ( NULL != parent ){
		MenuItemPeer* parentPeer = parent->getPeer();
		HMENU menuHandle = (HMENU)parentPeer->getMenuID();
		if ( NULL != menuHandle ){
			if ( TRUE == GetMenuItemInfo( menuHandle, index, TRUE, &info ) ){
				
				if ( (info.fType & MFT_SEPARATOR) != 0 ) {
					// draw separator
					RECT tmp = drawItemStruct.rcItem;

					tmp.top += (tmp.bottom - tmp.top) >>1; 	// vertical center

					DrawEdge( drawItemStruct.hDC, &tmp, EDGE_ETCHED, BF_TOP);		// draw separator line					
				} 
				else {
					// not a separator
					bool menuDisabled = (drawItemStruct.itemState & ODS_GRAYED) != 0;
					bool menuSelected = (drawItemStruct.itemState & ODS_SELECTED) != 0;
					bool menuChecked  = (drawItemStruct.itemState & ODS_CHECKED) != 0;
					bool menuHasButn = false;

					

					RECT tmpBtnRect = {drawItemStruct.rcItem.left + 2, drawItemStruct.rcItem.top,
										drawItemStruct.rcItem.left + 2 + (drawItemStruct.rcItem.right - drawItemStruct.rcItem.left),
										drawItemStruct.rcItem.top + (drawItemStruct.rcItem.bottom - drawItemStruct.rcItem.top) };

					// no button: look for custom checked/unchecked bitmaps																		
					if ( (true == menuChecked) || (NULL != info.hbmpUnchecked) ) {
						menuHasButn = 
							draw3DCheckmark( drawItemStruct.hDC, tmpBtnRect, 
												menuSelected, 
												menuChecked ? info.hbmpChecked : info.hbmpUnchecked );
					}
					
					// Done with button, now paint text. First do background if needed.
					int cxButn = tmpBtnRect.bottom - tmpBtnRect.top;				// width of button

					COLORREF colorBG = ::GetSysColor( menuSelected ? COLOR_HIGHLIGHT : COLOR_MENU );

					if ( (true == menuSelected) || (drawItemStruct.itemAction==ODA_SELECT) ) {
						// selected or selection state changed: paint text background
						RECT rcBG = drawItemStruct.rcItem;	// whole rectangle

						if ( true == menuHasButn) {	// if there's a button:
							rcBG.left += cxButn + CXGAP;			//  don't paint over it!
						}
						fillMenuItemRect( drawItemStruct.hDC, rcBG, colorBG );						
					}
					
					// compute text rectangle and colors
					RECT rcText = drawItemStruct.rcItem;				 // start w/whole item
					rcText.left += cxButn + CXGAP + CXTEXTMARGIN; // left margin
					rcText.right -= cxButn;				 // right margin
					::SetBkMode( drawItemStruct.hDC, TRANSPARENT );			 // paint transparent text
					
					COLORREF colorText = 
						GetSysColor( menuDisabled ?  COLOR_GRAYTEXT : 
										(menuSelected ? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT));
					
					// Now paint menu item text.	No need to select font,
					// because windows sets it up before sending WM_DRAWITEM
					//
					if ( (true == menuDisabled) && ((false == menuSelected) || (colorText == colorBG)) ) {
						// disabled: draw hilite text shifted southeast 1 pixel for embossed
						// look. Don't do it if item is selected, tho--unless text color same
						// as menu highlight color. Got it?
						//
						rcText.left += 1;
						rcText.top += 1;

						drawMenuItemText( drawItemStruct.hDC, rcText, GetSysColor(COLOR_3DHILIGHT) );
						rcText.left -= 1;
						rcText.top -= 1;
					}

					drawMenuItemText( drawItemStruct.hDC, rcText, colorText ); // finally!
				}				
			}
		}
	}
}

bool Win32MenuItem::draw3DCheckmark( HDC dc, const RECT& rc, const bool& bSelected, HBITMAP hbmCheck )
{
	bool result = false;

	return result;
}


// Shorthand to fill a rectangle with a solid color.
void Win32MenuItem::fillMenuItemRect( HDC dc, const RECT& rc, COLORREF color )
{
	HBRUSH brush = ::CreateSolidBrush( color );

	HBRUSH oldBrush = (HBRUSH)::SelectObject( dc, brush );

	PatBlt( dc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, PATCOPY);

	::SelectObject( dc, oldBrush );
	DeleteObject( brush );
}

void Win32MenuItem::drawMenuItemText( HDC dc, RECT rc, COLORREF color )
{
	String left = m_menuItem->getCaption();
	String right;

	int tabPos = left.find('\t');
	if ( tabPos != String::npos ) {
		if ( tabPos >= 0 ) {
			right = left.substr( tabPos + 1, left.size() - tabPos );
			
			left  = left.substr( 0, tabPos );
		}
	}
	
	::SetTextColor( dc, color );

	DrawText( dc, left.c_str(), left.size(), &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER );

	if (tabPos > 0) {
		DrawText( dc, right.c_str(), right.size(), &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_RIGHT);
	}
}