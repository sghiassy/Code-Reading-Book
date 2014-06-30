#if !defined(AFX_PREHEADERDLG_H__35C240F6_416C_47CE_97A5_D7D1659FEBB0__INCLUDED_)
#define AFX_PREHEADERDLG_H__35C240F6_416C_47CE_97A5_D7D1659FEBB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreHeaderDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// PreHeaderDlg dialog

class PreHeaderDlg : public CDialog
{
// Construction
public:
	PreHeaderDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PreHeaderDlg)
	enum { IDD = IDD_PRE_HDR_DLG };
	int		m_preDefinedHdrsSel;
	CString	m_preHdrText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PreHeaderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PreHeaderDlg)
	afx_msg void OnSelchangePredefinedHdrs();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREHEADERDLG_H__35C240F6_416C_47CE_97A5_D7D1659FEBB0__INCLUDED_)
