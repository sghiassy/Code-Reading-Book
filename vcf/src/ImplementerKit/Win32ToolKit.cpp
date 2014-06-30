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

// Win32ToolKit.cpp
#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"


using namespace VCF;


#define TOOLTIP_TIMERID					30565
#define TOOLTIP_TIMEOUT_TIMERID			30566

static UINT ToolTipTimerID = 0;
static UINT ToolTipTimoutTimerID = 0;

/**
*used by the ToolKitHookWndProc andthe construcor for Win32ToolKit()
*/
static HHOOK hookHandle;


class Win32ToolTip : public Window {
public:
	Win32ToolTip() {
		setColor( GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor( SYSCOLOR_TOOLTIP ) );
		getFont()->setColor( GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor( SYSCOLOR_TOOLTIP_TEXT ) );
		setFrameStyle( FST_NOBORDER_FIXED );
		setFrameTopmost( true );
		setBounds( &Rect(200, 200, 400, 235 ) );
		setVisible( true );
	};

	virtual ~Win32ToolTip() {};

	virtual void paint( GraphicsContext* ctx ) {
		Window::paint( ctx );
		ctx->setColor( Color::getColor( "black" ) );
		Rect r(0,0,getWidth(), getHeight() );
		ctx->rectangle( &r );
		ctx->strokePath();
		ctx->setCurrentFont( getFont() );
		ctx->textAt( (r.getWidth()/2.0) - (ctx->getTextWidth(getCaption())/2.0), 
						(r.getHeight()/2.0) - (ctx->getTextHeight("EM")/2.0 + 1), 
						getCaption() );
	}

	virtual void mouseClick( MouseEvent* e ) {
		hide();
	}

	virtual void mouseDown( MouseEvent* e ) {
		hide();
	}
};

class ToolTipWatcher : public ObjectWithEvents {
public:
	ToolTipWatcher() {
		m_currentControlWithTooltip = NULL;

		m_toolTip = NULL;		
	}

	virtual ~ToolTipWatcher() {
		if ( NULL != m_toolTip ) {
			delete m_toolTip;
		}
	}
	
	Control* getActualToolTipControl( Control* control, Point* pt ) {
		Control* result = NULL;
		
		Container* container = dynamic_cast<Container*>( control );
		if ( NULL != container ) {
			Enumerator<Control*>* children = container->getChildren();
			bool found = false;
			while ( (true == children->hasMoreElements()) && (false == found) ) {
				Control* child = children->nextElement();
				if ( true == child->isLightWeight() ) {
					Rect* bounds = child->getBounds();
					if ( true == bounds->containsPt( pt ) ) {
						Point tmpPt = *pt;
						child->translateToLocal( &tmpPt );
						result = getActualToolTipControl( child, &tmpPt );
						if ( NULL != result  ) {
							found = true;//quit the loop
						}
					}
				}
			}
		}
		else {
			result = control;			
		}
		
		return result;
	}

	void showTooltip( Control* control, Point* pt ) {
		
		if ( NULL == m_toolTip ) {
			m_toolTip = new Win32ToolTip();
			m_toolTip->setVisible( false );
			m_toolTip->addFocusLostHandler( 
								new FocusEventHandler<ToolTipWatcher>( this, 
																		ToolTipWatcher::onToolTipLostFocus,
																		"toolTipWatcherHandler" ) );
		}

		//pt is in screen coordinates
		Point tmpPt = *pt;
		Desktop::getDesktop()->translateFromScreenCoords( control, &tmpPt );

		Control* actualToolTipControl = getActualToolTipControl( control, &tmpPt );
		if ( NULL == actualToolTipControl ) {
			return;// nothing to do
		}

		String tooltip = actualToolTipControl->getToolTip();		
		
		ToolTipEvent tooltipEvent( actualToolTipControl, 0 ); 
		tooltipEvent.setToolTipLocation( pt );
		tooltipEvent.setCustomTooltipContext( m_toolTip->getContext() );
		Size sz;		
		tooltipEvent.setToolTipSize( &sz );

		if ( true == tooltip.empty() ) {
			//fire a tooltip requested
			tooltipEvent.setType( TOOLTIP_EVENT_TIP_REQESTED );

			actualToolTipControl->fireOnToolTipRequested( &tooltipEvent );

			tooltip = tooltipEvent.getToolTipString();

			tooltipEvent.setType( 0 );
		}

		if ( false == tooltip.empty() ) {
			
			actualToolTipControl->fireOnToolTip( &tooltipEvent );
			Point tmpPt = *tooltipEvent.getToolTipLocation();
			m_toolTip->setLeft( tmpPt.m_x );
			m_toolTip->setTop( tmpPt.m_y );

			sz = *tooltipEvent.getToolTipSize();	
			if ( sz.m_height > 0.0 ) {
				m_toolTip->setHeight( sz.m_height );
			}
			else {
				m_toolTip->setHeight( m_toolTip->getContext()->getTextHeight( "EM" ) + 5 );
			}

			if ( sz.m_width > 0.0 ) {
				m_toolTip->setWidth( sz.m_width );
			}
			else {
				m_toolTip->setWidth( m_toolTip->getContext()->getTextWidth( tooltip ) + 10 );
			}
			
			m_toolTip->setCaption( tooltip );
			m_toolTip->show();
			m_currentControlWithTooltip = actualToolTipControl;
		}
	}

