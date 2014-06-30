#if !defined(AFX_PAGE2DLG_H__0DBAB4DC_A1BC_4C22_867C_C299FD923EC3__INCLUDED_)
#define AFX_PAGE2DLG_H__0DBAB4DC_A1BC_4C22_867C_C299FD923EC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Page2Dlg.h : header file
//

#define VCF_STATIC_LINK		0
#define VCF_DLL_LINK		1

/////////////////////////////////////////////////////////////////////////////
// Page2Dlg dialog

class Page2Dlg : public CAppWizStepDlg
{
// Construction
public:
	Page2Dlg();   // standard constructor
	int m_LibLinkage;
// Dialog Data
	//{{AFX_DATA(Page2Dlg)
	enum { IDD = IDD_PAGE2 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Page2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Page2Dlg)
	//afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnDynamicLib();
	afx_msg void OnStaticLib();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE2DLG_H__0DBAB4DC_A1BC_4C22_867C_C299FD923EC3__INCLUDED_)
