#if !defined(AFX_FILEEXTENSIONPROPDLG_H__34C6927D_D007_436F_B82D_F4967B3CFC43__INCLUDED_)
#define AFX_FILEEXTENSIONPROPDLG_H__34C6927D_D007_436F_B82D_F4967B3CFC43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileExtensionPropDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FileExtensionPropDlg dialog

class FileExtensionPropDlg : public CDialog
{
// Construction
public:
	FileExtensionPropDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(FileExtensionPropDlg)
	enum { IDD = IDD_FILE_EXT_PROPS };
	CString	m_extension;
	CString	m_extensionDescription;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FileExtensionPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FileExtensionPropDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEEXTENSIONPROPDLG_H__34C6927D_D007_436F_B82D_F4967B3CFC43__INCLUDED_)
