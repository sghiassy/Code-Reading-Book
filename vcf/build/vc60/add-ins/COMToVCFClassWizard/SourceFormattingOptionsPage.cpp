// SourceFormattingOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "comtovcfclasswizard.h"
#include "SourceFormattingOptionsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SourceFormattingOptionsPage property page

IMPLEMENT_DYNCREATE(SourceFormattingOptionsPage, CPropertyPage)

SourceFormattingOptionsPage::SourceFormattingOptionsPage() : CPropertyPage(SourceFormattingOptionsPage::IDD)
{
	//{{AFX_DATA_INIT(SourceFormattingOptionsPage)
	m_commentsLabel = _T("//\n//comments...\n//");
	m_useFunctionComments = FALSE;
	m_functionComments = _T("/**\n*\n*/\nvoid fooFunction();");
	m_functionLabel = _T("void fooFunction();");
	m_useCPPComments = FALSE;
	m_useLowerCaseFuncNames = TRUE;
	m_javaDocCommentsLabel = _T("/**\n*@param int -\n*@return bool -\n*/\nbool fooFunction( int i );");
	m_useJavaDocParamComments = FALSE;
	//}}AFX_DATA_INIT
}

SourceFormattingOptionsPage::~SourceFormattingOptionsPage()
{
}

void SourceFormattingOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SourceFormattingOptionsPage)
	DDX_Text(pDX, IDC_COMMENTS_LABEL, m_commentsLabel);
	DDX_Check(pDX, IDC_FUNCTION_COMMENTS, m_useFunctionComments);
	DDX_Text(pDX, IDC_FUNCTION_COMMENTS_LABEL, m_functionComments);
	DDX_Text(pDX, IDC_FUNCTION_LABEL, m_functionLabel);
	DDX_Check(pDX, IDC_USE_CPP_COMMENTS, m_useCPPComments);
	DDX_Check(pDX, IDC_USE_LOWER_CASE_FUNC_NAMES, m_useLowerCaseFuncNames);
	DDX_Text(pDX, IDC_JAVADOC_COMMENTS_LABEL, m_javaDocCommentsLabel);
	DDX_Check(pDX, IDC_USE_JAVADOC_PARAM_COMMENTS, m_useJavaDocParamComments);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SourceFormattingOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(SourceFormattingOptionsPage)
	ON_BN_CLICKED(IDC_USE_LOWER_CASE_FUNC_NAMES, OnUseLowerCaseFuncNames)
	ON_BN_CLICKED(IDC_FUNCTION_COMMENTS, OnFunctionComments)
	ON_BN_CLICKED(IDC_USE_CPP_COMMENTS, OnUseCppComments)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_USE_JAVADOC_PARAM_COMMENTS, OnUseJavadocParamComments)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SourceFormattingOptionsPage message handlers

void SourceFormattingOptionsPage::OnUseLowerCaseFuncNames() 
{
	m_useLowerCaseFuncNames = !m_useLowerCaseFuncNames;
	if ( TRUE == m_useLowerCaseFuncNames ) {
		m_functionLabel = _T("void fooFunction();");
	}
	else {
		m_functionLabel = _T("void FooFunction();");
	}

	UpdateData( FALSE );
}

void SourceFormattingOptionsPage::OnFunctionComments() 
{
	m_useFunctionComments = !m_useFunctionComments;
	if ( TRUE == m_useFunctionComments ) {

		m_functionComments = _T("/**\n*\n*/\nvoid fooFunction();");
	}
	else {
		m_functionComments = _T("void fooFunction();");
	}

	UpdateData( FALSE );
}

void SourceFormattingOptionsPage::OnUseCppComments() 
{
	m_useCPPComments = !m_useCPPComments;
	if ( TRUE == m_useCPPComments ) {

		m_commentsLabel = _T("//\n//comments...\n//");
	}
	else {
		m_commentsLabel = _T("/**\n*comments...\n*/");
	}

	UpdateData( FALSE );
}

HBRUSH SourceFormattingOptionsPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = NULL;
	if ( (IDC_JAVADOC_COMMENTS_LABEL == pWnd->GetDlgCtrlID()) || (IDC_FUNCTION_LABEL == pWnd->GetDlgCtrlID()) || (IDC_FUNCTION_COMMENTS_LABEL == pWnd->GetDlgCtrlID()) || (IDC_COMMENTS_LABEL == pWnd->GetDlgCtrlID()) ) {
		hbr = ::CreateSolidBrush( ::GetSysColor( COLOR_WINDOW ) );
		pDC->SetBkMode( TRANSPARENT );
	}
	else {
		hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	}
	
			
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL SourceFormattingOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont font;
	font.CreatePointFont( 100, "Courier New" );
	CWnd* pWnd = this->GetDlgItem( IDC_FUNCTION_LABEL );
	pWnd->SetFont( &font );
	font.Detach();

	font.CreatePointFont( 100, "Courier New" );
	pWnd = this->GetDlgItem( IDC_FUNCTION_COMMENTS_LABEL );
	pWnd->SetFont( &font );
	font.Detach();

	font.CreatePointFont( 100, "Courier New" );
	pWnd = this->GetDlgItem( IDC_COMMENTS_LABEL );
	pWnd->SetFont( &font );
	font.Detach();
	
	font.CreatePointFont( 100, "Courier New" );
	pWnd = this->GetDlgItem( IDC_JAVADOC_COMMENTS_LABEL );
	pWnd->SetFont( &font );
	font.Detach();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SourceFormattingOptionsPage::OnUseJavadocParamComments() 
{
	m_useJavaDocParamComments = !m_useJavaDocParamComments;
	if ( TRUE == m_useJavaDocParamComments ) {

		m_javaDocCommentsLabel = _T("/**\n*@param int -\n*@return bool -\n*/\nbool fooFunction( int i );");
	}
	else {
		m_javaDocCommentsLabel = _T("bool fooFunction( int i );");
	}

	UpdateData( FALSE );	
}
