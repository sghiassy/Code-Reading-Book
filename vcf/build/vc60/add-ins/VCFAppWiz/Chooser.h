#if !defined(AFX_CHOOSER_H__105DAEDE_B66D_4B50_915C_4262F2625593__INCLUDED_)
#define AFX_CHOOSER_H__105DAEDE_B66D_4B50_915C_4262F2625593__INCLUDED_

// chooser.h : declaration of the CDialogChooser class
//             This class keeps track of what dialogs to pop up when.

#define LAST_DLG 4

#include <vector>


class CDialogChooser
{
public:
	CDialogChooser();
	~CDialogChooser();

	// All calls by mfcapwz.dll to CAppWizTest3AppWiz::Next
	//  & CAppWizTest3AppWiz::Back are delegated to these member
	//  functions, which keep track of what dialog is up
	//  now, and what to pop up next.
	CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);
	
	int GetLinkType();

	BOOL NeedVCFNet();

	BOOL NeedVCFRemote();

	BOOL NeedsOpenGLSupport();

	BOOL NeedsAdvancedFeatures();

	BOOL NeedsUndoRedo();

	BOOL NeedsStdFileMenu();

	std::vector<CString>& GetFileExts();
	
	std::vector<CString>& GetFileExtDecriptions();

	BOOL NeedsHelpMenu();

	BOOL NeedsSplashScreen();
protected:
	// Current step's index into the internal array m_pDlgs
	int m_nCurrDlg;

	// Internal array of pointers to the steps
	CAppWizStepDlg* m_pDlgs[LAST_DLG + 1];
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSER_H__105DAEDE_B66D_4B50_915C_4262F2625593__INCLUDED_)
