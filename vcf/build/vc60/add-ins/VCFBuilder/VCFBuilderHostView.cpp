// VCFBuilderHostView.cpp : implementation file
//


#include "stdafx.h"


#include "VCFBuilderHostView.h"






#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace VCF;
using namespace VCFBuilder;



/////////////////////////////////////////////////////////////////////////////
// VCFBuilderHostView

IMPLEMENT_DYNCREATE(VCFBuilderHostView, CView)

VCFBuilderHostView::VCFBuilderHostView()
{
	//m_vcfBuilderUIPanel = NULL;
}

VCFBuilderHostView::~VCFBuilderHostView()
{
}


BEGIN_MESSAGE_MAP(VCFBuilderHostView, CView)
	//{{AFX_MSG_MAP(VCFBuilderHostView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VCFBuilderHostView drawing

void VCFBuilderHostView::OnDraw(CDC* pDC)
{
	CRect r(0,0,0,0);
	GetClientRect( r );
	CBrush back;
	back.CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
	pDC->FillRect( r, &back );
}

/////////////////////////////////////////////////////////////////////////////
// VCFBuilderHostView diagnostics

#ifdef _DEBUG
void VCFBuilderHostView::AssertValid() const
{
	CView::AssertValid();
}

void VCFBuilderHostView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// VCFBuilderHostView message handlers

BOOL VCFBuilderHostView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	CView::PreCreateWindow(cs);
	
	cs.lpszClass = AfxRegisterWndClass( CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, NULL, NULL,NULL );

	return TRUE;
}

void VCFBuilderHostView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	
}

int VCFBuilderHostView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	VCFBuilderUIApplication::getVCFBuilderUI()->initMainWindow();
	m_vcfBuilderUIPanel = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();

	HWND hWnd = (HWND)m_vcfBuilderUIPanel->getPeer()->getHandleID();

	::SetParent( hWnd, this->m_hWnd );


	//Control* c = NULL;
	
	return 0;
}

void VCFBuilderHostView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if ( NULL != m_vcfBuilderUIPanel )	{
		m_vcfBuilderUIPanel->setBounds( &Rect(0,0,cx,cy) );
	}
}
void VCFBuilderHostView::newProject()
{
	if ( NULL != m_vcfBuilderUIPanel )	{
		m_vcfBuilderUIPanel->newProject( NULL );
	}
}