	void onToolTipLostFocus( FocusEvent* e ) {
		Window* w = (Window*)e->getSource();
		w->hide();
	};

	Win32ToolTip* m_toolTip;
	Point m_checkPt;
	Control* m_currentControlWithTooltip;
};

static ToolTipWatcher* toolTipWatcher = NULL;

LRESULT CALLBACK ToolKitHookWndProc( int nCode, WPARAM wParam, LPARAM lParam );


/**
*this is a weird case that Win32 gives us, so here's the deal:
*WM_NOTIFY messages are sent to parent windows, so normally, all the 
*handling is done there. This is fine and well if you are writing a
*big fat SDK switch statement (like the one you're looking at now :),
*but really sucks if you have your code broken up into a well organized 
*framework (like I am attempting to build here ! ). What this means is 
*that control objects can never handle their own notifications! So to 
*facilitate this we'll do our processing here (which at the moment is nothing)
*and then pass the message right back to the child that sent it !
*The reason this code is here, instead of in AbstractWin32Component::handleEventMessages(),
*is due to the way windows are created in the framework. A hidden dummy window, 
*whose parent is the Desktop, is created and made available as a temporary parent
*to Peers before the created window is actually hooked into into the real
*parent. What seems to happen with the common controls is the first parent hwnd sent 
*in to the control when it is first created is the one to which all future WM_NOTIFY
*messages are sent to. So the code below exists to route those messages back to 
*the child Win32Object where they are handled.
*First the lParam is typecasted into a NMHDR structure. This gives us access to 
*the hwnd and hopefully the Win32Object. If we have a valid Win32Object then we 
*call the Win32Object::handleEventMessages(), passing the NMHDR.code field as the 
*reflected window message, the original wParam, which gets ignored anyways, and
*the original lParam, which upon arriving back at the child control will again
*get typcast back to some other notification structure. Gotta love M$ !!!!!
*PS: Yes, this is one of the biggest hacks I've seen  ;-)
*PPS This also works for WM_COMMAND messages as well
*/
LRESULT CALLBACK DummyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch ( message ){
		case WM_LBUTTONUP:	{
			StringUtils::trace( "DummyWndProc::WM_LBUTTONUP\n" );
		}
		break;

		case WM_CREATE:	{
			//toolTipWatcher = new ToolTipWatcher();
			//ToolTipTimerID = ::SetTimer( hWnd, TOOLTIP_TIMERID, 500, NULL );
		}
		break;
		
		case WM_TIMER:	{		
			/*
			UINT wTimerID = wParam;
			if ( ToolTipTimerID == wTimerID ) {
				POINT pt = {0};
				if ( TRUE == GetCursorPos( &pt ) ) {
					Point tmpPt( pt.x, pt.y );
					HWND hwndCursorIsOver = WindowFromPoint( pt );
					if ( NULL != hwndCursorIsOver ) {
						ScreenToClient( hwndCursorIsOver, &pt );
						HWND childWnd = ChildWindowFromPointEx( hwndCursorIsOver, pt, CWP_SKIPINVISIBLE );
						if ( childWnd != NULL ) {
							ClientToScreen( hwndCursorIsOver, &pt );
							
							hwndCursorIsOver = childWnd;

							ScreenToClient( hwndCursorIsOver, &pt );
						}
						
						if ( FALSE != IsWindow( hwndCursorIsOver ) ) {
							Win32Object* win32Obj = Win32Object::getWin32ObjectFromHWND( hwndCursorIsOver );
							
							if ( NULL != win32Obj ) {
								tmpPt.m_y += (::GetSystemMetrics( SM_CYCURSOR )/2);
								
								if ( (toolTipWatcher->m_checkPt.m_x == tmpPt.m_x) && (toolTipWatcher->m_checkPt.m_y == tmpPt.m_y) ) {
									toolTipWatcher->showTooltip( win32Obj->getPeerControl(), &tmpPt );
									ToolTipTimoutTimerID = ::SetTimer( hWnd, TOOLTIP_TIMEOUT_TIMERID, 5000, NULL );
								}
								else {
									if ( NULL != toolTipWatcher->m_toolTip ) {
										toolTipWatcher->m_toolTip->hide();
									}
								}
							}
						}
					}
					toolTipWatcher->m_checkPt = tmpPt;
				}				
			}
			
			else if ( ToolTipTimoutTimerID == wTimerID ) {
				::KillTimer( hWnd, TOOLTIP_TIMEOUT_TIMERID );
				
				if ( NULL != toolTipWatcher->m_toolTip ) {
					toolTipWatcher->m_toolTip->hide();
				}
				
			}
			*/
		}
		break;

		case WM_DESTROY:	{					
			//::KillTimer( hWnd, TOOLTIP_TIMERID );
			//delete toolTipWatcher;
			result = ::DefWindowProc( hWnd, message, wParam, lParam );
		}
		break;

		case WM_NOTIFY:{			
			NMHDR* notificationHdr = (LPNMHDR)lParam;
			Win32Object* win32Obj = Win32Object::getWin32ObjectFromHWND( notificationHdr->hwndFrom );
			if ( NULL != win32Obj ){
				result = win32Obj->handleEventMessages( notificationHdr->code, wParam, lParam );
			}
		}
		break;

		case WM_COMMAND:{
			UINT notifyCode = HIWORD(wParam); 
			//wID = LOWORD(wParam); // item, control, or accelerator identifier , ignored for now        
			HWND hwndCtl = (HWND) lParam;
			Win32Object* win32Obj = Win32Object::getWin32ObjectFromHWND( hwndCtl );
			if ( NULL != win32Obj ){
				result = win32Obj->handleEventMessages( notifyCode, wParam, lParam );
			}
			result = ::DefWindowProc( hWnd, message, wParam, lParam );
		}
		break;
		/*
		case WM_CTLCOLOREDIT : case WM_CTLCOLORBTN: case WM_CTLCOLORLISTBOX: {
			
			HWND hwndCtl = (HWND) lParam; // handle to static control 
			Win32Object* win32Obj = Win32Object::getWin32ObjectFromHWND( hwndCtl );
			if ( NULL != win32Obj ){
				result = win32Obj->handleEventMessages( message, wParam, lParam );
			}
			if ( result != 0 ) {
				result = ::DefWindowProc( hWnd, message, wParam, lParam );
			}
		}
		break;
		*/
		default : {
			result = ::DefWindowProc( hWnd, message, wParam, lParam );
		}
		break;
	}
	return result;
}

