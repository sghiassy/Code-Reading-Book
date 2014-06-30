// Commands.h : Declaration of the CCommands

#ifndef __COMMANDS_H_
#define __COMMANDS_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCommands

class CVCFNewClassWiz;

class ATL_NO_VTABLE CCommands : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CCommands, &CLSID_Commands>,
	public IDispatchImpl<ICommands, &IID_ICommands, &LIBID_VCFNEWCLASSWIZLib>
{
public:

DECLARE_NOT_AGGREGATABLE(CCommands)

BEGIN_COM_MAP(CCommands)
	COM_INTERFACE_ENTRY(ICommands)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

friend class CVCFNewClassWiz;

private:

	CComPtr<IApplication> m_piApplication;

public:

	CCommands();

// ICommands
public:	
	STDMETHOD(NewVCFClass)();
};

typedef CComObject<CCommands> CCommandsObj;

#endif //__COMMANDS_H_
