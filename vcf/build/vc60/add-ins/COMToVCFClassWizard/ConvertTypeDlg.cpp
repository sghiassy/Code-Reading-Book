// ConvertTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "comtovcfclasswizard.h"
#include "ConvertTypeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ConvertTypeDlg dialog


ConvertTypeDlg::ConvertTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ConvertTypeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ConvertTypeDlg)
	m_type = _T("");
	m_typeToConvertTo = _T("");
	//}}AFX_DATA_INIT
}


void ConvertTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ConvertTypeDlg)
	DDX_Text(pDX, IDC_TYPE_EDT, m_type);
	DDX_CBString(pDX, IDC_TYPE_TO_CONVERT_TO, m_typeToConvertTo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ConvertTypeDlg, CDialog)
	//{{AFX_MSG_MAP(ConvertTypeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ConvertTypeDlg message handlers
