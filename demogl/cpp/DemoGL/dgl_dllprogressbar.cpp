//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllprogressbar.cpp
// PURPOSE: Implementation of the CProgressBar class
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
CProgressBar::CProgressBar()
{
	InitMembers();

	// Initialize the mutex for the progressbar value access. This has to be thread safe.
	InitializeCriticalSection(&m_csValue);
}


// Purpose: destructor
CProgressBar::~CProgressBar()
{
	// Delete the mutex
	DeleteCriticalSection(&m_csValue);
}


// Purpose: initializes membervariables
void
CProgressBar::InitMembers()
{
	m_iMaximumValue=0;
	m_iMinimumValue=0;
	m_iCurrentValue=0;
	m_iType=0;
	m_fBGColor_R=0.0f;
	m_fBGColor_G=0.0f;
	m_fBGColor_B=0.0f;
	m_fBarColor_R=0.0f;
	m_fBarColor_G=0.0f;
	m_fBarColor_B=0.0f;
}


// Purpose: Increases the current value with iAmount
void
CProgressBar::IncCurrentValue(int iAmount)
{
	if(iAmount>0)
	{
		EnterCriticalSection(&m_csValue);
			// don't allow negative numbers
			m_iCurrentValue+=iAmount;
			if(m_iCurrentValue > m_iMaximumValue)
			{
				m_iCurrentValue=m_iMaximumValue;
			}
		LeaveCriticalSection(&m_csValue);
	}
}


// Purpose: increase the maximum value with iAmount
void
CProgressBar::IncMaxValue(int iAmount)
{
	if(iAmount>0)
	{
		EnterCriticalSection(&m_csValue);
			// don't allow negative numbers
			m_iMaximumValue+=iAmount;
		LeaveCriticalSection(&m_csValue);
	}
}


// Purpose: paints the progressbar, using the drawroutine that belongs to the current type.
void
CProgressBar::Paint()
{
	switch(m_iType)
	{
		case DGL_CNTRL_PGB_LINEAR:
		{
			DrawProgressBar_Linear();
		}; break;
		case DGL_CNTRL_PGB_BLOCKS_2D:
		{
			DrawProgressBar_Blocks_2D();
		}; break;
		// Add more types here.
	}
}


// Purpose: sets the background color of the progressbar. This background color will be the
// 'off color'
void
CProgressBar::SetBackGroundColor(float fRed, float fGreen, float fBlue)
{
	m_fBGColor_R=fRed;
	m_fBGColor_G=fGreen;
	m_fBGColor_B=fBlue;
}


// Purpose: sets the barcolor.
void
CProgressBar::SetBarColor(float fRed, float fGreen, float fBlue)
{
	m_fBarColor_R=fRed;
	m_fBarColor_G=fGreen;
	m_fBarColor_B=fBlue;
}


// Purpose: sets the current value of the progressbar, affecting the bar visuals. Clamped to
// maximum value and to min value
void
CProgressBar::SetCurrentValue(int iValue)
{
	EnterCriticalSection(&m_csValue);
		m_iCurrentValue=iValue;
		if(m_iCurrentValue>m_iMaximumValue)
		{
			m_iCurrentValue=m_iMaximumValue;
		}
		if(m_iCurrentValue<m_iMinimumValue)
		{
			m_iCurrentValue=m_iMinimumValue;
		}
	LeaveCriticalSection(&m_csValue);
}


// Purpose: sets minimum and maximum values. Current value will be set to minimum.
// Will fail if minimum > maximum. 
void
CProgressBar::SetMinMaxValues(int iMinimum, int iMaximum)
{
	EnterCriticalSection(&m_csValue);
		if(iMinimum<=iMaximum)
		{
			// allowed
			m_iMinimumValue=iMinimum;
			m_iMaximumValue=iMaximum;
			m_iCurrentValue=m_iMinimumValue;
		}
	LeaveCriticalSection(&m_csValue);
}


// Purpose: sets the progressbar type.
void
CProgressBar::SetBarType(int iType)
{
	m_iType=iType;
}


