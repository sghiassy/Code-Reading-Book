#if !defined(AFX_VCFBUILDERPROJECT_H__49118D47_07CD_45A8_A225_C1BA8DC643A4__INCLUDED_)
#define AFX_VCFBUILDERPROJECT_H__49118D47_07CD_45A8_A225_C1BA8DC643A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VCFBuilderProject.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVCFBuilderProject document

class CVCFBuilderProject : public CDocument
{
protected:
	CVCFBuilderProject();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CVCFBuilderProject)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCFBuilderProject)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVCFBuilderProject();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CVCFBuilderProject)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCFBUILDERPROJECT_H__49118D47_07CD_45A8_A225_C1BA8DC643A4__INCLUDED_)
