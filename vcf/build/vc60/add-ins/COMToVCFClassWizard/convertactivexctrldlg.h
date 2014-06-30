#if !defined(AFX_CONVEDRTACTIVEXCTRLDLG_H__52335416_5931_4F85_9BD8_227B11801C7A__INCLUDED_)
#define AFX_CONVEDRTACTIVEXCTRLDLG_H__52335416_5931_4F85_9BD8_227B11801C7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConvedrtActiveXCtrlDlg.h : header file

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// ConvertActiveXCtrlDlg dialog

class ConvertActiveXCtrlDlg : public CDialog
{
// Construction
public:
	void FindAllActiveXControls();
	ConvertActiveXCtrlDlg(CWnd* pParent = NULL);   // standard constructor

	CLSID m_selectedAxCtrlCLSID;
// Dialog Data
	//{{AFX_DATA(ConvertActiveXCtrlDlg)
	enum { IDD = IDD_CONVERT_AX_CTRL };
	CListCtrl	m_activeXCtrlsList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ConvertActiveXCtrlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ConvertActiveXCtrlDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CImageList m_imageList;	
	BOOL HasServer32( HKEY hkCLSID );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONVEDRTACTIVEXCTRLDLG_H__52335416_5931_4F85_9BD8_227B11801C7A__INCLUDED_)
