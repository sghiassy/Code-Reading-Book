//////////////////////////////////////////////////////////////////////
// FILE: dgl_dlltimelineevent.h
// PURPOSE: include file of implementation of the CTimelineEvent class.
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
// v1.3: No changes.
// v1.2: No changes.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#ifndef _DGL_DLLTIMELINEEVENT_H
#define _DGL_DLLTIMELINEEVENT_H

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////
#define DGL_TLE_MAXARGS		64

////////////////////////////////////////////////////////////////////////
//                          
//						CLASS DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
// Purpose: multitype parameter class for parameterstorage for a timeline event.
// It can hold more than 1 value at once, representing the same thing in different types.
// useful for string ID's which are later converted into binary ID's.
class CTLEParameter
{
	public:
						CTLEParameter(void);
		virtual			~CTLEParameter(void);
		float			GetFloatValue();
		int				GetIntValue();
		char			*GetStringValue();
		void			StoreValue(float fValue);
		void			StoreValue(char *pszValue);
		void			StoreValue(int iValue);

	private:
		CStdString		m_sStringValue;
		int				m_iIntValue;
		float			m_fFloatValue;
};


class CTimeLineEvent
{
	// PUBLIC MEMBERS NOT ALLOWED

	// public method declarations
	public:
						CTimeLineEvent(void);
		virtual			~CTimeLineEvent(void);
		CTimeLineEvent	*GetNext(void);
		CTimeLineEvent	*GetPrev(void);
		void			SetNext(CTimeLineEvent *pNext);
		void			SetPrev(CTimeLineEvent *pPrev);
		long			GetTimeSpot(void);
		int				GetCommand(void);
		char			*GetCommandParamString(int iOrder);
		int				GetCommandParamInt(int iOrder);
		float			GetCommandParamFloat(int iOrder);
		void			SetObject(char *pszObject);
		void			SetTimeSpot(long lTimeSpot);
		void			SetCommand(int iCommand);
		void			SetCommandParam(char *pszParam, int iOrder);
		void			SetCommandParam(int iParam, int iOrder);
		void			SetCommandParam(float fParam, int iOrder);
		int				GetAmountParams(void);
		char			*GetObject(void);

	// private method declarations
	private:
	
	// private member declarations
	private:
		CTimeLineEvent	*m_pNext;
		CTimeLineEvent	*m_pPrev;
		long			m_lTimeSpot;
		int				m_iCommand;
		CStdString		m_sObject;
		CTLEParameter	m_arrCommandParam[DGL_TLE_MAXARGS];
		int				m_iAmountParams;
};


#endif	// _DGL_DLLTIMELINEEVENT_H
