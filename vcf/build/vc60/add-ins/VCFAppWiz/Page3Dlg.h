#if !defined(AFX_PAGE3DLG_H__DCE13931_EFF5_4158_9CD4_9DE6866C1009__INCLUDED_)
#define AFX_PAGE3DLG_H__DCE13931_EFF5_4158_9CD4_9DE6866C1009__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Page3Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Page3Dlg dialog

class Page3Dlg : public CAppWizStepDlg
{
// Construction
public:
	Page3Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Page3Dlg)
	enum { IDD = IDD_PAGE3 };
	BOOL	m_useVCFNet;
	BOOL	m_useVCFRemote;
	BOOL	m_useOpenGL;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Page3Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Page3Dlg)	
	afx_msg void OnVcfnet();
	afx_msg void OnVcfremote();
	afx_msg void OnOpengl();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE3DLG_H__DCE13931_EFF5_4158_9CD4_9DE6866C1009__INCLUDED_)
