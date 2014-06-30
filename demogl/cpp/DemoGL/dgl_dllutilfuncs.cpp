//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllutilfuncs.cpp
// PURPOSE: in here are all the utilityfunctions the system has. 
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// this is the file with the utilityfunctions that are floating around in the system.
// if it can't be stored in a dedicated file it's stored here.
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
// v1.3: Added a lot new functions and fixed bugs.
// v1.2: Added a lot new functions and fixed bugs.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#include "Demogl\dgl_dllstdafx.h"
#include "Misc\unzip.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//                      Public routines exported by the library
////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//        DEPRECATED FUNCTIONS. HERE FOR COMPATIBILITY REASONS. 
//            DON'T USE THESE. USE DEMOGL_* VARIANTS INSTEAD
///////////////////////////////////////////////////////////////////////
// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_ConsoleLogLine() instead.
void
LogLineToConsole(const char *sLine, bool bScroll)
{
	DEMOGL_ConsoleLogLine(bScroll, sLine);
}


// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_FileGetLength() instead.
long
GetLengthOfFile(const char *sFilename)
{
	return DEMOGL_FileGetLength(sFilename);
}


// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_FileLoad() instead.
byte
*LoadFile(const char *sFilename)
{
	return DEMOGL_FileLoad(sFilename);
}


// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_ExtensionCheckIfAvailable() instead.
bool
IsExtensionAvailable(int iExtension)
{
	return DEMOGL_ExtensionCheckIfAvailable(iExtension);
}


// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_GetVersion() instead.
void
GetDemoGLVersion(float *fVersion, long *lBuildnr)
{
	DEMOGL_GetVersion(fVersion, lBuildnr);
}


// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_MessageHandlerEnable() instead.
void 
EnableMessageHandler(const char *sEffectName)
{
	DEMOGL_MessageHandlerEnable(sEffectName);
}


// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_MessageHandlerDisable() instead.
void 
DisableMessageHandler(const char *sEffectName)
{
	DEMOGL_MessageHandlerDisable(sEffectName);
}


// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_LoadingSplashEnable() instead.
void
EnableLoadingSplash(const char *sSplashScreenTexture, int iFileType, float fOpacity)
{
	DEMOGL_LoadingSplashEnable(sSplashScreenTexture,iFileType,fOpacity);
}


// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_FileFree() instead.
void
FreeFile(byte *byFile)
{
	DEMOGL_FileFree(byFile);
}


// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_ExecuteCommand() instead.
void 
ExecuteCommand(const char *sCommand)
{
	DEMOGL_ExecuteCommand(sCommand);
}


// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_SetVariable() instead.
void
SetbSwapBuffers(bool bYN)
{
	DEMOGL_SetVariable(DGL_VF_SWAPBUFFERS,bYN);
}


///////////////////////////////////////////////////////////////////////
//                     END DEPRECATED FUNCTIONS.                     //
///////////////////////////////////////////////////////////////////////


// Purpose: sets the console properties. Calling this routine during application execution (thus, from an effect) will
// cause the console contents to be lost. 
// The properties passed will override the build in presets and will reinitialize the console. If you call this routine
// before the application start routines no information will be lost. 
// Returns: SYS_OK if succeeded
//          SYS_NOK if:
// - one or more properties exceeded the minimum values, so the console couldn't be initialized correctly. 
//   see dgl_dllmain.h for these minimum values.
// - the amount of lines on screen is > than iAmLinesInBuffer+CNSL_MINAMLINESONSCREEN
// - the amount of lines in debuginfo exceeds or is equal to the amount of lines in the buffer.
// - the amount of lines in debuginfo is > iAmLinesOnScreen-4. (4 lines are needed for: time, fps and 2 inputlines)
int		
DEMOGL_ConsoleSetAppearance(const int iAmCharsPerLine, const int iAmLinesInBuffer,
				const int iAmLinesOnScreen, const int iAmLinesInDebugInfo)
{
	// check first if the passed properties exceed the minimum 
	if((CNSL_MINAMCHARSPERLINE > iAmCharsPerLine)||(CNSL_MINAMLINESONSCREEN > iAmLinesOnScreen)||
		(CNSL_MINAMLINESINDBGINFO > iAmLinesInDebugInfo)||(CNSL_MINAMLINESINBUFFER > iAmLinesInBuffer))
	{
		return SYS_NOK;
	}

	// check if there are more on screen than there are in the buffer
	if((iAmLinesOnScreen>(iAmLinesInBuffer+CNSL_MINAMLINESONSCREEN))||
		(iAmLinesInDebugInfo>=iAmLinesInBuffer))
	{
		return SYS_NOK;
	}

	if(iAmLinesInDebugInfo>(iAmLinesOnScreen-4))
	{
		return SYS_NOK;
	}

	// set the options and calculate the buffers.
	m_gpConsole->SetAmCharsPerLine(iAmCharsPerLine);
	m_gpConsole->SetAmLinesInBuffer(iAmLinesInBuffer);
	m_gpConsole->SetAmLinesInDebugInfo(iAmLinesInDebugInfo);
	m_gpConsole->SetAmLinesOnScreen(iAmLinesOnScreen);
	m_gpConsole->CreateConsoleBuffers();

	FillConsoleHeader();
	return SYS_OK;
}


// Purpose: retrieves window specific handles for the caller to use them in special purpose routines,
// like wglFont routines.
// If the caller calls this routine before DEMOGL_AppRun(), the returned values are undefined.
void
DEMOGL_GetWindowSpecificHandles(HWND * const hWndMainWindow, HDC * const hDCMainWindow)
{
	if((!hWndMainWindow)||(!hDCMainWindow))
	{
		// pointers are invalid
		return;
	}

	// get the handles from the demodat store
	*hWndMainWindow=m_gpDemoDat->GethWnd();
	*hDCMainWindow=m_gpDemoDat->GethDC();
}



// Purpose: gets the commandline and parses it. It will store the results in m_sclpSSCLParams struct inside
// DemoDat and will return a pointer to that struct to the caller if the parsing went ok. If the parsing had
// errors NULL is returned.
//
// Caller should NOT free the struct passed.
SScreenSaverCLParams
*DEMOGL_ParseScreenSaverCL(const LPSTR lpCmdLine)
{
	int			iResult;
	CStdString	sCmdLine;

	if(!lpCmdLine)
	{
		return NULL;
	}

	sCmdLine.Format("%s",lpCmdLine);

	// Parse it
	iResult=ParseScreenSaverCL(&sCmdLine[0]);
	if(iResult==SYS_OK)
	{
		// return pointer to screensaver exectype params
		return m_gpDemoDat->GetSSCLParams();
	}
	else
	{
		return NULL;
	}
}


// Purpose: this routine is for the using application to set variables in DemoGL. See documentation for
// values of these variables plus the flags how to set these.
// There is more than 1 version of this routine, for the different types of values.
void 
DEMOGL_SetVariable(const int iFlag, const char *pszValue)
{
	switch(iFlag)
	{
		case DGL_VF_SRCDIR:
		{
			m_gpDemoDat->SetSrcDir(pszValue);
		}; break;
		case DGL_VF_DATAFILENAME:
		{
			m_gpDemoDat->SetDataFilename(pszValue);
		}; break;
		case DGL_VF_SCRIPTFILENAME:
		{
			m_gpDemoDat->SetScriptFilename(pszValue);
		}; break;
		// ADD MORE SETTABLE VARIABLES HERE
	}
}


// Purpose: this routine is for the using application to set variables in DemoGL. See documentation for
// values of these variables plus the flags how to set these.
// There is more than 1 version of this routine, for the different types of values.
void 
DEMOGL_SetVariable(const int iFlag, const int iValue)
{
	switch(iFlag)
	{
		case DGL_VF_FILESRC:
		{
			m_gpDemoDat->SetFileSrc(iValue);
		}; break;
		case DGL_VF_MUSICSYS:
		{
			m_gpDemoDat->SetMusicSubSystemType(iValue);
		}; break;
		case DGL_VF_INITFAILURERESPONSE:
		{
			m_gpDemoDat->SetInitFailureResponse(iValue);
		}; break;
		// ADD MORE SETTABLE VARIABLES HERE
	}
}


