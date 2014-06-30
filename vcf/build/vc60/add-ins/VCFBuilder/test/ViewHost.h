#if !defined(AFX_VIEWHOST_H__1FE1CF48_1C87_4FD4_9FA3_F8E65A05D684__INCLUDED_)
#define AFX_VIEWHOST_H__1FE1CF48_1C87_4FD4_9FA3_F8E65A05D684__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewHost.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewHost window

class CViewHost : public CView
{
// Construction
	DECLARE_DYNCREATE(CViewHost)
public:
	CViewHost();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewHost)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	virtual void OnDraw(CDC* pDC);
// Implementation
public:
	virtual ~CViewHost();

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewHost)
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWHOST_H__1FE1CF48_1C87_4FD4_9FA3_F8E65A05D684__INCLUDED_)
