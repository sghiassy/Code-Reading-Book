// TestDlg.h : header file
//

#if !defined(AFX_TESTDLG_H__96D04AA8_7984_44F2_9D04_F5E75D7FF55C__INCLUDED_)
#define AFX_TESTDLG_H__96D04AA8_7984_44F2_9D04_F5E75D7FF55C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class TypeLibEntry{
public:
	TypeLibEntry( GUID classID, WORD wMajorVer, WORD wMinorVer ) {
		m_classID = classID;
		m_wMajorVer = wMajorVer;
		m_wMinorVer = wMinorVer;
	}

	virtual ~TypeLibEntry(){};
	CString m_tlbName;
	GUID m_classID;
	WORD m_wMajorVer;
	WORD m_wMinorVer;
};


class TypeLibHolder;
class CoClassHolder;
class InterfaceHolder;

/////////////////////////////////////////////////////////////////////////////
// TypeLibraryConverterDlg dialog
class TypeLibraryConverterDlg : public CDialog
{
// Construction
public:
	TypeLibraryConverterDlg(CWnd* pParent = NULL);	// standard constructor

	void GenerateImplementation( TypeLibHolder* pTypeLibHolder );
// Dialog Data
	//{{AFX_DATA(TypeLibraryConverterDlg)
	enum { IDD = IDD_TYPELIB_CONVERTER };
	CProgressCtrl	m_conversionProgress;
	CStatic	m_progressLabel;
	CListCtrl	m_typeLibList;
	CString	m_cppDir;
	CString	m_headerDir;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TypeLibraryConverterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	
	void LoadSettingsFromRegistry();

	void CreateRegistrySettings();

	void SaveSettingsToRegistry();

	void UpdateProgress( CString message );

	void InitializeProgress( TypeLibHolder* pTypeLibHolder );

	void FinishProgress();

	void GenerateSingleUnitperClassImpl( TypeLibHolder* pTypeLibHolder );

	CString GenerateCoClassHeader( CoClassHolder* pCoClass, TypeLibHolder* pTypeLibHolder );
	CString GenerateCoClassImpl( CoClassHolder* pCoClass, TypeLibHolder* pTypeLibHolder );

	CString GenerateInterface( InterfaceHolder* pInterface, TypeLibHolder* pTypeLibHolder );

	void GenerateSingleFileForTLB( TypeLibHolder* pTypeLibHolder );

	void GenerateSingleFileForIFaceSeparateCoClasses( TypeLibHolder* pTypeLibHolder );

	void GenerateSingleFileForIFaceEnumsSeparateCoClasses( TypeLibHolder* pTypeLibHolder );
	
	CString GenerateClassRegFunction( TypeLibHolder* pTypeLibHolder );
	
	CString GenerateClassRegFunctionImpl( TypeLibHolder* pTypeLibHolder );

	CString GenerateTLBImpl( TypeLibHolder* pTypeLibHolder );

	CString	m_getMethodPrefix;
	CString	m_setMethodPrefix;
	CString	m_memberPrefix;
	BOOL	m_singleUnitPerClass;
	CString	m_classPrefix;

	BOOL	m_convertCoClasses;
	BOOL	m_useATLForCoClassImpl;
	BOOL	m_useTLBAsNamespace;
	
	int m_fileDistributionType;

	CImageList m_imageList;
	std::vector<TypeLibEntry*> m_tlbEntrys;
	void ExpandTypeLibs();
	// Generated message map functions
	//{{AFX_MSG(TypeLibraryConverterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConvert();
	afx_msg void OnDestroy();
	afx_msg void OnOptions();
	afx_msg void OnBrowseForHdrDir();
	afx_msg void OnBrowseForCppDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTDLG_H__96D04AA8_7984_44F2_9D04_F5E75D7FF55C__INCLUDED_)
