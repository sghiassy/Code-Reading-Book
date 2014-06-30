//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllscriptfuncs.h
// PURPOSE: include file of implementation of dgl_dllscriptfuncs.cpp
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
// v1.3: Added new functions and constants. Changed some (internal) constants 
// v1.2: Added new functions and constants
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#ifndef _DGL_DLLSCRIPTFUNCS_H
#define _DGL_DLLSCRIPTFUNCS_H

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////
// script specific stuff
#define	SCR_MAXCMDTOKENLENGTH			64
#define SCR_MAXLINELENGTH				1024					// maximum of length of a scriptline. (way too big, so it should be ok)
#define SCR_MAXTOKENSPERROW				DGL_TLE_MAXARGS + 3		// maximum amount of tokens per row in the script.
#define	SCR_DELIMITER					";"
#define	SCR_COMMENTMARKER				'#'

// Predefined objects.
#define _MUSICSYSTEM					"_MUSICSYSTEM"		// DEPRECATED.
#define _SYSTEM							"_SYSTEM"
#define _SOUNDSYSTEM					"_SOUNDSYSTEM"
// add more here.

////////////////////////////////////////////////////////////////////////
//                          
//						ENUMS
//
////////////////////////////////////////////////////////////////////////
// all command tokens possible.
enum ECmdToken
{
	CT_NOP,
	CT_START,
	CT_LOAD,
	CT_PAUSE,
	CT_END,
	CT_CONTINUE,
	CT_PREPARE,
	CT_STOP,
	CT_MOVETOLYR,
	CT_HIDE,
	CT_SHOW,
	CT_SEND,
	CT_SETTIME,
	CT_SEEK,
	CT_CAPTUREMOUSE,
	// Added in 1.3: (_SOUNDSYSTEM specific)
	CT_LOADMP3,
	CT_PLAYMP3,
	CT_STOPMP3,
	CT_PAUSEMP3,
	CT_CONTINUEMP3,
	CT_SEEKINMP3,
	CT_EAXMIXMP3,
	CT_SETVOLUMEMP3,
	CT_SETPANMP3,
	CT_FREEMP3,
	CT_LOADMOD,
	CT_PLAYMOD,
	CT_STOPMOD,
	CT_PAUSEMOD,
	CT_CONTINUEMOD,
	CT_SEEKINMOD,
	CT_EAXMIXMOD,
	CT_SETVOLUMEMOD,
	CT_SETPANMOD,
	CT_FREEMOD,
	CT_LOADSMPL,
	CT_PLAYSMPL,
	CT_STOPSMPL,
	CT_PAUSESMPL,
	CT_CONTINUESMPL,
	CT_SEEKINSMPL,
	CT_EAXMIXSMPL,
	CT_SETVOLUMESMPL,
	CT_SETPANSMPL,
	CT_FREESMPL,
	CT_SETEAXENV,
	CT_SETVOLUME,
	CT_STOPALL,
	CT_PAUSEALL,
	CT_CONTINUEALL,
	// Other new commands:
	CT_RESTART,
	CT_SENDINT,
	CT_SENDFLOAT,
	CT_SENDSTRING,

	/////////////////////////
	// ADD MORE HERE. 
	/////////////////////////

	CT_MAXCMDTOKENS
};

////////////////////////////////////////////////////////////////////////
//                          
//						TYPEDEFS
//
////////////////////////////////////////////////////////////////////////

// Purpose: struct for the CmdTokenarray. Needed for the Lexical analyzer and the ScriptCommandToText routine
typedef struct
{
	ECmdToken		m_iCmdToken;
	char			m_sCmdToken[SCR_MAXCMDTOKENLENGTH];
} SCmdToken;

////////////////////////////////////////////////////////////////////////
//                          
//						FUNCTON DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
extern	void				InitTokenArray(void);
extern	bool				IsNumeric(char *sToCheck);
extern	int					Lex(char *sCLContent);
extern	int					LoadAndParseScript(void);
extern	CTimeLineEvent		*ParseTokens(int iCLCounter, bool bErrorMsgs);
extern	char				*ScriptCommandToText(int iCommand);
extern	char				*SkipWhiteSpace(char *sToSkip);
extern	void				TrimWhiteSpaceR(char *sString);

#endif	// _DGL_DLLSCRIPTFUNCS_H