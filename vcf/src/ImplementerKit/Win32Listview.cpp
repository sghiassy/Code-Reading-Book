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

// Win32Listview.cpp

#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"
#include "ListViewControl.h"

using namespace VCFWin32;
using namespace VCF;


Win32Listview::Win32Listview( ListViewControl* listviewControl ):
	AbstractWin32Component( listviewControl ),
	m_listviewControl( listviewControl )
{
	
	m_itemAddedHandler = 
		new ItemEventHandler<Win32Listview>( this, Win32Listview::onItemAdded, "Win32Listview::onItemAdded" );

	m_itemDeletedHandler = 
		new ItemEventHandler<Win32Listview>( this, Win32Listview::onItemDeleted, "Win32Listview::onItemDeleted" );

	m_itemChangedHandler = 
		new ItemEventHandler<Win32Listview>( this, Win32Listview::onItemChanged, "Win32Listview::onItemChanged" );

	m_itemSelectedHandler = 
		new ItemEventHandler<Win32Listview>( this, Win32Listview::onItemSelected, "Win32Listview::onItemSelected" );

	m_itemPaintedHandler = 
		new ItemEventHandler<Win32Listview>( this, Win32Listview::onItemPaint, "Win32Listview::onItemPaint" );

	createParams();
	m_backColor.copy( listviewControl->getColor() );
	/*
	if ( true != isRegistered() ){
		registerWin32Class( getClassName(), m_wndProc  );
	}
	*/

	Win32ToolKit* toolkit = (Win32ToolKit*)UIToolkit::getDefaultUIToolkit();
	HWND parent = toolkit->getDummyParent();	

	String className = getClassName();

	m_hwnd = ::CreateWindowEx( m_exStyleMask, 
		                             WC_LISTVIEW,//"SysListView32",//className.c_str(), 
									 m_windowCaption.c_str(),	
									 m_styleMask, 
		                             m_bounds.m_left, 
									 m_bounds.m_top, 
									 m_bounds.getWidth(), 
									 m_bounds.getHeight(), 
									 parent, 
									 NULL, 
									 ::GetModuleHandle(NULL), 
									 NULL );


	if ( NULL != m_hwnd ){	
		Win32Object::registerWin32Object( this );
		m_oldListviewWndProc = (WNDPROC)::SetWindowLong( m_hwnd, GWL_WNDPROC, (LONG)m_wndProc ); 
		m_defaultWndProc = NULL;//m_oldListviewWndProc;
		COLORREF backColor = RGB(m_backColor.getRed() * 255.0,
									m_backColor.getGreen() * 255.0,
									m_backColor.getBlue() * 255.0 );
		ListView_SetBkColor( m_hwnd, backColor );		
	}
	else {
		//throw exception
	}

	setCreated( true );
}

Win32Listview::~Win32Listview()
{

}

void Win32Listview::createParams()
{
	m_exStyleMask = WS_EX_CLIENTEDGE;
	m_styleMask = BORDERED_VIEW | LVS_SINGLESEL | LVS_ALIGNLEFT | LVS_AUTOARRANGE | LVS_ICON | LVS_SHOWSELALWAYS | LVS_SORTASCENDING | LVS_EDITLABELS;
}

