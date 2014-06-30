#if !defined(AFX_TYPECONVERSIONOPTIONSPAGE_H__70A536DA_92AA_4717_A900_67EBDE05F613__INCLUDED_)
#define AFX_TYPECONVERSIONOPTIONSPAGE_H__70A536DA_92AA_4717_A900_67EBDE05F613__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TypeConversionOptionsPage.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// TypeConversionOptionsPage dialog

class TypeConversionOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(TypeConversionOptionsPage)

// Construction
public:
	TypeConversionOptionsPage();
	~TypeConversionOptionsPage();

// Dialog Data
	//{{AFX_DATA(TypeConversionOptionsPage)
	enum { IDD = IDD_TYPE_OPTIONS };
	CListCtrl	m_typeList;
	BOOL	m_constRefVariables;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(TypeConversionOptionsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(TypeConversionOptionsPage)
	afx_msg void OnDblclkTypelist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnEditType();
	afx_msg void OnConstVariables();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TYPECONVERSIONOPTIONSPAGE_H__70A536DA_92AA_4717_A900_67EBDE05F613__INCLUDED_)
