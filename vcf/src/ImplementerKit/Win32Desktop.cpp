//Win32Desktop.h
#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"

using namespace VCF;



Win32Desktop::Win32Desktop( Desktop* desktop )
{
	m_desktop = desktop;
	if ( NULL == m_desktop ) {
		//throw exception
	}
	
}

Win32Desktop::~Win32Desktop()
{
	
}

void Win32Desktop::desktopBeginPainting( Rect* clippingRect )
{
	m_hClipRgn = NULL;
	DWORD flags = DCX_WINDOW|DCX_CACHE;

	if ( NULL != clippingRect ) {
		m_hClipRgn = ::CreateRectRgn( clippingRect->m_left, clippingRect->m_top, clippingRect->m_right, clippingRect->m_bottom );
		flags |= DCX_INTERSECTRGN;
	}

	HDC dc = NULL;
	
	
	if ( TRUE == LockWindowUpdate( ::GetDesktopWindow() ) ) {
		flags |= DCX_LOCKWINDOWUPDATE;		
	}

	dc = ::GetDCEx( ::GetDesktopWindow(), m_hClipRgn, flags );

	if ( NULL == dc ) {
		//throw exception !!
	}
	m_desktop->getContext()->getPeer()->setContextID( (long)dc );
}

void Win32Desktop::desktopEndPainting()
{
	::LockWindowUpdate( NULL );//unlocks the window update
	HDC dc = (HDC)m_desktop->getContext()->getPeer()->getContextID();
	m_desktop->getContext()->getPeer()->setContextID( (long)0 );
	ReleaseDC( ::GetDesktopWindow(), dc );
	if ( NULL != m_hClipRgn ) {
		DeleteObject( m_hClipRgn );
	}
	InvalidateRect( ::GetDesktopWindow(), NULL, TRUE );
}

bool Win32Desktop::desktopSupportsVirtualDirectories()
{
	return true;
}

bool Win32Desktop::desktopHasFileSystemDirectory()
{
	return true;
}

String Win32Desktop::desktopGetDirectory()
{
	String result = "";	

	IMalloc* mallocObj = NULL;
	HRESULT hr = SHGetMalloc(&mallocObj);
	if ( SUCCEEDED(hr) ) {
		VCFChar desktopPath[MAX_PATH];
		memset( desktopPath, 0, MAX_PATH );
		LPITEMIDLIST pidl;
		
		hr = ::SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl);
		if ( SUCCEEDED(hr) ) {
			BOOL err = SHGetPathFromIDList(pidl, desktopPath);
			
			if ( FALSE != err ) {
				result = desktopPath;
			}
			
			mallocObj->Free( pidl );
		}

		mallocObj->Release();
	}	
	
	return result;
}

ulong32 Win32Desktop::desktopGetHandleID()
{
	return (ulong32)GetDesktopWindow();
}

void Win32Desktop::desktopSetDesktop( Desktop* desktop )
{
	m_desktop = desktop;
}

void Win32Desktop::desktopTranslateToScreenCoords( Control* control, Point* pt )
{
	POINT win32Point = { pt->m_x, pt->m_y };
	::ClientToScreen( (HWND)control->getPeer()->getHandleID(), &win32Point );
	pt->m_x = win32Point.x;
	pt->m_y = win32Point.y;
}

void Win32Desktop::desktopTranslateFromScreenCoords( Control* control, Point* pt )
{
	POINT win32Point = { pt->m_x, pt->m_y };
	::ScreenToClient( (HWND)control->getPeer()->getHandleID(), &win32Point );
	pt->m_x = win32Point.x;
	pt->m_y = win32Point.y;
}
