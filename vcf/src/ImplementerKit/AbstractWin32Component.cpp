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

// AbstractWin32Component.cpp

#include "ApplicationKit.h"
#include "Win32Object.h"
#include "AbstractWin32Component.h"
#include "Win32ToolKit.h"
#include "Win32MenuItem.h"
#include "Win32FontManager.h"
#include "Scrollable.h"
#include "ScrollPeer.h"
#include "Win32ScrollPeer.h"
#include "Win32Peer.h"
#include "Win32Font.h"

using namespace VCF;

using namespace VCFWin32;



AbstractWin32Component::AbstractWin32Component()
{
	init();
	setPeerControl( NULL );
}

AbstractWin32Component::AbstractWin32Component( Control* component )
{	
	init();
	setPeerControl( component );
}

AbstractWin32Component::~AbstractWin32Component()
{
	if ( NULL != m_hwnd ){	
		

		BOOL err = ::DestroyWindow( m_hwnd );		
		if ( FALSE == err )  {
			//throw RuntimeException( MAKE_ERROR_MSG_2("DestroyWindow failed") );
			err = GetLastError();
		}
		destroyWindowHandle();
		m_hwnd = NULL;
		m_wndProc = NULL;
		m_defaultWndProc = NULL;
		m_peerControl = NULL;
	}
}

void AbstractWin32Component::init()
{
	
}

long AbstractWin32Component::getHandleID()
{
	long result = 0;
	result = (long)m_hwnd;
	return result;
}

VCF::String AbstractWin32Component::getText()
{
	VCF::String result = "";
	
	int size = ::GetWindowTextLength( m_hwnd )+1;
	if ( size > 1 ) {
		VCFChar* tmpText = new VCFChar[size];
		memset( tmpText, 0, size );
		::GetWindowText( m_hwnd, tmpText, size );
		result = tmpText;
		delete [] tmpText;
	}
	return result;
}

void AbstractWin32Component::setText( const VCF::String& text )
{
	::SetWindowText( m_hwnd, text.c_str() );
}

void AbstractWin32Component::setBounds( VCF::Rect* rect )
{
	::SetWindowPos( m_hwnd, NULL, rect->m_left, rect->m_top, 
                    rect->getWidth(), rect->getHeight(), SWP_NOACTIVATE | SWP_NOOWNERZORDER| SWP_NOZORDER );	
}

VCF::Rect* AbstractWin32Component::getBounds()
{
	VCF::Rect* result = NULL;
	RECT r;
	::GetWindowRect( m_hwnd, &r );
	
	HWND parent = ::GetParent( m_hwnd );

	if ( NULL != parent ){ 		
		POINT pt = {0};
		pt.x = r.left;	
		pt.y = r.top;
		::ScreenToClient( parent, &pt );
		r.left = pt.x;
		r.top = pt.y;

		pt.x = r.right;	
		pt.y = r.bottom;
		::ScreenToClient( parent, &pt );

		r.right = pt.x;
		r.bottom = pt.y;

		m_bounds.setRect( r.left, r.top, r.right, r.bottom );
		result = &m_bounds;
	}
	else{
		m_bounds.setRect( r.left, r.top, r.right, r.bottom );
		result = &m_bounds;	
	}
	return result;
}

void AbstractWin32Component::setVisible( const bool& visible )
{
	if ( true == visible ){
		::ShowWindow( m_hwnd, SW_SHOW );
	}
	else{
		::ShowWindow( m_hwnd, SW_HIDE );
	}
}

bool AbstractWin32Component::getVisible()
{
	bool result = false;
	DWORD style = GetWindowLong( m_hwnd, GWL_STYLE );
	result =  (style & WS_VISIBLE ) != 0;
	return result;
}

unsigned long AbstractWin32Component::getStyleMask()
{
	unsigned long result = 0;
	result = GetWindowLong( m_hwnd, GWL_STYLE );
	return result;
}

void AbstractWin32Component::setStyleMask( const unsigned long& styleMask )
{

}

VCF::Control* AbstractWin32Component::getControl()
{
	return m_peerControl;
}

void AbstractWin32Component::setControl( VCF::Control* component )
{	
	setPeerControl( component );
}

void AbstractWin32Component::setParent( VCF::Control* parent )
{
	VCF::ControlPeer* Peer = parent->getPeer();
	if ( NULL == dynamic_cast<Frame*>(m_peerControl) ){
		HWND wndParent = (HWND)Peer->getHandleID();
		
		if ( NULL == wndParent ){
			//throw exception !!!
		}
		
		::SetParent( m_hwnd, wndParent );
	}
}

