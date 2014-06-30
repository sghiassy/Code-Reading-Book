#if !defined(AFX_VCFCONSOLEWIZAW_H__475A8F80_32E8_4C36_8768_ED296FE05249__INCLUDED_)
#define AFX_VCFCONSOLEWIZAW_H__475A8F80_32E8_4C36_8768_ED296FE05249__INCLUDED_

// VCFConsoleWizaw.h : header file
//

class CDialogChooser;

// All function calls made by mfcapwz.dll to this custom AppWizard (except for
//  GetCustomAppWizClass-- see VCFConsoleWiz.cpp) are through this class.  You may
//  choose to override more of the CCustomAppWiz virtual functions here to
//  further specialize the behavior of this custom AppWizard.
class CVCFConsoleWizAppWiz : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);
		
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
	virtual void CustomizeProject(IBuildProject* pProject);

	CDialogChooser* m_pChooser;

};

// This declares the one instance of the CVCFConsoleWizAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global VCFConsoleWizaw.  (Its definition is in VCFConsoleWizaw.cpp.)
extern CVCFConsoleWizAppWiz VCFConsoleWizaw;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCFCONSOLEWIZAW_H__475A8F80_32E8_4C36_8768_ED296FE05249__INCLUDED_)
