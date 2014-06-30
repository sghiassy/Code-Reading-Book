//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllstartsystem.cpp
// PURPOSE: in here are all the startup functions that are needed to get the system up and running.
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// this is the file with the startup functions are for starting the system and get it running
// like the startup dialog stuff. 
// 
//////////////////////////////////////////////////////////////////////
// Part of DemoGL Demo System sourcecode. See version information
//////////////////////////////////////////////////////////////////////
// COPYRIGHTS:
// Copyright (c)1999-2001 Solutions Design. All rights reserved.
// Central DemoGL Website: www.demogl.com.
// 
// Released under the following license: (BSD)
// -------------------------------------------
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met: 
//
// 1) Redistributions of source code must retain the above copyright notice, this list of 
//    conditions and the following disclaimer. 
// 2) Redistributions in binary form must reproduce the above copyright notice, this list of 
//    conditions and the following disclaimer in the documentation and/or other materials 
//    provided with the distribution. 
// 
// THIS SOFTWARE IS PROVIDED BY SOLUTIONS DESIGN ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SOLUTIONS DESIGN OR CONTRIBUTORS BE LIABLE FOR 
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
//
// The views and conclusions contained in the software and documentation are those of the authors 
// and should not be interpreted as representing official policies, either expressed or implied, 
// of Solutions Design. 
//
// See also License.txt in the sourcepackage.
//
//////////////////////////////////////////////////////////////////////
// Contributers to the code:
//		- Frans Bouma [FB]
//////////////////////////////////////////////////////////////////////
// VERSION INFORMATION.
//
// v1.3: For 80% recoded.
// v1.2: Rescheduling of programflow
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//        DEPRECATED FUNCTIONS. HERE FOR COMPATIBILITY REASONS. 
//            DON'T USE THESE. USE DEMOGL_* VARIANTS INSTEAD
///////////////////////////////////////////////////////////////////////
// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_SetStartupDialogVars() instead.
void
SetStartupDialogVars(const char *sDemoName, const char *sCreators, bool bFullScreen,
		int iResolution, bool bDisplay32bpp, bool bTexture32bpp, bool bSound)
{
	SStartupDat		*pStartupDat;

	// create structure
	InitializeStartupDialogDefaults();
	// will use for the soundsystem settings, the defaults set by InitializeStartupDialogDefaults()
	
	pStartupDat=m_gpDemoDat->GetStartupDat();
	strncpy(pStartupDat->m_sDemoName,sDemoName,DGL_SD_STRINGLENGTH);
	strncpy(pStartupDat->m_sReleasedBy,sCreators,DGL_SD_STRINGLENGTH);
	pStartupDat->m_bFullScreen=bFullScreen;
	pStartupDat->m_bDQ32bit=bDisplay32bpp;
	pStartupDat->m_bSound=bSound;
	pStartupDat->m_bTQ32bit=bTexture32bpp;
	pStartupDat->m_iResolution=iResolution;
}


// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_RunApp() instead.
void
RunApp(HINSTANCE hInstance, const char *sWindowTitle, bool bUseCrypting, byte byKey1, byte byKey2)
{
	DEMOGL_AppRun(hInstance,m_gpDemoDat->GetStartupDat(), sWindowTitle,bUseCrypting,byKey1,byKey2,RUNTYPE_NORMAL);
}

// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_SetVariable() instead.
void 
OverruleDefaults(bool bVSYNC, int iFileSRC, const char *sSRCDir, const char *sDataFilename, const char *sScriptFilename, 
			bool bShowFPS, bool bNeverShowFPS, bool bNeverShowConsole, bool bShowDebugInfo, bool bNeverShowDebugInfo, int iMusicSys )
{
	DEMOGL_SetVariable(DGL_VF_VSYNC,bVSYNC);
	DEMOGL_SetVariable(DGL_VF_FILESRC,iFileSRC);
	DEMOGL_SetVariable(DGL_VF_SRCDIR,sSRCDir);
	DEMOGL_SetVariable(DGL_VF_DATAFILENAME,sDataFilename);
	DEMOGL_SetVariable(DGL_VF_SCRIPTFILENAME,sScriptFilename);
	DEMOGL_SetVariable(DGL_VF_SHOWFPS,bShowFPS);
	DEMOGL_SetVariable(DGL_VF_NEVERSHOWFPS,bNeverShowFPS);
	DEMOGL_SetVariable(DGL_VF_NEVERSHOWCONSOLE, bNeverShowConsole);
	DEMOGL_SetVariable(DGL_VF_SHOWDEBUGINFO, bShowDebugInfo);
	DEMOGL_SetVariable(DGL_VF_NEVERSHOWDEBUGINFO,bNeverShowDebugInfo);
	DEMOGL_SetVariable(DGL_VF_MUSICSYS, iMusicSys);
}