VCF::Control* AbstractWin32Component::getParent()
{
	return NULL;
}

bool AbstractWin32Component::isFocused()
{
	HWND focusedHandle = ::GetFocus();
	bool result = (NULL != focusedHandle) && (focusedHandle == m_hwnd);
	return result;
}

void AbstractWin32Component::setFocus( const bool& focused )
{
	::SetFocus( m_hwnd );
}

bool AbstractWin32Component::isEnabled()
{	
	return (::IsWindowEnabled( m_hwnd ) != 0);
}

void AbstractWin32Component::setEnabled( const bool& enabled )
{
	::EnableWindow( m_hwnd, enabled );
}

void AbstractWin32Component::setFont( Font* font )
{	
	if ( NULL != font ){
		Win32Font* win32FontPeer = NULL;
		FontPeer* fontPeer = font->getFontPeer();
		win32FontPeer = dynamic_cast<Win32Font*>(fontPeer );
		if ( NULL != win32FontPeer ){
			HFONT fontHandle = Win32FontManager::getFontHandleFromFontPeer( win32FontPeer );
			if ( NULL != fontHandle ){
				::SendMessage( m_hwnd, WM_SETFONT, (WPARAM)fontHandle, MAKELPARAM(TRUE, 0) );
			}
		}
		else {
			//what the hell is a non Win32Font doing here !!!! throw exception !!!!!
		}
	}
}

