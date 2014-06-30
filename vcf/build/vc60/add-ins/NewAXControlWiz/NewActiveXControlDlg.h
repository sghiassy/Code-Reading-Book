#if !defined(AFX_NEWACTIVEXCONTROLDLG_H__9DDB534C_6D82_4E68_A7FF_3036F8DADD9A__INCLUDED_)
#define AFX_NEWACTIVEXCONTROLDLG_H__9DDB534C_6D82_4E68_A7FF_3036F8DADD9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewActiveXControlDlg.h : header file
//

#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
// NewActiveXControlDlg dialog

class NewActiveXControlDlg : public CDialog
{
// Construction
public:
	NewActiveXControlDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(NewActiveXControlDlg)
	enum { IDD = IDD_NEW_ACTIVEX_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NewActiveXControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(NewActiveXControlDlg)
	afx_msg void OnSelectControl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWACTIVEXCONTROLDLG_H__9DDB534C_6D82_4E68_A7FF_3036F8DADD9A__INCLUDED_)
