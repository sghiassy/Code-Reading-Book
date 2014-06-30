#if !defined(AFX_NEWLISTENERDLG_H__BF973D0D_DFFA_49C1_8FA2_9627B4A7DE4D__INCLUDED_)
#define AFX_NEWLISTENERDLG_H__BF973D0D_DFFA_49C1_8FA2_9627B4A7DE4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewListenerDlg.h : header file
//

#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
// NewListenerDlg dialog

class NewListenerDlg : public CDialog
{
// Construction
public:
	NewListenerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(NewListenerDlg)
	enum { IDD = IDD_NEW_LISTENER };
	CListCtrl	m_listenerMethodList;
	CListCtrl	m_eventTypeList;
	CString	m_listenerClassName;
	CString	m_eventClassName;
	//}}AFX_DATA

	CString m_listenerClassDecl;
	CString m_eventClassDecl;
	CString m_eventClassImpl;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NewListenerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(NewListenerDlg)
	afx_msg void OnAddListenerMeth();
	afx_msg void OnAddEventType();
	afx_msg void OnDeleteEventType();
	afx_msg void OnEdtEventType();
	afx_msg void OnDeleteListenerMeth();
	afx_msg void OnEditListenerMeth();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWLISTENERDLG_H__BF973D0D_DFFA_49C1_8FA2_9627B4A7DE4D__INCLUDED_)
