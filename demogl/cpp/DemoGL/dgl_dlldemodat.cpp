//////////////////////////////////////////////////////////////////////
// FILE: dgl_dlldemodat.cpp
// PURPOSE: Implementation of the CDemoDat class.
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// In here the CDemoDat class is declared. It's used to store the app related
// data.
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
// v1.3: Added a lot new features.
// v1.2: Some fixes
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////
// Purpose: constructor
CDemoDat::CDemoDat()
{
	// Initialize variables. This has to be done once, so no private method 
	// constructed.

	// system state related
	m_iSystemState=SSTATE_PREBOOT;
	m_bLoadingPBEnabled=false;
	m_iRunType=RUNTYPE_NORMAL;			// with startup dialog.
	m_bExecPriorStartTimeTLEsAtReset=false;	
	m_pCurrentTLE=NULL;

	// timer related
	m_lFramesSinceStart = 0;
	m_iFramesThisSecond=0;
	m_bTickerStarted=false;
	m_lMilliSecondsPassed=0;
	m_lMilliSecondsLastTime=0;
	m_lTickFrequency=0;
	m_lTickerStart=0;
	m_fElapsedTimeInSecs=0;

	// window/rc/instance related
	m_iWindowWidth=0;
	m_iWindowHeight=0;
	m_iTexturePixelDepthHint=0;
	m_hInstance=NULL;
	m_bSwapBuffers=true;
	m_hWnd=NULL;
	m_hDC=NULL;
	m_hRC=NULL;
	m_iDepth=0;
	m_iTexDepth=0;
	m_bAnOverlayWasSwitchedOff=false;
	m_bUseCrypting=false;
	m_byKey1=0;
	m_byKey2=0;
	m_lDecryptionKey=0;
	m_iPixelFormatNo=0;
	m_iPixelFormatZDepth=0;
	m_iPixelFormatColorDepth=0;
	m_iPixelFormatStencilDepth=0;
	m_lCurrentTimeAtTickerReset=0;
	m_bSoundCanBeEnabled=false;
	m_sRegistryKeyName.Format("%s","");

	// misc variables related
	m_sWorkingDir.Format("%s","");
	m_sOGLRenderer.Format("%s","");
	m_sOGLVendor.Format("%s","");
	m_sOGLVersion.Format("%s","");
	m_sOGLExtensions.Format("%s","");
	m_iOGLMaxTextureSize=256;				// set max texture size to 256x256. This is a legacy compatability variable.
	m_iTexNameTexIDDelta=1;					// delta between first OGLtexture name (default 1) and m_garrpTextures[] base (0)

	// Loading splash related
	m_bLoadingSplashEnabled=false;
	m_iLoadingSplashFileType=0;
	m_fLoadingSplashOpacity=0.0f;

	// screen saver attributes related
	m_sclpSSCLParams.bInitialized=false;
	m_sclpSSCLParams.hWnd=NULL;
	m_sclpSSCLParams.iSaverExecType=DGL_SSAVERET_UNKNOWN;

	// flags that will be used to actually display the elements they represent
	// These flags get set when the visible flag is set and/or their lockflag 
	// For example: m_bFPSVisible gets set when m_bShowFPS (visible) or m_bNeverShowFPS (locker)
	// is set.
	// These flags also get set when the user presses a key that will display the element they represent.
	m_bConsoleVisible=true;
	m_bFPSVisible=false;
	m_bInputPromptVisible=false;
	m_bDebugInfoVisible=false;

	/////////////////////////////////////////////
	// User overrulable defaults. Values are defined in dgl_dllmain.h
	/////////////////////////////////////////////
	m_sDataFilename.Format("%s",DGL_DEFAULT_DATAFILENAME);
	m_sScriptFilename.Format("%s",DGL_DEFAULT_SCRIPTFILENAME);
	m_bShowFPS=DGL_DEFAULT_SHOWFPS;
	m_sSrcDir.Format("%s",DGL_DEFAULT_SRCDIR);
	m_bShowDebugInfo=DGL_DEFAULT_SHOWDEBUGINFO;
	m_bVSYNC=DGL_DEFAULT_VSYNC;
	m_bNeverShowDebugInfo=DGL_DEFAULT_NEVERSHOWDEBUGINFO;
	m_bNeverShowFPS=DGL_DEFAULT_NEVERSHOWFPS;
	m_bNeverShowConsole=DGL_DEFAULT_NEVERSHOWCONSOLE;
	m_bNeverRescaleTextures=DGL_DEFAULT_NEVERRESCALETEXTURES;
	m_iFileSrc=DGL_DEFAULT_FILESRC;
	m_iMusicSubSystemType = DGL_DEFAULT_MUSICSYS;
	m_bQuickBoot=DGL_DEFAULT_QUICKBOOT;
	m_bUseRenderFrameEx=DGL_DEFAULT_USERENDERFRAMEEX;
	m_iInitFailureResponse=DGL_DEFAULT_INITFAILURERESPONSE;
	// ADD MORE USER OVERRULABLE DEFAULTS HERE.

	// Initialize Startupdat object
	ZeroMemory((void *)&m_ssdStartupDat,sizeof(SStartupDat));

	// internal Sound System Object dat object related
	m_ssoSSODat.m_bSound_Possible=false;
	m_ssoSSODat.m_iAmDevices=0;

	// perform additional init actions
	InitializeCriticalSection(&m_csSystemStateMutex);
	ClearInputBuffer();
}