// Purpose: draws a linear progressbar. (type DGL_CNTRL_PGB_LINEAR)
void
CProgressBar::DrawProgressBar_Linear()
{
	float	fX1, fY1, fX2, fY2;

	// Draw linear progressbar with 2 quads.
	glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		if(CGuiControl::m_bBlend)
		{
			glEnable(GL_BLEND);
			// set blendfunc
			glBlendFunc(CGuiControl::m_iBlendFuncSrc,CGuiControl::m_iBlendFuncDst);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		////////////////////////
		// Draw background.
		////////////////////////
		fX1=CGuiControl::m_fBottomLeftX;
		fY1=CGuiControl::m_fBottomLeftY;
		fX2=fX1+CGuiControl::m_fWidth;
		fY2=fY1+CGuiControl::m_fHeight;

		glColor4f(m_fBGColor_R, m_fBGColor_G, m_fBGColor_B, CGuiControl::m_fOpacity);
		glBegin(GL_QUADS);
			glVertex3f(fX1,fY1,CGuiControl::m_fZ);
			glVertex3f(fX2,fY1,CGuiControl::m_fZ);
			glVertex3f(fX2,fY2,CGuiControl::m_fZ);
			glVertex3f(fX1,fY2,CGuiControl::m_fZ);
		glEnd();

		if(!((m_iMinimumValue==m_iMaximumValue)||(m_iCurrentValue==m_iMinimumValue)))
		{
			// there is a bar drawable.

			////////////////////////
			// Draw bar
			////////////////////////
			// X1 and Y1 stay the same.
			if(CGuiControl::m_bHorizontal)
			{
				// horizontal bar
				// fY2 stays the same.
				fX2=fX1 + (CGuiControl::m_fWidth * ( float(m_iCurrentValue-m_iMinimumValue)/ float(m_iMaximumValue-m_iMinimumValue)));
			}
			else
			{
				// vertical bar
				// fX2 stays the same
				fY2=fY1 + (CGuiControl::m_fHeight * ( float(m_iCurrentValue-m_iMinimumValue)/ float(m_iMaximumValue-m_iMinimumValue)));
			}

			glColor4f(m_fBarColor_R, m_fBarColor_G, m_fBarColor_B, CGuiControl::m_fOpacity);
			glBegin(GL_QUADS);
				glVertex3f(fX1,fY1,CGuiControl::m_fZ);
				glVertex3f(fX2,fY1,CGuiControl::m_fZ);
				glVertex3f(fX2,fY2,CGuiControl::m_fZ);
				glVertex3f(fX1,fY2,CGuiControl::m_fZ);
			glEnd();
		}
	glPopAttrib();
}


// Purpose: draws a progress bar with 2D blocks. (type DGL_CNTRL_PGB_BLOCKS_2D)
void
CProgressBar::DrawProgressBar_Blocks_2D()
{
	float	fX1, fY1, fX2, fY2, fDeltaX, fDeltaY, fBlockWidth, fBlockHeight;
	int		i, iMaxAmountBlocks;

	// Draw linear progressbar with 2 quads.
	glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		if(CGuiControl::m_bBlend)
		{
			glEnable(GL_BLEND);
			// set blendfunc
			glBlendFunc(CGuiControl::m_iBlendFuncSrc,CGuiControl::m_iBlendFuncDst);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		////////////////////////
		// Draw background.
		////////////////////////
		fX1=CGuiControl::m_fBottomLeftX;
		fY1=CGuiControl::m_fBottomLeftY;
		fX2=fX1+CGuiControl::m_fWidth;
		fY2=fY1+CGuiControl::m_fHeight;

		glColor4f(m_fBGColor_R, m_fBGColor_G, m_fBGColor_B, CGuiControl::m_fOpacity);
		glBegin(GL_QUADS);
			glVertex3f(fX1,fY1,CGuiControl::m_fZ);
			glVertex3f(fX2,fY1,CGuiControl::m_fZ);
			glVertex3f(fX2,fY2,CGuiControl::m_fZ);
			glVertex3f(fX1,fY2,CGuiControl::m_fZ);
		glEnd();

		if(!((m_iMinimumValue==m_iMaximumValue)||(m_iCurrentValue==m_iMinimumValue)))
		{
			// there is a bar drawable. Now draw this bar with blocks.

			iMaxAmountBlocks=m_iMaximumValue-m_iMinimumValue;
			if(CGuiControl::m_bHorizontal)
			{
				fDeltaX=0.015f * CGuiControl::m_fWidth;
				fBlockWidth=((CGuiControl::m_fWidth-((float)(iMaxAmountBlocks-1) * fDeltaX))/(float)iMaxAmountBlocks);
				
				for(i=0,fX2=fX1+fBlockWidth;i<m_iCurrentValue;i++)
				{
					////////////////////////
					// Draw block horizontal
					////////////////////////
					glColor4f(m_fBarColor_R, m_fBarColor_G, m_fBarColor_B, CGuiControl::m_fOpacity);
					glBegin(GL_QUADS);
						glVertex3f(fX1,fY1,CGuiControl::m_fZ);
						glVertex3f(fX2,fY1,CGuiControl::m_fZ);
						glVertex3f(fX2,fY2,CGuiControl::m_fZ);
						glVertex3f(fX1,fY2,CGuiControl::m_fZ);
					glEnd();
					fX1+=fBlockWidth+fDeltaX;
					fX2=fX1+fBlockWidth;
				}
			}
			else
			{
				fDeltaY=0.015f * CGuiControl::m_fHeight;
				fBlockHeight=((CGuiControl::m_fHeight-((float)(iMaxAmountBlocks-1) * fDeltaY))/(float)iMaxAmountBlocks);
				for(i=0, fY2=fY1+fBlockHeight;i<m_iCurrentValue;i++)
				{
					////////////////////////
					// Draw block vertical
					////////////////////////
					glColor4f(m_fBarColor_R, m_fBarColor_G, m_fBarColor_B, CGuiControl::m_fOpacity);
					glBegin(GL_QUADS);
						glVertex3f(fX1,fY1,CGuiControl::m_fZ);
						glVertex3f(fX2,fY1,CGuiControl::m_fZ);
						glVertex3f(fX2,fY2,CGuiControl::m_fZ);
						glVertex3f(fX1,fY2,CGuiControl::m_fZ);
					glEnd();
					fY1+=fBlockHeight+fDeltaY;
					fY2=fY1+fBlockHeight;
				}
			}
		}
	glPopAttrib();
}


