//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllstdafx.h
// PURPOSE: global include file for DemoGL
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
// v1.3: Reshuffled the order. Added new files. Kicked out not used files.
// v1.2: No changes.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#ifndef _DGL_DLLSTDAFX_H
#define _DGL_DLLSTDAFX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define	DLLEXPORT	__declspec(dllexport)

#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>

#include "Misc\src_beautifiers.h"

#include "Misc\Bass.h"
#include "Misc\StdString.h"
#include "JpegLib\jpegfile.h"
#include "Distribution\DemoGL_glext.h"
#include "Distribution\DemoGL_Extensions.h"
#include "DemoGL\dgl_dllfifoqueue.h"
#include "DemoGL\resource.h"
#include "DemoGL\dgl_dlllowleveldebugger.h"
#include "DemoGL\dgl_dllstartupdialog.h"
#include "DemoGL\dgl_dllbootutilfuncs.h"
#include "DemoGL\dgl_dlldemodat.h"
#include "DemoGL\dgl_dllkernel.h"
#include "DemoGL\dgl_dllscriptfuncs.h"
#include "DemoGL\dgl_dlleffect.h"
#include "DemoGL\dgl_dlleffectstore.h"
#include "DemoGL\dgl_dllendsystem.h"
#include "DemoGL\dgl_dllextensions.h"
#include "DemoGL\dgl_dlllayer.h"
#include "DemoGL\dgl_dlllayerutilfuncs.h"
#include "DemoGL\dgl_dllstartsystem.h"
#include "DemoGL\dgl_dllguicontrol.h"
#include "DemoGL\dgl_dllprogressbar.h"
#include "DemoGL\dgl_dllsysconsole.h"
#include "DemoGL\dgl_dlltexture.h"
#include "DemoGL\dgl_dlltextureutilfuncs.h"
#include "DemoGL\dgl_dlltimelineevent.h"
#include "DemoGL\dgl_dlltimelineutilfuncs.h"
#include "DemoGL\dgl_dlltoken.h"
#include "DemoGL\dgl_dllutilfuncs.h"
#include "DemoGL\dgl_dllmain.h"

#endif	// _DGL_DLLSTDAFX_H