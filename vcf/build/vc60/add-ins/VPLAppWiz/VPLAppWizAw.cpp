// VPLAppWizaw.cpp : implementation file
//

#include "stdafx.h"
#include "VPLAppWiz.h"
#include "VPLAppWizaw.h"
#include <comdef.h>
#include <atlbase.h>
#include "Chooser.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is called immediately after the custom AppWizard is loaded.  Initialize
//  the state of the custom AppWizard here.
void CVPLAppWizAppWiz::InitCustomAppWiz()
{
	m_pChooser = new CDialogChooser;

	// There are no steps in this custom AppWizard.
	SetNumberOfSteps(LAST_DLG);

	// Add build step to .hpj if there is one
	m_Dictionary[_T("HELP")] = _T("1");

	// Inform AppWizard that we're making a DLL.
	m_Dictionary[_T("PROJTYPE_DLL")] = _T("1");

	// TODO: Add any other custom AppWizard-wide initialization here.	
}

// This is called just before the custom AppWizard is unloaded.
void CVPLAppWizAppWiz::ExitCustomAppWiz()
{
	ASSERT(m_pChooser != NULL);
	delete m_pChooser;
	m_pChooser = NULL;
}

// This is called when the user clicks "Create..." on the New Project dialog
CAppWizStepDlg* CVPLAppWizAppWiz::Next(CAppWizStepDlg* pDlg)
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
		CString tmp = strRoot;
		tmp.MakeUpper();
		m_Dictionary[_T("ROOT_")] = tmp;
		CString includeFile; 
		includeFile = "\"" + strRoot + ".h\"";
		m_Dictionary[_T("ROOT_INCLUDE")] = includeFile;
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
		
		// Return NULL to indicate there are no more steps.  (In this case, there are
		//  no steps at all.)
	}
	return result;
}

// This is called when the user clicks "Back" on one of the custom
//  AppWizard's steps.
CAppWizStepDlg* CVPLAppWizAppWiz::Back(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return m_pChooser->Back(pDlg);
}

void CVPLAppWizAppWiz::CustomizeProject(IBuildProject* pProject)
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

				pConfig->get_Name( (BSTR*)&configName );

				CString s = configName;
				if ( s.Find( "Release" ) != -1 ){
					t = 2;
				}
				else if ( s.Find( "Debug" ) != -1 ){
					t = 1;
				}

				_variant_t reserved;
				pConfig->AddToolSettings( tool, 0, reserved );
				
				tool = "cl.exe";
				_bstr_t setting = "";				
				CString strRoot;
				
				m_Dictionary.Lookup(_T("root"), strRoot);
				
				_bstr_t rootName = strRoot;

				switch ( t ){
					case debug: {
						setting = "/GR /MDd /DNO_MFC /I$(VCF_INCLUDE)\\core /I$(VCF_INCLUDE)\\exceptions "\
							      "/I$(VCF_INCLUDE)\\dragdrop /I$(VCF_INCLUDE)\\events /I$(VCF_INCLUDE)\\graphics "\
								  "/I$(VCF_INCLUDE)\\implementer /I$(VCF_INCLUDE)\\implementerKit "\
								  "/I$(VCF_INCLUDE)\\utils /I$(VCF_INCLUDE)\\io /I$(VCF_INCLUDE)\\com";
					}
					break;

					case release: {
						setting = "/GR /MD /DNO_MFC /I$(VCF_INCLUDE)\\core /I$(VCF_INCLUDE)\\exceptions "\
							      "/I$(VCF_INCLUDE)\\dragdrop /I$(VCF_INCLUDE)\\events /I$(VCF_INCLUDE)\\graphics "\
								  "/I$(VCF_INCLUDE)\\implementer /I$(VCF_INCLUDE)\\implementerKit "\
								  "/I$(VCF_INCLUDE)\\utils /I$(VCF_INCLUDE)\\io /I$(VCF_INCLUDE)\\com";
					}
					break;
				}

				pConfig->AddToolSettings( tool, setting, reserved );
				
				strRoot.MakeUpper();
				strRoot = "/D" + strRoot + "_EXPORTS";
				setting = strRoot;
				pConfig->AddToolSettings( tool, setting, reserved );
				
				setting = "/YX";
				pConfig->RemoveToolSettings( tool, setting, reserved );

				tool = "link.exe";
				switch ( t ){
					case debug: {
						setting = "FoundationKit_d.lib GraphicsKit_d.lib ApplicationKit_d.lib comctl32.lib rpcrt4.lib opengl32.lib glaux.lib glu32.lib /libpath:$(VCF_LIB)";
						pConfig->AddToolSettings( tool, setting, reserved );
						setting = "/out:\"Debug/" + rootName + ".vpl\"";
						pConfig->RemoveToolSettings( tool, setting, reserved );

						setting = "/out:\"Debug/" + rootName + ".vpl\"";
						pConfig->AddToolSettings( tool, setting, reserved );
					}
					break;

					case release: {
						setting = "FoundationKit.lib GraphicsKit.lib ApplicationKit.lib comctl32.lib rpcrt4.lib opengl32.lib glaux.lib glu32.lib /libpath:$(VCF_LIB)";
						pConfig->AddToolSettings( tool, setting, reserved );
						setting = "/out:\"Release/" + rootName + ".vpl\"";
						pConfig->RemoveToolSettings( tool, setting, reserved );

						setting = "/out:\"Release/" + rootName + ".vpl\"";
						pConfig->AddToolSettings( tool, setting, reserved );
					}
					break;
				}

				//setting = "/out:\"Debug/VPLAppWiz.awx\""; 
				
				
				pConfig->Release();
			}
		}
		pConfigs->Release();
	}
}


// Here we define one instance of the CVPLAppWizAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global VPLAppWizaw.
CVPLAppWizAppWiz VPLAppWizaw;

