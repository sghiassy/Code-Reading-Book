#if !defined(AFX_PAGE1DLG_H__DBEC2D79_1C0E_4B69_A975_F53B591C7FF6__INCLUDED_)
#define AFX_PAGE1DLG_H__DBEC2D79_1C0E_4B69_A975_F53B591C7FF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Page1Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Page1Dlg dialog

class Page1Dlg : public CAppWizStepDlg
{
// Construction
public:
	Page1Dlg();   // standard constructor

	virtual BOOL OnDismiss();
// Dialog Data
	//{{AFX_DATA(Page1Dlg)
	enum { IDD = IDD_PAGE1 };
	CStatic	m_image;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Page1Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Page1Dlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE1DLG_H__DBEC2D79_1C0E_4B69_A975_F53B591C7FF6__INCLUDED_)
