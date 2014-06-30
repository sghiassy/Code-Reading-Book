#if !defined(AFX_CONVERTTYPEDLG_H__0CB13278_AA52_4368_B3DA_8A5C2DFA4A19__INCLUDED_)
#define AFX_CONVERTTYPEDLG_H__0CB13278_AA52_4368_B3DA_8A5C2DFA4A19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConvertTypeDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// ConvertTypeDlg dialog

class ConvertTypeDlg : public CDialog
{
// Construction
public:
	ConvertTypeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ConvertTypeDlg)
	enum { IDD = IDD_CONVERT_TYPE };
	CString	m_type;
	CString	m_typeToConvertTo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ConvertTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ConvertTypeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONVERTTYPEDLG_H__0CB13278_AA52_4368_B3DA_8A5C2DFA4A19__INCLUDED_)
