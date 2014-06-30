// ViewHost.cpp : implementation file
//

#include "stdafx.h"
#include "test.h"
#include "ViewHost.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewHost

IMPLEMENT_DYNCREATE(CViewHost, CView)

CViewHost::CViewHost()
{
}

CViewHost::~CViewHost()
{
}


BEGIN_MESSAGE_MAP(CViewHost, CView)
	//{{AFX_MSG_MAP(CViewHost)
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CViewHost message handlers


void CViewHost::OnDraw(CDC* pDC)
{
	CBrush fill( ::GetSysColor( COLOR_BTNFACE ) );

	CBrush* oldBrush = pDC->SelectObject( &fill );
	CRect rect(0,0,0,0);
	GetClientRect( rect );
	pDC->Rectangle( rect );
	pDC->SelectObject( oldBrush );

	pDC->TextOut( 20, 100, "CViewHost" );
}

BOOL CViewHost::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}