// Purpose: destructor
CDemoDat::~CDemoDat()
{
	DeleteCriticalSection(&m_csSystemStateMutex);
}


// Purpose: gets the generated Registry key name
char
*CDemoDat::GetRegistryKeyName()
{
	return &m_sRegistryKeyName[0];
}


// Purpose: sets the generated registry key name
void
CDemoDat::SetRegistryKeyName(char *pszKeyName)
{
	m_sRegistryKeyName.Format("%s",pszKeyName);
}


// Purpose: sets the TexNameTexIDDelta variable
void
CDemoDat::SetTexNameTexIDDelta(int iTexNameTexIDDelta)
{
	m_iTexNameTexIDDelta=iTexNameTexIDDelta;
}


// Purpose: gets the TexNameTexIDDelta variable
int
CDemoDat::GetTexNameTexIDDelta()
{
	return m_iTexNameTexIDDelta;
}


// Purpose: returns the OGL max texture size variable
int
CDemoDat::GetOGLMaxTextureSize()
{
	return m_iOGLMaxTextureSize;
}


// Purpose: sets the OGL max texture size variable
void
CDemoDat::SetOGLMaxTextureSize(int iMaxTextureSize)
{
	m_iOGLMaxTextureSize=iMaxTextureSize;
}


// Purpose: Gets the opengl version string
char
*CDemoDat::GetOGLVersion()
{
	return &m_sOGLVersion[0];
}


// Purpose: Sets the opengl version string
void
CDemoDat::SetOGLVersion(char *pszOGLVersion)
{
	if(pszOGLVersion)
	{
		m_sOGLVersion.Format("%s",pszOGLVersion);
	}
}


// Purpose: Gets the opengl vendor string
char
*CDemoDat::GetOGLVendor()
{
	return &m_sOGLVendor[0];
}


// Purpose: Sets the opengl vendor string
void
CDemoDat::SetOGLVendor(char *pszOGLVendor)
{
	if(pszOGLVendor)
	{
		m_sOGLVendor.Format("%s",pszOGLVendor);
	}
}


// Purpose: Gets the opengl Renderer string
char
*CDemoDat::GetOGLRenderer()
{
	return &m_sOGLRenderer[0];
}


// Purpose: Sets the opengl Renderer string
void
CDemoDat::SetOGLRenderer(char *pszOGLRenderer)
{
	if(pszOGLRenderer)
	{
		m_sOGLRenderer.Format("%s",pszOGLRenderer);
	}
}


// Purpose: Gets the opengl Extensions string
char
*CDemoDat::GetOGLExtensions()
{
	return &m_sOGLExtensions[0];
}


