#if !defined(AFX_DEVSTUDIOMAINWND_H__666A28B0_9F25_4494_861B_A5A3F93985ED__INCLUDED_)
#define AFX_DEVSTUDIOMAINWND_H__666A28B0_9F25_4494_861B_A5A3F93985ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DevStudioMainWnd.h : header file
//

#include "SubClassWnd.h"
class VCFBuilderHostView;

/////////////////////////////////////////////////////////////////////////////
// CDevStudioMainWnd window

class CDevStudioMainWnd : public CSubClassWnd
{
// Construction
public:
	CDevStudioMainWnd(HWND hWndSubclass=NULL);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDevStudioMainWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDevStudioMainWnd();

	HWND GetMDIClientHWND();

	VCFBuilderHostView* GetVCFBuilderHost() {
		return m_vcfBuilderHost;
	}

	void CreateVCFHost();

	void SetVCFHostVisible( bool visible );

	static CDevStudioMainWnd* globalDevStudioMainWnd;
	// Generated message map functions
protected:
	//{{AFX_MSG(CDevStudioMainWnd)	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	VCFBuilderHostView*	m_vcfBuilderHost;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVSTUDIOMAINWND_H__666A28B0_9F25_4494_861B_A5A3F93985ED__INCLUDED_)
