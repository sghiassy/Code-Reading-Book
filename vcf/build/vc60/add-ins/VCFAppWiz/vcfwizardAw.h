#if !defined(AFX_VCFWIZARDAW_H__CCF12C33_AB5F_4502_99E6_E60EEE704867__INCLUDED_)
#define AFX_VCFWIZARDAW_H__CCF12C33_AB5F_4502_99E6_E60EEE704867__INCLUDED_

// vcfwizardaw.h : header file
//

class CDialogChooser;

// All function calls made by mfcapwz.dll to this custom AppWizard (except for
//  GetCustomAppWizClass-- see vcfwizard.cpp) are through this class.  You may
//  choose to override more of the CCustomAppWiz virtual functions here to
//  further specialize the behavior of this custom AppWizard.
class CVcfwizardAppWiz : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
		
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
	virtual void CustomizeProject(IBuildProject* pProject);
	virtual CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);

	virtual LPCTSTR LoadTemplate( LPCTSTR lpszTemplateName, DWORD& rdwSize, HINSTANCE hInstance = NULL );

	CDialogChooser *m_pChooser;
};

// This declares the one instance of the CVcfwizardAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global Vcfwizardaw.  (Its definition is in vcfwizardaw.cpp.)
extern CVcfwizardAppWiz Vcfwizardaw;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCFWIZARDAW_H__CCF12C33_AB5F_4502_99E6_E60EEE704867__INCLUDED_)