// Purpose: Sets the opengl Extensions string
void
CDemoDat::SetOGLExtensions(char *pszOGLExtensions)
{
	if(pszOGLExtensions)
	{
		// Inserts a space behind the string for the extension string parser.
		m_sOGLExtensions.Format("%s ",pszOGLExtensions);
	}
}


// Purpose: sets the flag that signals prior starttime execution of TLE's at reset
void
CDemoDat::SetbExecPriorStartTimeTLEsAtReset(bool bYN)
{
	m_bExecPriorStartTimeTLEsAtReset=bYN;
}


// Purpose: gets the flag that signals prior starttime execution of TLE's at reset
bool
CDemoDat::GetbExecPriorStartTimeTLEsAtReset()
{
	return m_bExecPriorStartTimeTLEsAtReset;
}


// Purpose: Sets the init failure response.
void
CDemoDat::SetInitFailureResponse(int iResponse)
{
	m_iInitFailureResponse=iResponse;
}


// Purpose: retrieves the init failure response
int
CDemoDat::GetInitFailureResponse()
{
	return m_iInitFailureResponse;
}


// Purpose: sets bUseRenderFrameEx value. 
// (version 1.3 uses a new RenderFrame() method in CEffect baseclass). 
void
CDemoDat::SetbUseRenderFrameEx(bool bYN)
{
	m_bUseRenderFrameEx=bYN;
}


// Purpose: gets bUseRenderFrameEx value
bool
CDemoDat::GetbUseRenderFrameEx()
{
	return m_bUseRenderFrameEx;
}


// Purpose: retrieves if the sound enable control may be enabled on the configdialog. Is settable through pStartupDat.
bool
CDemoDat::GetbSoundCanBeEnabled()
{
	return m_bSoundCanBeEnabled;
}


// Purpose: set the value that will be used to reset the timerticker when that timer is reset (at the start of the
// kernelloop.) This way we can tweak what the value will be when we restart the application. Time in milliseconds
void
CDemoDat::SetCurrentTimeAtTickerReset(long lTimePassed)
{
	m_lCurrentTimeAtTickerReset=lTimePassed;
}


// Purpose: gets the value that will be used to reset the timerticker when that timer is reset. In Milliseconds
long
CDemoDat::GetCurrentTimeAtTickerReset()
{
	return m_lCurrentTimeAtTickerReset;
}


// Purpose: returns the value of the pCurrentTLE pointer, thus a pointer to the
// current active TimeLineEvent.
CTimeLineEvent
*CDemoDat::GetCurrentTLE()
{
	return m_pCurrentTLE;
}


// Purpose: sets the value of the current TLE, which is the current TimeLineEvent that is
// not executed and thus first in line to get executed.
void
CDemoDat::SetCurrentTLE(CTimeLineEvent *pCurrentTLE)
{
	m_pCurrentTLE=pCurrentTLE;
}


// Purpose: sets the workingdir of the current running application. Only used in RUNTYPE_SAVER_* mode.
void
CDemoDat::SetWorkingDir(char *pszWorkingDir)
{
	m_sWorkingDir.Format("%s",pszWorkingDir);
}


// Purpose: returns the workingdir as stored in DemoDat. Only used in RUNTYPE_SAVER_* mode.
char
*CDemoDat::GetWorkingDir()
{
	return &m_sWorkingDir[0];
}


// Purpose: returns a constant pointer to the m_sclpSSCLParams struct, located inside CDemoDat.
// This struct is used to store commandline parameters passed to the application when in RUNTYPE_SAVER_*.
SScreenSaverCLParams
*CDemoDat::GetSSCLParams()
{
	return &m_sclpSSCLParams;
}


// Purpose: sets the runtype
void
CDemoDat::SetRunType(int iRunType)
{
	m_iRunType=iRunType;
}


// Purpose: gets the runtype
int
CDemoDat::GetRunType()
{
	return m_iRunType;
}


// Purpose: sets the default QuickBoot
void
CDemoDat::SetbQuickBoot(bool bYN)
{
	m_bQuickBoot=bYN;
}


