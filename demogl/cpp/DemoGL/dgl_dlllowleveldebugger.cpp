//////////////////////////////////////////////////////////////////////
// FILE: dgl_dlllowleveldebugger.cpp
// PURPOSE: in here are the lowlevel debugroutines.
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// Lowlevel debug routines for the DemoGL dll itself. it will write into a
// logfile. Nice for debugging of the dll when the console is not enough. 
//
// Specify _DGLDEBUG in projectproperties at the Cpreprocessor directives, so the
// routines get compiled in the code.
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
// v1.3: Added to codebase.
// v1.2: -
// v1.1: -
//////////////////////////////////////////////////////////////////////

#include <time.h>
#include <stdio.h>

#include "Demogl\dgl_dllstdafx.h"

//////////////////////////////////////////////////////////////////////
//
//                       GLOBALS
//
//////////////////////////////////////////////////////////////////////

static	FILE	*m_gfpLogFile=NULL;
static	int		m_giCallLevel=0;
static	char	m_gsCallLevelTabs[DGLDBG_MAXCALLLEVELS];

//////////////////////////////////////////////////////////////////////
//
//                        CODE. 
//
// All routines will be empty if _DGLDEBUG is not specified as C preprocessor directive.
//
//////////////////////////////////////////////////////////////////////

// Purpose: opens the logfile for writing. Call this routine before anything
// else. Call _dgldbg_CloseLogFile() to close the file.
// The logfile can be dumped in the root of the systemdrive, if the application is run
// as a screensaver. Adjust name in .h file eventually.
void
_dgldbg_OpenLogFile()
{
#ifdef _DGLDEBUG
	time_t		lTime;

	time(&lTime);

	if(!m_gfpLogFile)
	{
		// open logfile for writing. this automatically clears the contents already written
		m_gfpLogFile=fopen(DGLDBG_LOGFILENAME,"w");
		fputs("Created and opened on: ",m_gfpLogFile);
		fputs(ctime(&lTime),m_gfpLogFile);
		fputs("----------------------------------------\n\n",m_gfpLogFile);
	}
	else
	{
		// not closed and reopened.
		fputs("\n--------\nFile closed by lowlevel debugger and reopened.\n--------\n",m_gfpLogFile);
		fclose(m_gfpLogFile);
		// reopen for append.
		m_gfpLogFile=fopen(DGLDBG_LOGFILENAME,"wa");
	}
#endif
}


// Purpose: closes the logfile opened with _dgldbg_OpenLogFile()
void
_dgldbg_CloseLogFile()
{
#ifdef _DGLDEBUG
	if(m_gfpLogFile)
	{
		fclose(m_gfpLogFile);
	}
#endif
}


// Purpose: logs the given string into the logfile, also the name of the callerfunction.
// Use this to log a certain string from within a routine. use the special start/stop routines
// to log entrance and exits of functions.
void
_dgldbg_LogString(char *pszToLog, char *pszCallerName)
{
#ifdef _DGLDEBUG
	CStdString			sToLog;

	if(m_gfpLogFile)
	{
		sToLog.Format("%s%s logs: %s\n",_dgldbg_CallLevel2String(),pszCallerName, pszToLog);
		fputs(&sToLog[0],m_gfpLogFile);
		// flush to disk so filecache will not fuck up debugging when dll dies
		fflush(m_gfpLogFile);
	}
#endif
}


// Purpose: special routine to log the entrance of a routine
void
_dgldbg_EnteredRoutine(char *pszRoutineName)
{
#ifdef _DGLDEBUG
	CStdString			sToLog;

	if(m_gfpLogFile)
	{
		sToLog.Format("%sEntered: %s\n",_dgldbg_CallLevel2String(),pszRoutineName);
		fputs(&sToLog[0],m_gfpLogFile);
		// flush to disk so filecache will not fuck up debugging when dll dies
		fflush(m_gfpLogFile);
	}
	m_giCallLevel++;
#endif
}


