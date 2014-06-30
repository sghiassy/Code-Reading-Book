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

// Win32Tree.cpp

#include "ApplicationKit.h"
#include "TreeControl.h"
#include "AppKitPeerConfig.h"


using namespace VCFWin32;
using namespace VCF;

Win32Tree::Win32Tree( TreeControl* tree ):
	AbstractWin32Component( tree ),
	m_treeControl( tree )
{	
	this->init();

	this->createParams();

	m_backColor.copy( m_treeControl->getColor() );

	Win32ToolKit* toolkit = (Win32ToolKit*)UIToolkit::getDefaultUIToolkit();
	HWND parent = toolkit->getDummyParent();	

	String className = this->getClassName();

	this->m_hwnd = ::CreateWindowEx( this->m_exStyleMask, 
		                             WC_TREEVIEW,
									 this->m_windowCaption.c_str(),	
									 this->m_styleMask, 
		                             this->m_bounds.m_left, 
									 this->m_bounds.m_top, 
									 this->m_bounds.getWidth(), 
									 this->m_bounds.getHeight(), 
									 parent, 
									 NULL, 
									 ::GetModuleHandle(NULL), 
									 NULL );


	if ( NULL != this->m_hwnd ){	
		Win32Object::registerWin32Object( this );
		m_oldTreeWndProc = (WNDPROC)::SetWindowLong( this->m_hwnd, GWL_WNDPROC, (LONG)m_wndProc ); 
		this->m_defaultWndProc = NULL;//m_oldTreeWndProc;
		
		
	}
	else {
		//throw exception
	}

	this->setCreated( true );
}

Win32Tree::~Win32Tree()
{
	if ( NULL != m_imageListCtrl ) {
		BOOL err = ImageList_Destroy( m_imageListCtrl );
	}
}

void Win32Tree::init()
{	
	m_oldTreeWndProc = NULL;
	m_imageListCtrl = NULL;

	m_itemAddedHandler = 
		new ItemEventHandler<Win32Tree>( this, Win32Tree::onItemAdded, "Win32Tree::onItemAdded" );

	m_itemDeletedHandler = 
		new ItemEventHandler<Win32Tree>( this, Win32Tree::onItemDeleted, "Win32Tree::onItemDeleted" );

	m_itemChangedHandler = 
		new ItemEventHandler<Win32Tree>( this, Win32Tree::onItemChanged, "Win32Tree::onItemChanged" );

	m_itemSelectedHandler = 
		new ItemEventHandler<Win32Tree>( this, Win32Tree::onItemSelected, "Win32Tree::onItemSelected" );

	m_itemPaintedHandler = 
		new ItemEventHandler<Win32Tree>( this, Win32Tree::onItemPaint, "Win32Tree::onItemPaint" );

}

void Win32Tree::createParams()
{
	this->m_exStyleMask = WS_EX_CLIENTEDGE;
	this->m_styleMask = BORDERED_VIEW | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_NOTOOLTIPS | TVS_SHOWSELALWAYS;
}

TreeModel* Win32Tree::getTreeModel()
{
	return NULL;
}

void Win32Tree::setTreeModel( TreeModel* model )
{

}

double Win32Tree::getItemIndent()
{
	return TreeView_GetIndent( m_hwnd );
}

void Win32Tree::setItemIndent( const double& indent )
{
	BOOL err = TreeView_SetIndent( this->m_hwnd, (int)indent );
}

ImageList* Win32Tree::getImageList()
{
	return NULL;
}

void Win32Tree::setImageList( ImageList* imageList )
{

	if ( NULL != m_imageListCtrl ) {
		BOOL err = ImageList_Destroy( m_imageListCtrl );
	}
	m_imageListCtrl = NULL;
	TreeView_SetImageList( this->m_hwnd, m_imageListCtrl, TVSIL_NORMAL );
	
	if ( imageList != NULL ) {
		m_imageListCtrl = ImageList_Create( imageList->getImageWidth(), imageList->getImageHeight(), 
										ILC_COLOR24|ILC_MASK, imageList->getImageCount(), 4 );	

		Image* masterImage = imageList->getMasterImage();
		Win32Image* win32Img = (Win32Image*)masterImage;
		HBITMAP hbmImage = win32Img->getBitmap();
		int err = ImageList_AddMasked( m_imageListCtrl, hbmImage, RGB(0,255,0) );
		if ( err < 0 ) {
			//error condition !
		}
		
		TreeView_SetImageList( this->m_hwnd, m_imageListCtrl, TVSIL_NORMAL );

		
		EventHandler* imgListHandler = getEventHandler( "ImageListHandler" );
		if ( NULL == imgListHandler ) {
			imgListHandler = 
				new ImageListEventHandler<Win32Tree>(this, Win32Tree::onImageListImageChanged, "ImageListHandler" );	
		
		}	
		imageList->addSizeChangedHandler( imgListHandler );
		imageList->addImageAddedHandler( imgListHandler );
		imageList->addImageDeletedHandler( imgListHandler );
	}
}
bool first = true;