// Purpose: retrieves value of Quickboot
bool
CDemoDat::GetbQuickBoot()
{
	return m_bQuickBoot;
}


// Purpose: sets bLoadingPBEnabled member
void
CDemoDat::SetbLoadingPBEnabled(bool bYN)
{
	m_bLoadingPBEnabled=bYN;
}


// Purpose: gets bLoadingPBEnabled member
bool
CDemoDat::GetbLoadingPBEnabled()
{
	return m_bLoadingPBEnabled;
}


// Purpose: sets the hInstance of the application (for resource loading)
void
CDemoDat::SethInstance(HINSTANCE hInstance)
{
	m_hInstance=hInstance;
}


// Purpose: get SSoundSystemOptionDat pointer
SSoundSystemOptionDat
*CDemoDat::GetSSODat()
{
	return &m_ssoSSODat;
}


// Purpose: sets pixelformat descriptor data
void
CDemoDat::SetPixelFormatData(int iPixelFormatNo, int iPixelFormatZDepth, int iPixelFormatStencilDepth, int iPixelFormatColorDepth)
{
	m_iPixelFormatNo=iPixelFormatNo;
	m_iPixelFormatZDepth=iPixelFormatZDepth;
	m_iPixelFormatStencilDepth=iPixelFormatStencilDepth;
	m_iPixelFormatColorDepth=iPixelFormatColorDepth;
}


// Purpose: gets m_iPixelFormatNo
int
CDemoDat::GetPixelFormatNo()
{
	return m_iPixelFormatNo;
}


// Purpose: gets m_iPixelFormatZDepth
int
CDemoDat::GetPixelFormatZDepth()
{
	return m_iPixelFormatZDepth;
}


// Purpose: gets m_iPixelFormatColorDepth
int
CDemoDat::GetPixelFormatColorDepth()
{
	return m_iPixelFormatColorDepth;
}


// Purpose: gets m_iPixelFormatStencilDepth
int
CDemoDat::GetPixelFormatStencilDepth()
{
	return m_iPixelFormatStencilDepth;
}


// Purpose: get the value of bNeverRescaleTextures
bool
CDemoDat::GetbNeverRescaleTextures()
{
	return m_bNeverRescaleTextures;
}


// Purpose: set the value of bNeverRescaleTextures
void
CDemoDat::SetbNeverRescaleTextures(bool bYN)
{
	m_bNeverRescaleTextures=bYN;
}


// Purpose: sets the pointer to the startupdata structure, needed for startup dialogs and
// startup initialization routines. It also initializes some variables stored in CDemoDat with the
// data in pStartupDat.
void
CDemoDat::SetStartupDat(const SStartupDat *pStartupDat)
{
	// make duplicate of data passed to method.
	CopyMemory((void *)&m_ssdStartupDat, pStartupDat, sizeof(SStartupDat));

	// Set flag for soundenable control in dialog. This is controlled by the m_bSound flag in StartupDat
	// structure.
	m_bSoundCanBeEnabled=pStartupDat->m_bSound;
}


// Purpose: gets the set pointer to the startupdat struct
SStartupDat
*CDemoDat::GetStartupDat()
{
	return &m_ssdStartupDat;
}


// Purpose: adds a char at the end of the inputbuffer (visible when pressing F6 in debugmode)
void
CDemoDat::AddCharToInputBuffer(char cCH)
{
	if(m_iIBLength>=IB_MAXLINELENGTH)
	{
		// buffer full...
		return;
	}
	// add character to string.
	m_sInputBuffer[m_iIBLength]=cCH;
	// pad with 0. There is room for this 0.
	m_sInputBuffer[++m_iIBLength]='\0';
}


// Purpose: sets loadingsplash properties
void
CDemoDat::SetLoadingSplashParams(const char *pszSplashTexture, int iFileType, float fOpacity)
{
	if(pszSplashTexture)
	{
		m_sLoadingSplashTextureName.Format("%s",pszSplashTexture);
		m_iLoadingSplashFileType=iFileType;
		m_bLoadingSplashEnabled=true;
		m_fLoadingSplashOpacity=fOpacity;
	}
}


