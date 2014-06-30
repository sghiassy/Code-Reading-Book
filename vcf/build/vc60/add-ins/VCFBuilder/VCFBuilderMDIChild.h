#if !defined(AFX_VCFBUILDERMDICHILD_H__52F6C50A_5BFD_41B1_BE7B_32C9ED4CAA9E__INCLUDED_)
#define AFX_VCFBUILDERMDICHILD_H__52F6C50A_5BFD_41B1_BE7B_32C9ED4CAA9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VCFBuilderMDIChild.h : header file
//



class VCFBuilderMDIChildWnd 
{
public:
	static LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	static TCHAR* WndClassName;

	static void RegisterClass( HINSTANCE hInstance );
};


/////////////////////////////////////////////////////////////////////////////
// VCFBuilderMDIChild frame

class VCFBuilderMDIChild : public CMDIChildWnd
{
	DECLARE_DYNCREATE(VCFBuilderMDIChild)
protected:
	VCFBuilderMDIChild();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	void newProject();
	static VCFBuilderMDIChild* globalVCFBuilderMDIChildWnd;
	static HICON globalVCFBuilderIcon;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VCFBuilderMDIChild)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~VCFBuilderMDIChild();

	// Generated message map functions
	//{{AFX_MSG(VCFBuilderMDIChild)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCFBUILDERMDICHILD_H__52F6C50A_5BFD_41B1_BE7B_32C9ED4CAA9E__INCLUDED_)
