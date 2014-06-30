//Win32ScrollPeer.h
#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"


using namespace VCF;


std::map<HWND,Control*> Win32ScrollPeer::hScrollBarMap;

class Win32ScrollCorner : public CustomControl {
public:
	Win32ScrollCorner() {
		
	}

	virtual ~Win32ScrollCorner(){};

	virtual void paint( GraphicsContext* context ) {
		CustomControl::paint( context );
		Color* dark = GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor( SYSCOLOR_SHADOW );
		Color* light = GraphicsToolkit::getDefaultGraphicsToolkit()->getSystemColor( SYSCOLOR_HIGHLIGHT );
		double x = getWidth() - 1;
		double y = 5;

		context->setColor( light );
		context->moveTo( x, y);
		x = 5;
		y = getHeight()-1;
		context->lineTo( x, y);
		context->strokePath();

		x = getWidth() - 2;
		y = 7;
		context->setColor( dark );
		context->moveTo( x, y);
		x = 6;
		y = getHeight()-1;
		context->lineTo( x, y);
		context->strokePath();
		x = getWidth() - 2;
		y = 8;
		context->moveTo( x, y);
		x = 7;
		y = getHeight()-1;
		context->lineTo( x, y);
		context->strokePath();


		//line 2
		x = getWidth() - 1;
		y = 10;
		context->setColor( light );
		context->moveTo( x, y);
		x = 10;
		y = getHeight()-1;
		context->lineTo( x, y);
		context->strokePath();

		x = getWidth() - 2;
		y = 12;
		context->setColor( dark );
		context->moveTo( x, y);
		x = 11;
		y = getHeight()-1;
		context->lineTo( x, y);
		context->strokePath();

		x = getWidth() - 2;
		y = 13;
		context->moveTo( x, y);
		x = 12;
		y = getHeight()-1;
		context->lineTo( x, y);
		context->strokePath();

		//line 3
		x = getWidth() - 1;
		y = 15;
		context->setColor( light );
		context->moveTo( x, y);
		x = 15;
		y = getHeight()-1;
		context->lineTo( x, y);
		context->strokePath();

		x = getWidth() - 2;
		y = 17;
		context->setColor( dark );
		context->moveTo( x, y);
		x = 16;
		y = getHeight()-1;
		context->lineTo( x, y);
		context->strokePath();

		x = getWidth() - 2;
		y = 18;
		context->moveTo( x, y);
		x = 19;
		y = getHeight()-1;
		context->lineTo( x, y);
		context->strokePath();
	}
};


Win32ScrollPeer::Win32ScrollPeer( Control* scrollableControl )
{
	this->m_scrollableControl = scrollableControl;
	m_vScrollHWnd = NULL;
	m_hScrollHWnd = NULL;
	m_scrollCorner = NULL;
}

Win32ScrollPeer::~Win32ScrollPeer()
{
	delete m_scrollCorner;
}

void Win32ScrollPeer::removeScrollBarsFromMap()
{
	std::map<HWND,Control*>::iterator found = Win32ScrollPeer::hScrollBarMap.find( m_vScrollHWnd );
	if ( found != Win32ScrollPeer::hScrollBarMap.end() ) {
		Win32ScrollPeer::hScrollBarMap.erase( found );
	}

	found = Win32ScrollPeer::hScrollBarMap.find( m_hScrollHWnd );
	if ( found != Win32ScrollPeer::hScrollBarMap.end() ) {
		Win32ScrollPeer::hScrollBarMap.erase( found );
	}
}

void Win32ScrollPeer::addScrollBarsToMap()
{
	Win32ScrollPeer::hScrollBarMap[m_vScrollHWnd] = m_scrollableControl;
	Win32ScrollPeer::hScrollBarMap[m_hScrollHWnd] = m_scrollableControl;
}

Control* Win32ScrollPeer::getScrollableControlFromHWnd( HWND scrollhWnd )
{
	Control* result = NULL;
	std::map<HWND,Control*>::iterator found = Win32ScrollPeer::hScrollBarMap.find( scrollhWnd );
	if ( found != Win32ScrollPeer::hScrollBarMap.end() ) {
		result = found->second;
	}	
	return result;
}

