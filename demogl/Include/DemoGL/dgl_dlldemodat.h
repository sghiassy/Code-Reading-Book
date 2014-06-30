//////////////////////////////////////////////////////////////////////
// FILE: dgl_dlldemodat.h
// PURPOSE: include file of implementation of the CDemoDat class.
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// In here the CDemoDat class is declared. It's used to store the app 
// and system related data.
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
// v1.3: Added new methods and members
// v1.2: Added new methods and members
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#ifndef _DGL_DLLDEMODAT_H
#define _DGL_DLLDEMODAT_H

#include "DemoGL\dgl_dllsoundsystem.h"
#include "DemoGL\dgl_dlltimelineevent.h"

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////
// Inputbuffer line length max
#define IB_MAXLINELENGTH		150

////////////////////////////////////////////////////////////////////////
//                          
//						CLASS DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
// Purpose: class to hold all system data of the DemoGL system.
class CDemoDat
{
	// PUBLIC MEMBERVARIABLES NOT ALLOWED

	// public methods
	public:
								CDemoDat(void);
		virtual					~CDemoDat(void);
		void					AddCharToInputBuffer(char cCH);
		float					CalcFPS(void);
		void					CalcElapsedTimeInSeconds(void);
		void					ClearInputBuffer(void);
		void					DelLastCharFromInputBuffer(void);
		bool					GetbAnOverlayWasSwitchedOff(void);
		bool					GetbConsoleVisible(void);
		bool					GetbDebugInfoVisible(void);
		bool					GetbExecPriorStartTimeTLEsAtReset(void);
		bool					GetbFPSVisible(void);
		bool					GetbFullScreen(void);
		bool					GetbInputPromptVisible(void);
		bool					GetbLoadingPBEnabled(void);
		bool					GetbLoadingSplashEnabled(void);
		bool					GetbNeverShowDebugInfo(void);
		bool					GetbNeverShowConsole(void);
		bool					GetbNeverShowFPS(void);
		bool					GetbNeverRescaleTextures(void);
		bool					GetbQuickBoot(void);
		bool					GetbShowDebugInfo(void);
		bool					GetbShowFPS(void);
		bool					GetbSound(void);
		bool					GetbSoundCanBeEnabled(void);
		bool					GetbSwapBuffers(void);
		bool					GetbUseCrypting(void);
		bool					GetbUseRenderFrameEx(void);
		bool					GetbVSYNC(void);
		CTimeLineEvent			*GetCurrentTLE(void);
		long					GetCurrentTimeAtTickerReset(void);
		char					*GetDataFilename(void);
		long					GetDecryptionKey(void);
		float					GetfElapsedTimeInSecs(void);
		int						GetFileSrc(void);
		HDC						GethDC(void);
		HINSTANCE				GethInstance(void);
		HGLRC					GethRC(void);
		HWND					GethWnd(void);
		int						GetiDepth(void);
		int						GetInitFailureResponse(void);
		char					*GetInputBuffer(void);
		int						GetInputBufferLength(void);
		int						GetiTexDepth(void);
		int						GetiWindowHeight(void);
		int						GetiWindowWidth(void);
		byte					GetKey1(void);
		byte					GetKey2(void);
		int						GetLoadingSplashFileType(void);
		float					GetLoadingSplashOpacity(void);
		char					*GetLoadingSplashTextureName(void);
		int						GetMusicSubSystemType(void);
		char					*GetOGLExtensions(void);
		int						GetOGLMaxTextureSize(void);
		char					*GetOGLRenderer(void);
		char					*GetOGLVendor(void);
		char					*GetOGLVersion(void);
		int						GetPixelFormatColorDepth(void);
		int						GetPixelFormatNo(void);
		int						GetPixelFormatStencilDepth(void);
		int						GetPixelFormatZDepth(void);
		char					*GetRegistryKeyName(void);
		int						GetRunType(void);
		char					*GetScriptFilename(void);
		char					*GetSrcDir(void);
		SScreenSaverCLParams	*GetSSCLParams(void);
		SSoundSystemOptionDat	*GetSSODat(void);
		SStartupDat				*GetStartupDat(void);
		int						GetSystemState(void);
		int						GetTexNameTexIDDelta(void);
		GLint					GetTexturePixelDepthHint(void);
		char					*GetWorkingDir(void);
		void					SetAppSpecs(int iWidth, int iHeight, int iDepth, int iTexDepth, HINSTANCE hInstance, GLint iPixelDepthHint);
		void					SetbConsoleVisible(bool bVisible);
		void					SetbDebugInfoVisible(bool bYN);
		void					SetbExecPriorStartTimeTLEsAtReset(bool bYN);
		void					SetbFPSVisible(bool bYN);
		void					SetbLoadingPBEnabled(bool bYN);
		void					SetbNeverShowDebugInfo(bool bYN);
		void					SetbNeverShowFPS(bool bYN);
		void					SetbNeverShowConsole(bool bYN);
		void					SetbNeverRescaleTextures(bool bYN);
		void					SetbQuickBoot(bool bYN);
		void					SetbShowFPS(bool bYN);
		void					SetbShowDebugInfo(bool bYN);
		void					SetbSound(bool bYN);
		void					SetbSwapBuffers(bool bYN);
		void					SetbUseCrypting(bool bYN);
		void					SetbUseRenderFrameEx(bool bYN);
		void					SetbVSYNC(bool bYN);
		void					SetCurrentTimeAtTickerReset(long lTimePassed);
		void					SetCurrentTLE(CTimeLineEvent *pCurrentTLE);
		void					SetDataFilename(const char *pszDataFilename);
		void					SetDecryptionKey(long lDecriptKey);
		void					SetFileSrc(int iSrc);
		void					SetInitFailureResponse(int iResponse);
		void					SetiWindowHeight(int iHeight);
		void					SetiWindowWidth(int iWidth);
		void					SethInstance(HINSTANCE hInstance);
		void					SetKeyValues(byte byKey1, byte byKey2);
		void					SetLoadingSplashParams(const char *pszSplashTexture, int iFileType, float fOpacity);
		void					SetMusicSubSystemType(int iType);
		void					SetPixelFormatData(int iPixelFormatNo, int iPixelFormatZDepth, int iPixelFormatStencilDepth, int iPixelFormatColorDepth);
		void					SetOGLExtensions(char *pszOGLExtensions);
		void					SetOGLMaxTextureSize(int iMaxTextureSize);
		void					SetOGLVendor(char *pszOGLVendor);
		void					SetOGLVersion(char *pszOGLVersion);
		void					SetOGLRenderer(char *pszOGLRenderer);
		void					SetRegistryKeyName(char *pszKeyName);
		void					SetRunType(int iRunType);
		void					SetScriptFilename(const char *pszScriptFilename);
		void					SetSrcDir(const char *pszSrcDir);
		void					SetStartupDat(const SStartupDat *pStartupDat);
		void					SetSystemState(int iState);
		void					SetTexNameTexIDDelta(int iTexNameTexIDDelta);
		void					SetTimePassed(long lMilliseconds);		
		int						SetupTicker(void);
		void					SetWindowVars(HWND hWnd, HDC hDC, HGLRC hRC);
		void					SetWorkingDir(char *pszWorkingDir);
		void					ToggleConsoleVisible(void);
		void					ToggleFPSDisplay(void);
		void					ToggleDebugInfoDisplay(void);
		void					ToggleInputPrompt(void);

