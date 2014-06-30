// VCFBuilderProject.cpp : implementation file
//

#include "stdafx.h"
#include "test.h"
#include "VCFBuilderProject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVCFBuilderProject

IMPLEMENT_DYNCREATE(CVCFBuilderProject, CDocument)

CVCFBuilderProject::CVCFBuilderProject()
{
}

BOOL CVCFBuilderProject::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CVCFBuilderProject::~CVCFBuilderProject()
{
}


BEGIN_MESSAGE_MAP(CVCFBuilderProject, CDocument)
	//{{AFX_MSG_MAP(CVCFBuilderProject)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVCFBuilderProject diagnostics

#ifdef _DEBUG
void CVCFBuilderProject::AssertValid() const
{
	CDocument::AssertValid();
}

void CVCFBuilderProject::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVCFBuilderProject serialization

void CVCFBuilderProject::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVCFBuilderProject commands
