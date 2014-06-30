#if !defined(AFX_SOURCEFORMATTINGOPTIONSPAGE_H__A018D5EB_2FEB_4A2F_8BC5_51AF44DF6F86__INCLUDED_)
#define AFX_SOURCEFORMATTINGOPTIONSPAGE_H__A018D5EB_2FEB_4A2F_8BC5_51AF44DF6F86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SourceFormattingOptionsPage.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// SourceFormattingOptionsPage dialog

class SourceFormattingOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(SourceFormattingOptionsPage)

// Construction
public:
	SourceFormattingOptionsPage();
	~SourceFormattingOptionsPage();

// Dialog Data
	//{{AFX_DATA(SourceFormattingOptionsPage)
	enum { IDD = IDD_SOURCE_FORMATTING };
	CString	m_commentsLabel;
	BOOL	m_useFunctionComments;
	CString	m_functionComments;
	CString	m_functionLabel;
	BOOL	m_useCPPComments;
	BOOL	m_useLowerCaseFuncNames;
	CString	m_javaDocCommentsLabel;
	BOOL	m_useJavaDocParamComments;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SourceFormattingOptionsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SourceFormattingOptionsPage)
	afx_msg void OnUseLowerCaseFuncNames();
	afx_msg void OnFunctionComments();
	afx_msg void OnUseCppComments();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnUseJavadocParamComments();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOURCEFORMATTINGOPTIONSPAGE_H__A018D5EB_2FEB_4A2F_8BC5_51AF44DF6F86__INCLUDED_)