	// private members
	private:
		_int64					m_lTickFrequency;				// ticks per second of the high resolution counter of the system.
		_int64					m_lTickerStart;					// highres counter start amount.
		float					m_fElapsedTimeInSecs;			// seconds passed since app started, like 3.0003 seconds :)
		long					m_lCurrentTimeAtTickerReset;	// the value of the timepassed in milliseconds when the timerticker will be reset.
		bool					m_bTickerStarted;
		bool					m_bExecPriorStartTimeTLEsAtReset;		// flag that will signal the the kernel starter if TLE's prior startime should be executed after a reset.
		int						m_iWindowWidth;
		int						m_iWindowHeight;
		GLint					m_iTexturePixelDepthHint;		// GL_RGBA8 or GL_RGBA5_A1
		bool					m_bConsoleVisible;
		HINSTANCE				m_hInstance;
		HWND					m_hWnd; 
		HDC						m_hDC; 
		HGLRC					m_hRC; 
		int						m_iDepth;
		int						m_iTexDepth;
		unsigned long			m_lFramesSinceStart;			// amount of frames since start. this can hold several years of frames :)
		int						m_iFramesThisSecond;
		unsigned long			m_lMilliSecondsPassed;
		unsigned long			m_lMilliSecondsLastTime;
		bool					m_bFPSVisible;
		bool					m_bDebugInfoVisible;
		bool					m_bInputPromptVisible;
		bool					m_bAnOverlayWasSwitchedOff;
		bool					m_bLoadingPBEnabled;
		int						m_iRunType;
		bool					m_bSoundCanBeEnabled;

