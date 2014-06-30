//////////////////////////////////////////////////////////////////////
// FILE: dgl_dlleffectstore.cpp
// PURPOSE: in here is the CEffectstore class implementation
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

#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

// Purpose: constructor
CEffectStore::CEffectStore()
{
	m_sEffectName.Format("%s","");
	m_pEffectObject=NULL;
	m_bMessageHandlerEnabled =false;
	m_pNext=NULL;
}


// Purpose: destructor
// Additional Info: do NOT DELETE the effectobject. we just store pointers here. delete is done in mainapp.
CEffectStore::~CEffectStore()
{
	m_pEffectObject=NULL;
	m_pNext=NULL;
}


// Purpose: gets the effectobject stored in this slot
CEffect
*CEffectStore::GetEffectObject()
{
	return m_pEffectObject;
}


// Purpose: gets the next EffectSTore object in this single linked list.
CEffectStore
*CEffectStore::GetNext()
{
	return m_pNext;
}


// Purpose: gets the effectobject name, which is used by the scriptinterpreter.
char
*CEffectStore::GetEffectName()
{
	return &m_sEffectName[0];
}


// Purpose: stores the pointer to the effectobject into this slot.
void
CEffectStore::StoreEffectObject(CEffect *const pEffectObject, const char *pszEffectName)
{
	m_sEffectName.Format("%s",pszEffectName);
	m_pEffectObject = pEffectObject;
}


// Purpose: sets the next EffectSTore object that followes this object in the single linked list
void
CEffectStore::SetNext(CEffectStore *pNext)
{
	m_pNext = pNext;
}


// Purpose: checks if the effectobject in this store has a messagehandler registered. if so: true is returned,
// otherwise false
bool
CEffectStore::HasMessageHandler()
{
	return m_bMessageHandlerEnabled;
}


// Purpose: sets the status (Enabled/Disabled) of the messagehandler flag. if bYN is true, then this effectobject has it's
// messagehandler enabled.
void
CEffectStore::SetMessageHandlerEnabled(bool bYN)
{
	m_bMessageHandlerEnabled=bYN;
}