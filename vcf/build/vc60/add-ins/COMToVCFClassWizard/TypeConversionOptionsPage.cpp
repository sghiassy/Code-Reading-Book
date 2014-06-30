// TypeConversionOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "comtovcfclasswizard.h"
#include "TypeConversionOptionsPage.h"
#include "ConvertTypeDlg.h"
#include "TypeLibDump.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TypeConversionOptionsPage property page

IMPLEMENT_DYNCREATE(TypeConversionOptionsPage, CPropertyPage)

TypeConversionOptionsPage::TypeConversionOptionsPage() : CPropertyPage(TypeConversionOptionsPage::IDD)
{
	//{{AFX_DATA_INIT(TypeConversionOptionsPage)
	m_constRefVariables = FALSE;
	//}}AFX_DATA_INIT
}

TypeConversionOptionsPage::~TypeConversionOptionsPage()
{
}

void TypeConversionOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TypeConversionOptionsPage)
	DDX_Control(pDX, IDC_TYPELIST, m_typeList);
	DDX_Check(pDX, IDC_CONST_VARIABLES, m_constRefVariables);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TypeConversionOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(TypeConversionOptionsPage)
	ON_NOTIFY(NM_DBLCLK, IDC_TYPELIST, OnDblclkTypelist)
	ON_BN_CLICKED(IDC_EDIT_TYPE, OnEditType)
	ON_BN_CLICKED(IDC_CONST_VARIABLES, OnConstVariables)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TypeConversionOptionsPage message handlers

void TypeConversionOptionsPage::OnDblclkTypelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnEditType();

	*pResult = 0;
}

BOOL TypeConversionOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CRect rect(0,0,0,0);
	this->GetClientRect( rect );
	int w = rect.Width() / 2;
	m_typeList.InsertColumn( 0, "COM Type", LVCFMT_LEFT, w );
	m_typeList.InsertColumn( 1, "Converted Type", LVCFMT_LEFT, w );

	m_typeList.InsertItem( 0, "VARIANT" );
	m_typeList.SetItemText( 0, 1, "VCF::VariantData" );

	m_typeList.InsertItem( 1, "BSTR" );
	m_typeList.SetItemText( 1, 1, "VCF::String" );

	m_typeList.InsertItem( 2, "IUnknown" );
	m_typeList.SetItemText( 2, 1, "VCF::Object" );

	m_typeList.InsertItem( 3, "IDispatch" );
	m_typeList.SetItemText( 3, 1, "VCF::Object" );

	m_typeList.InsertItem( 4, "HRESULT" );
	m_typeList.SetItemText( 4, 1, "unsigned long" );

	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void TypeConversionOptionsPage::OnEditType() 
{
	int index = this->m_typeList.GetSelectionMark();

	ConvertTypeDlg dlg;
	dlg.m_type = m_typeList.GetItemText( index, 0 );
	dlg.m_typeToConvertTo = m_typeList.GetItemText( index, 1 );
	if ( IDOK == dlg.DoModal() ) {
		m_typeList.SetItemText( index, 1, dlg.m_typeToConvertTo );
	}
}



void TypeConversionOptionsPage::OnConstVariables() 
{
	this->m_constRefVariables = !m_constRefVariables;
	UpdateData();
}
