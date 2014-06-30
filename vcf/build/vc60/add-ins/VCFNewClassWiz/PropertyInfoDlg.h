#if !defined(AFX_PROPERTYINFODLG_H__C610C432_68FE_4FA8_B637_E283E68EB440__INCLUDED_)
#define AFX_PROPERTYINFODLG_H__C610C432_68FE_4FA8_B637_E283E68EB440__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyInfoDlg.h : header file
//

#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
// PropertyInfoDlg dialog

class PropertyInfoDlg : public CDialog
{
// Construction
public:
	PropertyInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PropertyInfoDlg)
	enum { IDD = IDD_PROPERTY };
	CComboBox	m_propTypeList;
	CString	m_propName;
	BOOL	m_readOnlyProp;
	CString		m_propType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropertyInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropertyInfoDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYINFODLG_H__C610C432_68FE_4FA8_B637_E283E68EB440__INCLUDED_)
