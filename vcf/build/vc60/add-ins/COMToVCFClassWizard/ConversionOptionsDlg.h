#if !defined(AFX_CONVERSIONOPTIONSDLG_H__6ACA6120_D8D3_4818_99E0_9B31A68D1007__INCLUDED_)
#define AFX_CONVERSIONOPTIONSDLG_H__6ACA6120_D8D3_4818_99E0_9B31A68D1007__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConversionOptionsDlg.h : header file
//

#define IGNORE_SINGLE_FILE							-1
#define SINGLE_FILE_FOR_TLB							0
#define SINGLE_FILE_FOR_IFACE_COCLASS				1
#define SINGLE_FILE_FOR_IFACE_TYPEDEF_COCLASS		2


/////////////////////////////////////////////////////////////////////////////
// ConversionOptionsDlg dialog

class ConversionOptionsDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(ConversionOptionsDlg)
// Construction
public:
	ConversionOptionsDlg();   // standard constructor

// Dialog Data
	//{{AFX_DATA(ConversionOptionsDlg)
	enum { IDD = IDD_OPTIONS };
	CString	m_getMethodPrefix;
	CString	m_setMethodPrefix;
	CString	m_memberPrefix;
	BOOL	m_singleUnitPerClass;
	CString	m_classPrefix;
	BOOL	m_implementToString;
	CString	m_baseClass;
	BOOL	m_useVCF_RTTI;
	//}}AFX_DATA

	int m_fileDistributionType;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ConversionOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ConversionOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSingleUnitPerClass();
	afx_msg void OnEditchangeBaseClass();
	afx_msg void OnUseVcfRtti();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONVERSIONOPTIONSDLG_H__6ACA6120_D8D3_4818_99E0_9B31A68D1007__INCLUDED_)