// Purpose: this routine is for the using application to set variables in DemoGL. See documentation for
// values of these variables plus the flags how to set these.
// There is more than 1 version of this routine, for the different types of values.
void 
DEMOGL_SetVariable(const int iFlag, const bool bValue)
{
	switch(iFlag)
	{
		case DGL_VF_VSYNC:
		{
			m_gpDemoDat->SetbVSYNC(bValue);
		}; break;
		case DGL_VF_SHOWFPS:
		{
			m_gpDemoDat->SetbShowFPS(bValue);
		}; break;
		case DGL_VF_NEVERSHOWFPS:
		{
			m_gpDemoDat->SetbNeverShowFPS(bValue);
		}; break;
		case DGL_VF_NEVERSHOWCONSOLE:
		{
			m_gpDemoDat->SetbNeverShowConsole(bValue);
		}; break;
		case DGL_VF_SHOWDEBUGINFO:
		{
			m_gpDemoDat->SetbShowDebugInfo(bValue);
		}; break;
		case DGL_VF_NEVERSHOWDEBUGINFO:
		{
			m_gpDemoDat->SetbNeverShowDebugInfo(bValue);
		}; break;
		case DGL_VF_NEVERRESCALETEXTURES:
		{
			m_gpDemoDat->SetbNeverRescaleTextures(bValue);
		}; break;
		case DGL_VF_QUICKBOOT:
		{
			m_gpDemoDat->SetbQuickBoot(bValue);
		}; break;
		case DGL_VF_SWAPBUFFERS:
		{
			m_gpDemoDat->SetbSwapBuffers(bValue);
		}; break;
		case DGL_VF_USERENDERFRAMEEX:
		{
			m_gpDemoDat->SetbUseRenderFrameEx(bValue);
		}; break;
		// ADD MORE SETTABLE VARIABLES HERE
	}
}



// Purpose loads a file. Wrapper function around FLoadFile(), which is an internal function
byte
*DEMOGL_FileLoad(const char *pszFilename)
{
	return FLoadFile(pszFilename);
}


// purpose: gets the length of the file. It's a wrapper function around FFileLength, which is an internal function.
long
DEMOGL_FileGetLength(const char *pszFilename)
{
	return FFileLength(pszFilename);
}


// Purpose: logs line to console. This function is for effectobjects only.
void
DEMOGL_ConsoleLogLine(const bool bScroll, const char *pszLine)
{
	CStdString		sToLog;

	if(!pszLine)
	{
		return;
	}

	sToLog.Format("%s",pszLine);

	m_gpConsole->LogLineToConsole(&sToLog[0],bScroll,false);
	UpdateScreenIfNecessary();
}


// Purpose: logs line to the console but uses a variable parameter list, thus ideal for debug logging.
// works as printf. Uses DEMOGL_ConsoleLogLine to actually log the string.
void
DEMOGL_ConsoleLogLineV(const bool bScroll, const char *pszFormat, ...)
{
	CStdString		sToLog;
	va_list			valArguments;

	if(!pszFormat)
	{
		return;
	}

	// retrieve arguments and store them in a string using the format string passed.
	va_start(valArguments, pszFormat);
	sToLog.FormatV(pszFormat, valArguments);
	va_end(valArguments);

	// Log it.
	DEMOGL_ConsoleLogLine(bScroll, &sToLog[0]);
}


// Purpose: checks if an extension is available. This routine is a wrapperfunction to get 
// access to the extensionsobject's contents
bool
DEMOGL_ExtensionCheckIfAvailable(const int iExtension)
{
	return m_gpExtensions->GetExtensionAvailability(iExtension);
}


// Purpose: checks if an extension is available. It won't use DemoGL's internal extension database but will check the provided
// extension string with the list of extensions provided by the OpenGL driver.
bool
DEMOGL_ExtensionCheckIfAvailable(const char *pszExtensionName)
{
	CStdString		sExtensions, sExtensionName;

	if(m_gpDemoDat->GetSystemState()==SSTATE_PREBOOT)
	{
		// application window hasn't been created.
		return false;
	}

	// get the extensions.
	sExtensions.Format("%s",glGetString(GL_EXTENSIONS));
	// make lowercase
	sExtensions.ToLower();

	// grab the constant string containing the extensionname
	sExtensionName.Format("%s",pszExtensionName);
	sExtensionName.ToLower();
	// check it.
	return (strstr(&sExtensions[0],&sExtensionName[0])!=NULL);
}


// Purpose: returns DemoGL version in the 2 parameters passed to this function.
void
DEMOGL_GetVersion(float * const fVersion, long * const lBuildnr)
{
	if(fVersion)
	{
		*fVersion=DEMOGL_VERSION;
	}
	if(lBuildnr)
	{
		*lBuildnr=DEMOGL_BUILD;
	}
}


// Purpose: Sets the given effectobjects' messagehandler flag to true
void 
DEMOGL_MessageHandlerEnable(const char *pszEffectName)
{
	CEffectStore	*pES;
	CStdString		sToLog;
	
	pES = GetEffectStoreObject(pszEffectName);
	if(pES)
	{
		pES->SetMessageHandlerEnabled(true);
		sToLog.Format("Enabled messagehandler of effect %s.",pszEffectName);
		LogFeedback(&sToLog[0],true,"EnableMessageHandler", true);
	}
	else
	{
		sToLog.Format("Could not find effect %s. Messagehandler not enabled.",pszEffectName);
		LogFeedback(&sToLog[0],true,"EnableMessageHandler",false);
	}
}


// Purpose: Sets the given effectobjects' messagehandler flag to false
void 
DEMOGL_MessageHandlerDisable(const char *pszEffectName)
{
	CEffectStore	*pES;
	CStdString		sToLog;
	
	pES = GetEffectStoreObject(pszEffectName);
	if(pES)
	{
		pES->SetMessageHandlerEnabled(false);
		sToLog.Format("Disabled messagehandler of effect %s.",pszEffectName);
		LogFeedback(&sToLog[0],true,"DisableMessageHandler", true);
	}
	else
	{
		sToLog.Format("Could not find effect %s. Messagehandler not disabled.",pszEffectName);
		LogFeedback(&sToLog[0],true,"DisableMessageHandler", false);
	}
}


// Purpose: Enables the usage of a loading splash screen. Opacity: 1.0 if full opaque, 0.0 is fully transparent.
void
DEMOGL_LoadingSplashEnable(const char *pszSplashScreenTexture, const int iFileType, const float fOpacity)
{
	m_gpDemoDat->SetLoadingSplashParams(pszSplashScreenTexture, iFileType, fOpacity);
}


// Purpose: frees memory allocated when DEMOGL_FileLoad() was used
void
DEMOGL_FileFree(const byte *pbyFile)
{
	free((void *)pbyFile);
}


// Purpose: lets effect/applications execute commands. Like _SYSTEM;END;
// It first uses lex and parsetoken to get a timeline
// event object that is executable. it's then executed with DoExecuteTimelineEvent IF there was no error.
// Additional Info: this function is called from the MainMsgHandler when the user presses return AND there is something in the
// inputbuffer.
void 
DEMOGL_ExecuteCommand(const char *pszCommand)
{
	CStdString			sToExecute, sError;
	int					iResult;
	CTimeLineEvent		*pTLE;

	// get and reformat inputbuffer. (add 0; as time to the string, because that's the format the lexical analyzer understands.)
	sToExecute.Format("0;%s\0",pszCommand);
	// jam string through lex.
	iResult=Lex(&sToExecute[0]);
	switch(iResult)
	{
		case LA_ISCOMMENT:
		case LA_ISEMPTY:
		{
			// Comment or empty.. ignore.
			return;
		}; break;
		case LA_TOKENIZED:
		{
			// line is tokenized. Enter tokenparser, which will convert the tokens into objects which can be used.
			pTLE = ParseTokens(0,false);
			if(!pTLE)
			{
				// there was an error. Log the line to the console
				sError.Format("Error in command %s. Skipped.", &sToExecute[0]);
				LogFeedback(&sError[0],true,"ExecuteInputBuffer",false);
				return;
			}
			else
			{
				// went ok, insert in timeline
				DoExecuteTimelineEvent(pTLE, true);
				// we should delete it.
				delete pTLE;
			}
		}; break;
	}
}