// Purpose: initializes the Startup Dialog datastruct with the defaults.
// DEPRECATED FROM VERSION 1.3. ONLY USED BY DEPRECATED FUNCTIONS. DON'T USE IN OWN FUNCTIONS.
void
InitializeStartupDialogDefaults()
{
	SStartupDat		*pStartupDat;

	// Get pointer to struct.
	pStartupDat=m_gpDemoDat->GetStartupDat();
	
	// If allocated fill it with default parameters
	if(pStartupDat)
	{
		// Initialize the sddDialogData structure with the default values.
		pStartupDat->m_bDQ32bit=(DISPLAY32BIT==1);
		pStartupDat->m_bFullScreen=(FULLSCREEN==1);
		pStartupDat->m_bSound=(SOUND==1);
		pStartupDat->m_bTQ32bit=(TEXTURE32BIT==1);
		pStartupDat->m_iResolution=RES800x600;
		pStartupDat->m_bSaveInRegistry=false;
		pStartupDat->m_bRescaleTextures=false;
		pStartupDat->m_bSS_3DSound=false;
		pStartupDat->m_bSS_16bit=true;
		pStartupDat->m_bSS_A3D=false;
		pStartupDat->m_bSS_EAX=false;
		pStartupDat->m_bSS_LowQuality=false;
		pStartupDat->m_bSS_Stereo=true;
		pStartupDat->m_iSS_SoundDevice=0;
		memset(pStartupDat->m_sDemoName,0,DGL_SD_STRINGLENGTH);;
		memset(pStartupDat->m_sReleasedBy,0,DGL_SD_STRINGLENGTH);;
		// Add more options here.
	}
}


///////////////////////////////////////////////////////////////////////
//                     END DEPRECATED FUNCTIONS.                     //
///////////////////////////////////////////////////////////////////////

// Purpose: general application startpoint. It depends on the iRunType which kind of application is
// started, and thus which type of RUNTYPE is set. Execution will not be returned to the caller
// until the application ends. 
// Returns SYS_OK if execution went ok, SYS_NOK otherwise.
int
DEMOGL_AppRun(const HINSTANCE hInstance, const SStartupDat *psdStartupDat,
						const char *pszWindowTitle, const bool bUseCrypting, const byte byKey1, 
						const byte byKey2, const int iRunType)
{
	// set the runtype for this execution
	m_gpDemoDat->SetRunType(iRunType);

	// select routine that will execute the requested runtype
	switch(iRunType)
	{
		case RUNTYPE_NORMAL:
		{
			// Normal execution, use build in dialog and startup code which recovers from nonfatal errors.
			return DoAppRun_Normal(hInstance, psdStartupDat,pszWindowTitle,bUseCrypting, byKey1, byKey2);
		}; break;
		case RUNTYPE_NODIALOG:
		{
			// As normal execution but don't use startup dialog but use the info supplied in psdStartupDat.
			// if that info makes the application fail on startup, fail completely. No recovery possible.
			return DoAppRun_NoDialog(hInstance, psdStartupDat,pszWindowTitle,bUseCrypting, byKey1, byKey2);
		}; break;
		case RUNTYPE_SAVER_CONFIG:
		{
			// Start in config screensaver mode (i.e. the user clicked on 'config' in display properties' screensaver tab.
			// Displays build in dialog for execution parameters and exists after that. psdStartupDat is used to set the
			// controls on the dialog, if no saved configdata is found.
			return DoAppRun_Saver(hInstance, psdStartupDat,pszWindowTitle,bUseCrypting, byKey1, byKey2, DGL_SSAVERET_CONFIG);
		}; break;
		case RUNTYPE_SAVER_NORMAL:
		{
			// Run as normal saver, thus as the user wants to execute it. psdStartupDat is overruled with the data
			// saved in the registry using RUNTYPE_SAVER_CONFIG.
			return DoAppRun_Saver(hInstance, psdStartupDat,pszWindowTitle,bUseCrypting, byKey1, byKey2, DGL_SSAVERET_NORMAL);
		}; break;
		case RUNTYPE_SAVER_PREVIEW:
		{
			// Run as normal saver but in the tiny window on the display properties tab. Use saved data for bitdepth etc
			// and sound is disabled.
			return DoAppRun_Saver(hInstance, psdStartupDat,pszWindowTitle,bUseCrypting, byKey1, byKey2, DGL_SSAVERET_PREVIEW);
		}; break;
		case RUNTYPE_SAVER_PASSWORD:
		{
			// not supported.
			return SYS_NOK;
		}; break;
		////////////////////////////////
		// ADD MORE RUNTYPES HERE
		////////////////////////////////
		default:
		{
			// unknown runtype. 
			return SYS_NOK;
		}; break;
	}
}


