// DSAddin.h : Declaration of the CDSAddin

#ifndef __VCFNewClassWiz_H_
#define __VCFNewClassWiz_H_

#include "resource.h"       // main symbols
#include "Commands.h"

/////////////////////////////////////////////////////////////////////////////
// CDSAddin

class ATL_NO_VTABLE CVCFNewClassWiz : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CVCFNewClassWiz, &CLSID_VCFNewClassWiz>,
	public IDSAddIn
{
public:

DECLARE_REGISTRY_RESOURCEID(IDR_VCFNewClassWiz)

DECLARE_NOT_AGGREGATABLE(CVCFNewClassWiz)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CVCFNewClassWiz)
	COM_INTERFACE_ENTRY(IDSAddIn)
END_COM_MAP()

private:

	CCommandsObj*			m_pCommands;

// IDSAddin
public:
	STDMETHOD(OnConnection)(THIS_ IApplication* pApp, VARIANT_BOOL bFirstTime,	long dwCookie, VARIANT_BOOL* OnConnection);
	STDMETHOD(OnDisconnection)(THIS_ VARIANT_BOOL bLastTime);
};

#endif //__DSADDIN_H_