// Purpose: enables the loading progress bar at boot/load time. The progress values will be set by the kernel.
void
DEMOGL_LoadingPBEnable(const int iType, const float fWidth, const float fHeight,
			const float fBottomLeftX, const float fBottomLeftY)
{
	m_gpDemoDat->SetbLoadingPBEnabled(true);
	m_gpConsole->SetLoadingPBCntrl(m_gpLoadingPB);

	m_gpLoadingPB->SetBarType(iType);
	m_gpLoadingPB->SetPosition(fBottomLeftX, fBottomLeftY, 0.0f);
	m_gpLoadingPB->SetSize(fWidth, fHeight);
}


// Purpose: sets loadingprogress bar appearance settings. has only any effect if loadingprogressbar is
// enabled, using DEMOGL_EnableLoadingPB()
void
DEMOGL_LoadingPBSetAppearance(const bool bHorizontal, const bool bBlend, 
						const GLenum iBlendSrc, const GLenum iBlendDst, const float fOpacity,
						const float fBGColor_R, const float fBGColor_G,	const float fBGColor_B, 
						const float fBarColor_R, const float fBarColor_G, const float fBarColor_B)
{
	if(m_gpDemoDat->GetbLoadingPBEnabled())
	{
		// progressbar is enabled, set options.
		m_gpLoadingPB->SetBackGroundColor(fBGColor_R, fBGColor_G, fBGColor_B);
		m_gpLoadingPB->SetBlending(bBlend, iBlendSrc, iBlendDst,fOpacity);
		m_gpLoadingPB->SetOrientation(bHorizontal);
		m_gpLoadingPB->SetBarColor(fBarColor_R, fBarColor_G, fBarColor_B);
	}
}

////////////////////////////////////////////////////////////////////////
//                         Library ONLY routines
////////////////////////////////////////////////////////////////////////


