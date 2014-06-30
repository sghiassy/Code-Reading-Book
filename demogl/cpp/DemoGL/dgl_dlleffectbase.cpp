//////////////////////////////////////////////////////////////////////
// FILE: dgl_dlleffectbase.cpp
// PURPOSE: Implementation of the CEffect baseclass	
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// Definition of Base class for effectclasses.
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
// v1.3: Added new methods.
// v1.2: Added new methods.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

// local includes
#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

// Purpose: constructor
CEffect::CEffect()
{
}


// Purpose: destructor
CEffect::~CEffect()
{
}


// Purpose: init function. Will be called when derived class is registered and initialized by DemoGL.
// Is Virtual function so can be overruled.
int
CEffect::Init(int iWidth, int iHeight)
{
	return SYS_OK;
}


// Purpose: prepare function. Will be called when there is a prepare command specified in the script for the derived class.
// Is Virtual function so can be overruled
void
CEffect::Prepare()
{

}


///////////////////////////////
// DEPRECATED
///////////////////////////////
// Purpose: ReceiveParam function. This function is called by the SEND command, so it's possible to send custom parameters
// to the effectobjects at predefined times. This way you can control what the object is doing on a certain time.
void
CEffect::ReceiveParam(char *sParam)
{
}
///////////////////////////////
// END DEPRECATED
///////////////////////////////


// Purpose: render frame function that will be called every framecycle when the derived effect is on stage :)
// Is Virtual function so can be overruled. (Has to be overruled. this one does nothing)
// The current time elapsed in seconds is passed
void
CEffect::RenderFrame(long lElapsedTime)
{
}


// Purpose: ends the effect. Will be called when there is an END command specified or at the end of the program
// Is Virtual function so can be overruled
void
CEffect::End()
{
}


// Purpose: this is the messagehandler routine of the effectobject. DemoGL will call this routine when the effect is
// on a visible layer AND the effect has enabled the messagehandler.
void
CEffect::MessageHandler(HWND hWnd, UINT uMsg, WPARAM  wParam, LPARAM  lParam)
{
}


// Purpose: Receives an integer parameter send by the scriptcommand SENDINT. Also passed are the name of the
// value and the layer for which this value is ment for. 
void
CEffect::ReceiveInt(char *pszName, int iParam, long lLayer)
{
}

// Purpose: Receives a float parameter send by the scriptcommand SENDFLOAT. Also passed are the name of the
// value and the layer for which this value is ment for.  
void
CEffect::ReceiveFloat(char *pszName, float fParam, long lLayer)
{
}

// Purpose: Receives a string parameter send by the scriptcommand SENDSTRING. Also passed are the name of the
// value and the layer for which this value is ment for. 
void
CEffect::ReceiveString(char *pszName, char *pszParam, long lLayer)
{
}


// Purpose: As RenderFrame, but now also the layernumber is passed for which this renderframe is called.
// f.e. If an effect runs on 2 layers, RenderFrameEx is called twice, once with the first layer number the
// effect is running on, then with the second layernumber the effect is running on. This way the effect
// code can determine how to render itself based on the layer it's on.
void
CEffect::RenderFrameEx(long lElapsedTime, long lLayer)
{
}