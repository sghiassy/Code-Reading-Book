// PostHeaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcfnewclasswiz.h"
#include "PostHeaderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PostHeaderDlg dialog


PostHeaderDlg::PostHeaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PostHeaderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PostHeaderDlg)
	m_postHeaderText = _T("");
	//}}AFX_DATA_INIT
}


void PostHeaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PostHeaderDlg)
	DDX_Text(pDX, IDC_POST_HDR_TEXT, m_postHeaderText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PostHeaderDlg, CDialog)
	//{{AFX_MSG_MAP(PostHeaderDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PostHeaderDlg message handlers
