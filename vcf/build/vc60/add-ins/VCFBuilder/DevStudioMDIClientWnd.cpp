// DevStudioMDIClientWnd.cpp : implementation file
//

#include "stdafx.h"
#include "VCFBuilder.h"
#include "DevStudioMDIClientWnd.h"
#include "VCFBuilderMDIChild.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDevStudioMDIClientWnd* CDevStudioMDIClientWnd::globalDevStudioMDIChildMgr = NULL;

/////////////////////////////////////////////////////////////////////////////
// CDevStudioMDIClientWnd

CDevStudioMDIClientWnd::CDevStudioMDIClientWnd(HWND hWndSubclass, IApplication* pDevStdioApp):
	CSubClassWnd(hWndSubclass)
{
	m_pDevStdioApp = pDevStdioApp;
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
	ON_MESSAGE( WM_MDICREATE, OnMDICreateWnd )
	ON_MESSAGE( WM_MDIDESTROY, OnMDIDestroyWnd )
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

LRESULT CDevStudioMDIClientWnd::OnMDICreateWnd( WPARAM wParam, LPARAM lParam )
{	
	OutputDebugString( "CDevStudioMDIClientWnd::OnMDICreateWnd\n" );
	
	/*
	CComQIPtr<IDispatch> spDisp;

	HRESULT hr = m_pDevStdioApp->get_Windows(&spDisp);
	if ( (NULL != spDisp.p) && (SUCCEEDED(hr)) ) {
		CComQIPtr<IWindows> spWindows = spDisp;
		if ( NULL != spWindows.p ) {
			long wndCount = 0;
			spWindows->get_Count( &wndCount );
			char tmp[256];
			sprintf( tmp, "Number of open windows, according to the DevStdio IApplication Interface: %d\n", wndCount );
			OutputDebugString( tmp );
		}		
	}
	*/

	return Default();
}

LRESULT CDevStudioMDIClientWnd::OnMDIDestroyWnd( WPARAM wParam, LPARAM lParam )
{	
	OutputDebugString( "CDevStudioMDIClientWnd::OnMDIDestroyWnd\n" );
	
	if ( NULL != VCFBuilderMDIChild::globalVCFBuilderMDIChildWnd ) {
		HWND hwndChild = (HWND)wParam;
		if ( hwndChild == VCFBuilderMDIChild::globalVCFBuilderMDIChildWnd->m_hWnd ) {
			
		}
		else {
			VCFBuilderMDIChild::globalVCFBuilderMDIChildWnd->MDIDestroy();
		}
	}
	/*
	CComQIPtr<IDispatch> spDisp;

	HRESULT hr = m_pDevStdioApp->get_Windows(&spDisp);
	if ( (NULL != spDisp.p) && (SUCCEEDED(hr)) ) {
		CComQIPtr<IWindows> spWindows = spDisp;
		if ( NULL != spWindows.p ) {
			long wndCount = 0;
			spWindows->get_Count( &wndCount );
			char tmp[256];
			sprintf( tmp, "Number of open windows, according to the DevStdio IApplication Interface: %d\n", wndCount );
			OutputDebugString( tmp );
		}		
	}
	*/
	return Default();
}