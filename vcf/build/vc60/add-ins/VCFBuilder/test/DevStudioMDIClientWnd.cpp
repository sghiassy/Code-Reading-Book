// DevStudioMDIClientWnd.cpp : implementation file
//

#include "stdafx.h"
#include "test.h"
#include "DevStudioMDIClientWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDevStudioMDIClientWnd* CDevStudioMDIClientWnd::globalDevStudioMDIChildMgr = NULL;

/////////////////////////////////////////////////////////////////////////////
// CDevStudioMDIClientWnd

CDevStudioMDIClientWnd::CDevStudioMDIClientWnd(HWND hWndSubclass):
	CSubClassWnd(hWndSubclass)
{
	this->DoSubclass();
}

CDevStudioMDIClientWnd::~CDevStudioMDIClientWnd()
{
}


BEGIN_MESSAGE_MAP(CDevStudioMDIClientWnd, CSubClassWnd)
	//{{AFX_MSG_MAP(CDevStudioMDIClientWnd)
	//ON_WM_MDIACTIVATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_MDIACTIVATE, OnMDIActivate )
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDevStudioMDIClientWnd message handlers
/*
void CDevStudioMDIClientWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CSubClassWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	
	OutputDebugString( "CDevStudioMDIClientWnd::OnMDIActivate\n" );		
}
*/

LRESULT CDevStudioMDIClientWnd::OnMDIActivate( WPARAM wParam, LPARAM lParam )
{
	OutputDebugString( "CDevStudioMDIClientWnd::OnMDIActivate\n" );
	return Default();
}