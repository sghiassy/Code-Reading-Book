// DevStudioMainWnd.cpp : implementation file
//

#include "stdafx.h"
#include "test.h"
#include "DevStudioMainWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDevStudioMainWnd * CDevStudioMainWnd::globalDevStudioMainWnd = NULL;

/////////////////////////////////////////////////////////////////////////////
// CDevStudioMainWnd

CDevStudioMainWnd::CDevStudioMainWnd(HWND hWndSubclass):
	CSubClassWnd( hWndSubclass )
{
	DoSubclass();

	TCHAR tmp[256];
	::GetWindowText( hWndSubclass, tmp, 256 );
	OutputDebugString( tmp );

}

CDevStudioMainWnd::~CDevStudioMainWnd()
{
}


BEGIN_MESSAGE_MAP(CDevStudioMainWnd, CSubClassWnd)
	//{{AFX_MSG_MAP(CDevStudioMainWnd)
	ON_WM_MDIACTIVATE()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDevStudioMainWnd message handlers

void CDevStudioMainWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CSubClassWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	
	OutputDebugString( "CDevStudioMainWnd::OnMDIActivate\n" );	
}

void CDevStudioMainWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CSubClassWnd::OnActivate(nState, pWndOther, bMinimized);
	
	if ( WA_INACTIVE ==	nState ) {
		OutputDebugString( "CDevStudioMainWnd::OnActivate state = WA_INACTIVE\n" );
	}
	else if ( WA_ACTIVE == nState ) {
		OutputDebugString( "CDevStudioMainWnd::OnActivate state = WA_ACTIVE\n" );
	}
}
