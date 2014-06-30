// Page3Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcfwizard.h"
#include "Page3Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Page3Dlg dialog


Page3Dlg::Page3Dlg(CWnd* pParent /*=NULL*/)
	: CAppWizStepDlg(Page3Dlg::IDD)
{
	//{{AFX_DATA_INIT(Page3Dlg)
	m_useVCFNet = FALSE;
	m_useVCFRemote = FALSE;
	m_useOpenGL = FALSE;
	//}}AFX_DATA_INIT
}


void Page3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Page3Dlg)
	DDX_Check(pDX, IDC_VCFNET, m_useVCFNet);
	DDX_Check(pDX, IDC_VCFREMOTE, m_useVCFRemote);
	DDX_Check(pDX, IDC_OPENGL, m_useOpenGL);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Page3Dlg, CDialog)
	//{{AFX_MSG_MAP(Page3Dlg)
	ON_BN_CLICKED(IDC_VCFNET, OnVcfnet)
	ON_BN_CLICKED(IDC_VCFREMOTE, OnVcfremote)
	ON_BN_CLICKED(IDC_OPENGL, OnOpengl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Page3Dlg message handlers

void Page3Dlg::OnVcfnet() 
{
	// TODO: Add your control notification handler code here
	m_useVCFNet = !m_useVCFNet;
	UpdateData( FALSE );
}

void Page3Dlg::OnVcfremote() 
{
	// TODO: Add your control notification handler code here
	m_useVCFRemote = !m_useVCFRemote;
	if ( TRUE == m_useVCFRemote ) {
		m_useVCFNet = TRUE;
	}
	UpdateData( FALSE );
}

void Page3Dlg::OnOpengl() 
{
	m_useOpenGL = !m_useOpenGL;
	UpdateData( FALSE );
}

BOOL Page3Dlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CStatic* wnd = (CStatic*)this->GetDlgItem( IDC_IMAGE3 );
	CBitmap bmp;
	bmp.LoadBitmap( IDB_BITMAP2 );
	wnd->SetBitmap( (HBITMAP)bmp.Detach() );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
