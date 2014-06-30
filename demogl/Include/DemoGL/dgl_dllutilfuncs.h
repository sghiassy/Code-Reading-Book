//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllutilfuncs.h
// PURPOSE: include file of dgl_dllutilfuncs.cpp
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

#ifndef _DGL_DLLUTILFUNCS_H
#define _DGL_DLLUTILFUNCS_H

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////
#define MAXPATHFILENAMELENGTH		2048

////////////////////////////////////////////////////////////////////////
//                          
//						FILE SCOPE FUNCTION DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
inline	long				DoFileLengthLookup(FILE *fp);
		long				DoFileLengthInDataFileLookup(char *sPathAndFile);
		byte				*LoadFileFromDataFile(char *sPathAndFile);

////////////////////////////////////////////////////////////////////////
//                          
//						FUNCTION DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
extern	void				CallEnabledEventHandlers(HWND hWnd, UINT uMsg, WPARAM  wParam, LPARAM  lParam);
extern	bool				CheckOSType(int iOSType);
extern	void				DecryptBuffer(long lBufferLength, byte *pBuffer);
extern	void				DetermineSSaverWorkingDir(void);
extern	void				DisplayHelp(void);
extern	void				DisplayRegisteredObjectsInConsole(void);
extern	void				EffectuateVariable_VSYNC(void);
extern	void				ExecuteInputBuffer(void);
extern	long				FFileLength(const char *sFilename);
extern	void				FillConsoleHeader(void);
extern	byte				*FLoadFile(const char *sFilename);
extern	int					GenerateRegistryKeyName(char *pszDemoName);
extern	CEffect				*GetEffectObject(char *sEffectName);
extern	CEffectStore		*GetEffectStoreObject(const char *sEffectName);
extern	void				IncLoadingPBMaxValue(int iAmount);
extern	void				IncLoadingPBValue(int iAmount);
extern	void				ListICDInfo(void);
extern	void				LoadStartupDatFromRegistry(SStartupDat *pStartupDat);
extern	void				LogFeedback(char *sText,bool bScroll, char *sFunction, bool bNotIfQuickBooting, bool bUpdateScreen=true);
extern	char				*ReadWorkingDirFromRegistry(void);
extern	void				RedrawTheWindow(void);
extern	void				RemoveEffectStore(char *sEffectName);
extern	void				SaveStartupDatInRegistry(SStartupDat *pStartupDat);
extern	void				SwapSlashesForZip(CStdString *sPathAndFile);
extern	void				ToggleConsole(void);
extern	void				ToggleDebugInfoDisplay(void);
extern	void				ToggleFPSDisplay(void);
extern	void				ToggleInputPrompt(void);
extern	void				UpdateScreenIfNecessary(void);

//////////////////////////////////////////////////////////
//                    functions also exported from the dll.
//////////////////////////////////////////////////////////
extern	DLLEXPORT	void	DEMOGL_ConsoleLogLine(const bool bScroll, const char *pszLine);
extern	DLLEXPORT	void	DEMOGL_ConsoleLogLineV(const bool bScroll, const char *pszFormat, ...);
extern	DLLEXPORT	int		DEMOGL_ConsoleSetAppearance(const int iAmCharsPerLine, const int iAmLinesInBuffer,
									const int iAmLinesOnScreen, const int iAmLinesInDebugInfo);
extern	DLLEXPORT	void	DEMOGL_ExecuteCommand(const char *pszCommand);
extern	DLLEXPORT	bool	DEMOGL_ExtensionCheckIfAvailable(const int iExtension);
extern	DLLEXPORT	bool	DEMOGL_ExtensionCheckIfAvailable(const char *pszExtensionName);
extern	DLLEXPORT	void	DEMOGL_FileFree(const byte *pbyFile);
extern	DLLEXPORT	long	DEMOGL_FileGetLength(const char *pszFilename);
extern	DLLEXPORT	byte	*DEMOGL_FileLoad(const char *pszFilename);
extern	DLLEXPORT	void	DEMOGL_GetVersion(float * const fVersion, long * const lBuildnr);
extern	DLLEXPORT	void	DEMOGL_GetWindowSpecificHandles(HWND * const hWndMainWindow, HDC * const hDCMainWindow);
extern	DLLEXPORT	void	DEMOGL_LoadingPBEnable(const int iType, const float fWidth, const float fHeight,
									const float fBottomLeftX, const float fBottomLeftY);
extern	DLLEXPORT	void	DEMOGL_LoadingPBSetAppearance(const bool bHorizontal, const bool bBlend, 
									const GLenum iBlendSrc, const GLenum iBlendDst, const float fOpacity,
									const float fBGColor_R, const float fBGColor_G,	const float fBGColor_B, 
									const float fBarColor_R, const float fBarColor_G, const float fBarColor_B);
extern	DLLEXPORT	void	DEMOGL_LoadingSplashEnable(const char *pszSplashScreenTexture, const int iFileType, 
									const float fOpacity);
extern	DLLEXPORT	void	DEMOGL_MessageHandlerEnable(const char *pszEffectName);
extern	DLLEXPORT	void	DEMOGL_MessageHandlerDisable(const char *pszEffectName);
extern	DLLEXPORT	SScreenSaverCLParams	*DEMOGL_ParseScreenSaverCL(const LPSTR lpCmdLine);
extern	DLLEXPORT	void	DEMOGL_SetVariable(const int iFlag, const int iValue);
extern	DLLEXPORT	void	DEMOGL_SetVariable(const int iFlag, const bool bValue);
extern	DLLEXPORT	void	DEMOGL_SetVariable(const int iFlag, const char *pszValue);

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
	extern	DLLEXPORT void LogLineToConsole(const char *sLine, bool bScroll);
	extern	DLLEXPORT long GetLengthOfFile(const char *sFilename);
	extern	DLLEXPORT byte *LoadFile(const char *sFilename);
	extern	DLLEXPORT bool IsExtensionAvailable(int iExtension);
	extern	DLLEXPORT void GetDemoGLVersion(float *fVersion, long *lBuildnr);
	extern	DLLEXPORT void EnableMessageHandler(const char *sEffectName);
	extern	DLLEXPORT void DisableMessageHandler(const char *sEffectName);
	extern	DLLEXPORT void EnableLoadingSplash(const char *sSplashScreenTexture, int iFileType, float fOpacity);
	extern	DLLEXPORT void FreeFile(byte *byFile);
	extern	DLLEXPORT void ExecuteCommand(const char *sCommand);
	extern	DLLEXPORT void SetbSwapBuffers(bool bYN);
#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _DGL_DLLUTILFUNCS_H