// Purpose: special routines to log the leaving of a routine
void
_dgldbg_LeftRoutine(char *pszRoutineName, int iReturnValue)
{
#ifdef _DGLDEBUG
	CStdString			sToLog;

	m_giCallLevel--;

	if(m_gfpLogFile)
	{
		sToLog.Format("%sLeft: %s. Returnvalue (int): %d\n",_dgldbg_CallLevel2String(),pszRoutineName, iReturnValue);
		fputs(&sToLog[0],m_gfpLogFile);
		// flush to disk so filecache will not fuck up debugging when dll dies
		fflush(m_gfpLogFile);
	}
#endif
}


// Purpose: special routines to log the leaving of a routine
void
_dgldbg_LeftRoutine(char *pszRoutineName)
{
#ifdef _DGLDEBUG
	CStdString			sToLog;

	m_giCallLevel--;

	if(m_gfpLogFile)
	{
		sToLog.Format("%sLeft: %s. No returnvalue.\n",_dgldbg_CallLevel2String(),pszRoutineName);
		fputs(&sToLog[0],m_gfpLogFile);
		// flush to disk so filecache will not fuck up debugging when dll dies
		fflush(m_gfpLogFile);
	}
#endif
}


// Purpose: special routines to log the leaving of a routine
void
_dgldbg_LeftRoutine(char *pszRoutineName, char *pszReturnValue)
{
#ifdef _DGLDEBUG
	CStdString			sToLog;

	m_giCallLevel--;

	if(m_gfpLogFile)
	{
		sToLog.Format("%sLeft: %s. Returnvalue (string): %s\n",_dgldbg_CallLevel2String(),
							pszRoutineName, pszReturnValue);
		fputs(&sToLog[0],m_gfpLogFile);
		// flush to disk so filecache will not fuck up debugging when dll dies
		fflush(m_gfpLogFile);
	}
#endif
}


// Purpose: special routines to log the leaving of a routine
void
_dgldbg_LeftRoutine(char *pszRoutineName, void *pReturnVal)
{
#ifdef _DGLDEBUG
	CStdString			sToLog;

	m_giCallLevel--;

	if(m_gfpLogFile)
	{
		sToLog.Format("%sLeft: %s. Returnvalue (pointer) %X\n",_dgldbg_CallLevel2String(),
							pszRoutineName, pReturnVal);
		fputs(&sToLog[0],m_gfpLogFile);
		// flush to disk so filecache will not fuck up debugging when dll dies
		fflush(m_gfpLogFile);
	}
#endif
}


// Purpose: logs the passed message into the logfile.
void
_dgldbg_LogMessage(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef _DGLDEBUG
	CStdString			sToLog;

	if(m_gfpLogFile)
	{
		sToLog.Format("%sMSG received: %s (%X). HWND=%d. wParam=%X. lParam=%X. SystemState: %s\n",
				_dgldbg_CallLevel2String(),	_dgldbg_Message2String(uiMsg), uiMsg, hWnd,
					wParam, lParam, _dgldbg_SystemState2String());
		fputs(&sToLog[0],m_gfpLogFile);
		// flush to disk so filecache will not fuck up debugging when dll dies
		fflush(m_gfpLogFile);
	}
#endif
}


// Purpose: inserts tabs for every calllevel deep we're called. Returns a pointer to the global
// array m_gsCallLevelTabs()
char
*_dgldbg_CallLevel2String()
{
#ifdef _DGLDEBUG
	int		i;

	memset(m_gsCallLevelTabs,0,DGLDBG_MAXCALLLEVELS);
	for(i=0;(i<m_giCallLevel) && (i<DGLDBG_MAXCALLLEVELS);i++)
	{
		m_gsCallLevelTabs[i]='\t';
	}
	return &m_gsCallLevelTabs[0];
#else
	return NULL;
#endif
}


