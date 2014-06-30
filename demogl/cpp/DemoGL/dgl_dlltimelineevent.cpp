//////////////////////////////////////////////////////////////////////
// FILE: dgl_dlltimelineevent.cpp
// PURPOSE: in here is the CTimelineevent class implementation
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// this is the file with the main control functions for running the demo.
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
// v1.3: Added new features.
// v1.2: No Changes.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//////////////////////// CTLEParameter Class ///////////////////////////
////////////////////////////////////////////////////////////////////////

// Purpose: constructor
CTLEParameter::CTLEParameter()
{
	m_iIntValue=0;
	m_fFloatValue=0.0f;
	m_sStringValue.Format("%s","");
}


// Purpose: destructor
CTLEParameter::~CTLEParameter()
{

}


// Purpose: gets the float version of the parameter
float
CTLEParameter::GetFloatValue()
{
	return m_fFloatValue;
}


// Purpose: gets the int version of the parameter
int
CTLEParameter::GetIntValue()
{
	return m_iIntValue;
}


// Purpose: gets the string version of the parameter
char
*CTLEParameter::GetStringValue()
{
	return &m_sStringValue[0];
}


// Purpose: sets the floatversion of the parameter
void
CTLEParameter::StoreValue(float fValue)
{
	m_fFloatValue=fValue;
}


// Purpose: sets the intversion of the parameter
void
CTLEParameter::StoreValue(int iValue)
{
	m_iIntValue=iValue;
}


// Purpose: sets the string version of the parameter
void
CTLEParameter::StoreValue(char *pszValue)
{
	m_sStringValue.Format("%s",pszValue);
}


////////////////////////////////////////////////////////////////////////
//////////////////////// CTimeLineEvent Class //////////////////////////
////////////////////////////////////////////////////////////////////////

// Purpose: constructor
CTimeLineEvent::CTimeLineEvent()
{
	m_pNext=NULL;
	m_pPrev=NULL;
	m_lTimeSpot=0;
	m_iCommand=CT_NOP;
	m_iAmountParams=-1;

	// m_arrCommandParam array is initialized by the constructors.

	m_sObject="";
}


// Purpose: destructor
CTimeLineEvent::~CTimeLineEvent()
{
	// nothing.
}


// Purpose: returns a pointer to the next object in the line.
CTimeLineEvent
*CTimeLineEvent::GetNext()
{
	return m_pNext;
}


// Purpose: returns a pointer to the prev object in the line.
CTimeLineEvent
*CTimeLineEvent::GetPrev()
{
	return m_pPrev;
}


// Purpose: Sets the next poiner
void
CTimeLineEvent::SetNext(CTimeLineEvent *pNext)
{
	m_pNext=pNext;
}


// Purpose: Sets the prev poiner
void
CTimeLineEvent::SetPrev(CTimeLineEvent *pPrev)
{
	m_pPrev=pPrev;
}


// Purpose: sets timespot
void
CTimeLineEvent::SetTimeSpot(long lTimeSpot)
{
	m_lTimeSpot = lTimeSpot;
}


// Purpose: get Timespot
long
CTimeLineEvent::GetTimeSpot()
{
	return m_lTimeSpot;
}


// Purpose: get command
int
CTimeLineEvent::GetCommand()
{
	return m_iCommand;
}


// Purpose: set command
void
CTimeLineEvent::SetCommand(int iCommand)
{
	m_iCommand = iCommand;
}


// Purpose: set iOrder-th commandparam's string value. Will ignore sParam if iOrder > DGL_TLE_MAXARGS
void
CTimeLineEvent::SetCommandParam(char *pszParam, int iOrder)
{
	if((iOrder<DGL_TLE_MAXARGS)&&(iOrder>=0))
	{
		m_arrCommandParam[iOrder].StoreValue(pszParam);
	}
	if(iOrder>m_iAmountParams)
	{
		m_iAmountParams=iOrder;
	}
}


// Purpose: set iOrder-th commandparam's int value. Will ignore iParam if iOrder > DGL_TLE_MAXARGS
void
CTimeLineEvent::SetCommandParam(int iParam, int iOrder)
{
	if((iOrder<DGL_TLE_MAXARGS)&&(iOrder>=0))
	{
		m_arrCommandParam[iOrder].StoreValue(iParam);
	}
	if(iOrder>m_iAmountParams)
	{
		m_iAmountParams=iOrder;
	}
}


// Purpose: set iOrder-th commandparam's float value. Will ignore fParam if iOrder > DGL_TLE_MAXARGS
void
CTimeLineEvent::SetCommandParam(float fParam, int iOrder)
{
	if((iOrder<DGL_TLE_MAXARGS)&&(iOrder>=0))
	{
		m_arrCommandParam[iOrder].StoreValue(fParam);
	}
	if(iOrder>m_iAmountParams)
	{
		m_iAmountParams=iOrder;
	}
}


// Purpose: gets the amount of parameters stored.
int
CTimeLineEvent::GetAmountParams()
{
	if(m_iAmountParams>=0)
	{
		return m_iAmountParams+1;
	}
	return 0;
}


// Purpose: get iOrder-th commandparam's stringvalue. Ignored if iOrder<0 or > DGL_TLE_MAXARGS
char 
*CTimeLineEvent::GetCommandParamString(int iOrder)
{
	if((iOrder<DGL_TLE_MAXARGS)&&(iOrder>=0))
	{
		return m_arrCommandParam[iOrder].GetStringValue();
	}
	else
	{
		return NULL;
	}
}


// Purpose: get the iOrder-th commandparam's intvalue. Ignored if iOrder<0 or > DGL_TLE_MAXARGS
int
CTimeLineEvent::GetCommandParamInt(int iOrder)
{
	if((iOrder<DGL_TLE_MAXARGS)&&(iOrder>=0))
	{
		return m_arrCommandParam[iOrder].GetIntValue();
	}
	else
	{
		return 0;
	}
} 


// Purpose: get the iOrder-th commandparam's intvalue. Ignored if iOrder<0 or > DGL_TLE_MAXARGS
float
CTimeLineEvent::GetCommandParamFloat(int iOrder)
{
	if((iOrder<DGL_TLE_MAXARGS)&&(iOrder>=0))
	{
		return m_arrCommandParam[iOrder].GetFloatValue();
	}
	else
	{
		return 0.0f;
	}
} 


// Purpose: set Object
void
CTimeLineEvent::SetObject(char *pszObject)
{
	m_sObject = pszObject;
}


// Purpose: get object
char
*CTimeLineEvent::GetObject()
{
	return &m_sObject[0];
}
