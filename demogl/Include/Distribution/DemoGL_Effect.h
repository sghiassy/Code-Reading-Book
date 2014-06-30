//////////////////////////////////////////////////////////////////////
// FILE: DemoGL_Effect.h
// PURPOSE: Include file to use the effectbaseclass
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// Definition of CEffect base class
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
//////////////////////////////////////////////////////////////////////
// Contributers to the code:
//		- Frans Bouma [FB]
//////////////////////////////////////////////////////////////////////
// VERSION INFORMATION.
//
// v1.3: Added methods.
// v1.2: Added methods.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#ifndef _DEMOGL_EFFECT_H
#define _DEMOGL_EFFECT_H

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////
#define DLLIMPORT	__declspec(dllimport)


////////////////////////////////////////////////////////////////////////
//                          
//						CLASS DEFINITIONS. _DO NOT ALTER THIS_
//
////////////////////////////////////////////////////////////////////////

class DLLIMPORT	CEffect
{
	// PUBLIC MEMBERVARIABLES NOT ALLOWED

	// public method declarations
	////////////////////////////////
	//
	// DO NOT ALTER THIS
	//
	////////////////////////////////
	public:
						CEffect();
		virtual			~CEffect();
		virtual	int		Init(int iWidth, int iHeight);
		virtual	void	RenderFrame(long lElapsedTime);
		virtual void	ReceiveParam(char *sParam);			// DEPRECATED FROM v1.3
		virtual	void	Prepare(void);
		virtual	void	End(void);
		virtual void	MessageHandler(HWND hWnd, UINT uMsg, WPARAM  wParam, LPARAM  lParam);
		virtual void	ReceiveInt(char *pszName, int iParam, long lLayer);
		virtual void	ReceiveFloat(char *pszName, float fParam, long lLayer);
		virtual void	ReceiveString(char *pszName, char *pszParam, long lLayer);
		virtual void	RenderFrameEx(long lElapsedTime, long lLayer);
};

#endif	// _DEMOGL_EFFECT_H