ATOM RegisterDummyWndClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)DummyWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "DummyWnd";
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}


Win32ToolKit::Win32ToolKit():
	UIToolkit()
{
	m_dummyParentWnd = NULL;	
}	


Win32ToolKit::~Win32ToolKit()
{
	
}

ApplicationPeer* Win32ToolKit::createApplicationPeer()
{
	return new Win32Application();
}

TextPeer* Win32ToolKit::createTextPeer( TextControl* component, const bool& isMultiLineControl, ComponentType componentType)
{
	return new Win32Edit( component, isMultiLineControl );
}

TreePeer* Win32ToolKit::createTreePeer( TreeControl* component, ComponentType componentType )
{
	return new Win32Tree( component );
}

ListviewPeer* Win32ToolKit::createListViewPeer( ListViewControl* component, ComponentType componentType )
{
	return new Win32Listview( component );
}

DialogPeer* Win32ToolKit::createDialogPeer( Frame* owner, Dialog* component, ComponentType componentType )
{
	return new Win32Dialog( owner, component );
}

DialogPeer* Win32ToolKit::createDialogPeer()
{
	return new Win32Dialog();
}

void Win32ToolKit::createDummyParentWindow()
{	
	RegisterDummyWndClass( ::GetModuleHandle( NULL ) );

	HWND parent = ::GetDesktopWindow();
	m_dummyParentWnd = ::CreateWindow( "DummyWnd", NULL, WS_CHILD , 0, 0, 0, 0, parent, NULL, ::GetModuleHandle( NULL ), NULL ); 
}

