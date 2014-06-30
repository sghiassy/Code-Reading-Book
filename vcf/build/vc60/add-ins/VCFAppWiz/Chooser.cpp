// chooser.cpp : Implements the CDialogChooser class
//

#include "stdafx.h"
#include "vcfwizard.h"
#include "chooser.h"
#include "Page1Dlg.h"
#include "Page2Dlg.h"
#include "Page3Dlg.h"
#include "Page4Dlg.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// On construction, set up internal array with pointers to each step.
CDialogChooser::CDialogChooser()
{
	m_pDlgs[0] = NULL;

	m_pDlgs[1] = new Page1Dlg;	

	m_pDlgs[2] = new Page2Dlg;

	m_pDlgs[3] = new Page3Dlg;

	m_pDlgs[4] = new Page4Dlg;

	m_nCurrDlg = 0;
}
// Remember where the custom steps begin, so we can delete them in
//  the destructor
#define FIRST_CUSTOM_STEP 1
#define LAST_CUSTOM_STEP 1

// The destructor deletes entries in the internal array corresponding to
//  custom steps.
CDialogChooser::~CDialogChooser()
{
	for (int i = FIRST_CUSTOM_STEP; i <= LAST_CUSTOM_STEP; i++)
	{
		ASSERT(m_pDlgs[i] != NULL);
		delete m_pDlgs[i];
	}
}

// Use the internal array to determine the next step.
CAppWizStepDlg* CDialogChooser::Next(CAppWizStepDlg* pDlg)
{
	ASSERT(0 <= m_nCurrDlg && m_nCurrDlg < LAST_DLG);
	ASSERT(pDlg == m_pDlgs[m_nCurrDlg]);

	m_nCurrDlg++;
	return m_pDlgs[m_nCurrDlg];
}

// Use the internal array to determine the previous step.
CAppWizStepDlg* CDialogChooser::Back(CAppWizStepDlg* pDlg)
{
	ASSERT(1 <= m_nCurrDlg && m_nCurrDlg <= LAST_DLG);
	ASSERT(pDlg == m_pDlgs[m_nCurrDlg]);

	m_nCurrDlg--;
	return m_pDlgs[m_nCurrDlg];
}

int CDialogChooser::GetLinkType()
{
	Page2Dlg* dlg = (Page2Dlg*)m_pDlgs[2];
	return dlg->m_LibLinkage;
}

BOOL CDialogChooser::NeedVCFNet()
{
	Page3Dlg* dlg = (Page3Dlg*)m_pDlgs[3];
	return dlg->m_useVCFNet;
}

BOOL CDialogChooser::NeedVCFRemote()
{
	Page3Dlg* dlg = (Page3Dlg*)m_pDlgs[3];
	return dlg->m_useVCFRemote;
}

BOOL CDialogChooser::NeedsOpenGLSupport()
{
	Page3Dlg* dlg = (Page3Dlg*)m_pDlgs[3];
	return dlg->m_useOpenGL;
}

BOOL CDialogChooser::NeedsAdvancedFeatures()
{
	Page4Dlg* dlg = (Page4Dlg*)m_pDlgs[4];
	return dlg->m_useAdvanced;
}

BOOL CDialogChooser::NeedsUndoRedo()
{
	Page4Dlg* dlg = (Page4Dlg*)m_pDlgs[4];
	return dlg->m_useUndoRedo;
}

BOOL CDialogChooser::NeedsStdFileMenu()
{
	Page4Dlg* dlg = (Page4Dlg*)m_pDlgs[4];
	return dlg->m_useStdFileMenu;
}

std::vector<CString>& CDialogChooser::GetFileExts()
{
	Page4Dlg* dlg = (Page4Dlg*)m_pDlgs[4];
	return dlg->m_extsList;
}

std::vector<CString>& CDialogChooser::GetFileExtDecriptions()
{
	Page4Dlg* dlg = (Page4Dlg*)m_pDlgs[4];
	return dlg->m_extDescsList;
}

BOOL CDialogChooser::NeedsHelpMenu()
{
	Page4Dlg* dlg = (Page4Dlg*)m_pDlgs[4];
	return dlg->m_useHelpMenu;
}

BOOL CDialogChooser::NeedsSplashScreen()
{
	Page4Dlg* dlg = (Page4Dlg*)m_pDlgs[4];
	return dlg->m_useSplashScreen;
}