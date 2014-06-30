// Page1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcfconsolewiz.h"
#include "Page1Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Page1Dlg dialog


Page1Dlg::Page1Dlg()
	: CAppWizStepDlg(Page1Dlg::IDD)
{
	//{{AFX_DATA_INIT(Page1Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void Page1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CAppWizStepDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Page1Dlg)
	DDX_Control(pDX, IDC_IMAGE, m_image);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Page1Dlg, CAppWizStepDlg)
	//{{AFX_MSG_MAP(Page1Dlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Page1Dlg message handlers

BOOL Page1Dlg::OnDismiss()
{
	if (!UpdateData(TRUE))
		return FALSE;

	// TODO: Set template variables based on the dialog's data.

	return TRUE;	// return FALSE if the dialog shouldn't be dismissed
}

BOOL Page1Dlg::OnInitDialog() 
{
	CAppWizStepDlg::OnInitDialog();
	
	CBitmap bmp;
	bmp.LoadBitmap( IDB_SPLASH );
	this->m_image.SetBitmap( bmp );
	bmp.Detach();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