LRESULT AbstractWin32Component::handleEventMessages( UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultWndProc )
{
	#ifdef NDEBUG
	//StringUtils::traceWithArgs( "%p(AbstractWin32Component)::handleEventMessages( %#06x, %d, %d, %x )\n",
	//	                         this, message, wParam, lParam, defaultWndProc );
	#endif
	LRESULT result = TRUE;//0;	
	
	switch ( message ) {

		case WM_ERASEBKGND :{
			result = 1;	
		}
		break;

		case WM_CTLCOLOREDIT : case WM_CTLCOLORBTN: case WM_CTLCOLORLISTBOX: {
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
				HWND hwndCtl = (HWND) lParam; // handle to static control 
				if ( hwndCtl != m_hwnd ) {
					Win32Object* win32Obj = Win32Object::getWin32ObjectFromHWND( hwndCtl );
					if ( NULL != win32Obj ){
						result = win32Obj->handleEventMessages( message, wParam, lParam );
					}
				}
			}
		}
		break;

		case WM_PAINT:{
			if ( true == isCreated() ){
				if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
					PAINTSTRUCT ps;
					HDC contextID = 0;
					contextID = ::BeginPaint( m_hwnd, &ps);	
					if ( true == m_peerControl->isDoubleBuffered() ){						
						
						VCF::GraphicsContext* ctx = m_peerControl->getContext();
						
						Rect tmp(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);					
						
						VCF::GraphicsContext memCtx( tmp.getWidth(), tmp.getHeight() );
						
						memCtx.setOrigin( -tmp.m_left, -tmp.m_top );					
						
						m_peerControl->paint( &memCtx );
						
						HDC memDC = (HDC)memCtx.getPeer()->getContextID();
						
						int err = BitBlt( ps.hdc, tmp.m_left, tmp.m_top, tmp.getWidth(), tmp.getHeight(),
							memDC, tmp.m_left, tmp.m_top, SRCCOPY );
						
						if ( err == FALSE ) {
							err = GetLastError();
							StringUtils::traceWithArgs( "error in BitBlt during drawing of double buffered Comp: error code=%d\n",
								err );	
						}					
					}
					else {
						VCF::GraphicsContext* ctx = m_peerControl->getContext();
						
						ctx->getPeer()->setContextID( (long)ps.hdc );
						
						m_peerControl->paint( ctx );
					}
					
					::EndPaint( m_hwnd, &ps);
				}
			}
		}
		break;

		case WM_SIZE: {
			result = defaultWndProcedure( message, wParam, lParam );
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
				VCF::Size sz( LOWORD(lParam), HIWORD(lParam) );
				
				VCF::ControlEvent event( m_peerControl, sz );			
				
				if ( m_peerControl ) {
					
					m_peerControl->processControlEvent( &event );
				}
			}
		}
		break;

		case WM_LBUTTONDOWN: case WM_MBUTTONDOWN: case WM_RBUTTONDOWN:{			
			if ( m_peerControl->getComponentState() == CS_NORMAL ) {
				result = defaultWndProcedure( message, wParam, lParam );
			}

			if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
				m_clickPt.m_x = Win32Utils::getXFromLParam( lParam );
				m_clickPt.m_y = Win32Utils::getYFromLParam( lParam );
				
				VCF::Point pt( m_clickPt.m_x, m_clickPt.m_y );
				
				VCF::MouseEvent event( getControl(), MOUSE_EVENT_DOWN,
					Win32Utils::translateButtonMask( wParam ),
					Win32Utils::translateKeyMask( wParam ), &pt );
				
				m_peerControl->processMouseEvent( &event );
			}
		}
		break;

		case WM_LBUTTONUP: case WM_MBUTTONUP: case WM_RBUTTONUP:{			
			if ( m_peerControl->getComponentState() == CS_NORMAL ) {
				result = defaultWndProcedure( message, wParam, lParam );
			}
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
				VCF::Point pt( Win32Utils::getXFromLParam( lParam ) , 
							   Win32Utils::getYFromLParam( lParam ) );
				VCF::Point tmpPt( Win32Utils::getXFromLParam( lParam ) , 
								  Win32Utils::getYFromLParam( lParam ) );
				
				WPARAM tmpWParam = wParam;
				switch ( message ){
					case WM_LBUTTONUP:{
						tmpWParam |= MK_LBUTTON;
					}
					break;

					case WM_MBUTTONUP:{
						tmpWParam |= MK_MBUTTON;
					}
					break;

					case WM_RBUTTONUP:{
						tmpWParam |= MK_RBUTTON;
					}
					break;
				}
				VCF::MouseEvent event( getControl(), MOUSE_EVENT_UP,
					Win32Utils::translateButtonMask( tmpWParam ),
					Win32Utils::translateKeyMask( tmpWParam ), &pt );
				if ( m_peerControl )
				m_peerControl->processMouseEvent( &event );
				
				/**
				*determine if we have to fire off a click event
				*/
				if ( m_clickPt.closeTo( tmpPt.m_x, tmpPt.m_y, 2 ) ){
					
					VCF::MouseEvent event( getControl(), MOUSE_EVENT_CLICK,
						Win32Utils::translateButtonMask( tmpWParam ),
						Win32Utils::translateKeyMask( tmpWParam ), &pt );
					if ( m_peerControl )
					m_peerControl->processMouseEvent( &event );
				}	
			}
		}
		break;
		
		case WM_SETFOCUS: {
			result = defaultWndProcedure( message, wParam, lParam );
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
				VCF::FocusEvent event( getControl() );
				if ( m_peerControl ) {
					m_peerControl->fireOnFocusGained( &event );
				}
			}
		}
		break;	

		case WM_KILLFOCUS : {
			result = defaultWndProcedure( message, wParam, lParam );
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
				VCF::FocusEvent event( getControl() );
				if ( m_peerControl ) {
					m_peerControl->fireOnFocusLost( &event );
				}
			}
		}
		break;	

		case WM_CREATE: {
			result = defaultWndProcedure( message, wParam, lParam );
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
				VCF::ComponentEvent event( getControl(), COMPONENT_EVENT_CREATED );
				if ( m_peerControl )
					m_peerControl->processComponentEvent( &event );
			}
			
		}
		break;	

		case WM_HELP : {
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
				HELPINFO* helpInfo = (HELPINFO*) lParam;
				if ( HELPINFO_WINDOW == helpInfo->iContextType ) {
					if ( helpInfo->hItemHandle == m_hwnd ) {
						m_peerControl->processWhatsThisHelpEvent();
					}
				}
				else if ( HELPINFO_MENUITEM == helpInfo->iContextType ) {
					HelpEvent event(m_peerControl);
					m_peerControl->fireOnHelpRequested( &event );
				}
			}
			
		}
		break;
		case VCF_CONTROL_CREATE: {
			VCF::ComponentEvent event( getControl(), COMPONENT_EVENT_CREATED );
			if ( m_peerControl )
			m_peerControl->processComponentEvent( &event );
		}
		break;	

		case WM_DESTROY: {	
			VCF::ComponentEvent event( getControl(), COMPONENT_EVENT_DELETED );
			m_peerControl->processComponentEvent( &event );

			result = defaultWndProcedure( message, wParam, lParam );
			
			destroyWindowHandle();			
		}
		break;

		case WM_LBUTTONDBLCLK: case WM_MBUTTONDBLCLK: case WM_RBUTTONDBLCLK:{			
			if ( m_peerControl->getComponentState() == CS_NORMAL ) {
				result = defaultWndProcedure( message, wParam, lParam );
			}
			
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
				VCF::Point pt( Win32Utils::getXFromLParam( lParam ) , 
					Win32Utils::getYFromLParam( lParam ) );
				VCF::MouseEvent event( getControl(), MOUSE_EVENT_DBLCLICK,
					Win32Utils::translateButtonMask( wParam ),
					Win32Utils::translateKeyMask( wParam ), &pt );
				if ( m_peerControl )
					m_peerControl->processMouseEvent( &event );
			}
		}
		break;

		case WM_MOVE: {
			result = defaultWndProcedure( message, wParam, lParam );
			
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
				VCF::Point pt( Win32Utils::getXFromLParam( lParam ) , 
					Win32Utils::getYFromLParam( lParam ) );
				
				VCF::ControlEvent event( m_peerControl, pt );
				if ( m_peerControl )
					m_peerControl->processControlEvent( &event );			
			}
		}
		break;
		
		case WM_MOUSEMOVE: {			
			if ( m_peerControl->getComponentState() == CS_NORMAL ) {
				result = defaultWndProcedure( message, wParam, lParam );
			}	
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) { 
				VCF::Point pt( Win32Utils::getXFromLParam( lParam ) , 
					Win32Utils::getYFromLParam( lParam ) );
				
				
				VCF::MouseEvent event( getControl(), MOUSE_EVENT_MOVE,
					Win32Utils::translateButtonMask( wParam ),
					Win32Utils::translateKeyMask( wParam ), &pt );
				
				m_peerControl->processMouseEvent( &event );
			}
		}
		break;

		case WM_CHAR: case WM_KEYDOWN: case WM_KEYUP: {			
			result = defaultWndProcedure( message, wParam, lParam );
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) { 	
				KeyboardData keyData = Win32Utils::translateKeyData( m_hwnd, lParam );
				unsigned long eventType = 0;
				switch ( message ){
					case WM_CHAR: {
						eventType = KEYBOARD_EVENT_DOWN;
					}
					break;
					
					case WM_KEYDOWN: {
						eventType = KEYBOARD_EVENT_PRESSED;//KEYBOARD_EVENT_DOWN;
					}
					break;
					
					case WM_KEYUP: {
						eventType = KEYBOARD_EVENT_UP;
					}
					break;
				}			
				
				unsigned long keyMask = Win32Utils::translateKeyMask( keyData.keyMask );
				ulong32 virtKeyCode = Win32Utils::translateVKCode( keyData.VKeyCode );
				VCF::KeyboardEvent event( m_peerControl, eventType, keyData.repeatCount, 
					keyMask, (VCF::VCFChar)keyData.character, virtKeyCode );
				
				m_peerControl->processKeyboardEvent( &event );
			}
		}
		break;		

		case WM_DRAWITEM : {
			UINT idCtl = (UINT) wParam;
			DRAWITEMSTRUCT* drawItemStruct = (DRAWITEMSTRUCT*) lParam;
			
			if ( (m_peerControl->getComponentState() != CS_DESTROYING) && (0 == idCtl) 
					&& (ODT_MENU == drawItemStruct->CtlType) ) { 
				// we have received a draw item for a menu item
				MenuItem* foundItem = Win32MenuItem::getMenuItemFromID( drawItemStruct->itemID );
				if ( NULL != foundItem ) {
					foundItem->setSelected( (drawItemStruct->itemState & ODS_SELECTED) != 0 );

					//foundItem->setChecked( (drawItemStruct->itemState & ODS_CHECKED) != 0 );

					//foundItem->setEnabled( (drawItemStruct->itemState & ODS_CHECKED) != 0 );

					if ( true == foundItem->canPaint() ) {
						
						Win32MenuItem* itemPeer = (Win32MenuItem*)foundItem->getPeer();

						itemPeer->drawDefaultMenuItem( idCtl, *drawItemStruct );

						GraphicsContext gc( (long)drawItemStruct->hDC );					
						foundItem->paint( &gc, &Rect(drawItemStruct->rcItem.left, drawItemStruct->rcItem.top,
														drawItemStruct->rcItem.right, drawItemStruct->rcItem.bottom) );
						gc.getPeer()->setContextID(0);
						
						result = TRUE;
					}
				}
			}
			else {
				result = defaultWndProcedure( message, wParam, lParam );
			}			
		}
		break;
		
		case WM_MEASUREITEM : {
			
			UINT idCtl = (UINT) wParam;
			MEASUREITEMSTRUCT* measureInfo = (MEASUREITEMSTRUCT*) lParam;
			if ( (m_peerControl->getComponentState() != CS_DESTROYING) && (0 == idCtl) 
					&& (ODT_MENU == measureInfo->CtlType) ) { 
				
				// we have received a draw item for a menu item
				MenuItem* foundItem = Win32MenuItem::getMenuItemFromID( measureInfo->itemID );
				if ( NULL != foundItem ) {

					if ( true == foundItem->canPaint() ) {
						result = TRUE;
						Rect* bounds = foundItem->getBounds();
						bool needsBounds = false;
						if ( NULL != bounds ) {
							needsBounds = ((bounds->getHeight() <= 0.0) && (bounds->getWidth() <= 0.0));
						}
						else {
							needsBounds = true;
						}
						if ( true == needsBounds ) {
							Win32MenuItem* peer = (Win32MenuItem*)foundItem->getPeer();
							peer->fillInMeasureItemInfo( *measureInfo );							
						}
						else {
							measureInfo->itemHeight = bounds->getHeight();
							measureInfo->itemWidth = bounds->getWidth();
						}
					}
				}				
			}
			else {			
				result = defaultWndProcedure( message, wParam, lParam );
			}			
		}
		break;
			
		case WM_MENUSELECT : {
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) { 
				UINT uItem = (UINT) LOWORD(wParam);
				UINT fuFlags = (UINT) HIWORD(wParam); 
				HMENU hmenuPopup = (HMENU) lParam;
				if ( (MF_MOUSESELECT & fuFlags) != 0 ) {								
					MenuItem* foundItem = Win32MenuItem::getMenuItemFromID( uItem );
					if ( NULL != foundItem ) {
						//StringUtils::trace( "MF_MOUSESELECT over item \"" + foundItem->getCaption() + "\"\n" );
						foundItem->setSelected( true );
						result = 0;
					}				
				}
				else {
					result = defaultWndProcedure( message, wParam, lParam );
				}
			}
		}
		break;

		case WM_INITMENUPOPUP : {	
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) { 
				HMENU hmenuPopup = (HMENU) wParam;//thisis the menu handle of the menu popuping up or dropping down
				if ( GetMenuItemCount( hmenuPopup ) > 0 ) {
					//UINT Pos = (UINT) LOWORD(lParam);
					//get the first item
					uint32 id = GetMenuItemID( hmenuPopup, 0 );
					if ( id != 0xFFFFFFFF ) {
						MenuItem* foundItem = Win32MenuItem::getMenuItemFromID( id );
						if ( NULL != foundItem ) {
							MenuItem* parent = foundItem->getParent();
							parent->update();
							Enumerator<MenuItem*>* menuItems = parent->getChildren();
							while ( menuItems->hasMoreElements() ) {
								MenuItem* child = menuItems->nextElement();
								child->update();
							}							
							result = 0;						
						}
					}
				}
				else {
					result = defaultWndProcedure( message, wParam, lParam );
				}
			}
		}
		break;

		case WM_NOTIFY : {
			result = defaultWndProcedure( message, wParam, lParam );
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) { 	
				NMHDR* notificationHdr = (LPNMHDR)lParam;
				Win32Object* win32Obj = Win32Object::getWin32ObjectFromHWND( notificationHdr->hwndFrom );
				if ( NULL != win32Obj ){
					win32Obj->handleEventMessages( notificationHdr->code, wParam, lParam );
				}
			}
		}
		break;

		case WM_VSCROLL : {
			result = 0;
			if ( NULL != m_peerControl ) {
				if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
					HWND hwndScrollBar = (HWND) lParam; 
					
					Scrollable* scrollable = m_peerControl->getScrollable();
					//if it's null then check for children
					if ( NULL == scrollable ) {
						Control* scrollControl = Win32ScrollPeer::getScrollableControlFromHWnd( hwndScrollBar );
						if ( NULL != scrollControl ) {
							scrollable = scrollControl->getScrollable();
						}
					}
					if ( NULL != scrollable ) {
						int nScrollCode = (int) LOWORD(wParam); // scroll bar value 
						short int nPos = (short int) HIWORD(wParam);  // scroll box position 
						// handle to scroll bar 
						//use this for 32bit scroll positions
						SCROLLINFO si = {0};
						si.cbSize = sizeof(si);
						si.fMask = SIF_POS | SIF_TRACKPOS;
						GetScrollInfo( hwndScrollBar, SB_CTL, &si );
						switch ( nScrollCode ) {
						case SB_THUMBPOSITION : case SB_THUMBTRACK : {
							
							scrollable->setVerticalPosition( si.nTrackPos );		
												}
							break;
						}					
					}
				}
			}
		}
		break;

		case WM_HSCROLL : {
			result = 0;
			if ( NULL != m_peerControl ) {
				if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
					
					HWND hwndScrollBar = (HWND) lParam; 
					
					Scrollable* scrollable = m_peerControl->getScrollable();
					//if it's null then check for children
					if ( NULL == scrollable ) {
						Control* scrollControl = Win32ScrollPeer::getScrollableControlFromHWnd( hwndScrollBar );
						if ( NULL != scrollControl ) {
							scrollable = scrollControl->getScrollable();
						}
					}
					if ( NULL != scrollable ) {
						int nScrollCode = (int) LOWORD(wParam); // scroll bar value 
						short int nPos = (short int) HIWORD(wParam);  // scroll box position 
						// handle to scroll bar 
						//use this for 32bit scroll positions
						SCROLLINFO si = {0};
						si.cbSize = sizeof(si);
						si.fMask = SIF_POS | SIF_TRACKPOS;
						GetScrollInfo( hwndScrollBar, SB_CTL, &si );
						switch ( nScrollCode ) {
						case SB_THUMBPOSITION : case SB_THUMBTRACK : {
							
							scrollable->setHorizontalPosition( si.nTrackPos );		
												}
							break;
						}					
					}
				}
			}
		}
		break;

		case WM_COMMAND:{
			if ( m_peerControl->getComponentState() != CS_DESTROYING ) {
				UINT notifyCode = HIWORD(wParam); 
				HWND hwndCtl = (HWND) lParam;
				Win32Object* win32Obj = NULL;
				if ( NULL != hwndCtl ){
					win32Obj = Win32Object::getWin32ObjectFromHWND( hwndCtl );
				}
				if ( NULL != win32Obj ){
					if ( NULL != win32Obj ){
						win32Obj->handleEventMessages( notifyCode, wParam, lParam );						
					}
					else {
						StringUtils::trace( "win32Obj == NULL!\n" );
						UINT wID = LOWORD(wParam);
						char tmp2[256];
						memset( tmp2, 0, 256 );
						sprintf( tmp2, "command ID: %d\n", wID );
						
						StringUtils::trace( String(tmp2) );
						
						defaultWndProcedure( message, wParam, lParam );
					}
				}
				else {
					switch ( notifyCode ) {
						case 0 : { //menu item command
							UINT wID = LOWORD(wParam);				
							
							MenuItem* item = Win32MenuItem::getMenuItemFromID( wID );
							if ( NULL != item ){						
								item->click();
							}
						}
						break;

						case 1 : { //accellerator
							
						}
						break;
					}
				}
			}
		}
		break;

		default:{
			result = defaultWndProcedure( message, wParam, lParam );
		}
		break;
	}
	/*
	if ( NULL != defaultWndProc ){
		m_defaultWndProc = oldProc;
	}
	*/
	return result;
}
void AbstractWin32Component::repaint( Rect* repaintRect )
{
	if ( NULL == repaintRect ){
		::InvalidateRect( m_hwnd, NULL, TRUE );
	}
	else {
		RECT rect = {0};
		rect.left = repaintRect->m_left;
		rect.top = repaintRect->m_top;
		rect.right = repaintRect->m_right;
		rect.bottom = repaintRect->m_bottom;
		::InvalidateRect( m_hwnd, &rect, TRUE );
	}
}

void AbstractWin32Component::keepMouseEvents()
{
	::SetCapture( m_hwnd );
}

void AbstractWin32Component::releaseMouseEvents()
{
	::ReleaseCapture();
}


String AbstractWin32Component::toString()
{
	String result = Win32Object::toString();
	char tmp[256];
	memset(tmp,0,256);
	sprintf(tmp, "\n\tm_peerControl=%p\n\tHWND=%p\n", m_peerControl, m_hwnd );
	result += tmp;
	return result;
}

void AbstractWin32Component::setCursor( Cursor* cursor )
{
	if ( NULL != cursor ) {
		::SetCursor( (HCURSOR)cursor->getPeer()->getCursorHandleID() );
	}
}