// Purpose: gets loadingsplash enabled
bool
CDemoDat::GetbLoadingSplashEnabled()
{
	return m_bLoadingSplashEnabled;
}


// Purpose: gets loadingsplashopacity
float
CDemoDat::GetLoadingSplashOpacity()
{
	return m_fLoadingSplashOpacity;
}


// Purpose: gets the loadingsplash texturefilename
char
*CDemoDat::GetLoadingSplashTextureName()
{
	if(m_bLoadingSplashEnabled)
	{
		return &m_sLoadingSplashTextureName[0];
	}
	else
	{
		return NULL;

	}
}


// Purpose: gets the filetype of the texturefile
int
CDemoDat::GetLoadingSplashFileType()
{
	return m_iLoadingSplashFileType;
}


// Purpose: deletes last character in inputbuffer. (a backspace)
void
CDemoDat::DelLastCharFromInputBuffer()
{
	if(m_iIBLength>0)
	{
		m_sInputBuffer[--m_iIBLength]='\0';
	}
}


// Purpose: Clears the inputbuffer.
void
CDemoDat::ClearInputBuffer()
{
	m_iIBLength=0;
	memset(m_sInputBuffer,0,IB_MAXLINELENGTH);
}


// Purpose: returns a pointer to the complete inputbuffer.
char
*CDemoDat::GetInputBuffer()
{
	return &m_sInputBuffer[0];
}


// Purpose: returns the length of the inputbuffer
int
CDemoDat::GetInputBufferLength()
{
	return m_iIBLength;
}


// Purpose: Toggles bInputPromptVisible
void
CDemoDat::ToggleInputPrompt()
{
	m_bInputPromptVisible=!m_bInputPromptVisible;
}


// Purpose: returns bInputPromptVisible
bool
CDemoDat::GetbInputPromptVisible()
{
	return m_bInputPromptVisible;
}


// Purpose: sets musicsubsystemtype		// DEPRECATED FROM v1.3
void
CDemoDat::SetMusicSubSystemType(int iType)
{
	m_iMusicSubSystemType=iType;
}


// Purpose: gets musicsubsystemtype		// DEPRECATED FROM v1.3
int
CDemoDat::GetMusicSubSystemType()
{
	return m_iMusicSubSystemType;
}


// Purpose: get Datafilename
char
*CDemoDat::GetDataFilename()
{
	return &m_sDataFilename[0];
}


// Purpose: get scriptfilename
char
*CDemoDat::GetScriptFilename()
{
	return &m_sScriptFilename[0];
}


// Purpose: Get bVSYNC
bool
CDemoDat::GetbVSYNC()
{
	return m_bVSYNC;
}


// Purpose: Get bShowFPS
bool
CDemoDat::GetbShowFPS()
{
	return m_bShowFPS;
}


// Purpose: get showdebuginfo
bool
CDemoDat::GetbShowDebugInfo()
{
	return m_bShowDebugInfo;
}


// Purpose: get nevershowdebuginfo
bool
CDemoDat::GetbNeverShowDebugInfo()
{
	return m_bNeverShowDebugInfo;
}


// Purpose: get nevershowconsole
bool
CDemoDat::GetbNeverShowConsole()
{
	return m_bNeverShowConsole;
}


// Purpose: get nevershowFPS
bool
CDemoDat::GetbNeverShowFPS()
{
	return m_bNeverShowFPS;
}


// Purpose: get the FileSrc
int
CDemoDat::GetFileSrc()
{
	return m_iFileSrc;
}


// Purpose: get the SrcDir
char
*CDemoDat::GetSrcDir()
{
	return &m_sSrcDir[0];
}


// Purpose set datafile
void
CDemoDat::SetDataFilename(const char *pszDataFilename)
{
	if(pszDataFilename)
	{
		m_sDataFilename.Format("%s", pszDataFilename);
	}
}


// Purpose set scriptfilename
void
CDemoDat::SetScriptFilename(const char *pszScriptFilename)
{
	if(pszScriptFilename)
	{
		m_sScriptFilename.Format("%s", pszScriptFilename);
	}
}


