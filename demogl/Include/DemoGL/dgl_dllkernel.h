//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllkernel.h
// PURPOSE: include file of implementation of dgl_dllkernel.cpp
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
// v1.3: Added new functions.
// v1.2: Added new functions.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#ifndef _DGL_DLLKERNEL_H
#define _DGL_DLLKERNEL_H

#include "DemoGL\dgl_dlleffect.h"


////////////////////////////////////////////////////////////////////////
//                          
//						TYPEDEFS
//
////////////////////////////////////////////////////////////////////////

// Purpose: definition of PTHREAD_START macro to have an easy _beginthread() call definition
typedef unsigned (__stdcall *PTHREAD_START) (void *);

////////////////////////////////////////////////////////////////////////
//                          
//						FUNCTION DEFINITIONS
//
////////////////////////////////////////////////////////////////////////

extern	void			CleanUpKernelInternals(void);
extern	void			KernelLoop(void);
extern	long	WINAPI	MainMsgHandler(HWND hWnd, UINT uMsg, WPARAM  wParam, LPARAM  lParam);
extern	void			RenderFrame(void);
extern	void			RenderLayerFrames(void);
extern	void			ReshapeCallBack(int iWidth, int iHeight);

extern	DLLEXPORT	void	DEMOGL_EffectRegister(CEffect * const pEffectObject, const char *pszEffectName);

/////////////////////////////////[IMPORTANT NOTICE]/////////////////////////////////////////////////////
// The functions below are marked 'deprecated' from version 1.3 but still supported for backwards compatibility
// They are just wrappers around the new DEMOGL_ versions so they don't take much DLL space.
//
//				    >>> Please use the new DEMOGL_* functions in your new code <<<
//
/////////////////////////////////[IMPORTANT NOTICE]/////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	extern	DLLEXPORT	void	RegisterEffect(CEffect *pEffectObject, const char *sEffectName);
#ifdef __cplusplus
}
#endif	// __cplusplus
#endif	// _DGL_DLLKERNEL_H