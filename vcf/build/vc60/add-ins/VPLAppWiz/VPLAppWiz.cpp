// VPLAppWiz.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "VPLAppWiz.h"
#include "VPLAppWizaw.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE VPLAppWizDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("VPLAPPWIZ.AWX Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(VPLAppWizDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(VPLAppWizDLL);

		// Register this custom AppWizard with MFCAPWZ.DLL
		SetCustomAppWizClass(&VPLAppWizaw);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("VPLAPPWIZ.AWX Terminating!\n");

		// Terminate the library before destructors are called
		AfxTermExtensionModule(VPLAppWizDLL);
	}
	return 1;   // ok
}
