// PropertyInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcfnewclasswiz.h"
#include "PropertyInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PropertyInfoDlg dialog


PropertyInfoDlg::PropertyInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PropertyInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PropertyInfoDlg)
	m_propName = _T("");
	m_readOnlyProp = FALSE;
	m_propType = _T("int");
	//}}AFX_DATA_INIT
}


void PropertyInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropertyInfoDlg)
	DDX_Control(pDX, IDC_PROP_TYPE, m_propTypeList);
	DDX_Text(pDX, IDC_PROP_NAME, m_propName);
	DDX_Check(pDX, IDC_READ_ONLY_PROP, m_readOnlyProp);
	//DDX_CBIndex(pDX, IDC_PROP_TYPE, m_propType);
	//}}AFX_DATA_MAP

	if ( pDX->m_bSaveAndValidate ){
		m_propTypeList.GetLBText( m_propTypeList.GetCurSel(), m_propType ); 
	}
	else {
		m_propTypeList.SelectString( 0, m_propType );
	}
}


BEGIN_MESSAGE_MAP(PropertyInfoDlg, CDialog)
	//{{AFX_MSG_MAP(PropertyInfoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropertyInfoDlg message handlers

BOOL PropertyInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
