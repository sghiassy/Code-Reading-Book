// ConversionOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "test.h"
#include "ConversionOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ConversionOptionsDlg dialog

IMPLEMENT_DYNCREATE(ConversionOptionsDlg, CPropertyPage)

ConversionOptionsDlg::ConversionOptionsDlg()
	: CPropertyPage(ConversionOptionsDlg::IDD)
{
	//{{AFX_DATA_INIT(ConversionOptionsDlg)
	m_getMethodPrefix = _T("get");
	m_setMethodPrefix = _T("set");
	m_memberPrefix = _T("m_");
	m_singleUnitPerClass = TRUE;
	m_classPrefix = _T("");
	m_implementToString = FALSE;
	m_baseClass = _T("");
	m_useVCF_RTTI = FALSE;
	//}}AFX_DATA_INIT

	m_fileDistributionType = SINGLE_FILE_FOR_TLB;
}


void ConversionOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	if ( m_baseClass != "VCF::Object" ) {
		m_useVCF_RTTI = FALSE;
	}

	//{{AFX_DATA_MAP(ConversionOptionsDlg)
	DDX_CBString(pDX, IDC_GET_METHOD, m_getMethodPrefix);
	DDX_CBString(pDX, IDC_SET_METHOD, m_setMethodPrefix);
	DDX_CBString(pDX, IDC_MEMBER_PREFIXS, m_memberPrefix);
	DDX_Check(pDX, IDC_SINGLE_UNIT_PER_CLASS, m_singleUnitPerClass);
	DDX_Text(pDX, IDC_CLASS_PREFIX, m_classPrefix);
	DDX_Check(pDX, IDC_TO_STRING, m_implementToString);
	DDX_CBString(pDX, IDC_BASE_CLASS, m_baseClass);
	DDX_Check(pDX, IDC_USE_VCF_RTTI, m_useVCF_RTTI);
	//}}AFX_DATA_MAP


	if ( pDX->m_bSaveAndValidate ) {
		CButton* btn = (CButton*)this->GetDlgItem( IDC_ENTIRE_TYPELIB );
		m_fileDistributionType = IGNORE_SINGLE_FILE;
		if ( btn->GetCheck() ) {
			m_fileDistributionType = SINGLE_FILE_FOR_TLB;
		}
		btn = (CButton*)this->GetDlgItem( IDC_INTERFACE_COCLASS );
		if ( btn->GetCheck() ) {			
			m_fileDistributionType = SINGLE_FILE_FOR_IFACE_COCLASS;
		}

		btn = (CButton*)this->GetDlgItem( IDC_INTERFACE_TYPEDEF_COCLASS );
		if ( btn->GetCheck() ) {			
			m_fileDistributionType = SINGLE_FILE_FOR_IFACE_TYPEDEF_COCLASS;
		}
		CWnd* wnd = this->GetDlgItem( IDC_USE_VCF_RTTI );
		wnd->EnableWindow( this->m_baseClass == "VCF::Object" );
		
	}
	else {
		CWnd* wnd = this->GetDlgItem( IDC_SINGLE_FILE_CHOICE );
		wnd->EnableWindow( !m_singleUnitPerClass );
		
		wnd = this->GetDlgItem( IDC_ENTIRE_TYPELIB );
		wnd->EnableWindow( !m_singleUnitPerClass );
		
		wnd = this->GetDlgItem( IDC_INTERFACE_COCLASS );
		wnd->EnableWindow( !m_singleUnitPerClass );

		wnd = this->GetDlgItem( IDC_INTERFACE_TYPEDEF_COCLASS );
		wnd->EnableWindow( !m_singleUnitPerClass );

		switch ( this->m_fileDistributionType ) {
			case SINGLE_FILE_FOR_TLB : {
				CButton* btn = (CButton*)this->GetDlgItem( IDC_ENTIRE_TYPELIB );
				btn->SetCheck( 1 );
			}
			break;

			case SINGLE_FILE_FOR_IFACE_COCLASS : {
				CButton* btn = (CButton*)this->GetDlgItem( IDC_INTERFACE_COCLASS );
				btn->SetCheck( 1 );
			}
			break;

			case SINGLE_FILE_FOR_IFACE_TYPEDEF_COCLASS : {
				CButton* btn = (CButton*)this->GetDlgItem( IDC_INTERFACE_TYPEDEF_COCLASS );
				btn->SetCheck( 1 );
			}
			break;
		}

		wnd = this->GetDlgItem( IDC_USE_VCF_RTTI );
		wnd->EnableWindow( this->m_baseClass == "VCF::Object" );
	}
}


BEGIN_MESSAGE_MAP(ConversionOptionsDlg, CPropertyPage)
	//{{AFX_MSG_MAP(ConversionOptionsDlg)
	ON_BN_CLICKED(IDC_SINGLE_UNIT_PER_CLASS, OnSingleUnitPerClass)
	ON_CBN_EDITCHANGE(IDC_BASE_CLASS, OnEditchangeBaseClass)
	ON_BN_CLICKED(IDC_USE_VCF_RTTI, OnUseVcfRtti)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ConversionOptionsDlg message handlers

BOOL ConversionOptionsDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ConversionOptionsDlg::OnSingleUnitPerClass() 
{
	this->m_singleUnitPerClass = !m_singleUnitPerClass;
	//this->UpdateData();
	this->UpdateData( FALSE );
}

void ConversionOptionsDlg::OnEditchangeBaseClass() 
{	
	UpdateData(TRUE);
}

void ConversionOptionsDlg::OnUseVcfRtti() 
{
	this->m_useVCF_RTTI = !m_useVCF_RTTI;

	UpdateData(FALSE);
}