// Purpose: Starts the application. This is the RUNTIME_NORMAL variant, which opens
// a startup dialog and from there starts the execution of the application.
// Control is NOT passed back to the caller, until the app exits.
// pStartupDat points to a structure that is used to fill in the parameters of the startupdialog
// and use them in further execution.
//
// Will return SYS_OK if succeeded, SYS_NOK otherwise (there was an unrecoverable error)
int
DoAppRun_Normal(const HINSTANCE hInstance, const SStartupDat *psdStartupDat, const char *pszWindowTitle, 
		const bool bUseCrypting, const byte byKey1, const byte byKey2)
{
	int						iResult, iWidth, iHeight, iDepth, iTexDepth;
	GLint					iPixelDepthHint;
	bool					bQuitStartupLoop;
	CStdString				sToLog;
	SStartupDat				*pStartupDat;
	SSoundSystemOptionDat	*pSSODat;

	// Store pStartupDat structure.
	if(psdStartupDat)
	{
		// store the data. Data is copied in own datastore so it can be modified.
		m_gpDemoDat->SetStartupDat(psdStartupDat);
	}
	else
	{
		// no startupdata supplied. 
		// test if it's already there and the user has set data via deprecated functions.
		if(!m_gpDemoDat->GetStartupDat())
		{
			// nope. nothing set. report error and die.
			MessageBox(NULL,"DoAppRun_Normal:: No SStartupDat structure provided. Cannot continue.",
						"DemoGL Fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
			return SYS_NOK;
		}
		// some data is there... continue.
	}

	// generate registry key name. Leave result for now.
	iResult=GenerateRegistryKeyName(m_gpDemoDat->GetStartupDat()->m_sDemoName);

	// store decryptkeys.
	m_gpDemoDat->SetKeyValues(byKey1, byKey2);

	// First check if crypting is set to ON. If so, check the keys with the keyfile in the datasource, and construct the
	// decryptkey.
	if(bUseCrypting)
	{
		// store decryption used flag.
		m_gpDemoDat->SetbUseCrypting(true);
		iResult = InitCryptingVars();
		if(iResult!=SYS_OK)
		{
			// crypting params failed. exit
			return SYS_NOK;
		}
	}
	else
	{
		// set decryption used flag to false
		m_gpDemoDat->SetbUseCrypting(false);
	}

	// get pointer to startupdat. We might already got this pointer in psdStartupDat, but
	// if the user set data via deprecated functions we should retrieve it anyway, so get the
	// official pointer.
	pStartupDat=m_gpDemoDat->GetStartupDat();
	if(!pStartupDat)
	{
		// not there... 
		MessageBox(NULL,"DoAppRun_Normal:: The internal object for startup information couldn't be allocated.\n\nAborting.",
						"DemoGL Fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
		return SYS_NOK;
	}

	// Determine what's possible with the soundsystem.
	pSSODat=DEMOGL_SoundSystemDetermineOptionsData();

	// Set Application instance handle
	m_gpDemoDat->SethInstance(hInstance);

	// Startup Dialog control loop. If the user presses OK and there is something wrong, we return to the
	// reopening of the dialog.
	for(bQuitStartupLoop=false;!bQuitStartupLoop;)
	{
		// set system state.
		m_gpDemoDat->SetSystemState(SSTATE_BOOT_STARTUPDIALOG);

		// open Startup Dialog. 
		iResult=DialogBoxParam(m_ghDLLInstance,MAKEINTRESOURCE(IDD_STARTFORM),NULL,StartupDlgMsgHandler,NULL);
		if((!iResult)||(iResult==SYS_SUD_CANCELCLICKED))
		{
			// user wants to exit. exit.
			return SYS_OK;
		}

		// user clicked OK. pStartupDat is now filled with information selected by
		// the user. Use that info for startup and execution.
		GetWidthHeightOfResolution(pStartupDat->m_iResolution, &iWidth, &iHeight);

		iDepth=16;
		if(pStartupDat->m_bDQ32bit)
		{
			iDepth=32;
		}
		
		iTexDepth=16;
		iPixelDepthHint=BPPDEPTHHINT16BIT;
		if(pStartupDat->m_bTQ32bit)
		{
			iTexDepth=32;
			iPixelDepthHint=BPPDEPTHHINT32BIT;
		}

		// store vital data in pDemoDat
		m_gpDemoDat->SetAppSpecs(iWidth, iHeight, iDepth, iTexDepth, hInstance, iPixelDepthHint);

		// Create Window. 
		m_gpDemoDat->SetSystemState(SSTATE_BOOT_MAINWNDCREATION);
		iResult=CreateMainWindow(pszWindowTitle);
		switch(iResult)
		{
			case SYS_CW_OK:
			{
				// everything was all right. 
				bQuitStartupLoop = true;
			}; break;
			case SYS_CW_NOTRECOVERABLEERROR:
			{
				// end here.
				return SYS_NOK;
			}; break;
			case SYS_CW_RECOVERABLEERROR:
			{
				// an error occured but can be recovered by selecting another option.
				continue;
			}; break;
		}
	}

	// Set systemstate into messagepump start. This will tick towards kernelloop state automatically
	// when the appropriate threads are finished. See MainMsgHandler for details
	m_gpDemoDat->SetSystemState(SSTATE_BOOT_MESSAGEPUMPSTART);

	// Dive into the kernelloop. Won't return until WM_QUIT is posted.
	KernelLoop();

	// Application ends.
	return SYS_OK;
}


// Purpose: Starts the application. This is the RUNTYPE_NODIALOG variant, which  doesn't open
// a startup dialog but just picks the psdStartupDat data and runs the app with that. Any failure will result in
// termination of the application.
// Control is NOT passed back to the caller, until the app exits.
// pStartupDat points to a structure that is used to fill in the parameters of the startupdialog
// and use them in further execution.
//
// Will return SYS_OK if succeeded, SYS_NOK otherwise (there was an unrecoverable error)
int
DoAppRun_NoDialog(const HINSTANCE hInstance, const SStartupDat *psdStartupDat, const char *pszWindowTitle, 
			const bool bUseCrypting, const byte byKey1, const byte byKey2)
{
	int						iResult, iWidth, iHeight, iDepth, iTexDepth;
	GLint					iPixelDepthHint;
	CStdString				sToLog;
	SStartupDat				*pStartupDat;

	// Store pStartupDat structure.
	if(psdStartupDat)
	{
		// store the data. Data is copied in own datastore so it can be modified.
		m_gpDemoDat->SetStartupDat(psdStartupDat);
	}
	else
	{
		// no startupdata supplied. 
		// test if it's already there and the user has set data via deprecated functions.
		if(!m_gpDemoDat->GetStartupDat())
		{
			// nope. nothing set. report error and die.
			MessageBox(NULL,"DoAppRun_NoDialog:: No SStartupDat structure provided. Cannot continue.",
						"DemoGL Fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
			return SYS_NOK;
		}
		// some data is there... continue.
	}

	// generate registry key name. Leave result for now.
	iResult=GenerateRegistryKeyName(m_gpDemoDat->GetStartupDat()->m_sDemoName);

	// store decryptkeys.
	m_gpDemoDat->SetKeyValues(byKey1, byKey2);

	// First check if crypting is set to ON. If so, check the keys with the keyfile in the datasource, and construct the
	// decryptkey.
	if(bUseCrypting)
	{
		// store decryption used flag.
		m_gpDemoDat->SetbUseCrypting(true);
		iResult = InitCryptingVars();
		if(iResult!=SYS_OK)
		{
			// crypting params failed. exit
			return SYS_NOK;
		}
	}
	else
	{
		// set decryption used flag to false
		m_gpDemoDat->SetbUseCrypting(false);
	}

	// get pointer to startupdat. We might already got this pointer in psdStartupDat, but
	// if the user set data via deprecated functions we should retrieve it anyway, so get the
	// official pointer.
	pStartupDat=m_gpDemoDat->GetStartupDat();
	if(!pStartupDat)
	{
		// not there... 
		MessageBox(NULL,"DoAppRun_NoDialog:: The internal object for startup information couldn't be allocated.\n\nAborting.",
						"DemoGL Fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
		return SYS_NOK;
	}

	// Set Application instance handle
	m_gpDemoDat->SethInstance(hInstance);

	// set system state.
	m_gpDemoDat->SetSystemState(SSTATE_BOOT_MAINWNDCREATION);

	GetWidthHeightOfResolution(pStartupDat->m_iResolution, &iWidth, &iHeight);

	iDepth=16;
	if(pStartupDat->m_bDQ32bit)
	{
		iDepth=32;
	}
	
	iTexDepth=16;
	iPixelDepthHint=BPPDEPTHHINT16BIT;
	if(pStartupDat->m_bTQ32bit)
	{
		iTexDepth=32;
		iPixelDepthHint=BPPDEPTHHINT32BIT;
	}

	// store vital data in pDemoDat
	m_gpDemoDat->SetAppSpecs(iWidth, iHeight, iDepth, iTexDepth, hInstance, iPixelDepthHint);

	// Create Window. 
	m_gpDemoDat->SetSystemState(SSTATE_BOOT_MAINWNDCREATION);
	iResult=CreateMainWindow(pszWindowTitle);
	switch(iResult)
	{
		case SYS_CW_OK:
		{
			// everything was all right. 
		}; break;
		case SYS_CW_NOTRECOVERABLEERROR:
		case SYS_CW_RECOVERABLEERROR:
		{
			// end here. The programmer should have used the build in dialog system that will recover.
			return SYS_NOK;
		}; break;
	}

	// Set systemstate into messagepump start. This will tick towards kernelloop state automatically
	// when the appropriate threads are finished. See MainMsgHandler for details
	m_gpDemoDat->SetSystemState(SSTATE_BOOT_MESSAGEPUMPSTART);

	// Dive into the kernelloop. Won't return until WM_QUIT is posted.
	KernelLoop();
	// Application ends.
	return SYS_OK;
}



// Purpose: Starts the application. This is the RUNTIME_SAVER_* variant, which will use the flag iSaverExecType to
// determine which execution path to follow. The caller should have called DEMOGL_ParseScreenSaverCL() first to initialize
// the screensaver execution type by parsing the commandline arguments. (data is stored in m_sclpSSCLParams struct inside
// DemoDat. If user didn't call DEMOGL_ParseScreenSaverCL(), execution is undefined (will most likely fail)
// Control is NOT passed back to the caller, until the app exits.
// pStartupDat points to a structure that is used to set execution parameters like screenwidth/height, bitdepth etc. 
// If there is any data saved in the registry, pStartupDat is filled with that data, otherwise the pStartupDat contents
// is used for execution/dialog initialisation. If pStartupDat == NULL, the routine will fail.
// ANY error will result in failure of execution.
//
// Will return SYS_OK if succeeded, SYS_NOK otherwise (there was an unrecoverable error)
int
DoAppRun_Saver(const HINSTANCE hInstance, const SStartupDat *psdStartupDat, const char *pszWindowTitle, 
			bool bUseCrypting, const byte byKey1, const byte byKey2, const int iSaverExecType)
{
	int						iResult, iWidth, iHeight, iDepth, iTexDepth;
	GLint					iPixelDepthHint;
	CStdString				sToLog;
	SStartupDat				*pStartupDat;
	RECT					rcParentWindow;

	// Store pStartupDat structure.
	if(psdStartupDat)
	{
		// store the data. Data is copied in own datastore so it can be modified.
		m_gpDemoDat->SetStartupDat(psdStartupDat);
	}
	else
	{
		// no startupdata supplied. 
		// test if it's already there and the user has set data via deprecated functions.
		if(!m_gpDemoDat->GetStartupDat())
		{
			// nope. nothing set. report error and die.
			MessageBox(NULL,"DoAppRun_Saver:: No SStartupDat structure provided. Cannot continue.",
						"DemoGL Fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
			return SYS_NOK;
		}
		// some data is there... continue.
	}

	// generate registry key name. Leave result for now.
	iResult=GenerateRegistryKeyName(m_gpDemoDat->GetStartupDat()->m_sDemoName);

	// Determine the working dir of the saver. (needed to read the datafile(s))
	DetermineSSaverWorkingDir();

	// store decryptkeys.
	m_gpDemoDat->SetKeyValues(byKey1, byKey2);

	// First check if crypting is set to ON. If so, check the keys with the keyfile in the datasource, and construct the
	// decryptkey.
	if(bUseCrypting)
	{
		// store decryption used flag.
		m_gpDemoDat->SetbUseCrypting(true);
		iResult = InitCryptingVars();
		if(iResult!=SYS_OK)
		{
			// crypting params failed. exit
			return SYS_NOK;
		}
	}
	else
	{
		// set decryption used flag to false
		m_gpDemoDat->SetbUseCrypting(false);
	}

	// get pointer to startupdat. We might already got this pointer in psdStartupDat, but
	// if the user set data via deprecated functions we should retrieve it anyway, so get the
	// official pointer.
	pStartupDat=m_gpDemoDat->GetStartupDat();
	if(!pStartupDat)
	{
		// not there... 
		MessageBox(NULL,"DoAppRun_Saver:: The internal object for startup information couldn't be allocated.\n\nAborting.",
						"DemoGL Fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
		return SYS_NOK;
	}

	// Set Application instance handle
	m_gpDemoDat->SethInstance(hInstance);

	// execute one of the commandline parameter handlers.
	switch(iSaverExecType)
	{
		case DGL_SSAVERET_UNKNOWN:
		case DGL_SSAVERET_PASSWORD:
		{
			// not supported or unknown flag. 
			return SYS_NOK;
		}; break;
		case DGL_SSAVERET_CONFIG:
		{

			// open the startup dialog. it should always save to registry.
			// it should use the hWnd in the screensaver properties as parentwindow to get child window removal
			// when the parentdialog is closed (display properties in windows)
			// open Startup Dialog. 
			iResult=DialogBoxParam(m_ghDLLInstance,MAKEINTRESOURCE(IDD_STARTFORM),
							m_gpDemoDat->GetSSCLParams()->hWnd, StartupDlgMsgHandler,NULL);
			// no further execution is allowed. return.
			return SYS_OK;
		}; break;
		case DGL_SSAVERET_PREVIEW:
		{

			// Read startupdata from registry (if any available).
			LoadStartupDatFromRegistry(pStartupDat);

			// open window dependant on passed HWND. 
			// for the rest do normal execution, except sound is disabled.
			
			// No sound available.
			pStartupDat->m_bSound=false;

			// calculate width and height of destination window (i.e. the size of the parentwindow)
			GetWindowRect(m_gpDemoDat->GetSSCLParams()->hWnd,&rcParentWindow);
			iWidth=rcParentWindow.right-rcParentWindow.left;
			iHeight=rcParentWindow.bottom-rcParentWindow.top;
			iDepth=16;
			if(pStartupDat->m_bDQ32bit)
			{
				iDepth=32;
			}
			
			iTexDepth=16;
			iPixelDepthHint=BPPDEPTHHINT16BIT;
			if(pStartupDat->m_bTQ32bit)
			{
				iTexDepth=32;
				iPixelDepthHint=BPPDEPTHHINT32BIT;
			}

			// store vital data in pDemoDat
			m_gpDemoDat->SetAppSpecs(iWidth, iHeight, iDepth, iTexDepth, hInstance, iPixelDepthHint);

			// Create Window. 
			m_gpDemoDat->SetSystemState(SSTATE_BOOT_MAINWNDCREATION);
			iResult=CreateMainWindowForSaverPreview();
			switch(iResult)
			{
				case SYS_CW_OK:
				{
					// everything was all right. 
				}; break;
				case SYS_CW_NOTRECOVERABLEERROR:
				{
					// an error occured but can be recovered by selecting another option.
					return SYS_NOK;
				}; break;
			}
		}; break;
		case DGL_SSAVERET_NORMAL:
		{
			// Read startupdata from registry (if any available).
			LoadStartupDatFromRegistry(pStartupDat);

			// run application normally .
			// Open window and then do normal execution.
			// get data from startup dat struct.
			GetWidthHeightOfResolution(pStartupDat->m_iResolution, &iWidth, &iHeight);
			iDepth=16;
			if(pStartupDat->m_bDQ32bit)
			{
				iDepth=32;
			}
			
			iTexDepth=16;
			iPixelDepthHint=BPPDEPTHHINT16BIT;
			if(pStartupDat->m_bTQ32bit)
			{
				iTexDepth=32;
				iPixelDepthHint=BPPDEPTHHINT32BIT;
			}

			// store vital data in pDemoDat
			m_gpDemoDat->SetAppSpecs(iWidth, iHeight, iDepth, iTexDepth, hInstance, iPixelDepthHint);

			// Create Window. 
			m_gpDemoDat->SetSystemState(SSTATE_BOOT_MAINWNDCREATION);
			iResult=CreateMainWindow(pszWindowTitle);
			switch(iResult)
			{
				case SYS_CW_OK:
				{
					// everything was all right. 
				}; break;
				case SYS_CW_NOTRECOVERABLEERROR:
				case SYS_CW_RECOVERABLEERROR:
				{
					// an error occured but can be recovered by selecting another option.
					return SYS_NOK;
				}; break;
			}
		}; break;
		default:
		{
			// unknown flag
			return SYS_NOK;
		}; break;
	}

	/////////////////////////////
	// if we enter here, we already have a window and just have to continue normal execution.
	/////////////////////////////

	// Set systemstate into messagepump start. This will tick towards kernelloop state automatically
	// when the appropriate threads are finished. See MainMsgHandler for details
	m_gpDemoDat->SetSystemState(SSTATE_BOOT_MESSAGEPUMPSTART);

	// Dive into the kernelloop. Won't return until WM_QUIT is posted.
	KernelLoop();

	// Application ends.
	return SYS_OK;
}



// Purpose: initializes the system. If everything is ok, SYS_OK is returned, otherwise the errorcode.
// in: all data is in pDemoDat
// Out: SYS_OK : all done well
//      Errorcode: some error occured. Check Errorcode
//
// Additional info: this function is the function of a new thread. This thread is created when a
// WM_DEMOGL_INITSYSTEM message arrives at the main message handler.
DWORD WINAPI
InitSystem(LPVOID pParam)
{
	CStdString		sToLog;

	// Increase Current Value in loading progressbar.
	IncLoadingPBValue(1);

	// parse extensions
	LogFeedback("---[List of Extensions found and marked available]---",true,"InitSystemThread", true);
	m_gpExtensions->ParseExtensionsString();
	// Increase Current Value in loading progressbar.
	IncLoadingPBValue(1);
	LogFeedback("---[Pixel format descriptor information]-------------",true,"InitSystemThread", true);

	// log pixelformat
	sToLog.Format("Pixel format descriptor (PFD) chosen: %d.", m_gpDemoDat->GetPixelFormatNo());
	LogFeedback(&sToLog[0],true,"InitSystemThread", true);
	sToLog.Format("PFD reports: Color buffer depth: %d bit.",m_gpDemoDat->GetPixelFormatColorDepth());
	LogFeedback(&sToLog[0],true,"InitSystemThread", true);
	sToLog.Format("PFD reports: Depth buffer depth: %d bit.", m_gpDemoDat->GetPixelFormatZDepth());
	LogFeedback(&sToLog[0],true,"InitSystemThread", true);
	sToLog.Format("PFD reports: Stencil buffer depth: %d bit.",m_gpDemoDat->GetPixelFormatStencilDepth());
	LogFeedback(&sToLog[0],true,"InitSystemThread", true);
	LogFeedback("---[End of PFD information]--------------------------",true,"InitSystemThread", true);

	//////////////////////
	//
	// ADD MORE WORKER THREAD STUFF HERE. THIS WILL THEN BE DONE WHILE FEEDBACK CAN BE RENDERED TO THE CONSOLE
	// DONT ADD OPENGL SPECIFIC STUFF HERE, BECAUSE THIS THREAD DOESNT HAVE A RENDERCONTEXT SO OPENGL IS NOT
	// AVAILABLE
	//
	//////////////////////

	// We're done with the init of the system. Sent message to start Init of the app related stuff
	PostMessage(m_gpDemoDat->GethWnd(),WM_DEMOGL_INITAPP,NULL,NULL);

	// thread ends.
	return 1;
}


// Purpose: initializes all registered effectobjects. This is done pre-execution and pre-prepare of the
// system. 
// Returns: SYS_OK if succeeded, Errorcode if error (the errorcode of the routine which produced the error, or a SYS_STOP
// if the developer has stated that DemoGL should die if there was an init error.
int
InitAllRegisteredEffectObjects()
{
	int					iEIResult, iResult;
	CStdString			sToLog;
	CEffectStore		*pCurr, *pNext;
	bool				bContinue;

	iResult=SYS_OK;
	for(pCurr = m_gpESHead, iEIResult=SYS_OK, bContinue=true;pCurr && bContinue;)
	{
		sToLog.Format("..Initializing effect %s.",pCurr->GetEffectName());
		LogFeedback(&sToLog[0],false,"InitAppThread",true);
		iEIResult = pCurr->GetEffectObject()->Init(m_gpDemoDat->GetiWindowWidth(),m_gpDemoDat->GetiWindowHeight());
		if(iEIResult!=SYS_OK)
		{
			iResult=iEIResult;
			sToLog.Format("Error initializing effect %s. Error returned: %d.", pCurr->GetEffectName(),iEIResult);
			LogFeedback(&sToLog[0],true,"InitAppThread", false);
			// check what we should do.
			switch(m_gpDemoDat->GetInitFailureResponse())
			{
				case DGL_AIFF_DIE:
				{
					// the developer has set the response of DemoGL to an Effect's init failure to 'Die' so
					// we should quit now. Do this by setting the resultflag.
					MessageBox(NULL,"There was a failure in one of the effect's Init() method.\n\nThe developer of this application has selected that DemoGL should end if such an error occures.",
							"DemoGL Fatal Error",MB_OK | MB_SYSTEMMODAL | MB_ICONERROR);
					bContinue=false;
					// return a SYS_STOP
					iResult=SYS_STOP;
				}; break;
				case DGL_AIFF_IGNORE:
				{
					// do nothin'
					pCurr=pCurr->GetNext();
				}; break;
				case DGL_AIFF_REMOVEEFFECT:
				{
					// Remove the effect
					pNext=pCurr->GetNext();
					RemoveEffectStore(pCurr->GetEffectName());
					pCurr=pNext;
				}; break;
				default:
				{
					// do nothin'
					pCurr=pCurr->GetNext();
				}; break;
			}
		}
		else
		{
			pCurr=pCurr->GetNext();
		}

		// Increase Current Value in loading progressbar.
		IncLoadingPBValue(1);
	}
	return iResult;
}


// Purpose: initialises application related stuff: effects, script...
// Additional Info: this function is the function of a new thread created by the main message handler
// when a WM_DEMOGL_INITAPP message arrives
DWORD WINAPI
InitApp(LPVOID pParam)
{
	int				iSLResult, iEIResult;
	CStdString		sToLog;
	bool			bUEOccured;			// Unrecoverable error has occured flag

	// set UnrecoverableErrorOccured flag to false.
	bUEOccured = false;

	m_gpDemoDat->SetSystemState(SSTATE_AINIT_SCRIPTPARSING);
	sToLog.Format("Loading and parsing script %s.", m_gpDemoDat->GetScriptFilename());
	LogFeedback(&sToLog[0],true,"InitAppThread", false);

	iSLResult = LoadAndParseScript();
	LogFeedback("Script loading and parsing finished.",true,"InitAppThread", true);

	// Increase Current Value in loading progressbar.
	IncLoadingPBValue(1);

	/////////////////////
	// ADD MORE INITSTUFF _HERE_
	////////////////////

	// for all registered objects, call init()
	m_gpDemoDat->SetSystemState(SSTATE_AINIT_EFFECTSINIT);
	LogFeedback("Initializing registered effectobjects.",true,"InitAppThread", false);

	if(m_gpESHead)
	{
		iEIResult=InitAllRegisteredEffectObjects();
	}
	LogFeedback("Initializing registered effectobjects done.",true,"InitAppThread", false);

	////////////////////
	// Do NOT add INITSTUFF below this line!!!!!
	////////////////////
	
	bUEOccured=(iEIResult==SYS_STOP);

	if(bUEOccured)
	{
		// Unrecoverable error occured. we cannot continue. Error is logged already to the console. Just flag the
		// application it should abort. Pass '1' in wParam, so no other errorbox will be shown
		PostMessage(m_gpDemoDat->GethWnd(),WM_DEMOGL_ABORT,1,0);
	}
	else
	{
		// we're done. Post a message to the windowmessagequeue so it will call the Prepare() function.
		PostMessage(m_gpDemoDat->GethWnd(),WM_DEMOGL_PREPARE,NULL,0);
	}
	return 1;
}


// Purpose: prepares last minute stuff before the system will start. 
// Additional info: this function is a function of a new thread that is started when WM_DEMOGL_PREPARE message
// is received by the main message handler
DWORD WINAPI
Prepare(LPVOID pParam)
{
	m_gpDemoDat->SetSystemState(SSTATE_AINIT_PRESTARTEVENTEXEC);
	// execute the timeline events that are scheduled on time 0. (the start).
	// we pass -1 to the routine so all events with timespots <0 will be executed.
	ExecuteTimeLine(-1);
	// Increase Current Value in loading progressbar.
	IncLoadingPBValue(1);

	PostMessage(m_gpDemoDat->GethWnd(),WM_DEMOGL_STARTKERNELLOOP,NULL,NULL);
	return 1;
}


