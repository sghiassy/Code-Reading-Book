// DSAddIn.h : header file
//

#if !defined(AFX_DSADDIN_H__4F7DDED0_AA21_4F0A_A8A5_73EFBDF441E6__INCLUDED_)
#define AFX_DSADDIN_H__4F7DDED0_AA21_4F0A_A8A5_73EFBDF441E6__INCLUDED_

#include "commands.h"

// {8907FE89-B98A-411E-95F8-47C5C309A3B6}
DEFINE_GUID(CLSID_DSAddIn,
0x8907fe89, 0xb98a, 0x411e, 0x95, 0xf8, 0x47, 0xc5, 0xc3, 0x9, 0xa3, 0xb6);

/////////////////////////////////////////////////////////////////////////////
// CDSAddIn

class CDSAddIn : 
	public IDSAddIn,
	public CComObjectRoot,
	public CComCoClass<CDSAddIn, &CLSID_DSAddIn>
{
public:
	DECLARE_REGISTRY(CDSAddIn, "VCF Builder",
		"VCFBUILDER Developer Studio Add-in", IDS_VCFBUILDER_LONGNAME,
		THREADFLAGS_BOTH)

	CDSAddIn() {}
	BEGIN_COM_MAP(CDSAddIn)
		COM_INTERFACE_ENTRY(IDSAddIn)
	END_COM_MAP()
	DECLARE_NOT_AGGREGATABLE(CDSAddIn)

// IDSAddIns
public:
	bool initVCFBuilderAddInCommands( const bool& bFirstTime, IApplication* pApplication );
	STDMETHOD(OnConnection)(THIS_ IApplication* pApp, VARIANT_BOOL bFirstTime,
		long dwCookie, VARIANT_BOOL* OnConnection);
	STDMETHOD(OnDisconnection)(THIS_ VARIANT_BOOL bLastTime);

protected:
	CCommandsObj* m_pCommands;
	DWORD m_dwCookie;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSADDIN_H__4F7DDED0_AA21_4F0A_A8A5_73EFBDF441E6__INCLUDED)
