// vcfwizardaw.cpp : implementation file
//

#include "stdafx.h"
#include "vcfwizard.h"
#include "vcfwizardaw.h"
#include <comdef.h>
#include <atlbase.h>
#include "Chooser.h"
#include "Page2Dlg.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is called immediately after the custom AppWizard is loaded.  Initialize
//  the state of the custom AppWizard here.
void CVcfwizardAppWiz::InitCustomAppWiz()
{
	m_pChooser = new CDialogChooser;
	
	SetNumberOfSteps(LAST_DLG);

	// Add build step to .hpj if there is one
	m_Dictionary[_T("HELP")] = _T("1");

	// TODO: Add any other custom AppWizard-wide initialization here.
}

// This is called just before the custom AppWizard is unloaded.
void CVcfwizardAppWiz::ExitCustomAppWiz()
{
	ASSERT(m_pChooser != NULL);
	delete m_pChooser;
	m_pChooser = NULL;
}

// This is called when the user clicks "Create..." on the New Project dialog
CAppWizStepDlg* CVcfwizardAppWiz::Next(CAppWizStepDlg* pDlg)
{
	CAppWizStepDlg* result = m_pChooser->Next(pDlg);	
	
	if ( pDlg == NULL) {	// By default, this custom AppWizard has no steps
		
		// Set template macros based on the project name entered by the user.
		
		// Get value of $$root$$ (already set by AppWizard)
		CString strRoot;
		m_Dictionary.Lookup(_T("root"), strRoot);
		
		m_Dictionary[_T("SplashScreen")] = CString("SplashScreen");

		m_Dictionary[_T("About")] = CString(strRoot+"About");
		
		m_Dictionary[_T("MainWindow")] = CString("MainWindow");
		m_Dictionary[_T("Splash")] = CString("Splash");
		m_Dictionary[_T("Application")] = CString(strRoot+"Application");
		
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
		
		
		// Return NULL to indicate there are no more steps.  (In this case, there are
		//  no steps at all.)
	}
	return result;
}

CAppWizStepDlg* CVcfwizardAppWiz::Back(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return m_pChooser->Back(pDlg);
}

