// DSAddin.cpp : Implementation of CVCFNewClassWiz
#include "stdafx.h"
#include "VCFNewClassWiz.h"
#include "VCFNewClassWizAddin.h"

/////////////////////////////////////////////////////////////////////////////
// CVCFNewClassWiz


///////////////////////////////////////////////////////////////////////////////
// IDSAddin

STDMETHODIMP CVCFNewClassWiz::OnConnection(IApplication* piApplication, VARIANT_BOOL bFirstTime, long dwCookie, VARIANT_BOOL* pbOnConnection)
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
					 bstrSampleCommand = "NewVCFClass";

	// SampleCommand
	bstrCmd = "NewVCFClass\nNew VCF Class\nA New Class Addin Command\nNew VCF Class";
	if (FAILED(piApplication->AddCommand(bstrCmd, bstrSampleCommand, 0, dwCookie, &bRet) || VARIANT_FALSE == bRet))
		return S_OK;	

	// create toolbar:
	if (VARIANT_TRUE == bFirstTime)
	{
		piApplication->AddCommandBarButton(dsGlyph, bstrSampleCommand, dwCookie);
	}

	// success
	*pbOnConnection = VARIANT_TRUE;

	return S_OK;
}

STDMETHODIMP CVCFNewClassWiz::OnDisconnection(VARIANT_BOOL bLastTime)
{
	// cleanup!
	m_pCommands->Release();
	m_pCommands = NULL;

	return S_OK;
}
