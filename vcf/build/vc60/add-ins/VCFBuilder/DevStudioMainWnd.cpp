// DevStudioMainWnd.cpp : implementation file
//

#include "stdafx.h"
#include "DevStudioMainWnd.h"
#include "VCFBuilderHostView.h"

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

	m_vcfBuilderHost = NULL;
}

CDevStudioMainWnd::~CDevStudioMainWnd()
{
}


BEGIN_MESSAGE_MAP(CDevStudioMainWnd, CSubClassWnd)
	//{{AFX_MSG_MAP(CDevStudioMainWnd)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDevStudioMainWnd message handlers


HWND CDevStudioMainWnd::GetMDIClientHWND()
{
	HWND result = NULL;
	char cClassName[256];
	
	result = GetTopWindow()->m_hWnd;
	::GetClassName(result, (LPTSTR)cClassName, sizeof(cClassName));
	while (strcmp(cClassName, "MDIClient") != 0)
	{
		result = ::GetNextWindow(result, GW_HWNDNEXT);
		ASSERT(result);
		GetClassName(result, (LPTSTR)cClassName, sizeof(cClassName));
	}
	
	return result;
}

void CDevStudioMainWnd::CreateVCFHost()
{
	m_vcfBuilderHost = new VCFBuilderHostView();
	m_vcfBuilderHost->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, (UINT)-1, NULL );
}

void CDevStudioMainWnd::SetVCFHostVisible( bool visible )
{
	if ( NULL != m_vcfBuilderHost ) {
		if ( true == visible ) {
			m_vcfBuilderHost->ShowWindow( SW_NORMAL );
		}
		else {
			m_vcfBuilderHost->ShowWindow( SW_HIDE );
		}
		CRect r(0,0,0,0);
		m_vcfBuilderHost->GetWindowRect( r );
		ScreenToClient( &r );
		if ( TRUE == m_vcfBuilderHost->IsWindowVisible() ) {
			m_vcfBuilderHost->SetWindowPos( &CWnd::wndTop, r.left, r.top, r.Width(), r.Height(), 0);
		}
		else {
			m_vcfBuilderHost->SetWindowPos( &CWnd::wndBottom, r.left, r.top, r.Width(), r.Height(), 0);
		}
	}
}