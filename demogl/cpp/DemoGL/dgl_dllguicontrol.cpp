//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllguicontrol.cpp
// PURPOSE: Implementation of the CGuiControl class
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// Implementation of the GUI control Baseclass CGuiControl. All gui controls
// except scrollbar are derived from this class.
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
// v1.2: --
// v1.1: --
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

// Purpose: constructor
CGuiControl::CGuiControl()
{
	InitMembers();
}


// Purpose: destructor
CGuiControl::~CGuiControl()
{

}


// Purpose: gets the Z value of the control. Needed for sorting controls on a screen
// to get them rendered correctly.
float
CGuiControl::GetZ()
{
	return m_fZ;
}


// Purpose: virtual function Paint. Should be overruled by the control class derived from
// this class
void
CGuiControl::Paint()
{
	// nothing
}


// Purpose: initializes membervariables. 
void
CGuiControl::InitMembers()
{
	m_fOpacity=1.0f;
	m_fWidth=0.0f;
	m_fHeight=0.0f;
	m_fBottomLeftX=0.0f;
	m_fBottomLeftY=0.0f;
	m_fZ=0.0f;
	m_bHorizontal=false;
	m_bBlend=false;
	m_iBlendFuncSrc=GL_SRC_COLOR;
	m_iBlendFuncDst=GL_DST_COLOR;
}


// Purpose: sets all blending related properties
void
CGuiControl::SetBlending(bool bBlend, unsigned int iBlendFuncSrc, unsigned int iBlendFuncDst, float fOpacity)
{
	m_bBlend=bBlend;
	m_iBlendFuncSrc=iBlendFuncSrc;
	m_iBlendFuncDst=iBlendFuncDst;
	m_fOpacity=fOpacity;
}


// Purpose: sets position of the control in 3D space. Controls are rendered using 2D techniques
// but because we need to overlay controls on top of eachother we use a Z value to control that.
void
CGuiControl::SetPosition(float fBottomLeftX, float fBottomLeftY, float fZ)
{
	m_fBottomLeftX=fBottomLeftX;
	m_fBottomLeftY=fBottomLeftY;
	m_fZ=fZ;
}


// Purpose: Sets the size of the control.
void
CGuiControl::SetSize(float fWidth, float fHeight)
{
	m_fWidth=fWidth;
	m_fHeight=fHeight;
}


// Purpose: sets the orientation of the control
void
CGuiControl::SetOrientation(bool bHorizontal)
{
	m_bHorizontal=bHorizontal;
}