// Purpose: Sets bVSYNC
void
CDemoDat::SetbVSYNC(bool bYN)
{
	m_bVSYNC=bYN;
	if(m_hRC)
	{
		// effectuate the value set 
		EffectuateVariable_VSYNC();
	}
}


// Purpose: sets bShowFPS
void
CDemoDat::SetbShowFPS(bool bYN)
{
	m_bShowFPS=bYN;
	if(!m_bNeverShowFPS)
	{
		m_bFPSVisible=m_bShowFPS;
		// now tell the RenderFrame routine it should clear the buffer before rendering stuff
		m_bAnOverlayWasSwitchedOff = !m_bFPSVisible;
	}
	else
	{
		m_bFPSVisible=false;
	}
}


// Purpose: sets bShowDebugInfo
void
CDemoDat::SetbShowDebugInfo(bool bYN)
{
	m_bShowDebugInfo=bYN;
	if(!m_bNeverShowDebugInfo)
	{
		m_bDebugInfoVisible=m_bShowDebugInfo;
		// now tell the RenderFrame routine it should clear the buffer before rendering stuff
		m_bAnOverlayWasSwitchedOff = !m_bDebugInfoVisible;
	}
	else
	{
		m_bDebugInfoVisible=false;
	}
}


// Purpose: sets bneverShowDebugInfo
void
CDemoDat::SetbNeverShowDebugInfo(bool bYN)
{
	m_bNeverShowDebugInfo=bYN;
	if(bYN)
	{
		m_bDebugInfoVisible=false;
	}
}


// Purpose: sets bneverShowConsole
void
CDemoDat::SetbNeverShowConsole(bool bYN)
{
	m_bNeverShowConsole=bYN;
}


// Purpose: sets bneverShowFPS
void
CDemoDat::SetbNeverShowFPS(bool bYN)
{
	m_bNeverShowFPS=bYN;
	if(bYN)
	{
		m_bFPSVisible=false;
	}
}


// Purpose: sets Filesrc
void
CDemoDat::SetFileSrc(int iSrc)
{
	m_iFileSrc = iSrc;
}


// Purpose: sets SrcDir
void
CDemoDat::SetSrcDir(const char *pszSrcDir)
{
	if(pszSrcDir)
	{
		m_sSrcDir.Format("%s",pszSrcDir);
	}
}


// Purpose: sets the specs of the app
void
CDemoDat::SetAppSpecs(int iWidth, int iHeight, int iDepth, int iTexDepth, HINSTANCE hInstance, GLint iPixelDepthHint)
{
	m_iWindowWidth = iWidth;
	m_iWindowHeight = iHeight;
	m_iDepth = iDepth;
	m_iTexDepth=iTexDepth;
	m_hInstance = hInstance;
	m_iTexturePixelDepthHint = iPixelDepthHint;
}


// Purpose: sets window related datavars like dc, rc and wnd handle
void
CDemoDat::SetWindowVars(HWND hWnd, HDC hDC, HGLRC hRC)
{
	m_hWnd = hWnd;
	m_hDC = hDC;
	m_hRC = hRC;
}


// Purpose: sets system state
void
CDemoDat::SetSystemState(int iState)
{
	EnterCriticalSection(&m_csSystemStateMutex);
		m_iSystemState = iState;
	LeaveCriticalSection(&m_csSystemStateMutex);
}


// purpose: get system state
int
CDemoDat::GetSystemState()
{
	return	m_iSystemState;
}


// Purpose: sets bSwapBuffers
void
CDemoDat::SetbSwapBuffers(bool bYN)
{
	m_bSwapBuffers=bYN;
}


// Purpose: gets bSwapBuffers
bool
CDemoDat::GetbSwapBuffers()
{
	return m_bSwapBuffers;
}


