// Commands.cpp : Implementation of CCommands
#include "stdafx.h"
#include "COMToVCFClassWizard.h"
#include "Commands.h"
#include "TypeLibraryConverterDlg.h"
#include "ConvertActiveXCtrlDlg.h"
#include "TypeLibDump.h"

/////////////////////////////////////////////////////////////////////////////
// CCommands

CCommands::CCommands()
{
}

/////////////////////////////////////////////////////////////////////////////
// ICommands

STDMETHODIMP CCommands::ConvertCOMTypeLib()
{
	AFX_MANAGE_STATE( AfxGetModuleState() );

	// obtain the MSDEV CWinApp object:
	// (this is the "magic")
	CWinApp* pApp = AfxGetApp();

	// IF THIS ASSERT FIRES, THEN YOU ARE LIKELY LINKING TO THE DEBUG MFC LIBRARY
	// YOU CANNOT DO THIS BECAUSE MSDEV IS LINKED TO THE RELEASE MFC LIBRARY
	// BEST THING TO DO IS DELETE YOUR DEBUG CONFIG AND MAKE A NEW ONE BASED 
	// ON YOUR EXISTING RELEASE CONFIG.  THEN, ADD DEBUG SUPPORT TO THE 
	// COMPILE AND LINK OPTIONS.  THIS WORKS BECAUSE YOU WILL STILL LINK TO
	// MFC42.DLL INSTEAD OF MFC42D.DLL
	ASSERT(pApp);

	if (NULL == pApp) return E_FAIL;

	//
	//	write code here
	//
	TypeLibraryConverterDlg dlg;
	if ( IDOK == dlg.DoModal() ) {
		if ( TRUE == dlg.m_addToProject ) {
			CComPtr<IBuildProject> proj;
			if ( SUCCEEDED( this->m_piApplication->get_ActiveProject( (IDispatch**)&proj ) ) ){
				std::vector<CString>::iterator it = dlg.m_fileList.begin();
				CComBSTR fileName;
				_variant_t reserved;
				while ( it != dlg.m_fileList.end() ) {
					CString s = *it;
					fileName = s;
					proj->AddFile( fileName, reserved );
					it++;
				}			
			}
		}
	}

	return S_OK;
}



STDMETHODIMP CCommands::ConvertActiveXControl()
{
	AFX_MANAGE_STATE( AfxGetModuleState() );

	ConvertActiveXCtrlDlg dlg;
	if ( IDOK == dlg.DoModal() ) {
		_bstr_t res = GenerateClassHeaderFromCOMCLSID( dlg.m_selectedAxCtrlCLSID );
		CStdioFile file( "d:\\Res.h", CFile::modeCreate | CFile::modeWrite | CFile::typeText );
		file.WriteString( res );
	}
	return S_OK;
}
