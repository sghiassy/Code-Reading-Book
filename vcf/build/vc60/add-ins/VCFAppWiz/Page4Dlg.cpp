// Page4Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcfwizard.h"
#include "Page4Dlg.h"
#include "FileExtsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Page4Dlg dialog


Page4Dlg::Page4Dlg(CWnd* pParent /*=NULL*/)
	: CAppWizStepDlg(Page4Dlg::IDD)
{
	//{{AFX_DATA_INIT(Page4Dlg)
	m_useAdvanced = FALSE;
	m_useStdFileMenu = TRUE;
	m_useUndoRedo = TRUE;
	m_useHelpMenu = TRUE;
	m_useSplashScreen = TRUE;
	//}}AFX_DATA_INIT
}


void Page4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CAppWizStepDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Page4Dlg)
	DDX_Control(pDX, IDC_IMAGE, m_image);
	DDX_Check(pDX, IDC_USE_ADVANCED, m_useAdvanced);
	DDX_Check(pDX, IDC_USE_STD_FILE_MNU, m_useStdFileMenu);
	DDX_Check(pDX, IDC_USE_UNDOREDO, m_useUndoRedo);
	DDX_Check(pDX, IDC_HELP_MNU, m_useHelpMenu);
	DDX_Check(pDX, IDC_SPLASH_SCR, m_useSplashScreen);
	//}}AFX_DATA_MAP

	if ( pDX->m_bSaveAndValidate ) {
		
	}
	else {
	 	CWnd* wnd = this->GetDlgItem(IDC_USE_STD_FILE_MNU);
		wnd->EnableWindow( m_useAdvanced );

		wnd = this->GetDlgItem(IDC_FILE_EXTS);
		wnd->EnableWindow( m_useAdvanced && m_useStdFileMenu );

		wnd = this->GetDlgItem(IDC_USE_UNDOREDO);
		wnd->EnableWindow( m_useAdvanced );

		wnd = this->GetDlgItem(IDC_HELP_MNU);
		wnd->EnableWindow( m_useAdvanced );

		wnd = this->GetDlgItem(IDC_SPLASH_SCR);
		wnd->EnableWindow( m_useAdvanced );
	}
}


BEGIN_MESSAGE_MAP(Page4Dlg, CAppWizStepDlg)
	//{{AFX_MSG_MAP(Page4Dlg)
	ON_BN_CLICKED(IDC_FILE_EXTS, OnFileExts)
	ON_BN_CLICKED(IDC_USE_ADVANCED, OnUseAdvanced)
	ON_BN_CLICKED(IDC_USE_STD_FILE_MNU, OnUseStdFileMnu)
	ON_BN_CLICKED(IDC_USE_UNDOREDO, OnUseUndoredo)
	ON_BN_CLICKED(IDC_HELP_MNU, OnHelpMnu)
	ON_BN_CLICKED(IDC_SPLASH_SCR, OnSplashScr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Page4Dlg message handlers

BOOL Page4Dlg::OnInitDialog() 
{
	CAppWizStepDlg::OnInitDialog();
	
	CBitmap bmp;
	bmp.LoadBitmap( IDB_BITMAP3 );
	this->m_image.SetBitmap( (HBITMAP)bmp.Detach() );	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Page4Dlg::OnFileExts() 
{
	FileExtsDlg dlg;
	if ( IDOK == dlg.DoModal() )	{
		m_extsList.clear();
		m_extDescsList.clear();
		m_extsList = dlg.m_extsList;
		m_extDescsList = dlg.m_extDescsList;
	}
}

void Page4Dlg::OnUseAdvanced() 
{
	m_useAdvanced = !m_useAdvanced;
	UpdateData( FALSE );	
}

void Page4Dlg::OnUseStdFileMnu() 
{
	m_useStdFileMenu = !m_useStdFileMenu;
	UpdateData( FALSE );	
}

void Page4Dlg::OnUseUndoredo() 
{
	m_useUndoRedo = !m_useUndoRedo;
	UpdateData( FALSE );	
}

void Page4Dlg::OnHelpMnu() 
{
	m_useHelpMenu = !m_useHelpMenu;
	UpdateData( FALSE );
}

void Page4Dlg::OnSplashScr() 
{
	m_useSplashScreen = !m_useSplashScreen;
	UpdateData( FALSE );
}
