// DSAddin.cpp : Implementation of CCOMToVCFClassWizard
#include "stdafx.h"
#include "COMToVCFClassWizard.h"
#include "COMToVCFClassWizardAddin.h"

/////////////////////////////////////////////////////////////////////////////
// CCOMToVCFClassWizard


///////////////////////////////////////////////////////////////////////////////
// IDSAddin

STDMETHODIMP CCOMToVCFClassWizard::OnConnection(IApplication* piApplication, VARIANT_BOOL bFirstTime, long dwCookie, VARIANT_BOOL* pbOnConnection)
{
	// prepare to fail!
	*pbOnConnection = VARIANT_FALSE;

	// create our command object
	CCommandsObj::CreateInstance(&m_pCommands);
	m_pCommands->AddRef();
	m_pCommands->m_piApplication = piApplication;
	CComQIPtr<IDispatch> piDispCmds = m_pCommands;

	// tell VC about us:
	if (FAILED(piApplication->SetAddInInfo((long)_Module.GetModuleInstance(), piDispCmds, IDR_TOOLBAR_MEDIUM, IDR_TOOLBAR_LARGE, dwCookie)))
		return S_OK;

	// register our commands with VC
	VARIANT_BOOL bRet;
	_bstr_t		 bstrCmd,
					 bstrSampleCommand = "ConvertCOMTypeLib";

	// ConvertCOMTypeLib
	bstrCmd = "ConvertCOMTypeLib\nConvert COMTypeLib\nConverts a COM Type Library into VCF Object stubs\nConvert COM Type Lib";
	if (FAILED(piApplication->AddCommand(bstrCmd, bstrSampleCommand, 0, dwCookie, &bRet) || VARIANT_FALSE == bRet))
		return S_OK;


	bstrSampleCommand = "ConvertActiveXControl";
	bstrCmd = "ConvertActiveXControl\nConvert ActiveX Control\nConverts and wraps an ActiveX Control into a VCF compatible object\nConvert ActiveX Control";
	if (FAILED(piApplication->AddCommand(bstrCmd, bstrSampleCommand, 1, dwCookie, &bRet) || VARIANT_FALSE == bRet))
		return S_OK;

	// create toolbar:
	if (VARIANT_TRUE == bFirstTime)
	{
		bstrSampleCommand = "ConvertCOMTypeLib";
		piApplication->AddCommandBarButton(dsGlyph, bstrSampleCommand, dwCookie);

		bstrSampleCommand = "ConvertActiveXControl";
		piApplication->AddCommandBarButton(dsGlyph, bstrSampleCommand, dwCookie);
	}

	// success
	*pbOnConnection = VARIANT_TRUE;

	return S_OK;
}

STDMETHODIMP CCOMToVCFClassWizard::OnDisconnection(VARIANT_BOOL bLastTime)
{
	// cleanup!
	m_pCommands->Release();
	m_pCommands = NULL;

	return S_OK;
}
