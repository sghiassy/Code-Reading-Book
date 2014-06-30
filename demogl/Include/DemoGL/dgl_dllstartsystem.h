//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllstartsystem.h
// PURPOSE: include file of dgl_dllstartsystem.cpp
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
// v1.3: Added new functions
// v1.2: Added new functions
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#ifndef _DGL_DLLSTARTSYSTEM_H
#define _DGL_DLLSTARTSYSTEM_H

#include "DemoGL\dgl_dllsoundsystem.h"

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////
// Stringlength definition for SStartupDat
#define DGL_SD_STRINGLENGTH			256

////////////////////////////////////////////////////////////////////////
//                          
//						TYPEDEFS
//
////////////////////////////////////////////////////////////////////////
// Purpose: startup parameters structure. The contents of this structure is used to fill in the
// possible options of the startup dialog or use them for application execution when a DEMOGL_AppRun() variant is 
// used that doesnt show a startupdialog but just picks the settings supplied in this structure.
typedef struct
{
	bool			m_bFullScreen;			// false: windowed mode is used.
	int				m_iResolution;			// one of the resolution constants.
	bool			m_bDQ32bit;				// false: 16bit colorbuffer depth is used
	bool			m_bTQ32bit;				// false: 16bit texturedata hint is passed to OpenGL
	bool			m_bSound;				// false: no sound will be played. no soundsystem will be initialized
	char			m_sDemoName[DGL_SD_STRINGLENGTH];
	char			m_sReleasedBy[DGL_SD_STRINGLENGTH];
	bool			m_bSaveInRegistry;		// true: save dialog settings in registry
	bool			m_bRescaleTextures;		// true: rescale textures to fit maximum size. Needed for voodoo3 cards.
	bool			m_bSS_3DSound;			// true: 3D calculations are processed on 3D enabled sound elements.
	int				m_iSS_SoundDevice;		// device nr. See BASS documentation.
	bool			m_bSS_Stereo;			// false: mono
	bool			m_bSS_16bit;			// false: 8 bit
	bool			m_bSS_LowQuality;		// false: 44.1khz
	bool			m_bSS_A3D;				// false: no A3D support.
	bool			m_bSS_EAX;				// false: no EAX support.
	int				m_iSS_OverallVolume;	// the value of the overal volume of the output. 0-100.

	// ADD MORE STARTUPDIALOG PROPERTIES HERE. DON'T FORGET TO ADD THESE NEW OPTIONS TO DemoGL_DLL.h,
	// AND TO Load/SaveStartupDatFromRegistry (both defined in dgl_dllstartupdialog.cpp) TOO!
} SStartupDat;


// Purpose: screensaver type struct. when the application should run as a RUNTYPE_SAVER_* variant the commandline should
// be parsed and on the commandline are parameters passed that will flag which variant of the screensaver type execution 
// should be ran: config dialog, preview on desktop properties dialog, fullscreen preview, normal saver execution.
// DEMOGL_ParseScreenSaverCL() will return a pointer to this structure, which will inform the caller how to call 
// DEMOGL_AppRun() correctly.
typedef struct
{
	HWND	hWnd;
	bool	bInitialized;
	int		iSaverExecType;
} SScreenSaverCLParams;


////////////////////////////////////////////////////////////////////////
//                          
//						FILE SCOPE FUNCTION DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
int	DoAppRun_Normal(const HINSTANCE hInstance, const SStartupDat *psdStartupDat, const char *pszWindowTitle, 
								const bool bUseCrypting, const byte byKey1, const byte byKey2);
int	DoAppRun_NoDialog(const HINSTANCE hInstance, const SStartupDat *psdStartupDat, const char *pszWindowTitle, 
								const bool bUseCrypting, const byte byKey1, const byte byKey2);
int	DoAppRun_Saver(const HINSTANCE hInstance, const SStartupDat *psdStartupDat, const char *pszWindowTitle, 
								bool bUseCrypting, const byte byKey1, const byte byKey2, const int iSaverExecType);

////////////////////////////////////////////////////////////////////////
//                          
//						FUNCTION DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
extern	DWORD WINAPI		InitApp(LPVOID pParam);
extern	DWORD WINAPI		InitSystem(LPVOID pParam);
extern	DWORD WINAPI		Prepare(LPVOID pParam);

extern	int					InitAllRegisteredEffectObjects(void);

extern	DLLEXPORT	int		DEMOGL_AppRun(const HINSTANCE hInstance, const SStartupDat *psdStartupDat,
								const char *pszWindowTitle, const bool bUseCrypting, const byte byKey1, 
								const byte byKey2, const int iRunType);
/////////////////////////////////[IMPORTANT NOTICE]/////////////////////////////////////////////////////
// The functions below are marked 'deprecated' from version 1.3 but still supported for backwards compatibility
// They are just wrappers around the new DEMOGL_ versions so they don't take much DLL space.
//
//				    >>> Please use the new DEMOGL_* functions in your new code <<<
//
/////////////////////////////////[IMPORTANT NOTICE]/////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
	extern	DLLEXPORT	void RunApp(HINSTANCE hInstance, const char *sWindowTitle, bool bUseCrypting, byte byKey1, byte byKey2);	// Deprecated from v1.3
	extern	DLLEXPORT	void OverruleDefaults(bool bVSYNC, int iFileSRC, const char *sSRCDir, const char *sDataFilename, const char *sScriptFilename, 
								bool bShowFPS, bool bNeverShowFPS, bool bNeverShowConsole, bool bShowDebugInfo, bool bNeverShowDebugInfo, int iMusicSys);	// Deprecated from v1.3
	extern	DLLEXPORT	void SetStartupDialogVars(const char *sDemoName, const  char *sCreators, bool bFullScreen, 
								int iResolution, bool bDisplay32bpp, bool bTexture32bpp, bool bSound);	// Deprecated from v1.3
#ifdef __cplusplus
}
#endif // __cplusplus
/////////////////////////////////[IMPORTANT NOTICE]/////////////////////////////////////////////////////
// The functions below are marked 'deprecated' from version 1.3 but still supported for backwards compatibility
// PLEASE DON'T USE THESE FUNCTIONS IN YOUR OWN FUNCTIONS. THESE FUNCTIONS ARE ONLY USED BY DEPRECATED FUNCTIONS.
/////////////////////////////////[IMPORTANT NOTICE]/////////////////////////////////////////////////////
extern	void InitializeStartupDialogDefaults(void);	

#endif	// _DGL_DLLSTARTSYSTEM_H