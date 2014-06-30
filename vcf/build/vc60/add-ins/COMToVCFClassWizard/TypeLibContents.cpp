// TypeLibContents.cpp : implementation file
//

#include "stdafx.h"
#include "Test.h"
#include "TypeLibContents.h"
#include "TypeLibTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TypeLibContents dialog


TypeLibContents::TypeLibContents(CWnd* pParent /*=NULL*/)
	: CDialog(TypeLibContents::IDD, pParent)
{
	//{{AFX_DATA_INIT(TypeLibContents)
	m_typeLibLabel = _T("");
	m_convertCoClasses = TRUE;
	m_useATLForCoClassImpl = FALSE;
	m_useTLBAsNamespace = TRUE;
	//}}AFX_DATA_INIT
	m_pTypeLib = NULL;
	m_pTypeLibView = NULL;
}


void TypeLibContents::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TypeLibContents)	
	DDX_Text(pDX, IDC_TYPELIB_LABEL, m_typeLibLabel);
	DDX_Check(pDX, IDC_CONVERT_COCLASSES, m_convertCoClasses);
	DDX_Check(pDX, IDC_USE_ATL_FOR_COCLASS_IMPL, m_useATLForCoClassImpl);
	DDX_Check(pDX, IDC_USE_TLB_AS_NAMESPACE, m_useTLBAsNamespace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TypeLibContents, CDialog)
	//{{AFX_MSG_MAP(TypeLibContents)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TypeLibContents message handlers

BOOL TypeLibContents::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ASSERT( m_pTypeLib );
	m_pTypeLibView = new CTypeLibTreeView( m_pTypeLib );
	CRect rect;
	CWnd* wnd = this->GetDlgItem( IDC_TYPELIB_CONTENT_HOLDER );
	wnd->GetWindowRect( rect );
	wnd->DestroyWindow();
	this->ScreenToClient( rect );
	m_pTypeLibView->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rect, this, IDC_TYPELIB_CONTENT_HOLDER );
	m_pTypeLibView->OnInitialUpdate();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void TypeLibContents::OnOK() 
{
	
	CDialog::OnOK();
}