// Purpose: calc frames per second.
float
CDemoDat::CalcFPS()
{
	float	fFPS;

	// calculate framespersecond
	m_lFramesSinceStart++;
	m_iFramesThisSecond++;
	m_lMilliSecondsPassed +=  ((long)(m_fElapsedTimeInSecs * 1000)) - m_lMilliSecondsLastTime;
	m_lMilliSecondsLastTime = (long)(m_fElapsedTimeInSecs * 1000);
	if(m_lMilliSecondsPassed)
	{
		fFPS = ((float)m_iFramesThisSecond)/((float)(m_lMilliSecondsPassed/1000.0f));
	}
	else
	{
		fFPS = 0.0f;
	}
	if(m_lMilliSecondsPassed > 1000)
	{
		// reset frames this second.
		m_iFramesThisSecond = 0;
		m_lMilliSecondsPassed = 0;
	}
	return fFPS;
}


// Purpose: gets iDepth
int
CDemoDat::GetiDepth()
{
	return m_iDepth;
}


// Purpose: gets iTExDepth
int
CDemoDat::GetiTexDepth()
{
	return m_iTexDepth;
}


// Purpose: gets the texturedepth hint
GLint
CDemoDat::GetTexturePixelDepthHint()
{
	return m_iTexturePixelDepthHint;
}


// purpose: gets fElapsedTimeInSecs
float
CDemoDat::GetfElapsedTimeInSecs()
{
	return m_fElapsedTimeInSecs;
}


// purpose: gets bConsoleVisible
bool
CDemoDat::GetbConsoleVisible()
{
	return m_bConsoleVisible;
}


// purpose: gets bFPSVisible
bool
CDemoDat::GetbFPSVisible()
{
	return m_bFPSVisible;
}


// Purpose: gets bDebugInfoVisible
bool
CDemoDat::GetbDebugInfoVisible()
{
	return m_bDebugInfoVisible;
}


// purpose: gets hDC
HDC
CDemoDat::GethDC()
{
	return m_hDC;
}


// purpose: gets hWnd
HWND
CDemoDat::GethWnd()
{
	return m_hWnd;
}


// purpose: gets hRC
HGLRC
CDemoDat::GethRC()
{
	return m_hRC;
}


// purpose: gets instance
HINSTANCE
CDemoDat::GethInstance()
{
	return m_hInstance;
}


// purpose: gets bsound
bool
CDemoDat::GetbSound()
{
	return m_ssdStartupDat.m_bSound;
}


// Purpose: gets bFullScreen
bool
CDemoDat::GetbFullScreen()
{
	return m_ssdStartupDat.m_bFullScreen;
}


// purpose: gets iwindowheight
int
CDemoDat::GetiWindowHeight()
{
	return m_iWindowHeight;
}


// purpose: gets iwindowwidth
int
CDemoDat::GetiWindowWidth()
{
	return m_iWindowWidth;
}


// Purpose: calculates the elapsed time in seconds
void
CDemoDat::CalcElapsedTimeInSeconds()
{
	__int64			lCurrentTicks;

	if(m_bTickerStarted)
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&lCurrentTicks);
		// now we have lTickFrequency ticks per second. We can now calc how long it took since the start. 
		m_fElapsedTimeInSecs=(float) ((double)(lCurrentTicks - m_lTickerStart)/(double)m_lTickFrequency);
	}
}


// purpose: sets bConsoleVisible
void
CDemoDat::SetbConsoleVisible(bool bVisible)
{
	m_bConsoleVisible = bVisible;
	if(!bVisible)
	{
		m_bAnOverlayWasSwitchedOff=true;
	}
}


// Purpose: sets bsound
 void
CDemoDat::SetbSound(bool bYN)
{
	m_ssdStartupDat.m_bSound = bYN;
}


// Purpose: sets up the high resolution ticker. 
// If not available, we can't time, so return SYS_NOK otherwise return  SYS_OK
int
CDemoDat::SetupTicker()
{
	if(!QueryPerformanceFrequency((LARGE_INTEGER *)&m_lTickFrequency))
	{
		// no timer available. 
		return SYS_NOK;
	}
	// get tickerstart.
	QueryPerformanceCounter((LARGE_INTEGER *)&m_lTickerStart);
	m_bTickerStarted=true;
	m_lFramesSinceStart = 0;
	m_iFramesThisSecond=0;
	m_lMilliSecondsPassed=0;
	m_lMilliSecondsLastTime=0;
	m_fElapsedTimeInSecs=0;

	// Insert starttime into the ticker.
	SetTimePassed(m_lCurrentTimeAtTickerReset);
	return SYS_OK;
}