HWND Win32ToolKit::getDummyParent()
{
	if ( NULL == m_dummyParentWnd ){
		this->createDummyParentWindow();
	}
	return this->m_dummyParentWnd;
}

MenuItemPeer* Win32ToolKit::createMenuItemPeer( MenuItem* item )
{
	return new Win32MenuItem( item );
}

MenuBarPeer* Win32ToolKit::createMenuBarPeer( MenuBar* menuBar )
{
	return new Win32MenuBar( menuBar );
}

ComboBoxPeer* Win32ToolKit::createComboBoxPeer( ComboBoxControl* component, ComponentType componentType)
{
	return NULL;//temporarilynew Win32ComboBox( component );
}

ButtonPeer* Win32ToolKit::createButtonPeer( CommandButton* component, ComponentType componentType)
{
	return new Win32Button( component );
}

ContextPeer* Win32ToolKit::createContextPeer( Control* component )
{
	return new Win32ControlContext( component->getPeer() );
}

CommonFileDialogPeer* Win32ToolKit::createCommonFileOpenDialogPeer( Control* owner )
{
	return new Win32FileOpenDialog( owner );
}

CommonFileDialogPeer* Win32ToolKit::createCommonFileSaveDialogPeer( Control* owner )
{
	return new Win32FileSaveDialog( owner );
}

CommonColorDialogPeer* Win32ToolKit::createCommonColorDialogPeer( Control* owner )
{
	return new Win32ColorDialog( owner );
}

CommonFolderBrowseDialogPeer* Win32ToolKit::createCommonFolderBrowseDialogPeer( Control* owner )
{
	return new Win32FolderBrowseDialog();
}

DragDropPeer* Win32ToolKit::createDragDropPeer()
{
	return NULL;
}

DataObjectPeer* Win32ToolKit::createDataObjectPeer()
{
	return NULL;
}

DropTargetPeer* Win32ToolKit::createDropTargetPeer()
{
	return new Win32DropTargetPeer();
}

PopupMenuPeer* Win32ToolKit::createPopupMenuPeer( PopupMenu* popupMenu )
{
	return new Win32PopupMenu( popupMenu );
}

CommonFontDialogPeer* Win32ToolKit::createCommonFontDialogPeer( Control* owner )
{
	return new Win32FontDialog( owner );
}

DesktopPeer* Win32ToolKit::createDesktopPeer( Desktop* desktop )
{
	return new Win32Desktop( desktop );
}

ScrollPeer* Win32ToolKit::createScrollPeer( Control* control )
{
	return new Win32ScrollPeer( control );
}

CursorPeer* Win32ToolKit::createCursorPeer( Cursor* cursor )
{
	return new Win32CursorPeer( cursor );
}

ControlPeer* Win32ToolKit::createControlPeer( Control* component, ComponentType componentType)
{
	ControlPeer* result = NULL;

	switch ( componentType ){
		case CT_LIGHTWEIGHT:{
			result = new LightweightComponent( component );
		}
		break;

		case CT_DEFAULT: case CT_HEAVYWEIGHT:{
			result = new Win32Component( component );
		}
		break;
	}
	return result;
}


WindowPeer* Win32ToolKit::createWindowPeer( Control* component, ComponentType componentType)
{
	return new Win32Window( component );
}
