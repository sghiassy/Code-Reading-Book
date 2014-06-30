//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllprogressbar.h
// PURPOSE: definition of the CProgressBar class, the class for the progressbar drawing in 
// a console overlay
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// This class renders the progressbar in the current viewport. it has several types
// build in.
//
// It uses CGuiControlBase as baseclass
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

#ifndef _DGL_DLLPROGRESSBAR_H
#define _DGL_DLLPROGRESSBAR_H

#include "DemoGL\dgl_dllguicontrol.h"

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////
// Progressbar types.
#define	DGL_CNTRL_PGB_LINEAR		1
#define DGL_CNTRL_PGB_BLOCKS_2D		2
//         add more types here


////////////////////////////////////////////////////////////////////////
//                          
//						CLASS DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
// Purpose: progressbar class, derived from CGuiControl base class. 
class CProgressBar:public CGuiControl
{
	public:
							CProgressBar(void);
					virtual	~CProgressBar(void);
		void				IncCurrentValue(int iAmount);
		void				IncMaxValue(int iAmount);
		void				Paint(void);
		void				SetBackGroundColor(float fRed, float fGreen, float fBlue);
		void				SetBarColor(float fRed, float fGreen, float fBlue);
		void				SetCurrentValue(int iValue);
		void				SetMinMaxValues(int iMinimum, int iMaximum);
		void				SetBarType(int iType);

		// NO PUBLIC MEMBERS ALLOWED

	private:
		void				DrawProgressBar_Linear(void);
		void				DrawProgressBar_Blocks_2D(void);
		void				InitMembers(void);

	private:
		int					m_iMaximumValue;
		int					m_iMinimumValue;
		int					m_iCurrentValue;
		int					m_iType;

		float				m_fBGColor_R;
		float				m_fBGColor_G;
		float				m_fBGColor_B;
		float				m_fBarColor_R;
		float				m_fBarColor_G;
		float				m_fBarColor_B;
		CRITICAL_SECTION	m_csValue;
};


#endif	// _DGL_DLLPROGRESSBAR_H