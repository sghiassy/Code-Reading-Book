// Commands.cpp : Implementation of CCommands
#include "stdafx.h"
#include "VCFNewClassWiz.h"
#include "Commands.h"
#include "NewClassDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CCommands

CCommands::CCommands()
{
}

/////////////////////////////////////////////////////////////////////////////
// ICommands

STDMETHODIMP CCommands::NewVCFClass()
{
	
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

	CComPtr<IBuildProject> proj;
	
	if ( SUCCEEDED( this->m_piApplication->get_ActiveProject( (IDispatch**)&proj ) ) ){
		
		NewClassDlg dlg;
		
		if ( IDOK == dlg.DoModal() ){
			CString s = dlg.GetClassDecl();
			CString tmp1 = dlg.m_headerName;
			CString tmp2 = dlg.m_CPPName;
			
			CString filename = dlg.m_headerName;
			
			CFile file( filename, CFile::modeCreate | CFile::modeWrite | CFile::typeText );
			file.Write( s.GetBuffer(0), s.GetLength() );
			
			if ( FALSE == dlg.m_isClassAnInterface ) {
				filename = dlg.m_CPPName;
				s = dlg.GetClassImpl();
				CFile file2( filename, CFile::modeCreate | CFile::modeWrite | CFile::typeText );
				file2.Write( s.GetBuffer(0), s.GetLength() );
			}
			
			CComBSTR cppFile;
			cppFile = dlg.m_CPPName;
			_variant_t reserved;
			if ( FALSE == dlg.m_isClassAnInterface ) {
				proj->AddFile( cppFile, reserved );
			}
			CComBSTR headerFile = dlg.m_headerName;
			
			proj->AddFile( headerFile, reserved );			
			
		}
		
	}
	else {
		AfxMessageBox( "You don't have an active project - please create one before using this Class Wizard." );
	}



	return S_OK;
}

