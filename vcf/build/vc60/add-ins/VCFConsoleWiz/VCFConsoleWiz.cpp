// VCFConsoleWiz.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "VCFConsoleWiz.h"
#include "VCFConsoleWizaw.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE VCFConsoleWizDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("VCFCONSOLEWIZ.AWX Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(VCFConsoleWizDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(VCFConsoleWizDLL);

		// Register this custom AppWizard with MFCAPWZ.DLL
		SetCustomAppWizClass(&VCFConsoleWizaw);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("VCFCONSOLEWIZ.AWX Terminating!\n");

		// Terminate the library before destructors are called
		AfxTermExtensionModule(VCFConsoleWizDLL);
	}
	return 1;   // ok
}
