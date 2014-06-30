#if !defined(AFX_NEWCLASSDLG_H__18CF376C_F843_4E68_9602_4EF5BC012E75__INCLUDED_)
#define AFX_NEWCLASSDLG_H__18CF376C_F843_4E68_9602_4EF5BC012E75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewClassDlg.h : header file
//
#include "resource.h"
#include <map>

class PropInfo {
public:
	CString Name;
	CString Type;
	bool readOnly;
};


/////////////////////////////////////////////////////////////////////////////
// NewClassDlg dialog

class NewClassDlg : public CDialog
{
// Construction
public:
	NewClassDlg(CWnd* pParent = NULL);   // standard constructor

	CString GetClassDecl(){
		
		return m_classDecl;
	}

	CString GetClassImpl(){
		
		return m_classImpl;
	}
// Dialog Data
	//{{AFX_DATA(NewClassDlg)
	enum { IDD = IDD_NEWCLASSDLG };
	CComboBox	m_nameSpace;
	CEdit	m_cppEdit;
	CEdit	m_headerEdit;
	CListCtrl	m_propList;
	CComboBox	m_derivesList;
	CString	m_className;
	CString	m_CPPName;
	CString	m_derivesFrom;
	CString	m_headerName;
	BOOL	m_supportsRTTI;
	BOOL	m_abstractClass;
	BOOL	m_useNameSpace;
	CString	m_nameSpaceText;
	BOOL	m_isClassAnInterface;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NewClassDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(NewClassDlg)
	afx_msg void OnChangeCppBtn();
	afx_msg void OnChangeHdrBtn();
	afx_msg void OnChangeClassName();
	afx_msg void OnSupportsVcfRtti();
	afx_msg void OnItemdblclickPropList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickPropList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPropspopupEdit();
	afx_msg void OnPropspopupRemove();
	afx_msg void OnPropspopupAdd();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdatePropspopupEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePropspopupRemove(CCmdUI* pCmdUI);
	afx_msg void OnEditPreHdr();
	afx_msg void OnEditPostHdr();
	afx_msg void OnUseNamespace();
	afx_msg void OnInterfaceClass();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_classDecl;
	CString m_classImpl;
	CString m_headerDir;
	CString m_cppDir;	
	CString m_preHdrText;
	CString m_postHdrText;

	void fillInPropInfo();
	void fillInPropMethods();
	void fillInPropMethodsImpl();
	void fillInPropMemberVars();
	//std::map<LVITEM, CString> m_propertyMap;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWCLASSDLG_H__18CF376C_F843_4E68_9602_4EF5BC012E75__INCLUDED_)
