#if !defined(AFX_VPLAPPWIZAW_H__22CECD32_1D44_459F_867F_87539928EB48__INCLUDED_)
#define AFX_VPLAPPWIZAW_H__22CECD32_1D44_459F_867F_87539928EB48__INCLUDED_

// VPLAppWizaw.h : header file
//

class CDialogChooser;

// All function calls made by mfcapwz.dll to this custom AppWizard (except for
//  GetCustomAppWizClass-- see VPLAppWiz.cpp) are through this class.  You may
//  choose to override more of the CCustomAppWiz virtual functions here to
//  further specialize the behavior of this custom AppWizard.
class CVPLAppWizAppWiz : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	virtual CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);	
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
	virtual void CustomizeProject(IBuildProject* pProject);

	CDialogChooser* m_pChooser;
};

// This declares the one instance of the CVPLAppWizAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global VPLAppWizaw.  (Its definition is in VPLAppWizaw.cpp.)
extern CVPLAppWizAppWiz VPLAppWizaw;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VPLAPPWIZAW_H__22CECD32_1D44_459F_867F_87539928EB48__INCLUDED_)
