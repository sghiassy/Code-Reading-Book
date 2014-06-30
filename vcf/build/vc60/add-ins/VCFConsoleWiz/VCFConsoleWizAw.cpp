// VCFConsoleWizaw.cpp : implementation file
//

#include "stdafx.h"
#include "VCFConsoleWiz.h"
#include "VCFConsoleWizaw.h"
#include <comdef.h>
#include <atlbase.h>
#include "Chooser.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is called immediately after the custom AppWizard is loaded.  Initialize
//  the state of the custom AppWizard here.
void CVCFConsoleWizAppWiz::InitCustomAppWiz()
{
	// There are no steps in this custom AppWizard.
	m_pChooser = new CDialogChooser;

	// Set the maximum number of steps.
	SetNumberOfSteps(LAST_DLG);

	// Add build step to .hpj if there is one
	m_Dictionary[_T("HELP")] = _T("1");

	// TODO: Add any other custom AppWizard-wide initialization here.
}

// This is called just before the custom AppWizard is unloaded.
void CVCFConsoleWizAppWiz::ExitCustomAppWiz()
{
	ASSERT(m_pChooser != NULL);
	delete m_pChooser;
	m_pChooser = NULL;
}

// This is called when the user clicks "Create..." on the New Project dialog
CAppWizStepDlg* CVCFConsoleWizAppWiz::Next(CAppWizStepDlg* pDlg)
{
	CAppWizStepDlg* result = m_pChooser->Next(pDlg);
	
	if ( pDlg == NULL) {	// By default, this custom AppWizard has no steps
		
		// Set template macros based on the project name entered by the user.
		
		// Get value of $$root$$ (already set by AppWizard)
		CString strRoot;
		m_Dictionary.Lookup(_T("root"), strRoot);
		
		// Set value of $$Doc$$, $$DOC$$
		CString strDoc = strRoot.Left(6);
		m_Dictionary[_T("Doc")] = strDoc;
		strDoc.MakeUpper();
		m_Dictionary[_T("DOC")] = strDoc;
		
		// Set value of $$MAC_TYPE$$
		strRoot = strRoot.Left(4);
		int nLen = strRoot.GetLength();
		if (strRoot.GetLength() < 4)
		{
			CString strPad(_T(' '), 4 - nLen);
			strRoot += strPad;
		}
		strRoot.MakeUpper();
		m_Dictionary[_T("MAC_TYPE")] = strRoot;
		
	}
	return result;
}

// This is called when the user clicks "Back" on one of the custom
//  AppWizard's steps.
CAppWizStepDlg* CVCFConsoleWizAppWiz::Back(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return m_pChooser->Back(pDlg);
}


void CVCFConsoleWizAppWiz::CustomizeProject(IBuildProject* pProject)
{
	// TODO: Add code here to customize the project.  If you don't wish
	//  to customize project, you may remove this virtual override.
	
	// This is called immediately after the default Debug and Release
	//  configurations have been created for each platform.  You may customize
	//  existing configurations on this project by using the methods
	//  of IBuildProject and IConfiguration such as AddToolSettings,
	//  RemoveToolSettings, and AddCustomBuildStep. These are documented in
	//  the Developer Studio object model documentation.

	// WARNING!!  IBuildProject and all interfaces you can get from it are OLE
	//  COM interfaces.  You must be careful to release all new interfaces
	//  you acquire.  In accordance with the standard rules of COM, you must
	//  NOT release pProject, unless you explicitly AddRef it, since pProject
	//  is passed as an "in" parameter to this function.  See the documentation
	//  on CCustomAppWiz::CustomizeProject for more information.

	IConfigurations* pConfigs = NULL;
	if ( SUCCEEDED(pProject->get_Configurations( &pConfigs )) ) {
		long count = 0;
		pConfigs->get_Count( &count );
		for (long i=0;i<count;i++){
			IConfiguration* pConfig = NULL;
			long t = i+1;
			_variant_t index = t;
			CComBSTR configName;		
			const int debug = 1;
			const int release = 2;
			if ( SUCCEEDED(pConfigs->Item( index, &pConfig )) ){
				_bstr_t tool = "mfc";
				_variant_t reserved;
				pConfig->AddToolSettings( tool, 0, reserved );
				
				tool = "cl.exe";
				_bstr_t setting = "";				
				
				pConfig->get_Name( (BSTR*)&configName );

				CString s = configName;
				if ( s.Find( "Release" ) != -1 ){
					t = 2;
				}
				else if ( s.Find( "Debug" ) != -1 ){
					t = 1;
				}
				
				switch ( t ){
					case debug: {
						setting = "/GR /MDd /DNO_MFC /I$(VCF_INCLUDE)\\core /I$(VCF_INCLUDE)\\exceptions "\
							      "/I$(VCF_INCLUDE)\\dragdrop /I$(VCF_INCLUDE)\\events "\
								  "/I$(VCF_INCLUDE)\\implementer /I$(VCF_INCLUDE)\\implementerKit "\
								  "/I$(VCF_INCLUDE)\\utils /I$(VCF_INCLUDE)\\io";
					}
					break;

					case release: {
						setting = "/GR /MD /DNO_MFC /I$(VCF_INCLUDE)\\core /I$(VCF_INCLUDE)\\exceptions "\
							      "/I$(VCF_INCLUDE)\\dragdrop /I$(VCF_INCLUDE)\\events "\
								  "/I$(VCF_INCLUDE)\\implementer /I$(VCF_INCLUDE)\\implementerKit "\
								  "/I$(VCF_INCLUDE)\\utils /I$(VCF_INCLUDE)\\io";
					}
					break;
				}

				pConfig->AddToolSettings( tool, setting, reserved );
				
				
				
				setting = "/YX";
				pConfig->RemoveToolSettings( tool, setting, reserved );

				setting = "/D_WINDOWS";
				pConfig->RemoveToolSettings( tool, setting, reserved );

				setting = "/D_CONSOLE";
				pConfig->AddToolSettings( tool, setting, reserved );

				setting = "/DFRAMEWORK_DLL";
				pConfig->AddToolSettings( tool, setting, reserved );

				tool = "link.exe";
				switch ( t ){
					case debug: {
						setting = "FoundationKit_d.lib rpcrt4.lib /libpath:$(VCF_LIB)";
					}
					break;

					case release: {
						setting = "FoundationKit.lib rpcrt4.lib /libpath:$(VCF_LIB)";
					}
					break;
				}

				if ( ! SUCCEEDED( pConfig->AddToolSettings( tool, setting, reserved ) ) ){
					TRACE( "failed to set linker variables\n" );
				}
				
				setting = "/subsystem:windows";
				pConfig->RemoveToolSettings( tool, setting, reserved );

				setting = "/subsystem:console";
				pConfig->AddToolSettings( tool, setting, reserved );

				pConfig->Release();
			}
		}
		pConfigs->Release();
	}
}


// Here we define one instance of the CVCFConsoleWizAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global VCFConsoleWizaw.
CVCFConsoleWizAppWiz VCFConsoleWizaw;

