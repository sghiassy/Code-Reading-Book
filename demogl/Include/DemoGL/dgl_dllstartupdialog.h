//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllstartupdialog.h
// PURPOSE: Headerfile for dgl_dllstartupdialog.cpp
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
// v1.2: --
// v1.1: --
//////////////////////////////////////////////////////////////////////

#ifndef _DGL_DLLSTARTUPDIALOG_H
#define _DGL_DLLSTARTUPDIALOG_H

#include "DemoGL\dgl_dllstdafx.h"
#include "DemoGL\dgl_dllstartsystem.h"

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////
// size of logo in OGL floats
#define LWIDTH					(4.0f * 0.7f)
#define LHEIGHT					(4.0f * 0.7f)

// transformation parameters needed to get (0.0) in center of poly
#define XTRANS					(LWIDTH / 2.0f)
#define YTRANS					(LHEIGHT / 2.0f)

// DLLists
#define MAXDGLDLISTS			2
#define FRONTQUADDL				0
#define BACKQUADDL				1

// Texture IDs defines
#define MAXDLGTEXTURES			2
#define LOGOTEXIDIDX			0
#define ENVMAPTEXIDIDX			1

#define DGLLOGOWIDTH			256
#define DGLLOGOHEIGHT			256

////////////////////////////////////////////////////////////////////////
//                          
//						FILE SCOPE FUNCTION DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
void		ControlSoundSystemDialogItems(HWND hDlg,bool bAllOff);
void		DrawDGLLogoInDlg(void);
void		DrawLogoOneSide(GLuint iDLList);
void		ExtractAndUploadADlgTextures(void);

////////////////////////////////////////////////////////////////////////
//                          
//						FUNCTION DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
extern	BOOL CALLBACK	AboutDlgMsgHandler(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
extern	VOID CALLBACK	AboutDlgTimerHandler( HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
extern	long WINAPI		AboutLogoMsgHandler(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
extern	BOOL CALLBACK	StartupDlgMsgHandler(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

#endif	// _DGL_DLLSTARTUPDIALOG_H
