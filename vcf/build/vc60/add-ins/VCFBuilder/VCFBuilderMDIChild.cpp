// VCFBuilderMDIChild.cpp : implementation file
//

#include "stdafx.h"
#include "VCFBuilder.h"
#include "VCFBuilderMDIChild.h"
#include "VCFBuilderHostView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TCHAR* VCFBuilderMDIChildWnd::WndClassName = "VCFBuilderMDIChildWnd";

VCFBuilderMDIChild* VCFBuilderMDIChild::globalVCFBuilderMDIChildWnd = NULL;

HICON VCFBuilderMDIChild::globalVCFBuilderIcon = NULL;

/////////////////////////////////////////////////////////////////////////////
// VCFBuilderMDIChild

IMPLEMENT_DYNCREATE(VCFBuilderMDIChild, CMDIChildWnd)

VCFBuilderMDIChild::VCFBuilderMDIChild()
{
}

VCFBuilderMDIChild::~VCFBuilderMDIChild()
{	
	VCFBuilderMDIChild::globalVCFBuilderMDIChildWnd = NULL;
}


BEGIN_MESSAGE_MAP(VCFBuilderMDIChild, CMDIChildWnd)
	//{{AFX_MSG_MAP(VCFBuilderMDIChild)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VCFBuilderMDIChild message handlers

BOOL VCFBuilderMDIChild::PreCreateWindow(CREATESTRUCT& cs) 
{
	CMDIChildWnd::PreCreateWindow(cs);
	
	cs.dwExStyle = 0;

	cs.lpszClass = AfxRegisterWndClass( CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, NULL, NULL, 
										VCFBuilderMDIChild::globalVCFBuilderIcon );

	return TRUE;
}

int VCFBuilderMDIChild::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetWindowText( "VCF Builder" );
	this->SetTitle( "VCF Builder" );

	//VCFBuilderHostView* vcfViewHost = new VCFBuilderHostView();
	//vcfViewHost->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, AFX_IDW_PANE_FIRST );
	
	return 0;
}

void VCFBuilderMDIChild::newProject()
{
	//VCFBuilderHostView* vcfViewHost = (VCFBuilderHostView*)GetActiveView();
	//vcfViewHost->newProject();
}

LRESULT CALLBACK VCFBuilderMDIChildWnd::WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT result = 0;

	switch ( uMsg ) {  
		/*
		case WM_MDIACTIVATE : {
			HWND hwndChildDeact = (HWND) wParam;
			HWND hwndChildAct = (HWND) lParam;          
			TCHAR tmp[256];
			sprintf( tmp, "VCFBuilderMDIChildWnd - WM_MDIACTIVATE hwndChildAct = %p hwndChildDeact = %p, hwnd = %p\n",
							hwndChildAct, hwndChildDeact, hwnd );

			if ( hwndChildDeact == hwnd ) {
				OutputDebugString( "VCFBuilderMDIChildWnd deactivated\n" );
			}
			result = DefWindowProc( hwnd, uMsg, wParam, lParam ); 
		}
		break;

		case WM_ACTIVATE : {
			int fActive = LOWORD(wParam);
			TCHAR tmp[256];
			if ( WA_INACTIVE == fActive ) {
				OutputDebugString( "VCFBuilderMDIChildWnd deactivated\n" );
			}
			result = DefWindowProc( hwnd, uMsg, wParam, lParam );
		}
		break;
		*/
		case WM_PAINT : {
			PAINTSTRUCT ps = {0};
			::BeginPaint( hwnd, &ps );
			HBRUSH brush = CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );

			FillRect( ps.hdc, &ps.rcPaint, brush );

			DeleteObject( brush );
			
			EndPaint( hwnd, &ps );
		}
		break;

		default : {
			result = DefWindowProc( hwnd, uMsg, wParam, lParam );
		}
		break;
	}

	return result;
}

void VCFBuilderMDIChildWnd::RegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wndClass = {0};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc  = VCFBuilderMDIChildWnd::WindowProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	wndClass.lpszClassName = VCFBuilderMDIChildWnd::WndClassName;
	
	//VCFBuilderMDIChild::globalVCFBuilderIcon

	RegisterClassEx( &wndClass );
}