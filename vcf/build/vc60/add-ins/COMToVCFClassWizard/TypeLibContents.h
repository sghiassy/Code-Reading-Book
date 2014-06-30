#if !defined(AFX_TYPELIBCONTENTS_H__E6BF1A7A_3C48_4EC2_84F7_6C88A3EAA39B__INCLUDED_)
#define AFX_TYPELIBCONTENTS_H__E6BF1A7A_3C48_4EC2_84F7_6C88A3EAA39B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TypeLibContents.h : header file
//

class CTypeLibTreeView;

/////////////////////////////////////////////////////////////////////////////
// TypeLibContents dialog

class TypeLibContents : public CDialog
{
// Construction
public:
	TypeLibContents(CWnd* pParent = NULL);   // standard constructor

	ITypeLib* m_pTypeLib;
// Dialog Data
	//{{AFX_DATA(TypeLibContents)
	enum { IDD = IDD_TYPELIB_CONTENTS };
	CString	m_typeLibLabel;
	BOOL	m_convertCoClasses;
	BOOL	m_useATLForCoClassImpl;
	BOOL	m_useTLBAsNamespace;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TypeLibContents)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTypeLibTreeView* m_pTypeLibView;
	// Generated message map functions
	//{{AFX_MSG(TypeLibContents)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TYPELIBCONTENTS_H__E6BF1A7A_3C48_4EC2_84F7_6C88A3EAA39B__INCLUDED_)