LRESULT Win32Listview::handleEventMessages( UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultWndProc )
{
	LRESULT result = 0;

	switch ( message ) {		
		case WM_ERASEBKGND :{
			Color* color = m_listviewControl->getColor();
			if ( (m_backColor.getRed() != color->getRed()) || (m_backColor.getGreen() != color->getGreen()) || (m_backColor.getBlue() != color->getBlue()) ) {
				COLORREF backColor = RGB(color->getRed() * 255.0,
									color->getGreen() * 255.0,
									color->getBlue() * 255.0 );

				ListView_SetBkColor( m_hwnd, backColor );
				
				m_backColor.copy( color );
			}
			result = CallWindowProc( m_oldListviewWndProc, m_hwnd, message, wParam, lParam );
		}
		break;

		case WM_PAINT:{
			result = CallWindowProc( m_oldListviewWndProc, m_hwnd, message, wParam, lParam );
		}
		break;

		case WM_CREATE:{				
			AbstractWin32Component::handleEventMessages( message, wParam, lParam );
		}
		break;
		
		case LVN_BEGINDRAG:{				
			NMLISTVIEW* lvNotificationHdr = (LPNMLISTVIEW)lParam;
			
		}
		break;

		case LVN_BEGINLABELEDIT:{
			
		}
		break;

		case LVN_BEGINRDRAG:{
			
		}
		break;

		case LVN_COLUMNCLICK:{
			
		}
		break;

		case LVN_DELETEALLITEMS:{
			
		}
		break;

		case LVN_DELETEITEM:{
			
		}
		break;

		case LVN_ENDLABELEDIT:{
			NMLVDISPINFO* lvDispInfo = (NMLVDISPINFO*)lParam;
			result = (NULL != lvDispInfo->item.pszText) ? TRUE : FALSE;
			if ( NULL != lvDispInfo->item.pszText ){
				String newCaption = lvDispInfo->item.pszText;			
				ListModel* model = m_listviewControl->getListModel();
				
				ListItem* item = model->getItemFromIndex( lvDispInfo->item.iItem );
				if ( NULL != item ){
					if ( lvDispInfo->item.iItem == item->getIndex() ) {	
						item->setCaption( newCaption );
						item->setSelected( true );
					}
				}
			}
		}
		break;

		case LVN_GETDISPINFO:{
			
		}
		break;

		case LVN_GETINFOTIP:{
			
		}
		break;

		case LVN_HOTTRACK:{
			
		}
		break;

		case LVN_INSERTITEM:{
			
		}
		break;

		case LVN_ITEMACTIVATE:{
			
		}
		break;

		case LVN_ITEMCHANGED:{
			NMLISTVIEW* lvNotificationHdr = (LPNMLISTVIEW)lParam;
			
			if ( -1 != lvNotificationHdr->iItem ){
				ListModel* model = m_listviewControl->getListModel();
			
				ListItem* item = model->getItemFromIndex( lvNotificationHdr->iItem );
				if ( NULL != item ){
					if ( (lvNotificationHdr->uChanged & LVIF_STATE) != 0 ) {
						if ( (lvNotificationHdr->uNewState & LVIS_SELECTED) != 0 ){
							if ( lvNotificationHdr->iItem == item->getIndex() ){
								POINT tmpPt = {0};
								GetCursorPos( &tmpPt );
								::ScreenToClient( m_hwnd, &tmpPt );
								ItemEvent event( m_listviewControl, ITEM_EVENT_SELECTED );
								Point pt( tmpPt.x, tmpPt.y );
								event.setPoint( &pt );
								
								item->setSelected( true );								
								
								m_listviewControl->fireOnItemSelectionChanged( &event );

							}
						}
					}
				}
			}

		}
		break;

		case LVN_ITEMCHANGING:{
			
		}
		break;

		case LVN_KEYDOWN:{
			
		}
		break;

		case LVN_MARQUEEBEGIN:{
			
		}
		break;

		case LVN_ODCACHEHINT:{
				
		}
		break;

		case LVN_ODFINDITEM:{
				
		}
		break;

		case LVN_ODSTATECHANGED:{
				
		}
		break;

		case LVN_SETDISPINFO:{
				
		}
		break;

		case NM_CUSTOMDRAW:{

			NMLVCUSTOMDRAW* listViewCustomDraw = (NMLVCUSTOMDRAW*)lParam;
			ListModel* model = m_listviewControl->getListModel();
			result = CDRF_DODEFAULT;
			if ( NULL != model ) {
				/**
				*CDDS_PREPAINT is at the beginning of the paint cycle. You 
				*implement custom draw by returning the proper value. In this 
				*case, we're requesting item-specific notifications.
				*/
				switch ( listViewCustomDraw->nmcd.dwDrawStage ) {
					case CDDS_PREPAINT : {					
						// Request prepaint notifications for each item.
						result = CDRF_NOTIFYITEMDRAW;					
					}
					break;

					case CDDS_ITEMPREPAINT : {					
						// Request prepaint notifications for each item.
						result = CDRF_NOTIFYPOSTPAINT;					
					}
					break;

					case CDDS_ITEMPOSTPAINT : {					
						// Request prepaint notifications for each item.						
						ListItem* item = model->getItemFromIndex( (ulong32 )listViewCustomDraw->nmcd.dwItemSpec );
						result = CDRF_DODEFAULT; 
						if ( NULL != item ) {
							if ( true == item->canPaint() ) {
								RECT tmp = {0};
								ListView_GetItemRect( m_hwnd, listViewCustomDraw->nmcd.dwItemSpec,
														&tmp, LVIR_BOUNDS );

								Rect itemRect( tmp.left, tmp.top, tmp.right, tmp.bottom );
						
								item->paint( m_peerControl->getContext(), &itemRect );		
							}
						}						
					}
					break;

					default : {
						result = CDRF_DODEFAULT; 
					}
					break;
				}				
			}
		}
		break;

		default: {
			result = AbstractWin32Component::handleEventMessages( message, wParam, lParam );
			if ( result != TRUE ) {
				result = CallWindowProc( m_oldListviewWndProc, m_hwnd, message, wParam, lParam );
			}
			
		}
		break;
	}	
	return result;
}

