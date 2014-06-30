#if !defined(AFX_POSTHEADERDLG_H__4837F8E4_9C39_4597_92F2_B7C5DBBD5276__INCLUDED_)
#define AFX_POSTHEADERDLG_H__4837F8E4_9C39_4597_92F2_B7C5DBBD5276__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PostHeaderDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// PostHeaderDlg dialog

class PostHeaderDlg : public CDialog
{
// Construction
public:
	PostHeaderDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PostHeaderDlg)
	enum { IDD = IDD_POST_HDR_DLG };
	CString	m_postHeaderText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PostHeaderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PostHeaderDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSTHEADERDLG_H__4837F8E4_9C39_4597_92F2_B7C5DBBD5276__INCLUDED_)
