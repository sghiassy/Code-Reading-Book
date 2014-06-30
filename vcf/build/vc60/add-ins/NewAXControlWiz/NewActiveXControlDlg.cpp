// NewActiveXControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "newaxcontrolwiz.h"
#include "NewActiveXControlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NewActiveXControlDlg dialog


NewActiveXControlDlg::NewActiveXControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(NewActiveXControlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(NewActiveXControlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void NewActiveXControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NewActiveXControlDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NewActiveXControlDlg, CDialog)
	//{{AFX_MSG_MAP(NewActiveXControlDlg)
	ON_BN_CLICKED(IDC_SELECT_CONTROL, OnSelectControl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NewActiveXControlDlg message handlers

void NewActiveXControlDlg::OnSelectControl() 
{
	COleInsertDialog dlg;
	if ( IDOK == dlg.DoModal(COleInsertDialog::ControlsOnly) ) {
		CLSID clsid = dlg.GetClassID();
		
	}
}