// purpose: toggles the boolean that contains the console visibility
void
CDemoDat::ToggleConsoleVisible()
{
	m_bConsoleVisible=!m_bConsoleVisible;
	m_bAnOverlayWasSwitchedOff = !m_bConsoleVisible;
}


// Purpose: gets the AnOverlayWasSwitchedOff switched off boolean. If true, an overlay (console, fps counter, debuginfo)
// is just switched off and there should be an extra clear of the colorbuffer. That variable is cleared immediately
bool
CDemoDat::GetbAnOverlayWasSwitchedOff()
{
	if(m_bAnOverlayWasSwitchedOff)
	{
		m_bAnOverlayWasSwitchedOff=false;
		return true;
	}
	else
	{
		return false;
	}
}


// Purpose: toggles fps display
void
CDemoDat::ToggleFPSDisplay()
{
	m_bFPSVisible=!m_bFPSVisible;
	// now tell the RenderFrame routine it should clear the buffer before rendering stuff
	m_bAnOverlayWasSwitchedOff = !m_bFPSVisible;
}


// Purpose: toggles debug info display
void
CDemoDat::ToggleDebugInfoDisplay()
{
	m_bDebugInfoVisible=!m_bDebugInfoVisible;
	// now tell the RenderFrame routine it should clear the buffer before rendering stuff
	m_bAnOverlayWasSwitchedOff = !m_bDebugInfoVisible;
}


// purpose: sets windowwidth
void
CDemoDat::SetiWindowWidth(int iWidth)
{
	m_iWindowWidth = iWidth;
}


// Purpose: sets windowheight
void
CDemoDat::SetiWindowHeight(int iHeight)
{
	m_iWindowHeight = iHeight;
}


// Purpose: sets the fps display value
void
CDemoDat::SetbFPSVisible(bool bYN)
{
	m_bFPSVisible = bYN;
}


// Purpose: sets the debuginfo display value
void
CDemoDat::SetbDebugInfoVisible(bool bYN)
{
	m_bDebugInfoVisible = bYN;
}


// Purpose: sets the timerticker to a value so that the call GetfElapsedTimeInSeconds will return the time passed to this function.
void
CDemoDat::SetTimePassed(long lMilliseconds)
{
	__int64		llTimeTicksDelta;
	long		lMillisecondsDelta;

	// the elapsed time is calculated with a ticker. That ticker is checked with a startvalue. the difference
	// of these 2 is an indication how many time has passed. If we move the startvalue up or down, we can set the
	// time to the value that is passed.
	lMillisecondsDelta = lMilliseconds - (long)(m_fElapsedTimeInSecs * 1000.0f);
	llTimeTicksDelta = (__int64)((float)(lMillisecondsDelta/1000.0f) * m_lTickFrequency);
	m_lTickerStart -=llTimeTicksDelta;
}


// Purpose: sets bUseCrypting
void
CDemoDat::SetbUseCrypting(bool bYN)
{
	m_bUseCrypting = bYN;
}


// Purpose: sets the decryption keys
void
CDemoDat::SetKeyValues(byte byKey1, byte byKey2)
{
	m_byKey1 = byKey1; 
	m_byKey2 = byKey2;
}


// Purpose: sets the decryption key
void
CDemoDat::SetDecryptionKey(long lDecryptionKey)
{
	m_lDecryptionKey = lDecryptionKey;
}


// Purpose: gets bUseCrypting
bool
CDemoDat::GetbUseCrypting()
{
	return m_bUseCrypting;
}


// Purpose: gets key1
byte
CDemoDat::GetKey1()
{
	return m_byKey1;
}


// Purpose: gets key2
byte
CDemoDat::GetKey2()
{
	return m_byKey2;
}


// purpose: gets decryption key
long
CDemoDat::GetDecryptionKey()
{
	return m_lDecryptionKey;
}

