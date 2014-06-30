#if !defined(AFX_FILEEXTSDLG_H__52CB1639_9B50_4692_B68D_559D61233CE5__INCLUDED_)
#define AFX_FILEEXTSDLG_H__52CB1639_9B50_4692_B68D_559D61233CE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileExtsDlg.h : header file
//

#include <vector>


/////////////////////////////////////////////////////////////////////////////
// FileExtsDlg dialog

class FileExtsDlg : public CDialog
{
// Construction
public:
	FileExtsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(FileExtsDlg)
	enum { IDD = IDD_FILES_EXTS };
	CListCtrl	m_fileExtsList;
	//}}AFX_DATA

	std::vector<CString> m_extsList;
	std::vector<CString> m_extDescsList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FileExtsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FileExtsDlg)
	afx_msg void OnAddFileExts();
	afx_msg void OnEditFileExts();
	afx_msg void OnDeleteFileExts();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkFileExtsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownFileExtsList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEEXTSDLG_H__52CB1639_9B50_4692_B68D_559D61233CE5__INCLUDED_)