// Purpose: effectuates the VSYNC value. Reads the value form CDemoDat
void
EffectuateVariable_VSYNC()
{
	PFNGLWGLSWAPINTERVALEXTPROC     wglSwapIntervalEXT;

	// effectuate the value set 
	if(m_gpExtensions->GetExtensionAvailability(DGL_WGL_EXT_swap_control))
	{
		// Get SwapInterval function entry
		wglSwapIntervalEXT = (PFNGLWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		// effectuate VSYNC.
		if(m_gpDemoDat->GetbVSYNC())
		{
			// enable VSYNC
			(*wglSwapIntervalEXT)(1);
			LogFeedback("VSYNC switched ON.",true,"MainMsgHandler:WM_DEMOGL_PREPARE", true);
		}
		else
		{
			// disable VSYNC
			(*wglSwapIntervalEXT)(0);
			LogFeedback("VSYNC switched OFF.",true,"MainMsgHandler:WM_DEMOGL_PREPARE", true);
		}
	}
}


// Purpose: Toggles the console's appearance
void
ToggleConsole()
{
	if(!m_gpDemoDat->GetbNeverShowConsole())
	{
		m_gpDemoDat->ToggleConsoleVisible();
	}
}


// Purpose: Toggles FPS display in overlay
void
ToggleFPSDisplay()
{
	if(!m_gpDemoDat->GetbNeverShowFPS())
	{
		m_gpDemoDat->ToggleFPSDisplay();
		if(m_gpDemoDat->GetbFPSVisible())
		{
			LogFeedback("FPSdisplay switched ON.", true, "ToggleFSPDisplay",false);
		}
		else
		{
			LogFeedback("FPSdisplay switched OFF.", true, "ToggleFSPDisplay",false);
		}
	}
}


// Purpose: Toggles DebugInfo display in overlay
void
ToggleDebugInfoDisplay()
{
	if(!m_gpDemoDat->GetbNeverShowDebugInfo())
	{
		m_gpDemoDat->ToggleDebugInfoDisplay();
		if(m_gpDemoDat->GetbDebugInfoVisible())
		{
			LogFeedback("DebugInfo display switched ON", true, "ToggleDebugInfoDisplay",false);
		}
		else
		{
			LogFeedback("DebugInfo display switched OFF", true, "ToggleDebugInfoDisplay",false);
		}
	}
}


// Purpose: toggles the inputprompt in the debuginfo overlay. It only turns the prompt ON if the
// debuginfo overlay is visible.
void
ToggleInputPrompt()
{
	if(m_gpDemoDat->GetbDebugInfoVisible())
	{
		m_gpDemoDat->ToggleInputPrompt();
		if(m_gpDemoDat->GetbInputPromptVisible())
		{
			LogFeedback("Inputprompt switched ON", true, "ToggleInputPrompt",false);
		}
		else
		{
			LogFeedback("Inputprompt switched OFF", true, "ToggleInputPrompt",false);
		}
	}
}


// Purpose: does a redraw of the window by posting a WM_DEMOGL_PAINT message which will redraw the window.
// Used by LogFeedback() when called from a worker thread.
void
RedrawTheWindow()
{
	PostMessage(m_gpDemoDat->GethWnd(),WM_DEMOGL_PAINT,NULL,NULL);
}	


// Purpose: lists the OpenGL ICD information which is stored in DemoDat
void
ListICDInfo()
{
	char			*sGSresult;
	CStdString		sToLog;

	LogFeedback("---OpenGL ICD information]-----------------------",true,"ListICDInfo", true);
	// get vendor and display this vendor.
	sGSresult = (char *)glGetString(GL_VENDOR);
	sToLog.Format("3D Hardware manufacturer: %s",sGSresult);
	LogFeedback(&sToLog[0],true,"ListICDInfo", true);

	// get chipset and display this chipset.
	sGSresult = (char *)glGetString(GL_RENDERER);
	sToLog.Format("Current Renderer, reported by OpenGL driver: %s",sGSresult);
	LogFeedback(&sToLog[0],true,"ListICDInfo", true);

	// get OpenGL driver version and display this version.
	sGSresult = (char *)glGetString(GL_VERSION);
	sToLog.Format("OpenGL ICD version: %s",sGSresult);
	LogFeedback(&sToLog[0],true,"ListICDInfo", true);

	// get extensions.
	sGSresult = (char *)glGetString(GL_EXTENSIONS);
	sToLog.Format("OpenGL extensions supported by OpenGL ICD: %s",sGSresult);
	LogFeedback(&sToLog[0],true,"ListICDInfo", true);
}


// Purpose: fills the console header with text.
void
FillConsoleHeader()
{
	CStdString	sHdrLine;

	sHdrLine.Format("DemoGL v%1.2f. Build: %d. Build status: %s.",DEMOGL_VERSION,DEMOGL_BUILD,DEMOGL_VERSIONTYPE);
	m_gpConsole->AddLineToHeader(&sHdrLine[0],0);
	m_gpConsole->AddLineToHeader(DEMOGL_COPYRIGHT,1);
	m_gpConsole->AddLineToHeader(DEMOGL_AUTHOR,2);
	m_gpConsole->AddLineToHeader(DEMOGL_MSS_COPYRIGHT, 3);
}


// Purpose: Determines the working dir for the current running RUNTYPE_SAVER_* mode application.
// Will first check the registry and if that fails, it will read the commandline and hack the path from that string.
// Working dir is stored in demodat. It will not touch workingdir if the runtype is not RUNTYPE_SAVER_*, because
// the working dir is not used in other runtypes
void
DetermineSSaverWorkingDir()
{
	char		*pWorkingDirFromReg, *pCommandLine, *pCur;
	char		sWorkingDir[MAXPATHNAMELENGTH];
	int			iMaxLength, iIdx;

	// Check if the correct runtype is calling...
	switch(m_gpDemoDat->GetRunType())
	{
		case RUNTYPE_NORMAL:
		case RUNTYPE_NODIALOG:
		{
			// shouldn't be here
			return;
		}; break;
		case RUNTYPE_SAVER_CONFIG:
		case RUNTYPE_SAVER_NORMAL:
		case RUNTYPE_SAVER_PREVIEW:
		case RUNTYPE_SAVER_PASSWORD:
		{
			// nothing
		}; break;
		////////////////////////////////
		// ADD MORE RUNTYPES HERE
		////////////////////////////////
		default:
		{
			// unknown runtype. 
			return;
		}; break;
	}

	memset(sWorkingDir,0,MAXPATHNAMELENGTH);

	// grab from registry.
	pWorkingDirFromReg=ReadWorkingDirFromRegistry();
	if(!pWorkingDirFromReg)
	{
		// not there, read it from commandline.
		pCommandLine=GetCommandLine();
		// first argument is the path plus the executable. first check if the OS has put '"' around the path.
		pCur=pCommandLine;
		if(*pCur=='\"')
		{
			// yup. eat away the string
			for(pCur++;(*pCur!=NULL) && (*pCur!='\"');pCur++){};
			if(!*pCur)
			{
				// ending '"' not found. no legitimate path found. 
				return;
			}
			// pCur points to ending '"'. 
			iMaxLength = pCur-(pCommandLine+1);
			if(iMaxLength>MAXPATHNAMELENGTH)
			{
				iMaxLength=MAXPATHNAMELENGTH;
			}
			strncpy(sWorkingDir,pCommandLine+1,iMaxLength);
		}
		else
		{
			// no string markers. this means there are no spaces in the path+filename. find the first
			// space and copy string before that space to the workingdir string
			for(;(*pCur!=NULL) && (*pCur!=' ');pCur++){};
			// routine quits with either pCur at the end of the string or at the first space.
			iMaxLength = pCur-pCommandLine;
			if(iMaxLength>MAXPATHNAMELENGTH)
			{
				iMaxLength=MAXPATHNAMELENGTH;
			}
			strncpy(sWorkingDir,pCommandLine,iMaxLength);
		}
		// now, walk back from the back of the string, until we find the first '\'. put there a '\0'
		for(iIdx=strlen(sWorkingDir)-1;iIdx>=0;iIdx--)
		{
			if(sWorkingDir[iIdx]=='\\')
			{
				// found it
				sWorkingDir[iIdx]='\0';
				break;
			}
		}
		if(iIdx==0)
		{
			// not a path found. return
			return;
		}
	}
	else
	{
		strncpy(sWorkingDir,pWorkingDirFromReg,MAXPATHNAMELENGTH);
		// free buffer
		free(pWorkingDirFromReg);
	}
	// store it in demodat
	m_gpDemoDat->SetWorkingDir(sWorkingDir);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTE: Of course there could have been 3 or so routines with general registry access, but there are just
// 3 cases where there should be registry access.. it's really a hassle to make universal registry access so
// to avoid that hassle for just 3 routines, there is no set of universal registry access, but 3 specific routines
// for the specific registry access.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Purpose: reads sWorkingDir value from registry. For RUNTYPE_SCREENSAVER. Custom screensaver installation programs
// should store a value named sWorkingDir in the application's regkey with the path where the data is stored. This path
// will be put in front of sSrcDir if loading from a dir or it will be put in front of datafilename if loading from
// a datafile. A trailing '\' will be cut off. If sWorkingDir is not found, NULL is returned.
//
// Caller should free the buffer received.
char 
*ReadWorkingDirFromRegistry()
{
	LONG			lResult;
	CStdString		sKeyName;
	HKEY			hStorageKey;
	DWORD			dwDataLength;
	char			*pData;
	int				iDirNameLength;

	// build keystring. use runtype info to formulate the string
	switch(m_gpDemoDat->GetRunType())
	{
		case RUNTYPE_NORMAL:
		case RUNTYPE_NODIALOG:
		{
			// shouldn't be reading this.
			return NULL;
		}; break;
		case RUNTYPE_SAVER_CONFIG:
		case RUNTYPE_SAVER_NORMAL:
		case RUNTYPE_SAVER_PREVIEW:
		case RUNTYPE_SAVER_PASSWORD:
		{
			sKeyName.Format("Software\\%s\\%s\\%s\\%s",REGKEY_CURRENTUSER_SUB,REGKEY_RT_SCREENSAVER,
						m_gpDemoDat->GetStartupDat()->m_sDemoName,REGKEY_DATA_SUB);
		}; break;
		////////////////////////////////
		// ADD MORE RUNTYPES HERE
		////////////////////////////////
		default:
		{
			// unknown runtype. 
			return NULL;
		}; break;
	}
	lResult=RegOpenKeyEx(HKEY_CURRENT_USER, &sKeyName[0],0,KEY_READ,&hStorageKey);
	if(lResult!=ERROR_SUCCESS)
	{
		// is not there or could not be opened. return
		return NULL;
	}
	// sWorkingDir
	pData=(char *)malloc(MAXPATHNAMELENGTH);
	memset(pData,0,MAXPATHNAMELENGTH);
	dwDataLength=MAXPATHNAMELENGTH;
	lResult=RegQueryValueEx(hStorageKey,"sWorkingDir",0,NULL,(byte *)pData,&dwDataLength);
	RegCloseKey(hStorageKey);
	if(lResult==ERROR_SUCCESS)
	{
		// find trailing '\' if any.
		iDirNameLength=strlen(pData);
		if(pData[iDirNameLength-1]=='\\')
		{
			pData[iDirNameLength-1]='\0';
		}
		return pData;
	}
	else
	{
		return NULL;
	}
}


// Purpose: generates the registry key where the data of the application is stored.
// Stores the generated info into CDemoDat and returns SYS_OK if succeeded, or SYS_NOK if not.
// It will NOT create nor check if the key is there. It just constructs the name
int
GenerateRegistryKeyName(char *pszDemoName)
{
	CStdString		sKeyName;

	// build keystring. use runtype info to formulate the string
	switch(m_gpDemoDat->GetRunType())
	{
		case RUNTYPE_NORMAL:
		case RUNTYPE_NODIALOG:
		{
			sKeyName.Format("Software\\%s\\%s\\%s\\%s",REGKEY_CURRENTUSER_SUB,REGKEY_RT_NORMAL,
						pszDemoName,REGKEY_DATA_SUB);
		}; break;
		case RUNTYPE_SAVER_CONFIG:
		case RUNTYPE_SAVER_NORMAL:
		case RUNTYPE_SAVER_PREVIEW:
		case RUNTYPE_SAVER_PASSWORD:
		{
			sKeyName.Format("Software\\%s\\%s\\%s\\%s",REGKEY_CURRENTUSER_SUB,REGKEY_RT_SCREENSAVER,
						pszDemoName,REGKEY_DATA_SUB);
		}; break;
		////////////////////////////////
		// ADD MORE RUNTYPES HERE
		////////////////////////////////
		default:
		{
			// no other types supported in this routine
			return SYS_NOK;
		}; break;
	}
	// store result in CDemoDat.
	m_gpDemoDat->SetRegistryKeyName(&sKeyName[0]);
	return SYS_OK;
}


// Purpose: Saves the date inside the passed startupdat structure in the registry. It uses the RunType of the current
// execution to locate where the data should be stored. The data is stored in the currentuser hive so more users of
// the same machine can store different settings
void
SaveStartupDatInRegistry(SStartupDat *pStartupDat)
{
	LONG			lResult;
	char			*pszKeyName;
	HKEY			hStorageKey;
	DWORD			dwData;

	pszKeyName=m_gpDemoDat->GetRegistryKeyName();

	// Open key using the createex function. If it's not there, it'll be created.
	lResult=RegCreateKeyEx(HKEY_CURRENT_USER, pszKeyName,0,NULL,REG_OPTION_NON_VOLATILE,
			KEY_READ|KEY_WRITE,NULL,&hStorageKey,NULL);

	if(lResult!=ERROR_SUCCESS)
	{
		// something was wrong, couldn't save to registry. Possible access right voilation.
		MessageBox(NULL,"Couldn't store the settings in the registry. You probably don't have enough access rights to do that",
				"DemoGL Non-fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL); 
		return;
	}
	
	///////////////////////
	// Store the StartupDat contents in the data
	///////////////////////
	// bFullScreen
	dwData=(DWORD)pStartupDat->m_bFullScreen;
	RegSetValueEx(hStorageKey,"bFullScreen",0,REG_DWORD,(byte *)&dwData,sizeof(DWORD));
	// iResolution
	dwData=(DWORD)pStartupDat->m_iResolution;
	RegSetValueEx(hStorageKey,"iResolution",0,REG_DWORD,(byte *)&dwData,sizeof(DWORD));
	// bDQ32bit
	dwData=(DWORD)pStartupDat->m_bDQ32bit;
	RegSetValueEx(hStorageKey,"bDQ32bit",0,REG_DWORD,(byte *)&dwData,sizeof(DWORD));
	// bTQ32bit
	dwData=(DWORD)pStartupDat->m_bTQ32bit;
	RegSetValueEx(hStorageKey,"bTQ32bit",0,REG_DWORD,(byte *)&dwData,sizeof(DWORD));
	// bSound
	dwData=(DWORD)pStartupDat->m_bSound;
	RegSetValueEx(hStorageKey,"bSound",0,REG_DWORD,(byte *)&dwData,sizeof(DWORD));
	// bRescaleTextures
	dwData=(DWORD)pStartupDat->m_bRescaleTextures;
	RegSetValueEx(hStorageKey,"bRescaleTextures",0,REG_DWORD,(byte *)&dwData,sizeof(DWORD));
	// iSS_SoundDevice
	dwData=(DWORD)pStartupDat->m_iSS_SoundDevice;
	RegSetValueEx(hStorageKey,"iSS_SoundDevice",0,REG_DWORD,(byte *)&dwData,sizeof(DWORD));
	// bSS_Stereo
	dwData=(DWORD)pStartupDat->m_bSS_Stereo;
	RegSetValueEx(hStorageKey,"bSS_Stereo",0,REG_DWORD,(byte *)&dwData,sizeof(DWORD));
	// bSS_16bit
	dwData=(DWORD)pStartupDat->m_bSS_16bit;
	RegSetValueEx(hStorageKey,"bSS_16bit",0,REG_DWORD,(byte *)&dwData,sizeof(DWORD));
	// bSS_LowQuality
	dwData=(DWORD)pStartupDat->m_bSS_LowQuality;
	RegSetValueEx(hStorageKey,"bSS_LowQuality",0,REG_DWORD,(byte *)&dwData,sizeof(DWORD));
	// bSS_A3D
	dwData=(DWORD)pStartupDat->m_bSS_A3D;
	RegSetValueEx(hStorageKey,"bSS_A3D",0,REG_DWORD,(byte *)&dwData,sizeof(DWORD));
	// bSS_EAX
	dwData=(DWORD)pStartupDat->m_bSS_EAX;
	RegSetValueEx(hStorageKey,"bSS_EAX",0,REG_DWORD,(byte *)&dwData,sizeof(DWORD));
	// iSS_OverallVolume
	dwData=(DWORD)pStartupDat->m_iSS_OverallVolume;
	RegSetValueEx(hStorageKey,"iSS_OverallVolume",0,REG_DWORD,(byte *)&dwData,sizeof(DWORD));

	// store the strings. for windows95 we need a different codepath.
	if(CheckOSType(OSTYPE_WINDOWS95))
	{
		// yes, we're on windows95. 
		RegSetValueEx(hStorageKey,"sDemoName",0,REG_SZ,(byte *)&pStartupDat->m_sDemoName[0],
				strlen(pStartupDat->m_sDemoName)+1);
		RegSetValueEx(hStorageKey,"sReleasedBy",0,REG_SZ,(byte *)&pStartupDat->m_sReleasedBy[0],
				strlen(pStartupDat->m_sReleasedBy)+1);
	}
	else
	{
		// winNT, win2k, win98 or other win32 os that's not win95
		RegSetValueEx(hStorageKey,"sDemoName",0,REG_SZ,(byte *)&pStartupDat->m_sDemoName[0],
				strlen(pStartupDat->m_sDemoName));
		RegSetValueEx(hStorageKey,"sReleasedBy",0,REG_SZ,(byte *)&pStartupDat->m_sReleasedBy[0],
				strlen(pStartupDat->m_sReleasedBy));
	}

	// ADD MORE STARTUPDAT MEMBERS HERE

	RegCloseKey(hStorageKey);
}


// Purpose: Loads the data for the startup structure from the registry in the startup structure passed.
// If no data is found, no data is loaded and the current values of the startup dat structure stay untouched.
// Uses the RunType variable to determine the location where the data is stored.
void
LoadStartupDatFromRegistry(SStartupDat *pStartupDat)
{
	LONG			lResult;
	CStdString		sToLog;
	char			*pszKeyName;
	HKEY			hStorageKey;
	DWORD			dwData, dwDataLength;
	char			sData[DGL_SD_STRINGLENGTH];

	pszKeyName=m_gpDemoDat->GetRegistryKeyName();

	// Open key using the openex function. If it's not there, we'll return with the pStartupDat
	// untouched.
	lResult=RegOpenKeyEx(HKEY_CURRENT_USER, pszKeyName,0,KEY_READ,&hStorageKey);
	if(lResult!=ERROR_SUCCESS)
	{
		// is not there or could not be opened. return
		return;
	}
	////////////////////////
	// Read the values inside the key.
	////////////////////////
	// iResolution
	dwDataLength=sizeof(DWORD);
	lResult=RegQueryValueEx(hStorageKey,"iResolution",0,NULL,(byte *)&dwData,&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		pStartupDat->m_iResolution=(int)dwData;
	}
	// bDQ32bit
	dwDataLength=sizeof(DWORD);
	lResult=RegQueryValueEx(hStorageKey,"bDQ32bit",0,NULL,(byte *)&dwData,&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		pStartupDat->m_bDQ32bit=(dwData==1);
	}
	// bFullscreen
	dwDataLength=sizeof(DWORD);
	lResult=RegQueryValueEx(hStorageKey,"bFullScreen",0,NULL,(byte *)&dwData,&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		pStartupDat->m_bFullScreen=(dwData==1);
	}
	// bTQ32bit
	dwDataLength=sizeof(DWORD);
	lResult=RegQueryValueEx(hStorageKey,"bTQ32bit",0,NULL,(byte *)&dwData,&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		pStartupDat->m_bTQ32bit=(dwData==1);
	}
	// bSound
	dwDataLength=sizeof(DWORD);
	lResult=RegQueryValueEx(hStorageKey,"bSound",0,NULL,(byte *)&dwData,&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		pStartupDat->m_bSound=(dwData==1);
	}
	// bRescaleTextures
	dwDataLength=sizeof(DWORD);
	lResult=RegQueryValueEx(hStorageKey,"bRescaleTextures",0,NULL,(byte *)&dwData,&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		pStartupDat->m_bRescaleTextures=(dwData==1);
	}
	// iSS_SoundDevice
	dwDataLength=sizeof(DWORD);
	lResult=RegQueryValueEx(hStorageKey,"iSS_SoundDevice",0,NULL,(byte *)&dwData,&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		pStartupDat->m_iSS_SoundDevice=(int)dwData;
	}
	// iSS_OverallVolume
	dwDataLength=sizeof(DWORD);
	lResult=RegQueryValueEx(hStorageKey,"iSS_OverallVolume",0,NULL,(byte *)&dwData,&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		pStartupDat->m_iSS_OverallVolume=(int)dwData;
	}
	// bSS_Stereo
	dwDataLength=sizeof(DWORD);
	lResult=RegQueryValueEx(hStorageKey,"bSS_Stereo",0,NULL,(byte *)&dwData,&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		pStartupDat->m_bSS_Stereo=(dwData==1);
	}
	// bSS_16bit
	dwDataLength=sizeof(DWORD);
	lResult=RegQueryValueEx(hStorageKey,"bSS_16bit",0,NULL,(byte *)&dwData,&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		pStartupDat->m_bSS_16bit=(dwData==1);
	}
	// bSS_LowQuality
	dwDataLength=sizeof(DWORD);
	lResult=RegQueryValueEx(hStorageKey,"bSS_LowQuality",0,NULL,(byte *)&dwData,&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		pStartupDat->m_bSS_LowQuality=(dwData==1);
	}
	// bSS_A3D
	dwDataLength=sizeof(DWORD);
	lResult=RegQueryValueEx(hStorageKey,"bSS_A3D",0,NULL,(byte *)&dwData,&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		pStartupDat->m_bSS_A3D=(dwData==1);
	}
	// bSS_EAX
	dwDataLength=sizeof(DWORD);
	lResult=RegQueryValueEx(hStorageKey,"bSS_EAX",0,NULL,(byte *)&dwData,&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		pStartupDat->m_bSS_EAX=(dwData==1);
	}

	// sDemoName
	dwDataLength=DGL_SD_STRINGLENGTH;
	memset(sData,0,DGL_SD_STRINGLENGTH);
	lResult=RegQueryValueEx(hStorageKey,"sDemoName",0,NULL,(byte *)&sData[0],&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		memset(pStartupDat->m_sDemoName,0,DGL_SD_STRINGLENGTH);
		strncpy(pStartupDat->m_sDemoName,sData,DGL_SD_STRINGLENGTH);
	}

	// sReleasedBy
	dwDataLength=DGL_SD_STRINGLENGTH;
	memset(sData,0,DGL_SD_STRINGLENGTH);
	lResult=RegQueryValueEx(hStorageKey,"sReleasedBy",0,NULL,(byte *)&sData[0],&dwDataLength);
	if(lResult==ERROR_SUCCESS)
	{
		memset(pStartupDat->m_sReleasedBy,0,DGL_SD_STRINGLENGTH);
		strncpy(pStartupDat->m_sReleasedBy,sData,DGL_SD_STRINGLENGTH);
	}

	// ADD MORE STARTUPDAT MEMBERS HERE

	RegCloseKey(hStorageKey);
}


// Purpose: checks if the current system is the system with the type passed.
// See for OS types dgl_dllmain.h
// Based on MSDN example.
bool 
CheckOSType(int iOSType)
{
	OSVERSIONINFOEX		osvi;
	BOOL				bOsVersionInfoEx;
	int					iOSTypeDetermined;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure,
	// which is supported on Windows 2000.
	// If that fails, try using the OSVERSIONINFO structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if(!(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		// If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if(! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
		{
			return false;
		}
	}

	switch (osvi.dwPlatformId)
	{
		case VER_PLATFORM_WIN32_NT:
		{
			// Test for the product.
			if(osvi.dwMajorVersion <= 4 )
			{
				iOSTypeDetermined=OSTYPE_WINDOWSNT;
			}
			if(osvi.dwMajorVersion == 5 )
			{
				iOSTypeDetermined=OSTYPE_WINDOWS2000;
			}
		};break;
		case VER_PLATFORM_WIN32_WINDOWS:
		{
			if((osvi.dwMajorVersion>4)||((osvi.dwMajorVersion== 4)&& (osvi.dwMinorVersion>0)))
			{
				iOSTypeDetermined=OSTYPE_WINDOWS98;
			}
			else
			{
				iOSTypeDetermined=OSTYPE_WINDOWS95;
			}
		};break;
		case VER_PLATFORM_WIN32s:
		{
			iOSTypeDetermined=OSTYPE_WIN32S;
		};break;
		default:
		{
			iOSTypeDetermined=OSTYPE_UNKNOWN;
		}; break;
   }
   return (iOSTypeDetermined==iOSType); 
}



// Purpose: increases the current value of the loading progressbar with iAmount and will flag the system to redraw
// the console with the progressbar if necessary.
void
IncLoadingPBValue(int iAmount)
{
	if(m_gpDemoDat->GetbLoadingPBEnabled())
	{
		// it's enabled, inc value
		m_gpLoadingPB->IncCurrentValue(iAmount);
		// redraw screen if the systemstate allows us to do that.
		UpdateScreenIfNecessary();
	}
}


// Purpose: increases maximum value of the loading progress bar with iAmount. Needed for dynamic routines like 
// DEMOGL_RegisterEffect() which will add 1 to the maximum value of the loading progress bar. It's not wise to call
// this routine while the system is already started.
void
IncLoadingPBMaxValue(int iAmount)
{
	m_gpLoadingPB->IncMaxValue(iAmount);
}


// Purpose: Displays Help in console.
void
DisplayHelp()
{
	DEMOGL_ConsoleLogLine(true, " ");
	DEMOGL_ConsoleLogLine(true, "Help (F1)");
	DEMOGL_ConsoleLogLine(true, "-----------------------------------------------------------------------");
	DEMOGL_ConsoleLogLine(true, "`, TAB, F12 or ~ toggles console (if not locked)");
	DEMOGL_ConsoleLogLine(true, "ESC or ALT-F4 quits application");
	DEMOGL_ConsoleLogLine(true, "F1 displays help in console");
	DEMOGL_ConsoleLogLine(true, "F2 toggles fps when console is not visible (if not locked)");
	DEMOGL_ConsoleLogLine(true, "F3 displays all layers with their effects and visibility");
	DEMOGL_ConsoleLogLine(true, "F4 displays the names of all registered effects");
	DEMOGL_ConsoleLogLine(true, "F5 toggles debug info overlay (if not locked)");
	DEMOGL_ConsoleLogLine(true, "F6 toggles input prompt (only active when debug info overlay is ON)");
	DEMOGL_ConsoleLogLine(true, "F7 displayes complete timeline and current spot on timeline");
	DEMOGL_ConsoleLogLine(true, "-----------------------------------------------------------------------");
}


// Purpose: displays all the registered effects currently available in DemoGL in the console.
void
DisplayRegisteredObjectsInConsole()
{
	CEffectStore	*pCurr;

	DEMOGL_ConsoleLogLine(true, " ");
	DEMOGL_ConsoleLogLine(true, " Effect name ");
	DEMOGL_ConsoleLogLine(true, "-------------------------------------------------------------");

	if(!m_gpESHead)
	{
		// no effects registered. 
		DEMOGL_ConsoleLogLine(true, " None registered.");
	}
	else
	{
		for(pCurr=m_gpESHead;pCurr;pCurr=pCurr->GetNext())
		{
			DEMOGL_ConsoleLogLineV(true, " %-40.40s",pCurr->GetEffectName());
		}
	}

	DEMOGL_ConsoleLogLine(true, "-------------------------------------------------------------");
}


// Purpose: gets the effectobject from the linked list. Will return NULL if not found
CEffect
*GetEffectObject(char *sEffectName)
{
	CEffectStore		*pES;

	pES = GetEffectStoreObject(sEffectName);

	if(!pES)
	{
		return NULL;
	}
	else
	{
		// found it
		return pES->GetEffectObject();
	}
}


// Purpose: gets the EffectStore Object of the effectobject that has the name passed
CEffectStore
*GetEffectStoreObject(const char *sEffectName)
{
	CEffectStore	*pES;

	pES = m_gpESHead;
	while(pES)
	{
		if(!strcmp(pES->GetEffectName(),sEffectName))
		{
			// found it
			return pES;
		}
		else
		{
			// it isn't the one. move on.
			pES = pES->GetNext();
		}
	}
	// not found.return  NULL
	return NULL;
}


// Purpose: removes Effectstore objects with the name of sEffectName
void
RemoveEffectStore(char *sEffectName)
{
	CEffectStore	*pCurr, *pPrev;

	pCurr = m_gpESHead;
	pPrev = NULL;
	while(pCurr)
	{
		// find all objects
		if(!strcmp(sEffectName,pCurr->GetEffectName()))
		{
			// found it.
			if(!pPrev)
			{
				// first one
				pPrev=pCurr;
				pCurr=pCurr->GetNext();
				delete pPrev;
				m_gpESHead = pCurr;
				pPrev=NULL;
				continue;
			}
			else
			{
				pPrev->SetNext(pCurr->GetNext());
				delete pCurr;
				pCurr=pPrev->GetNext();
				continue;
			}
		}
		pPrev=pCurr;
		pCurr=pCurr->GetNext();
	}
}


// Purpose: checks if the screen has to be repainted. Needed for asynchronous routines in other threads that
// have updated feedback information. This routine is only used during the multithreaded bootphase where other threads
// do the initializing of parts and the paintthread waits for drawmessages. The initthreads can then use this routine
// to flag the paintthread to redraw the window. This way no unnecessary redrawing is done during booting so all cpu
// power is used for the bootprocess itself.
void
UpdateScreenIfNecessary()
{
	if(!(m_gpDemoDat->GetSystemState()==SSTATE_ARUN_KERNELLOOP))
	{
		// boottime. check if console is up. If so, we need to redraw it via a special
		// message routine using RedrawTheWindow(). If not, and we're in kernel loop, the
		// screen get's repainted automatically, so no need for this routine.
		if(m_gpDemoDat->GetbConsoleVisible())
		{
			// it's up, redraw the console via a messagepost. This is needed because
			// we can be called from another thread.
			RedrawTheWindow();
		}
	}
}


// Purpose: creates a [currentsystemphase]::function:text" log for the console. It then automatically
// logs it to the console using the systemconsole's method LogLineToConsole.
void
LogFeedback(char *sText,bool bScroll, char *sFunction, bool bNotIfQuickBooting, bool bUpdateScreen)
{
	CStdString		sToLog, sSystemState;

	if(m_gpDemoDat->GetbQuickBoot() && bNotIfQuickBooting)
	{
		// do not log, we're quickbooting and then we shouldn't log the sText string, which is
		// only logged in normal booting.
		return;
	}

	switch(m_gpDemoDat->GetSystemState())
	{
		case SSTATE_PREBOOT:
		{
			sSystemState.Format("PBP");
		}; break;
		case SSTATE_BOOT_STARTUPDIALOG:
		case SSTATE_BOOT_MAINWNDCREATION:
		case SSTATE_BOOT_MESSAGEPUMPSTART:
		case SSTATE_BOOT_INITSTART:
		case SSTATE_BOOT_SYSOBJECTSINIT:
		{
			sSystemState.Format("BP ");
		}; break;
		case SSTATE_AINIT_EFFECTSINIT:
		case SSTATE_AINIT_SCRIPTPARSING:
		case SSTATE_AINIT_PRESTARTEVENTEXEC:
		{
			sSystemState.Format("AIP");
		}; break;
		case SSTATE_ARUN_KERNELLOOP:
		{
			sSystemState.Format("ARP");
		}; break;
		case SSTATE_ARUN_POSTKERNELLOOP:
		{
			sSystemState.Format("PRP");
		}; break;
		case SSTATE_AEND_SYSTEMCLEANUP:
		{
			sSystemState.Format("SCP");
		}; break;
		// ADD SUPPORT FOR MORE SYSTEMSTATES HERE
		default:
		{
			sSystemState.Format("???");
		}; break;
	};
#ifdef _DEBUG_SHOWFUNCTIONNAMES
	sToLog.Format("[%s]::%s:%s",&sSystemState[0],sFunction,sText);
#else
	sToLog.Format("[%s]::%s",&sSystemState[0],sText);
#endif
	m_gpConsole->LogLineToConsole(&sToLog[0],bScroll,true);

	if(bUpdateScreen)
	{
		// update the window if we're in the bootprocess.
		UpdateScreenIfNecessary();
	}
}


// purpose: loads the specified file into memory and returns a pointer to the file.
// if the file couldn't be loaded, it returns NULL.
// Additional info: 
//		- it determines where to load from. 
//		- caller has to clean up buffer.
//		- files are padded with a '0'. This is for textfiles so they can be searched easily.
byte
*FLoadFile(const char *sFilename)
{
	byte			*byBuffer;
	CStdString		sPathAndFile, sToLog;
	FILE			*fpFile;
	long			lFileLength;

	byBuffer = NULL;

	// construct path with filename. 
	if((strlen(m_gpDemoDat->GetSrcDir())>0)&&(m_gpDemoDat->GetSrcDir()))
	{
		sPathAndFile.Format("%s\\%s",m_gpDemoDat->GetSrcDir(), sFilename);
	}
	else
	{
		sPathAndFile.Format("%s",sFilename);
	}

	switch(m_gpDemoDat->GetFileSrc())
	{
		case SRC_LOADFROMDATAFILE:
		{
			// Load from the datafile
			lFileLength = DoFileLengthInDataFileLookup(&sPathAndFile[0]);
			if(lFileLength<=0)
			{
				// not valid. perhaps slashes have to be substituted.
				SwapSlashesForZip(&sPathAndFile);
				// load the data into a buffer. NULL is received if load failed.
				byBuffer=LoadFileFromDataFile(&sPathAndFile[0]);
				if(!byBuffer)
				{
					// couldn't load file... report
					sToLog.Format("Couldn't load file: %s.", sFilename);
					LogFeedback(&sToLog[0],true,"FLoadFile",false);
				}
			}
			else
			{
				// ok.
				byBuffer=LoadFileFromDataFile(&sPathAndFile[0]);
			}
		}; break;
		case SRC_LOADFROMDIR:
		{
			// Load from the dir.
			// if the runtype is RUNTYPE_SAVER_*, get the working dir and put that dir in front of the
			// dirname
			switch(m_gpDemoDat->GetRunType())
			{
				case RUNTYPE_SAVER_CONFIG:
				case RUNTYPE_SAVER_NORMAL:
				case RUNTYPE_SAVER_PREVIEW:
				case RUNTYPE_SAVER_PASSWORD:
				{
					sPathAndFile.Format("%s\\%s",m_gpDemoDat->GetWorkingDir(),&sPathAndFile[0]);
				}; break;
			}
			// path file name.
			// open file.
			fpFile = fopen(&sPathAndFile[0],"rb");
			if(!fpFile)
			{
				// couldn't load it. return NULL
				return NULL;
			}
			lFileLength=DoFileLengthLookup(fpFile);
			// alloc memory.
			byBuffer = (byte *)malloc((lFileLength+1) * sizeof byte);
			if(!byBuffer)
			{	
				// couldn't alloc memory.
				LogFeedback("Couldn't allocate memory. aborting",true,"FLoadFile",false);
				fclose(fpFile);
				return NULL;
			}
			// load the file
			fread(byBuffer,lFileLength,1,fpFile);
			// close it
			fclose(fpFile);

			// pad with 0
			byBuffer[lFileLength]='\0';
		}; break;
	}
	// check if crypting is used.
	if((m_gpDemoDat->GetbUseCrypting())&&(byBuffer))
	{
		// yes. So decrypt buffer.
		DecryptBuffer(lFileLength,byBuffer);
	}
	// return buffer
	return byBuffer;
}


// Purpose: determines the actual length of a file. It checks the actual source (datafile or dir)
long
FFileLength(const char *sFilename)
{
	CStdString		sPathAndFile;
	FILE			*fpFile;
	long			lFileLength;

	// construct path with filename. 
	if((strlen(m_gpDemoDat->GetSrcDir())>0)&&(m_gpDemoDat->GetSrcDir()))
	{
		sPathAndFile.Format("%s\\%s",m_gpDemoDat->GetSrcDir(), sFilename);
	}
	else
	{
		sPathAndFile.Format("%s",sFilename);
	}
	switch(m_gpDemoDat->GetFileSrc())
	{
		case SRC_LOADFROMDATAFILE:
		{
			// Load from the datafile
			lFileLength = DoFileLengthInDataFileLookup(&sPathAndFile[0]);
			if(lFileLength<=0)
			{
				// perhaps the zipfile has normal slashes as dir separator.
				SwapSlashesForZip(&sPathAndFile);
				lFileLength = DoFileLengthInDataFileLookup(&sPathAndFile[0]);
			}
		}; break;
		case SRC_LOADFROMDIR:
		{
			// Load from the dir.
			// if the runtype is RUNTYPE_SAVER_*, get the working dir and put that dir in front of the
			// dirname
			switch(m_gpDemoDat->GetRunType())
			{
				case RUNTYPE_SAVER_CONFIG:
				case RUNTYPE_SAVER_NORMAL:
				case RUNTYPE_SAVER_PREVIEW:
				case RUNTYPE_SAVER_PASSWORD:
				{
					sPathAndFile.Format("%s\\%s",m_gpDemoDat->GetWorkingDir(),&sPathAndFile[0]);
				}; break;
			}
			// open file.
			fpFile = fopen(&sPathAndFile[0],"rb");
			if(!fpFile)
			{
				// couldn't load it. return SYS_FILENOTFOUND;
				return SYS_FILENOTFOUND;
			}
			lFileLength=DoFileLengthLookup(fpFile);
			// close it
			fclose(fpFile);
		}; break;
	}
	return lFileLength;
}


// Purpose: returns the length of the file passed
inline long
DoFileLengthLookup(FILE *fp)
{
	long	lLength;

	fseek(fp,0,SEEK_END);
	lLength = ftell(fp);
	fseek(fp,0,SEEK_SET);

	return lLength;
}


// Purpose: swaps all '\\' into '/', because zipfiles store the paths using '/'. 
// Additional Info: double or more backslashes will be eaten so don't worry.
void
SwapSlashesForZip(CStdString *sPathAndFile)
{
	int		iLength, iSrc, iDest, i;
	char	sDestBuffer[MAXPATHFILENAMELENGTH], sSrcBuffer[MAXPATHFILENAMELENGTH];
	bool	bBackSlashFound;

	for(i=0;i<2048;sDestBuffer[i]='\0', sSrcBuffer[i]='\0', i++);

	iLength=strlen(sPathAndFile[0]);
	if(iLength> 2047)
	{
		strncpy(&sSrcBuffer[0],sPathAndFile[0], 2047);
	}
	else
	{
		strncpy(&sSrcBuffer[0],sPathAndFile[0], iLength);
	}

	bBackSlashFound = false;
	for(iSrc=0, iDest=0;iSrc<iLength;iSrc++)
	{
		// if it's NOT a backslash, simply put the char into the destbuffer.
		if(sSrcBuffer[iSrc]!='\\')
		{
			// yep
			sDestBuffer[iDest++]=sSrcBuffer[iSrc];
			bBackSlashFound=false;
		}
		else
		{
			// it's a backslash. 
			// check if we've already found one the last time, so we can just skip it
			if(!bBackSlashFound)
			{
				// no, so insert a '/' into the string
				sDestBuffer[iDest++]='/';
				bBackSlashFound = true;
			}
		}
	}

	// we're done. 
	sPathAndFile->Format("%s\0",&sDestBuffer[0]);
}


// Purpose: Loads a file from the datafile (zipfile) into memory and returns a pointer to it.
byte
*LoadFileFromDataFile(char *sPathAndFile)
{
	unzFile			ufFile;
	byte			*pBuffer;
	CStdString		sError, sDataFilePathAndFile;
	int				iResult;
	long			lFileLength;

	// if the runtype is RUNTYPE_SAVER_*, get the working dir and put that dir in front of the
	// datafile
	switch(m_gpDemoDat->GetRunType())
	{
		case RUNTYPE_NORMAL:
		case RUNTYPE_NODIALOG:
		{
			sDataFilePathAndFile.Format("%s",m_gpDemoDat->GetDataFilename());
		}; break;
		case RUNTYPE_SAVER_CONFIG:
		case RUNTYPE_SAVER_NORMAL:
		case RUNTYPE_SAVER_PREVIEW:
		case RUNTYPE_SAVER_PASSWORD:
		{
			sDataFilePathAndFile.Format("%s\\%s",m_gpDemoDat->GetWorkingDir(),m_gpDemoDat->GetDataFilename());
		}; break;
	}

	// open zipfile
	ufFile=unzOpen(&sDataFilePathAndFile[0]);
	if(!ufFile)
	{	
		// cannot open zipfile.
		return NULL;
	}

	// locate the file to load in the zipfile.
	if(unzLocateFile(ufFile,sPathAndFile,0)!=UNZ_OK)
	{
		// not found.
		return NULL;
	}

	// get the filelength
	lFileLength = DoFileLengthInDataFileLookup(sPathAndFile);
	if(lFileLength<0)
	{
		// error while checking filelength in datafile
		unzCloseCurrentFile(ufFile);
		unzClose(ufFile);
		return NULL;
	}

	// Open the file
	unzOpenCurrentFile(ufFile);

	pBuffer = (byte *)malloc((lFileLength+1) * sizeof byte);

	// read buffer.
	iResult = unzReadCurrentFile(ufFile,pBuffer,lFileLength);

	if(iResult<0)
	{
		unzCloseCurrentFile(ufFile);
		unzClose(ufFile);
		free(pBuffer);
		return NULL;
	}

	unzCloseCurrentFile(ufFile);
	unzClose(ufFile);

	// pad with 0 for ascii seekroutines.
	pBuffer[lFileLength]='\0';
	
	// return the buffer
	return pBuffer;
}



// Purpose: reports the length of a given file in the datafile.
long
DoFileLengthInDataFileLookup(char *sPathAndFile)
{
	unz_global_info		ugiGlobalInfo;
	int					iError;
	CStdString			sError, sDataFilePathAndFile;
	unz_file_info		ufiFileInfo;
	long				lFileLength;
	unzFile				ufFile;

	// if the runtype is RUNTYPE_SAVER_*, get the working dir and put that dir in front of the
	// datafile
	switch(m_gpDemoDat->GetRunType())
	{
		case RUNTYPE_NORMAL:
		case RUNTYPE_NODIALOG:
		{
			sDataFilePathAndFile.Format("%s",m_gpDemoDat->GetDataFilename());
		}; break;
		case RUNTYPE_SAVER_CONFIG:
		case RUNTYPE_SAVER_NORMAL:
		case RUNTYPE_SAVER_PREVIEW:
		case RUNTYPE_SAVER_PASSWORD:
		{
			sDataFilePathAndFile.Format("%s\\%s",m_gpDemoDat->GetWorkingDir(),m_gpDemoDat->GetDataFilename());
		}; break;
	}

	// open zipfile
	ufFile=unzOpen(&sDataFilePathAndFile[0]);
	if(!ufFile)
	{	
		// can't open zipfile.
		LogFeedback("Can't open Datafile. Aborting function.",true,"DoFileLengthInDataFileLookup",false);
		return SYS_ERRORINZIP;
	}

	iError = unzGetGlobalInfo(ufFile,&ugiGlobalInfo);
	if(iError!=UNZ_OK)
	{
		LogFeedback("Error in datafile. Aborting function.",true,"DoFileLengthInDataFileLookup",false);
		unzClose(ufFile);
		return SYS_ERRORINZIP;
	}

	// lookup the file
	if(unzLocateFile(ufFile,sPathAndFile,0)!=UNZ_OK)
	{
		// not found.
		unzClose(ufFile);
		return SYS_ERRORINZIP;
	}

	// load the fileinfo.
	iError = unzGetCurrentFileInfo(ufFile,&ufiFileInfo,NULL,0,NULL,0,NULL,0);

	if(iError!=UNZ_OK)
	{
		unzClose(ufFile);
		return SYS_ERRORINZIP;
	}

	lFileLength=ufiFileInfo.uncompressed_size;
	
	// close datafile.
	unzClose(ufFile);
	
	return lFileLength;
}


// Purpose: decrypts the passed buffer with the given decryptkey.
void 
DecryptBuffer(long lBufferLength, byte *pBuffer)
{
	long		lSource,lRealLength, lResult;
	int			i, iAmount;
	byte		*pBuf;

	pBuf = pBuffer;
	lRealLength = lBufferLength - (lBufferLength % 4);
	iAmount = (int)(lRealLength / 4);

	for(i=0;i<iAmount;i++)
	{
		lSource = (((long)pBuf[(i*4)]) << 24) |
					(((long)pBuf[(i*4)+1])<<16) |
					(((long)pBuf[(i*4)+2])<<8) |
					((long)pBuf[(i*4)+3]);
		lResult = (long)(((__int64)lSource - (__int64)m_gpDemoDat->GetDecryptionKey()) & 0xFFFFFFFF);
		// lResult now contains the 32bits result longword. Put back the bytes.
		pBuf[(i*4)] = (byte)((lResult & 0xFF000000) >> 24);
		pBuf[(i*4)+1] = (byte)((lResult & 0x00FF0000) >> 16);
		pBuf[(i*4)+2] = (byte)((lResult & 0x0000FF00) >> 8);
		pBuf[(i*4)+3] = (byte)(lResult & 0x000000FF);
	}
	// done
}


// Purpose: Executes the inputbuffer typed in by the user on the inputprompt. For now it uses ExecuteCommand
// to execute the string, because only commands are accepted at themoment.
void
ExecuteInputBuffer()
{
	ExecuteCommand(m_gpDemoDat->GetInputBuffer());
}


// Purpose: calls every enabled eventhandler of every ACTIVE effectobject and passes uMsg,
// wParam and lParam to the eventhandler method MessageHandler()
void
CallEnabledEventHandlers(HWND hWnd, UINT uMsg, WPARAM  wParam, LPARAM  lParam) 
{
	CLayer			*pLayer;
	CEffectStore	*pES;

	for(pLayer=m_gpLayerHead;pLayer;pLayer=pLayer->GetNext())
	{
		// if layer is hidden, then continue
		if(pLayer->GetLayerHidden())
		{
			continue;
		}

		// layer is visible. // get the effectstore object of the effect on this layer
		pES = GetEffectStoreObject(pLayer->GetEffectName());
		if(pES->HasMessageHandler())
		{
			// call handler
			pLayer->GetEffectObject()->MessageHandler(hWnd, uMsg, wParam,lParam);
		}
	}
}