void Win32Listview::addItem( ListItem * item )
{
	//LVM_INSERTITEM
	if ( NULL != item ){
		LVITEM lvItem = {0};
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.pszText = const_cast<char*>( item->getCaption().c_str() );
		lvItem.cchTextMax = item->getCaption().size();
		lvItem.lParam = (LPARAM)item;
		int index = ListView_InsertItem( m_hwnd, &lvItem );
		if ( index != -1 ) {
			item->setIndex( index );
		}
		else{
			//throw exception
		}
		item->addItemAddedHandler( m_itemAddedHandler );
		item->addItemDeletedHandler( m_itemDeletedHandler );
		item->addItemChangedHandler( m_itemChangedHandler );
		item->addItemSelectedHandler( m_itemSelectedHandler );
		item->addItemPaintHandler( m_itemPaintedHandler );		
	}
}

void Win32Listview::insertItem( const unsigned long& index, ListItem * item )
{
	if ( NULL != item ){
		LVITEM lvItem = {0};
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem = index;
		lvItem.pszText = const_cast<char*>( item->getCaption().c_str() );
		lvItem.cchTextMax = item->getCaption().size();
		lvItem.lParam = (LPARAM)item;
		int index = ListView_InsertItem( m_hwnd, &lvItem );
		if ( index != -1 ) {
			item->setIndex( index );
		}
		else{
			//throw exception
		}

		item->addItemAddedHandler( m_itemAddedHandler );
		item->addItemDeletedHandler( m_itemDeletedHandler );
		item->addItemChangedHandler( m_itemChangedHandler );
		item->addItemSelectedHandler( m_itemSelectedHandler );
		item->addItemPaintHandler( m_itemPaintedHandler );	
	}
}

void Win32Listview::clear()
{
	ListView_DeleteAllItems( m_hwnd );
}
		
void Win32Listview::deleteItem( ListItem* item )
{
	if ( NULL != item ){
		ListView_DeleteItem( m_hwnd, item->getIndex() );
	}
}

void Win32Listview::setFocusedItem(ListItem * item)
{
	if ( NULL != item ){
		ListView_SetItemState( m_hwnd, item->getIndex(),
			                   LVIS_FOCUSED, LVIS_FOCUSED );
	}
}

void Win32Listview::selectItem(ListItem * item)
{
	if ( NULL != item ){
		ListView_SetItemState( m_hwnd, item->getIndex(),
			                   LVIS_SELECTED, LVIS_SELECTED );
	}
}

Rect* Win32Listview::getItemRect( ListItem* item )
{
	RECT rect = {0};
	if ( NULL != item ){
		ListView_GetItemRect( m_hwnd, item->getIndex(), 
		                      &rect, LVIR_BOUNDS );
		m_itemRect.setRect( rect.left, rect.top, rect.right, rect.bottom );
	}
	else {
		return NULL;
	}
	return &m_itemRect;
}