		SSoundSystemOptionDat	m_ssoSSODat;
		SScreenSaverCLParams	m_sclpSSCLParams;

		CStdString				m_sWorkingDir;					// for RUNTYPE_SAVER_*
		CTimeLineEvent			*m_pCurrentTLE;

		CStdString				m_sOGLRenderer;					// from Rendercontext, for feedback to app
		CStdString				m_sOGLVendor;					// from Rendercontext, for feedback to app
		CStdString				m_sOGLVersion;					// from Rendercontext, for feedback to app
		CStdString				m_sOGLExtensions;				// from Rendercontext, for feedback to app and to determine the extensions
		int						m_iOGLMaxTextureSize;

		int						m_iTexNameTexIDDelta;			// delta between 0 and first OGL texture name. Used for textureid generation.

		// startup data support
		SStartupDat				m_ssdStartupDat;

		// pixelformat descriptor values.
		int						m_iPixelFormatNo;
		int						m_iPixelFormatZDepth;
		int						m_iPixelFormatStencilDepth;
		int						m_iPixelFormatColorDepth;
		
		int						m_iMusicSubSystemType;

		char					m_sInputBuffer[IB_MAXLINELENGTH+1];
		int						m_iIBLength;

		// Crypting vars
		bool					m_bUseCrypting;
		byte					m_byKey1;
		byte					m_byKey2;
		long					m_lDecryptionKey;

		int						m_iSystemState;
		CRITICAL_SECTION		m_csSystemStateMutex;			// Critical Section Mutex for SystemState var.
		bool					m_bLoadingSplashEnabled;
		CStdString				m_sLoadingSplashTextureName;
		int						m_iLoadingSplashFileType;
		float					m_fLoadingSplashOpacity;

		CStdString				m_sRegistryKeyName;

		// DEFAULTS of DemoGL which are overrulable.
		CStdString				m_sDataFilename;
		CStdString				m_sScriptFilename;
		bool					m_bVSYNC;
		bool					m_bShowFPS;
		bool					m_bShowDebugInfo;
		bool					m_bNeverShowDebugInfo;
		bool					m_bNeverShowFPS;
		bool					m_bNeverShowConsole;
		int						m_iFileSrc;					// can be SRC_LOADFROMDATAFILE or SRC_LOADFROMDIR
		CStdString				m_sSrcDir;					// is empty if m_iFileSRC = SRC_LOADFROMDATAFILE otherwise the path from where to
		bool					m_bNeverRescaleTextures;
		bool					m_bQuickBoot;
		bool					m_bUseRenderFrameEx;
		int						m_iInitFailureResponse;
		// END DEFAULTS
		
		bool					m_bSwapBuffers;
};

#endif	// _DGL_DLLDEMODAT_H