// FileExtsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcfwizard.h"
#include "FileExtsDlg.h"
#include "FileExtensionPropDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FileExtsDlg dialog


FileExtsDlg::FileExtsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FileExtsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(FileExtsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void FileExtsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FileExtsDlg)
	DDX_Control(pDX, IDC_FILE_EXTS_LIST, m_fileExtsList);
	//}}AFX_DATA_MAP

	if ( pDX->m_bSaveAndValidate ) {
		m_extsList.clear();
		m_extDescsList.clear();
		for ( int i=0;i<m_fileExtsList.GetItemCount();i++) {
			m_extsList.push_back( m_fileExtsList.GetItemText( i, 0 ) );
			m_extDescsList.push_back( m_fileExtsList.GetItemText( i, 1 ) );
		}
	}
	else
	{
		m_fileExtsList.DeleteAllItems();
		for ( int i=0;i<m_extsList.size();i++) {
			m_fileExtsList.InsertItem( i, m_extsList[i] );
			m_fileExtsList.SetItemText( i, 1, m_extDescsList[i] );
		}
	}
}


BEGIN_MESSAGE_MAP(FileExtsDlg, CDialog)
	//{{AFX_MSG_MAP(FileExtsDlg)
	ON_BN_CLICKED(IDC_ADD_FILE_EXTS, OnAddFileExts)
	ON_BN_CLICKED(IDC_EDIT_FILE_EXTS, OnEditFileExts)
	ON_BN_CLICKED(IDC_DELETE_FILE_EXTS, OnDeleteFileExts)
	ON_NOTIFY(NM_DBLCLK, IDC_FILE_EXTS_LIST, OnDblclkFileExtsList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_FILE_EXTS_LIST, OnKeydownFileExtsList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FileExtsDlg message handlers

void FileExtsDlg::OnAddFileExts() 
{
	FileExtensionPropDlg dlg;
	
	if ( IDOK == dlg.DoModal() ) {
		m_fileExtsList.InsertItem( m_fileExtsList.GetItemCount(), dlg.m_extension );
		m_fileExtsList.SetItemText( m_fileExtsList.GetItemCount()-1, 1, dlg.m_extensionDescription );

		UpdateData(TRUE);
	}
}

void FileExtsDlg::OnEditFileExts() 
{
	if ( 0 == m_fileExtsList.GetItemCount() ) {
		OnAddFileExts();
	}
	else {
		int idx = m_fileExtsList.GetSelectionMark();
		
		FileExtensionPropDlg dlg;
		dlg.m_extension = m_fileExtsList.GetItemText( idx, 0 );
		dlg.m_extensionDescription = m_fileExtsList.GetItemText( idx, 1 );
		if ( IDOK == dlg.DoModal() ) {
			m_fileExtsList.SetItemText( idx, 0, dlg.m_extension );
			m_fileExtsList.SetItemText( idx, 1, dlg.m_extensionDescription );
			UpdateData(TRUE);
		}
	}
	
}

void FileExtsDlg::OnDeleteFileExts() 
{	
	if ( 0 != m_fileExtsList.GetItemCount() ) {
		int idx = m_fileExtsList.GetSelectionMark();
		m_fileExtsList.DeleteItem( idx );
		UpdateData(TRUE);
	}
}

BOOL FileExtsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect r;
	GetClientRect( r );
	m_fileExtsList.InsertColumn( 0, "Extension", LVCFMT_LEFT, r.Width()/2 );
	m_fileExtsList.InsertColumn( 1, "Description", LVCFMT_LEFT, r.Width()/2 );
	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void FileExtsDlg::OnDblclkFileExtsList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnEditFileExts();
	*pResult = 0;
}

void FileExtsDlg::OnKeydownFileExtsList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	if ( pLVKeyDow->wVKey == VK_DELETE ) {
		OnDeleteFileExts();
	}
		
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