LRESULT Win32Tree::handleEventMessages( UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultWndProc )
{
	LRESULT result = 0;
	
	switch ( message ) {		
		case WM_PAINT:{
			result = CallWindowProc( m_oldTreeWndProc, m_hwnd, message, wParam, lParam );
		}
		break;

		case WM_ERASEBKGND :{
			Color* color = m_treeControl->getColor();
			if ( (m_backColor.getRed() != color->getRed()) || (m_backColor.getGreen() != color->getGreen()) || (m_backColor.getBlue() != color->getBlue()) ) {
				COLORREF backColor = RGB(color->getRed() * 255.0,
									color->getGreen() * 255.0,
									color->getBlue() * 255.0 );

				TreeView_SetBkColor( m_hwnd, backColor );
				
				m_backColor.copy( color );
			}
			result = CallWindowProc( m_oldTreeWndProc, m_hwnd, message, wParam, lParam );
		}
		break;

		case WM_CREATE:{				
			AbstractWin32Component::handleEventMessages( message, wParam, lParam );
		}
		break;

		case TVN_BEGINDRAG:{
			
		}
		break;

		case TVN_BEGINLABELEDIT:{
			
		}
		break;

		case TVN_BEGINRDRAG:{
			
		}
		break;

		case TVN_DELETEITEM:{
			
		}
		break;


		case TVN_ENDLABELEDIT:{
			
		}
		break;

		case TVN_GETDISPINFO:{
			
		}
		break;

		case TVN_GETINFOTIP:{
			
		}
		break;

		case TVN_ITEMEXPANDED:{
			NMTREEVIEW* treeview = (NMTREEVIEW*)lParam;
			TreeItem* item = (TreeItem*)treeview->itemNew.lParam;
			if ( NULL != item ) {
				if ( treeview->action & TVE_EXPAND ) {
					item->expand( true );
				}
				else if ( treeview->action & TVE_COLLAPSE ) {
					item->expand( false );
				}
			}
		}
		break;

		case TVN_ITEMEXPANDING:{
			
		}
		break;

		case TVN_KEYDOWN:{
			
		}
		break;

		case TVN_SELCHANGED:{
			NMTREEVIEW* treeview = (NMTREEVIEW*)lParam;
			TreeItem* item = (TreeItem*)treeview->itemNew.lParam;
			if ( NULL != item ) {
				item->setSelected( true );
			}
			item = (TreeItem*)treeview->itemOld.lParam;
			if ( NULL != item ) {
				item->setSelected( false );
			}
		}
		break;

		case TVN_SELCHANGING:{
			
		}
		break;

		case TVN_SETDISPINFO:{
			
		}
		break;

		case TVN_SINGLEEXPAND:{
			
		}
		break;
		
		case NM_CLICK :{
			POINT pt = {0};
			GetCursorPos( &pt );
			ScreenToClient( m_hwnd, &pt );
			Point tmpPt( pt.x, pt.y );
			VCF::MouseEvent event( getControl(), MOUSE_EVENT_CLICK,
						MOUSE_LEFT_BUTTON, KEY_UNDEFINED, &tmpPt );
			if ( m_peerControl ) {
					m_peerControl->processMouseEvent( &event );	
			}			
		}
		break;

		case NM_CUSTOMDRAW:{
			NMTVCUSTOMDRAW* treeViewDraw = (NMTVCUSTOMDRAW*)lParam;
			if ( NULL != treeViewDraw )	{
				switch ( treeViewDraw->nmcd.dwDrawStage ) {
					case CDDS_PREPAINT : {
						result = CDRF_NOTIFYITEMDRAW;
					}
					break;

					case CDDS_ITEMPREPAINT : {
						result = CDRF_NOTIFYPOSTPAINT;
					}
					break;

					case CDDS_ITEMPOSTPAINT : {
						result = CDRF_DODEFAULT;
						if ( NULL != treeViewDraw->nmcd.lItemlParam ) {
							TreeItem* item = (TreeItem*)treeViewDraw->nmcd.lItemlParam;
							std::map<TreeItem*,HTREEITEM>::iterator found =
								m_treeItems.find( item );
							if ( found != m_treeItems.end() ){								
								Rect itemRect;
								itemRect.m_left = treeViewDraw->nmcd.rc.left;
								itemRect.m_top = treeViewDraw->nmcd.rc.top;
								itemRect.m_right = treeViewDraw->nmcd.rc.right;
								itemRect.m_bottom = treeViewDraw->nmcd.rc.bottom;							
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
			AbstractWin32Component::handleEventMessages( message, wParam, lParam );
			result = CallWindowProc( this->m_oldTreeWndProc, m_hwnd, message, wParam, lParam );
			
		}
		break;
	}
	return result;
}

void Win32Tree::addItem( TreeItem* item )
{
	TVINSERTSTRUCT insert = {0};
	TVITEM tvItem = {0};
	TreeItem* parent = item->getParent();
	HTREEITEM itemParent = NULL;
	if ( NULL != parent ){
		std::map<TreeItem*,HTREEITEM>::iterator it =
		m_treeItems.find( parent );
		if ( it != m_treeItems.end() ){
			itemParent = it->second;			
		}
		else{
			//throw exception;
		}
	}
	tvItem.mask = TVIF_TEXT | TVIF_PARAM;	
	if ( this->m_imageListCtrl != NULL ) {
		tvItem.mask |= TVIF_IMAGE;	
	}
	tvItem.cchTextMax = item->getCaption().size()+1;
	VCFChar* tmpName = new VCFChar[tvItem.cchTextMax];
	memset( tmpName, 0, tvItem.cchTextMax );
	item->getCaption().copy( tmpName, tvItem.cchTextMax-1 );
	tvItem.pszText = tmpName;	
	tvItem.lParam = (LPARAM)item;
	insert.hParent = itemParent;
	insert.hInsertAfter = TVI_FIRST;
	insert.item = tvItem;
	HTREEITEM addedItem = TreeView_InsertItem( this->m_hwnd, &insert );
	delete [] tmpName;
	m_treeItems[item] = addedItem;

	item->addItemAddedHandler( m_itemAddedHandler );
	item->addItemChangedHandler( m_itemChangedHandler );
	item->addItemDeletedHandler( m_itemDeletedHandler );
	item->addItemPaintHandler( m_itemPaintedHandler );
	item->addItemSelectedHandler( m_itemSelectedHandler );

	//now check the children
	
	Enumerator<TreeItem*>* children = item->getChildren();
	if ( NULL != children ){
		while ( children->hasMoreElements() ){
			TreeItem* child = children->nextElement();
			if ( NULL != child ){
				addItem( child );
			}
		}
	}
}

void Win32Tree::clear()
{
	m_treeItems.clear();
	StringUtils::trace( "tree items cleared\n" );
	TreeView_DeleteAllItems( this->m_hwnd );
}

void Win32Tree::onItemPaint( ItemEvent* event )
{

}

void Win32Tree::onItemChanged( ItemEvent* event )
{
	if ( NULL != event ){
		switch ( event->getType() ){
			case ITEM_EVENT_TEXT_CHANGED:{
				Object* source = event->getSource();
				if ( NULL != source ){
					TreeItem* item = dynamic_cast<TreeItem*>(source);
					if ( NULL != item ){
						std::map<TreeItem*,HTREEITEM>::iterator it =
							m_treeItems.find( item );
						if ( it != m_treeItems.end() ){
							TVITEM tvItem = {0};
							tvItem.mask = TVIF_TEXT | TVIF_HANDLE;
							tvItem.hItem = it->second;	
							String caption = item->getCaption();
							tvItem.cchTextMax = caption.size();
							char* tmp = new VCFChar[tvItem.cchTextMax+1];
							memset( tmp, 0, tvItem.cchTextMax+1 );
							caption.copy( tmp, tvItem.cchTextMax );
							tvItem.pszText = tmp;							
							TreeView_SetItem( this->m_hwnd, &tvItem );
							delete [] tmp;
						}
					}
				}				
			}
			break;
		}
	}
}

void Win32Tree::onItemSelected( ItemEvent* event )
{

}

void Win32Tree::onItemAdded( ItemEvent* event )
{

}

	
void Win32Tree::onItemDeleted( ItemEvent* event )
{
	Object* source = event->getSource();
	if ( NULL != source ){
		TreeItem* item = dynamic_cast<TreeItem*>(source);
		if ( NULL != item ){
			std::map<TreeItem*,HTREEITEM>::iterator found =
				m_treeItems.find( item );
			if ( found != m_treeItems.end() ){
				HTREEITEM hItem = found->second;
				m_treeItems.erase( found );
				TreeView_DeleteItem( m_hwnd, hItem );
			}
		}
	}
}

void Win32Tree::onImageListImageChanged( ImageListEvent* event )
{
	ulong32 index = event->getIndexOfImage();
	switch ( event->getType() ) {
		case IMAGELIST_EVENT_WIDTH_CHANGED : {
			
		}
		break;

		case IMAGELIST_EVENT_HEIGHT_CHANGED : {
			
		}
		break;

		case IMAGELIST_EVENT_ITEM_ADDED : {			
			Win32Image* win32Img = (Win32Image*)event->getImage();
			HBITMAP hbmImage = win32Img->getBitmap();
			int err = ImageList_Add( m_imageListCtrl, hbmImage, NULL );
			if ( err < 0 ) {
				//error condition !
			}			
		}
		break;

		case IMAGELIST_EVENT_ITEM_DELETED : {
			
		}
		break;
	}
}

