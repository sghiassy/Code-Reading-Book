// DSAddIn.h : header file
//

#if !defined(AFX_DSADDIN_H__1D77408B_6D0D_4032_8421_F636BBFB84BD__INCLUDED_)
#define AFX_DSADDIN_H__1D77408B_6D0D_4032_8421_F636BBFB84BD__INCLUDED_

#include "commands.h"

// {1794E40B-4838-4576-A5DF-769CD3AA002A}
DEFINE_GUID(CLSID_DSAddIn,
0x1794e40b, 0x4838, 0x4576, 0xa5, 0xdf, 0x76, 0x9c, 0xd3, 0xaa, 0, 0x2a);

/////////////////////////////////////////////////////////////////////////////
// CDSAddIn

class CDSAddIn : 
	public IDSAddIn,
	public CComObjectRoot,
	public CComCoClass<CDSAddIn, &CLSID_DSAddIn>
{
public:
	DECLARE_REGISTRY(CDSAddIn, "Test.DSAddIn.1",
		"TEST Developer Studio Add-in", IDS_TEST_LONGNAME,
		THREADFLAGS_BOTH)

	CDSAddIn() {}
	BEGIN_COM_MAP(CDSAddIn)
		COM_INTERFACE_ENTRY(IDSAddIn)
	END_COM_MAP()
	DECLARE_NOT_AGGREGATABLE(CDSAddIn)

// IDSAddIns
public:
	STDMETHOD(OnConnection)(THIS_ IApplication* pApp, VARIANT_BOOL bFirstTime,
		long dwCookie, VARIANT_BOOL* OnConnection);
	STDMETHOD(OnDisconnection)(THIS_ VARIANT_BOOL bLastTime);

protected:
	CCommandsObj* m_pCommands;
	DWORD m_dwCookie;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSADDIN_H__1D77408B_6D0D_4032_8421_F636BBFB84BD__INCLUDED)
