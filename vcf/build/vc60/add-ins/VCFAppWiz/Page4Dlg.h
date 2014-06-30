#if !defined(AFX_PAGE4DLG_H__E9499B1E_9431_4E7F_97DB_58EA675999E2__INCLUDED_)
#define AFX_PAGE4DLG_H__E9499B1E_9431_4E7F_97DB_58EA675999E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Page4Dlg.h : header file
//


#include <vector>


/////////////////////////////////////////////////////////////////////////////
// Page4Dlg dialog

class Page4Dlg : public CAppWizStepDlg
{
// Construction
public:
	Page4Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Page4Dlg)
	enum { IDD = IDD_PAGE4 };
	CStatic	m_image;
	BOOL	m_useAdvanced;
	BOOL	m_useStdFileMenu;
	BOOL	m_useUndoRedo;
	BOOL	m_useHelpMenu;
	BOOL	m_useSplashScreen;
	//}}AFX_DATA


	std::vector<CString> m_extsList;
	std::vector<CString> m_extDescsList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Page4Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Page4Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnFileExts();
	afx_msg void OnUseAdvanced();
	afx_msg void OnUseStdFileMnu();
	afx_msg void OnUseUndoredo();
	afx_msg void OnHelpMnu();
	afx_msg void OnSplashScr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE4DLG_H__E9499B1E_9431_4E7F_97DB_58EA675999E2__INCLUDED_)