void Win32ScrollPeer::setScrollableControl( Control* scrollableControl )
{
	if ( NULL != m_scrollableControl ) 
	{	
		removeScrollBarsFromMap();
		BOOL err = DestroyWindow( m_vScrollHWnd );
		err = DestroyWindow( m_hScrollHWnd );
		m_vScrollHWnd = NULL;
		m_hScrollHWnd = NULL;
		delete m_scrollCorner;
	}
	this->m_scrollableControl = scrollableControl;
	if ( NULL != m_scrollableControl ) {
		HWND hwnd = (HWND)m_scrollableControl->getPeer()->getHandleID();
		
		Application* app = Application::getRunningInstance();
		HINSTANCE hInst = NULL;
		if ( NULL != app ) {
			hInst = (HINSTANCE)app->getPeer()->getHandleID();
		}
		else {
			hInst = ::GetModuleHandle(NULL);
		}
		m_vScrollHWnd = CreateWindowEx( 0, "SCROLLBAR", NULL, WS_CHILD | SBS_VERT | WS_VISIBLE, 0, 0, 200,
			CW_USEDEFAULT, hwnd, NULL, hInst, NULL );

		m_hScrollHWnd = CreateWindowEx( 0, "SCROLLBAR", NULL, WS_CHILD | SBS_HORZ | WS_VISIBLE, 0, 0, 200,
			CW_USEDEFAULT, hwnd, NULL, hInst, NULL );

		m_scrollCorner = new Win32ScrollCorner();
		HWND scHwnd = (HWND)m_scrollCorner->getPeer()->getHandleID();
		SetParent( scHwnd, hwnd );

		addScrollBarsToMap();
	}
}

void Win32ScrollPeer::scrollTo( const double& xPosition, const double& yPosition )
{
	SCROLLINFO si = {0};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS ;
	si.nPos = yPosition;
	
	HWND hwnd = (HWND)m_scrollableControl->getPeer()->getHandleID();
	SetScrollInfo( m_vScrollHWnd, SB_CTL, &si, TRUE );
	memset( &si, 0, sizeof(si) );
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS ;
	si.nPos = xPosition;
	SetScrollInfo( m_hScrollHWnd, SB_CTL, &si, TRUE );
}

void Win32ScrollPeer::recalcScrollPositions( Scrollable* scrollable )
{
	SCROLLINFO si = {0};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
	/**
	*if the m_scrollableControl is lightweight then the handle represents the parent's 
	*hwnd, so get the control's bounds, instead of using 0,0 for left,top
	*/
	Rect bounds(0,0,0,0);
	if ( true == m_scrollableControl->isLightWeight() ) {
		bounds = *(m_scrollableControl->getBounds());
	}
	else {
		bounds.setRect( 0, 0, m_scrollableControl->getWidth(), m_scrollableControl->getHeight() );
	}	

	int x = bounds.m_left + ((bounds.getWidth() - GetSystemMetrics(SM_CXHTHUMB)));
	int y = scrollable->getVerticalTopScrollSpace() + bounds.m_top;
	int w = GetSystemMetrics(SM_CXHTHUMB);
	int h = bounds.getHeight() - (scrollable->getVerticalTopScrollSpace() + scrollable->getVerticalBottomScrollSpace() + GetSystemMetrics(SM_CXHTHUMB));

	MoveWindow( m_vScrollHWnd, x, y, w, h, TRUE );

	x = scrollable->getHorizontalLeftScrollSpace() + bounds.m_left;
	y = bounds.m_top + (bounds.getHeight() - GetSystemMetrics(SM_CYVTHUMB));
	w = bounds.getWidth() - (scrollable->getHorizontalLeftScrollSpace() + scrollable->getHorizontalRightScrollSpace() + GetSystemMetrics(SM_CYVTHUMB));
	h = GetSystemMetrics(SM_CYVTHUMB);

	MoveWindow( m_hScrollHWnd, x, y, w, h, TRUE );

	m_scrollCorner->setBounds( &Rect( x+w,y, x+w+GetSystemMetrics(SM_CXHTHUMB), y+h ) );

	si.nPage = (scrollable->getVirtualViewHeight() > bounds.getHeight() ) ? bounds.getHeight() : 0;
	si.nPos = scrollable->getVerticalPosition();
	si.nMin = 0;
	si.nMax = (scrollable->getVirtualViewHeight() > bounds.getHeight() ) ? 
		scrollable->getVirtualViewHeight() : si.nMin;

	HWND hwnd = (HWND)m_scrollableControl->getPeer()->getHandleID();	

	SetScrollInfo( m_vScrollHWnd, SB_CTL, &si, TRUE );

	memset( &si, 0, sizeof(si) );
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
	si.nPage = (scrollable->getVirtualViewWidth() > bounds.getWidth() ) ? bounds.getWidth() : 0;
	si.nPos = scrollable->getHorizontalPosition();
	si.nMin = 0;
	si.nMax = (scrollable->getVirtualViewWidth() > bounds.getWidth() ) ? scrollable->getVirtualViewWidth() : si.nMin;
	
	SetScrollInfo( m_hScrollHWnd, SB_CTL, &si, TRUE );
}