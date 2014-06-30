#if !defined(AFX_SUBCLASSWND_H__8B612D75_6FA5_4C58_B73F_78C023CD52CE__INCLUDED_)
#define AFX_SUBCLASSWND_H__8B612D75_6FA5_4C58_B73F_78C023CD52CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SubClassWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSubClassWnd window

class CSubClassWnd : public CWnd
{
// Construction
public:
	CSubClassWnd(HWND hWndSubclass=NULL);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubClassWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	void DoUnsubclass();
	void DoSubclass(HWND hWndSubclass=NULL);
	virtual ~CSubClassWnd();

	// Generated message map functions
protected:
	bool m_bIsSubclassed;
	HWND m_Saved_hWnd;
	//{{AFX_MSG(CSubClassWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBCLASSWND_H__8B612D75_6FA5_4C58_B73F_78C023CD52CE__INCLUDED_)
