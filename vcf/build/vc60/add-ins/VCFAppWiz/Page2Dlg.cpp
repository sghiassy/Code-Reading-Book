// Page2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcfwizard.h"
#include "Page2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Page2Dlg dialog


Page2Dlg::Page2Dlg()
	: CAppWizStepDlg(Page2Dlg::IDD)
{
	//{{AFX_DATA_INIT(Page2Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_LibLinkage = VCF_DLL_LINK;
}


void Page2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CAppWizStepDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Page2Dlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	if ( pDX->m_bSaveAndValidate )	{
		CButton* btn1 = (CButton*)GetDlgItem(IDC_STATIC_LIB);
		if ( 0 == btn1->GetCheck() ) {
			m_LibLinkage = VCF_STATIC_LINK;
		}
		else {
			m_LibLinkage = VCF_DLL_LINK;
		}
	}
	else {
		CButton* btn1 = (CButton*)GetDlgItem(IDC_STATIC_LIB);
		CButton* btn2 = (CButton*)GetDlgItem(IDC_DYNAMIC_LIB);
		if ( VCF_DLL_LINK == m_LibLinkage ) {
			btn1->SetCheck( 0 );
			btn2->SetCheck( 1 );	
		}
		else if ( VCF_STATIC_LINK == m_LibLinkage ) {
			btn1->SetCheck( 1 );
			btn2->SetCheck( 0 );	
		}
	}
}


BEGIN_MESSAGE_MAP(Page2Dlg, CAppWizStepDlg)
	//{{AFX_MSG_MAP(Page2Dlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_DYNAMIC_LIB, OnDynamicLib)
	ON_BN_CLICKED(IDC_STATIC_LIB, OnStaticLib)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Page2Dlg message handlers

/*
void Page2Dlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CWnd* wnd = this->GetDlgItem( IDC_IMAGE2 );
	CDC* pDC = wnd->GetDC();
	CDC tmpDC;
	tmpDC.CreateCompatibleDC( NULL );
	CBitmap bmp;
	bmp.LoadBitmap( IDB_SPLASH );
	CBitmap* oldBMP = tmpDC.SelectObject( &bmp );
	BITMAP bmpInfo = {0};
	GetObject(bmp, sizeof(BITMAP), &bmpInfo );
	
	pDC->BitBlt( 1, 1, bmpInfo.bmWidth, bmpInfo.bmHeight, &tmpDC, 0, 0, SRCCOPY );	

	bmp.DeleteObject();
	tmpDC.SelectObject( oldBMP );
	tmpDC.DeleteDC();
	wnd->ReleaseDC( pDC );
	
	// Do not call CAppWizStepDlg::OnPaint() for painting messages
}
*/

BOOL Page2Dlg::OnInitDialog() 
{
	CAppWizStepDlg::OnInitDialog();
	CStatic* wnd = (CStatic*)this->GetDlgItem( IDC_IMAGE2 );
	CBitmap bmp;
	bmp.LoadBitmap( IDB_BITMAP1 );
	wnd->SetBitmap( (HBITMAP)bmp.Detach() );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Page2Dlg::OnDynamicLib() 
{
	// TODO: Add your control notification handler code here
	m_LibLinkage = VCF_DLL_LINK;
	UpdateData( FALSE );
}

void Page2Dlg::OnStaticLib() 
{
	// TODO: Add your control notification handler code here
	m_LibLinkage = VCF_STATIC_LINK;
	UpdateData( FALSE );
}
