#if !defined(AFX_VCFBUILDERHOSTVIEW_H__528A8358_7064_4F07_B830_1DA5774F6807__INCLUDED_)
#define AFX_VCFBUILDERHOSTVIEW_H__528A8358_7064_4F07_B830_1DA5774F6807__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VCFBuilderHostView.h : header file
//




/////////////////////////////////////////////////////////////////////////////
// VCFBuilderHostView view

class VCFBuilderHostView : public CView
{
protected:
	
	DECLARE_DYNCREATE(VCFBuilderHostView)

// Attributes
public:
	VCFBuilderHostView();           // protected constructor used by dynamic creation
// Operations
public:
	void newProject();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VCFBuilderHostView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~VCFBuilderHostView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(VCFBuilderHostView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	
	VCFBuilder::MainUIPanel * m_vcfBuilderUIPanel;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCFBUILDERHOSTVIEW_H__528A8358_7064_4F07_B830_1DA5774F6807__INCLUDED_)