bool Win32Listview::isItemSelected(ListItem* item)
{
	bool result = false;
	if ( NULL != item ){
		result = ( LVIS_SELECTED == 
			           ListView_GetItemState( m_hwnd, item->getIndex(), LVIS_SELECTED ) );
			                                              
	}
	return result;
}

ListItem* Win32Listview::isPtOverItem(Point* point)
{
	return NULL;
}

ListItem* Win32Listview::getFocusedItem()
{
	return NULL;
}

ListItem* Win32Listview::getSelectedItem()
{
	ListItem* result = NULL;
	int index = ListView_GetSelectionMark( m_hwnd );
	if ( index > -1 ){
		ListModel* model = m_listviewControl->getListModel();
		if ( NULL != model ){
			result = model->getItemFromIndex( index );
		}
	}
	return result;
}

Enumerator<ListItem*>* Win32Listview::getSelectedItems()
{
	int index = ListView_GetSelectionMark( m_hwnd );
	if ( index > -1 ){
		ListModel* model = m_listviewControl->getListModel();
		if ( NULL != model ){
			m_selectedItems.clear();
			ListItem* item = NULL;
			int i = 0;
			int count = ListView_GetSelectedCount( m_hwnd );
			while ( i < count ){
				if ( LVIS_SELECTED == ListView_GetItemState( m_hwnd, index, LVIS_SELECTED ) ){
					item = 	model->getItemFromIndex( index );
					m_selectedItems.push_back( item );
				}
				index++;
				i++;
			}
		}
	}
	else {
		return NULL;
	}

	m_enumContainer.initContainer( m_selectedItems );
	return m_enumContainer.getEnumerator();
}

void Win32Listview::rangeSelect( Rect* selectionRect )
{

}

void Win32Listview::addHeaderColumn( const String& columnName )
{
	LVCOLUMN column = {0};
	column.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
	column.cx = 200;
	column.fmt = LVCFMT_LEFT;
	column.pszText = const_cast<char*>( columnName.c_str() );
	column.cchTextMax = columnName.size();
	ListView_InsertColumn( m_hwnd, 0, &column );
}

void Win32Listview::insertHeaderColumn( const unsigned long& index, const String& columnName )
{

}

void Win32Listview::deleteHeaderColumn( const unsigned long& index )
{

}

IconStyleType Win32Listview::getIconStyle()
{
	DWORD style = ::GetWindowLong( m_hwnd, GWL_STYLE );
	return translateStyleToIconStyle( style );
}

void Win32Listview::setIconStyle( const IconStyleType& iconStyle )
{
	DWORD style = ::GetWindowLong( m_hwnd, GWL_STYLE );
	
	style &= ~LVS_ICON;
	style &= ~LVS_SMALLICON;
	style &= ~LVS_LIST;
	style &= ~LVS_REPORT;
	
	style |= translateIconStyleToStyleBit( iconStyle );

	::SetWindowLong( m_hwnd, GWL_STYLE, style );
	::SetWindowPos( m_hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED ); 
}

bool Win32Listview::getAllowsMultiSelect()
{
	DWORD style = ::GetWindowLong( m_hwnd, GWL_STYLE );
	return ((style & LVS_SINGLESEL) == 0);
}

void Win32Listview::setAllowsMultiSelect( const bool& allowsMultiSelect )
{
	DWORD style = ::GetWindowLong( m_hwnd, GWL_STYLE );
	if ( true == allowsMultiSelect ){
		style &= ~LVS_SINGLESEL;
	}
	else {		
		style |= LVS_SINGLESEL;
	}
	::SetWindowLong( m_hwnd, GWL_STYLE, style );
}

IconAlignType Win32Listview::getIconAlignment()
{
	DWORD style = ::GetWindowLong( m_hwnd, GWL_STYLE );
	return translateStyleToIconAlignment( style );	
}

void Win32Listview::setIconAlignment( const IconAlignType& iconAlignType )
{
	DWORD style = ::GetWindowLong( m_hwnd, GWL_STYLE );
	style |= translateIconAlignmentToStyleBit( iconAlignType );
	::SetWindowLong( m_hwnd, GWL_STYLE, style );
}

