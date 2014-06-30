#if !defined(AFX_VCFLIBRARYAPPWIZARDAW_H__DF1C6597_CADC_4F0F_B70C_05D52C123E1B__INCLUDED_)
#define AFX_VCFLIBRARYAPPWIZARDAW_H__DF1C6597_CADC_4F0F_B70C_05D52C123E1B__INCLUDED_

// VCFLibraryAppWizardaw.h : header file
//

class CDialogChooser;

// All function calls made by mfcapwz.dll to this custom AppWizard (except for
//  GetCustomAppWizClass-- see VCFLibraryAppWizard.cpp) are through this class.  You may
//  choose to override more of the CCustomAppWiz virtual functions here to
//  further specialize the behavior of this custom AppWizard.
class CVCFLibraryAppWizardAppWiz : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	virtual CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);
		
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
	virtual void CustomizeProject(IBuildProject* pProject);

protected:
	CDialogChooser* m_pChooser;
};

// This declares the one instance of the CVCFLibraryAppWizardAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global VCFLibraryAppWizardaw.  (Its definition is in VCFLibraryAppWizardaw.cpp.)
extern CVCFLibraryAppWizardAppWiz VCFLibraryAppWizardaw;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCFLIBRARYAPPWIZARDAW_H__DF1C6597_CADC_4F0F_B70C_05D52C123E1B__INCLUDED_)
