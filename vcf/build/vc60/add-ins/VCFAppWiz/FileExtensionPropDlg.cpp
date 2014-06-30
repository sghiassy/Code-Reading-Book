// FileExtensionPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcfwizard.h"
#include "FileExtensionPropDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FileExtensionPropDlg dialog


FileExtensionPropDlg::FileExtensionPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FileExtensionPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(FileExtensionPropDlg)
	m_extension = _T("");
	m_extensionDescription = _T("");
	//}}AFX_DATA_INIT
}


void FileExtensionPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FileExtensionPropDlg)
	DDX_Text(pDX, IDC_EXTENSION, m_extension);
	DDX_Text(pDX, IDC_EXTENSION_DESC, m_extensionDescription);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FileExtensionPropDlg, CDialog)
	//{{AFX_MSG_MAP(FileExtensionPropDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FileExtensionPropDlg message handlers