void CVcfwizardAppWiz::CustomizeProject(IBuildProject* pProject)
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
				
				
				
				setting = "/YX";
				pConfig->RemoveToolSettings( tool, setting, reserved );

				setting = "/D_WINDOWS";
				pConfig->RemoveToolSettings( tool, setting, reserved );				
				
				int linkType = this->m_pChooser->GetLinkType();
				if ( linkType == VCF_DLL_LINK ) {//VCF_STATIC_LINK
					setting = "/DFRAMEWORK_DLL";
					pConfig->AddToolSettings( tool, setting, reserved );
					setting = "/DGRAPHICSKIT_DLL";
					pConfig->AddToolSettings( tool, setting, reserved );
					setting = "/DAPPKIT_DLL";
					pConfig->AddToolSettings( tool, setting, reserved );
				}
				tool = "link.exe";
				if ( linkType == VCF_DLL_LINK ) {
					switch ( t ){
						case debug: {
							setting = "ApplicationKit_d.lib GraphicsKit_d.lib FoundationKit_d.lib ";
							if ( m_pChooser->NeedVCFRemote() ) {
								setting += "NetworkKit_d.lib RemoteObjectKit_d.lib ";
							}
							else if ( m_pChooser->NeedVCFNet() ) {
								setting += "NetworkKit_d.lib ";
							}
							setting += "rpcrt4.lib ";
							if ( m_pChooser->NeedsOpenGLSupport() ) {
								setting += "opengl32.lib glu32.lib glaux.lib ";
							}
							setting += "/libpath:$(VCF_LIB)";
						}
						break;

						case release: {
							setting = "ApplicationKit.lib GraphicsKit.lib FoundationKit.lib ";
							if ( m_pChooser->NeedVCFRemote() ) {
								setting += "NetworkKit.lib RemoteObjectKit.lib ";
							}
							else if ( m_pChooser->NeedVCFNet() ) {
								setting += "NetworkKit.lib ";
							}
							setting += "rpcrt4.lib ";
							if ( m_pChooser->NeedsOpenGLSupport() ) {
								setting += "opengl32.lib glu32.lib glaux.lib ";
							}
							setting += "/libpath:$(VCF_LIB)";
						}
						break;
					}
				}
				else if ( linkType == VCF_STATIC_LINK ) {
					switch ( t ){
						case debug: {
							//NetworkKit_sd, NetworkKitDLL_d, NetworkKit_s, NetworkKitDLL, RemoteObjectKit_sd, RemoteObjectKit_s, RemoteObjectKitDLL_d, RemoteObjectKitDLL
							setting = "ApplicationKit_sd.lib GraphicsKit_sd.lib FoundationKit_sd.lib libart_d.lib ";
							if ( m_pChooser->NeedVCFRemote() ) {
								setting += "NetworkKit_sd.lib RemoteObjectKit_sd.lib ";
							}
							else if ( m_pChooser->NeedVCFNet() ) {
								setting += "NetworkKit_sd.lib ";
							}
							setting += "comctl32.lib rpcrt4.lib opengl32.lib glu32.lib glaux.lib /libpath:$(VCF_LIB)";
						}
						break;

						case release: {
							setting = "ApplicationKit_s.lib GraphicsKit_s.lib FoundationKit_s.lib libart.lib ";
							if ( m_pChooser->NeedVCFRemote() ) {
								setting += "NetworkKit_s.lib RemoteObjectKit_s.lib ";
							}
							else if ( m_pChooser->NeedVCFNet() ) {
								setting += "NetworkKit_s.lib ";
							}
							setting += "comctl32.lib rpcrt4.lib opengl32.lib glu32.lib glaux.lib /libpath:$(VCF_LIB)";
						}
						break;
					}
				}
				if ( ! SUCCEEDED( pConfig->AddToolSettings( tool, setting, reserved ) ) ){
					TRACE( "failed to set linker variables\n" );
				}
				
				setting = "/subsystem:windows /entry:\"mainCRTStartup\"";
				pConfig->AddToolSettings( tool, setting, reserved );				

				pConfig->Release();
			}
		}
		pConfigs->Release();
	}
}


static bool firstLoad = true;

LPCTSTR CVcfwizardAppWiz::LoadTemplate( LPCTSTR lpszTemplateName, DWORD& rdwSize, HINSTANCE hInstance )
{
	if ( firstLoad ) {
		firstLoad = false;
		if ( m_pChooser->NeedsAdvancedFeatures() ) {
			m_Dictionary["ADVANCED_FEATURES"] = "1";
			if ( m_pChooser->NeedsSplashScreen() ) {
				m_Dictionary["SPLASH_SCR"] = "1";
			}

			if ( m_pChooser->NeedsStdFileMenu() ) {
				m_Dictionary["STD_FILE_MNU"] = "1";
				std::vector<CString>& extDescs = m_pChooser->GetFileExtDecriptions();
				std::vector<CString>& exts = m_pChooser->GetFileExts();
				CString size;
				size.Format( "%d", extDescs.size() );
				
				m_Dictionary[_T("EXT_SIZE")] = size;
				for (int i=0;i<extDescs.size();i++) {
					CString s1;
					s1.Format( "EXT_%d", i );
					m_Dictionary[s1] = exts[i];
					CString s2;
					s2.Format( "EXT_DESC_%d", i );
					m_Dictionary[s2] = extDescs[i];
				}
			}

			if ( m_pChooser->NeedsHelpMenu() ) {
				m_Dictionary["STD_HELP_MNU"] = "1";
			}

			if ( m_pChooser->NeedsUndoRedo() ) {
				m_Dictionary["UNDO_REDO"] = "1";
			}
		}	
	}
	return CCustomAppWiz::LoadTemplate( lpszTemplateName, rdwSize, hInstance );
}

// Here we define one instance of the CVcfwizardAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global Vcfwizardaw.
CVcfwizardAppWiz Vcfwizardaw;