// Purpose: converts the current systemstate to string.
char
*_dgldbg_SystemState2String()
{
#ifdef _DGLDEBUG
	switch(m_gpDemoDat->GetSystemState())
	{
		case SSTATE_PREBOOT: return "SSTATE_PREBOOT";
		case SSTATE_BOOT_STARTUPDIALOG: return "SSTATE_BOOT_STARTUPDIALOG";
		case SSTATE_BOOT_MAINWNDCREATION: return "SSTATE_BOOT_MAINWNDCREATION";
		case SSTATE_BOOT_MESSAGEPUMPSTART: return "SSTATE_BOOT_MESSAGEPUMPSTART";
		case SSTATE_BOOT_INITSTART: return "SSTATE_BOOT_INITSTART";
		case SSTATE_BOOT_SYSOBJECTSINIT: return "SSTATE_BOOT_SYSOBJECTSINIT";
		case SSTATE_AINIT_EFFECTSINIT: return "SSTATE_AINIT_EFFECTSINIT";
		case SSTATE_AINIT_SCRIPTPARSING: return "SSTATE_AINIT_SCRIPTPARSING";
		case SSTATE_AINIT_PRESTARTEVENTEXEC: return "SSTATE_AINIT_PRESTARTEVENTEXEC";
		case SSTATE_ARUN_KERNELLOOP: return "SSTATE_ARUN_KERNELLOOP";
		case SSTATE_ARUN_POSTKERNELLOOP: return "SSTATE_ARUN_POSTKERNELLOOP";
		case SSTATE_AEND_SYSTEMCLEANUP: return "SSTATE_AEND_SYSTEMCLEANUP";
		default: return "UNKNOWN STATE";
	}
#else
	return NULL;
#endif
}