bool Win32Listview::getAllowLabelEditing()
{
	DWORD style = ::GetWindowLong( m_hwnd, GWL_STYLE );
	return ((style & LVS_EDITLABELS) == 0);
}

void Win32Listview::setAllowLabelEditing( const bool& allowLabelEditing )
{
	DWORD style = ::GetWindowLong( m_hwnd, GWL_STYLE );
	if ( true == allowLabelEditing ){
		style |= LVS_EDITLABELS;
	}
	else {		
		style &= ~LVS_EDITLABELS;		
	}
	::SetWindowLong( m_hwnd, GWL_STYLE, style );
}

IconStyleType Win32Listview::translateStyleToIconStyle( const DWORD& wsStyle )
{
	IconStyleType result;
	if ( (wsStyle & LVS_ICON) != 0 ){
		result = IS_LARGE_ICON;	
	}
	else if ( (wsStyle & LVS_LIST) != 0 ){
		result = IS_LIST;	
	}
	else if ( (wsStyle & LVS_SMALLICON) != 0 ){
		result = IS_SMALL_ICON;	
	}
	else if ( (wsStyle & LVS_REPORT) != 0 ){
		result = IS_DETAILS;	
	}
	return result;
}

DWORD Win32Listview::translateIconStyleToStyleBit( const IconStyleType& iconStyle )
{
	DWORD result = 0;
	switch ( iconStyle ){
		case IS_LARGE_ICON:{
			result = LVS_ICON;
		}
		break;

		case IS_SMALL_ICON:{
			result = LVS_SMALLICON;
		}
		break;

		case IS_LIST:{
			result = LVS_LIST;
		}
		break;

		case IS_DETAILS:{
			result = LVS_REPORT;
		}
		break;
	}
	return result;
}

IconAlignType Win32Listview::translateStyleToIconAlignment( const DWORD& wsStyle )
{
	IconAlignType result;
	if ( (wsStyle & LVS_ALIGNTOP) != 0 ){
		result = IA_TOP;	
	}
	else if ( (wsStyle & LVS_ALIGNLEFT) != 0 ){
		result = IA_LEFT;	
	}
	else if ( (wsStyle & LVS_AUTOARRANGE) != 0 ){
		result = IA_AUTO_ARRANGE;	
	}
	else {
		result = IA_NONE;	
	}
	return result;
}

DWORD Win32Listview::translateIconAlignmentToStyleBit( const IconAlignType& iconAlignment )
{
	DWORD result = 0;
	switch ( iconAlignment ){
		case IA_TOP:{
			result = LVS_ALIGNTOP;
		}
		break;

		case IA_LEFT:{
			result = LVS_ALIGNLEFT;
		}
		break;

		case IA_AUTO_ARRANGE:{
			result = LVS_AUTOARRANGE;
		}
		break;		
		case IA_NONE:{
			result = 0;
		}
		break;
	}
	return result;
}

void Win32Listview::onItemPaint( ItemEvent* event )
{

}

void Win32Listview::onItemChanged( ItemEvent* event )
{	
	switch ( event->getType() ){
			case ITEM_EVENT_TEXT_CHANGED:{
				Object* source = event->getSource();
				if ( NULL != source ){
					ListItem* item = dynamic_cast<ListItem*>(source);
					if ( NULL != item ){						
						LVITEM lvItem = {0};
						lvItem.mask = LVIF_TEXT;						
						String caption = item->getCaption();
						lvItem.cchTextMax = caption.size();
						lvItem.iItem = item->getIndex();
						char* tmp = new char[lvItem.cchTextMax];
						memset( tmp, 0, lvItem.cchTextMax );
						caption.copy( tmp, lvItem.cchTextMax );
						lvItem.pszText = tmp;							
						ListView_SetItem( m_hwnd, &lvItem );
						delete [] tmp;
					}
				}				
			}
			break;
		}
}

void Win32Listview::onItemSelected( ItemEvent* event )
{

}

void Win32Listview::onItemAdded( ItemEvent* event )
{

}
	
void Win32Listview::onItemDeleted( ItemEvent* event )
{

}