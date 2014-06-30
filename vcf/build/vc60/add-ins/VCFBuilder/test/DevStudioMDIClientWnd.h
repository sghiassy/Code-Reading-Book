#if !defined(AFX_DEVSTUDIOMDICLIENTWND_H__93C60A6A_32B6_4A27_9A42_375F6297D3D5__INCLUDED_)
#define AFX_DEVSTUDIOMDICLIENTWND_H__93C60A6A_32B6_4A27_9A42_375F6297D3D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DevStudioMDIClientWnd.h : header file
//
#include "SubClassWnd.h"


/////////////////////////////////////////////////////////////////////////////
// CDevStudioMDIClientWnd window

class CDevStudioMDIClientWnd : public CSubClassWnd
{
// Construction
public:
	CDevStudioMDIClientWnd(HWND hWndSubclass);

// Attributes
public:

	static CDevStudioMDIClientWnd* globalDevStudioMDIChildMgr;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDevStudioMDIClientWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDevStudioMDIClientWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDevStudioMDIClientWnd)
	//afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	//}}AFX_MSG
	afx_msg LRESULT OnMDIActivate( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVSTUDIOMDICLIENTWND_H__93C60A6A_32B6_4A27_9A42_375F6297D3D5__INCLUDED_)