// Purpose: this function returns the stringversion of a message.
// this large list ripped from Minimal Screensaver Code by Lucian Wischik. 
// and adjusted to fit the needs of DemoGL.
char
*_dgldbg_Message2String(UINT uiMsg)
{
#ifdef _DGLDEBUG
	switch(uiMsg)
	{ 
		case WM_NULL: return "WM_NULL";
		case WM_CREATE: return "WM_CREATE";
		case WM_DESTROY: return "WM_DESTROY";
		case WM_MOVE: return "WM_MOVE";
		case WM_SIZE: return "WM_SIZE";
		case WM_ACTIVATE: return "WM_ACTIVATE";
		case WM_SETFOCUS: return "WM_SETFOCUS";
		case WM_KILLFOCUS: return "WM_KILLFOCUS";
		case WM_ENABLE: return "WM_ENABLE";
		case WM_SETREDRAW: return "WM_SETREDRAW";
		case WM_SETTEXT: return "WM_SETTEXT";
		case WM_GETTEXT: return "WM_GETTEXT";
		case WM_GETTEXTLENGTH: return "WM_GETTEXTLENGTH";
		case WM_PAINT: return "WM_PAINT";
		case WM_CLOSE: return "WM_CLOSE";
		case WM_QUERYENDSESSION: return "WM_QUERYENDSESSION";
		case WM_QUIT: return "WM_QUIT";
		case WM_QUERYOPEN: return "WM_QUERYOPEN";
		case WM_ERASEBKGND: return "WM_ERASEBKGND";
		case WM_SYSCOLORCHANGE: return "WM_SYSCOLORCHANGE";
		case WM_ENDSESSION: return "WM_ENDSESSION";
		case WM_SHOWWINDOW: return "WM_SHOWWINDOW";
		case WM_SETTINGCHANGE: return "WM_SETTINGCHANGE";
		case WM_DEVMODECHANGE: return "WM_DEVMODECHANGE";
		case WM_ACTIVATEAPP: return "WM_ACTIVATEAPP";
		case WM_FONTCHANGE: return "WM_FONTCHANGE";
		case WM_TIMECHANGE: return "WM_TIMECHANGE";
		case WM_CANCELMODE: return "WM_CANCELMODE";
		case WM_SETCURSOR: return "WM_SETCURSOR";
		case WM_MOUSEACTIVATE: return "WM_MOUSEACTIVATE";
		case WM_CHILDACTIVATE: return "WM_CHILDACTIVATE";
		case WM_QUEUESYNC: return "WM_QUEUESYNC";
		case WM_GETMINMAXINFO: return "WM_GETMINMAXINFO";
		case WM_PAINTICON: return "WM_PAINTICON";
		case WM_ICONERASEBKGND: return "WM_ICONERASEBKGND";
		case WM_NEXTDLGCTL: return "WM_NEXTDLGCTL";
		case WM_SPOOLERSTATUS: return "WM_SPOOLERSTATUS";
		case WM_DRAWITEM: return "WM_DRAWITEM";
		case WM_MEASUREITEM: return "WM_MEASUREITEM";
		case WM_DELETEITEM: return "WM_DELETEITEM";
		case WM_VKEYTOITEM: return "WM_VKEYTOITEM";
		case WM_CHARTOITEM: return "WM_CHARTOITEM";
		case WM_SETFONT: return "WM_SETFONT";
		case WM_GETFONT: return "WM_GETFONT";
		case WM_SETHOTKEY: return "WM_SETHOTKEY";
		case WM_GETHOTKEY: return "WM_GETHOTKEY";
		case WM_QUERYDRAGICON: return "WM_QUERYDRAGICON";
		case WM_COMPAREITEM: return "WM_COMPAREITEM";
		case WM_COMPACTING: return "WM_COMPACTING";
		case WM_COMMNOTIFY: return "WM_COMMNOTIFY";
		case WM_WINDOWPOSCHANGING: return "WM_WINDOWPOSCHANGING";
		case WM_WINDOWPOSCHANGED: return "WM_WINDOWPOSCHANGED";
		case WM_POWER: return "WM_POWER";
		case WM_COPYDATA: return "WM_COPYDATA";
		case WM_CANCELJOURNAL: return "WM_CANCELJOURNAL";
		case WM_NOTIFY: return "WM_NOTIFY";
		case WM_INPUTLANGCHANGEREQUEST: return "WM_INPUTLANGCHANGEREQUEST";
		case WM_INPUTLANGCHANGE: return "WM_INPUTLANGCHANGE";
		case WM_TCARD: return "WM_TCARD";
		case WM_HELP: return "WM_HELP";
		case WM_USERCHANGED: return "WM_USERCHANGED";
		case WM_NOTIFYFORMAT: return "WM_NOTIFYFORMAT";
		case WM_CONTEXTMENU: return "WM_CONTEXTMENU";
		case WM_STYLECHANGING: return "WM_STYLECHANGING";
		case WM_STYLECHANGED: return "WM_STYLECHANGED";
		case WM_DISPLAYCHANGE: return "WM_DISPLAYCHANGE";
		case WM_GETICON: return "WM_GETICON";
		case WM_SETICON: return "WM_SETICON";
		case WM_NCCREATE: return "WM_NCCREATE";
		case WM_NCDESTROY: return "WM_NCDESTROY";
		case WM_NCCALCSIZE: return "WM_NCCALCSIZE";
		case WM_NCHITTEST: return "WM_NCHITTEST";
		case WM_NCPAINT: return "WM_NCPAINT";
		case WM_NCACTIVATE: return "WM_NCACTIVATE";
		case WM_GETDLGCODE: return "WM_GETDLGCODE";
		case WM_NCMOUSEMOVE: return "WM_NCMOUSEMOVE";
		case WM_NCLBUTTONDOWN: return "WM_NCLBUTTONDOWN";
		case WM_NCLBUTTONUP: return "WM_NCLBUTTONUP";
		case WM_NCLBUTTONDBLCLK: return "WM_NCLBUTTONDBLCLK";
		case WM_NCRBUTTONDOWN: return "WM_NCRBUTTONDOWN";
		case WM_NCRBUTTONUP: return "WM_NCRBUTTONUP";
		case WM_NCRBUTTONDBLCLK: return "WM_NCRBUTTONDBLCLK";
		case WM_NCMBUTTONDOWN: return "WM_NCMBUTTONDOWN";
		case WM_NCMBUTTONUP: return "WM_NCMBUTTONUP";
		case WM_NCMBUTTONDBLCLK: return "WM_NCMBUTTONDBLCLK";
		case WM_KEYDOWN: return "WM_KEYDOWN";
		case WM_KEYUP: return "WM_KEYUP";
		case WM_CHAR: return "WM_CHAR";
		case WM_DEADCHAR: return "WM_DEADCHAR";
		case WM_SYSKEYDOWN: return "WM_SYSKEYDOWN";
		case WM_SYSKEYUP: return "WM_SYSKEYUP";
		case WM_SYSCHAR: return "WM_SYSCHAR";
		case WM_SYSDEADCHAR: return "WM_SYSDEADCHAR";
		case WM_IME_STARTCOMPOSITION: return "WM_IME_STARTCOMPOSITION";
		case WM_IME_ENDCOMPOSITION: return "WM_IME_ENDCOMPOSITION";
		case WM_IME_COMPOSITION: return "WM_IME_COMPOSITION";
		case WM_INITDIALOG: return "WM_INITDIALOG";
		case WM_COMMAND: return "WM_COMMAND";
		case WM_SYSCOMMAND: return "WM_SYSCOMMAND";
		case WM_TIMER: return "WM_TIMER";
		case WM_HSCROLL: return "WM_HSCROLL";
		case WM_VSCROLL: return "WM_VSCROLL";
		case WM_INITMENU: return "WM_INITMENU";
		case WM_INITMENUPOPUP: return "WM_INITMENUPOPUP";
		case WM_MENUSELECT: return "WM_MENUSELECT";
		case WM_MENUCHAR: return "WM_MENUCHAR";
		case WM_ENTERIDLE: return "WM_ENTERIDLE";
		case WM_CTLCOLORMSGBOX: return "WM_CTLCOLORMSGBOX";
		case WM_CTLCOLOREDIT: return "WM_CTLCOLOREDIT";
		case WM_CTLCOLORLISTBOX: return "WM_CTLCOLORLISTBOX";
		case WM_CTLCOLORBTN: return "WM_CTLCOLORBTN";
		case WM_CTLCOLORDLG: return "WM_CTLCOLORDLG";
		case WM_CTLCOLORSCROLLBAR: return "WM_CTLCOLORSCROLLBAR";
		case WM_CTLCOLORSTATIC: return "WM_CTLCOLORSTATIC";
		case WM_MOUSEMOVE: return "WM_MOUSEMOVE";
		case WM_LBUTTONDOWN: return "WM_LBUTTONDOWN";
		case WM_LBUTTONUP: return "WM_LBUTTONUP";
		case WM_LBUTTONDBLCLK: return "WM_LBUTTONDBLCLK";
		case WM_RBUTTONDOWN: return "WM_RBUTTONDOWN";
		case WM_RBUTTONUP: return "WM_RBUTTONUP";
		case WM_RBUTTONDBLCLK: return "WM_RBUTTONDBLCLK";
		case WM_MBUTTONDOWN: return "WM_MBUTTONDOWN";
		case WM_MBUTTONUP: return "WM_MBUTTONUP";
		case WM_MBUTTONDBLCLK: return "WM_MBUTTONDBLCLK";
		case WM_PARENTNOTIFY: return "WM_PARENTNOTIFY";
		case WM_ENTERMENULOOP: return "WM_ENTERMENULOOP";
		case WM_EXITMENULOOP: return "WM_EXITMENULOOP";
		case WM_NEXTMENU: return "WM_NEXTMENU";
		case WM_SIZING: return "WM_SIZING";
		case WM_CAPTURECHANGED: return "WM_CAPTURECHANGED";
		case WM_MOVING: return "WM_MOVING";
		case WM_POWERBROADCAST: return "WM_POWERBROADCAST";
		case WM_DEVICECHANGE: return "WM_DEVICECHANGE";
		case WM_IME_SETCONTEXT: return "WM_IME_SETCONTEXT";
		case WM_IME_NOTIFY: return "WM_IME_NOTIFY";
		case WM_IME_CONTROL: return "WM_IME_CONTROL";
		case WM_IME_COMPOSITIONFULL: return "WM_IME_COMPOSITIONFULL";
		case WM_IME_SELECT: return "WM_IME_SELECT";
		case WM_IME_CHAR: return "WM_IME_CHAR";
		case WM_IME_KEYDOWN: return "WM_IME_KEYDOWN";
		case WM_IME_KEYUP: return "WM_IME_KEYUP";
		case WM_MDICREATE: return "WM_MDICREATE";
		case WM_MDIDESTROY: return "WM_MDIDESTROY";
		case WM_MDIACTIVATE: return "WM_MDIACTIVATE";
		case WM_MDIRESTORE: return "WM_MDIRESTORE";
		case WM_MDINEXT: return "WM_MDINEXT";
		case WM_MDIMAXIMIZE: return "WM_MDIMAXIMIZE";
		case WM_MDITILE: return "WM_MDITILE";
		case WM_MDICASCADE: return "WM_MDICASCADE";
		case WM_MDIICONARRANGE: return "WM_MDIICONARRANGE";
		case WM_MDIGETACTIVE: return "WM_MDIGETACTIVE";
		case WM_MDISETMENU: return "WM_MDISETMENU";
		case WM_ENTERSIZEMOVE: return "WM_ENTERSIZEMOVE";
		case WM_EXITSIZEMOVE: return "WM_EXITSIZEMOVE";
		case WM_DROPFILES: return "WM_DROPFILES";
		case WM_MDIREFRESHMENU: return "WM_MDIREFRESHMENU";
		case WM_CUT: return "WM_CUT";
		case WM_COPY: return "WM_COPY";
		case WM_PASTE: return "WM_PASTE";
		case WM_CLEAR: return "WM_CLEAR";
		case WM_UNDO: return "WM_UNDO";
		case WM_RENDERFORMAT: return "WM_RENDERFORMAT";
		case WM_RENDERALLFORMATS: return "WM_RENDERALLFORMATS";
		case WM_DESTROYCLIPBOARD: return "WM_DESTROYCLIPBOARD";
		case WM_DRAWCLIPBOARD: return "WM_DRAWCLIPBOARD";
		case WM_PAINTCLIPBOARD: return "WM_PAINTCLIPBOARD";
		case WM_VSCROLLCLIPBOARD: return "WM_VSCROLLCLIPBOARD";
		case WM_SIZECLIPBOARD: return "WM_SIZECLIPBOARD";
		case WM_ASKCBFORMATNAME: return "WM_ASKCBFORMATNAME";
		case WM_CHANGECBCHAIN: return "WM_CHANGECBCHAIN";
		case WM_HSCROLLCLIPBOARD: return "WM_HSCROLLCLIPBOARD";
		case WM_QUERYNEWPALETTE: return "WM_QUERYNEWPALETTE";
		case WM_PALETTEISCHANGING: return "WM_PALETTEISCHANGING";
		case WM_PALETTECHANGED: return "WM_PALETTECHANGED";
		case WM_HOTKEY: return "WM_HOTKEY";
		case WM_PRINT: return "WM_PRINT";
		case WM_PRINTCLIENT: return "WM_PRINTCLIENT";
		case WM_HANDHELDFIRST: return "WM_HANDHELDFIRST";
		case WM_HANDHELDLAST: return "WM_HANDHELDLAST";
		case WM_AFXFIRST: return "WM_AFXFIRST";
		case WM_AFXLAST: return "WM_AFXLAST";
		case WM_PENWINFIRST: return "WM_PENWINFIRST";
		case WM_PENWINLAST: return "WM_PENWINLAST";
		case WM_DEMOGL_INITSYSTEM: return "WM_DEMOGL_INITSYSTEM";
		case WM_DEMOGL_INITAPP: return "WM_DEMOGL_INITAPP";
		case WM_DEMOGL_ABORT: return "WM_DEMOGL_ABORT";
		case WM_DEMOGL_STARTKERNELLOOP: return "WM_DEMOGL_STARTKERNELLOOP";
		case WM_DEMOGL_PREPARE: return "WM_DEMOGL_PREPARE";
		case WM_DEMOGL_PREPAREEFFECT: return "WM_DEMOGL_PREPAREEFFECT";
		case WM_DEMOGL_PAINT: return "WM_DEMOGL_PAINT";
		default: return "WM_????";
	}
#else
	return NULL;
#endif
}










