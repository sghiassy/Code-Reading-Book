//////////////////////////////////////////////////////////////////////
// FILE: dgl_dlllayerutilfuncs.cpp
// PURPOSE: in here are all the layer utilityfunctions the system has. 
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// this is the file with the layer utilityfunctions. Everything layer related is
// stored here.
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
// v1.3: Added a lot new functions and fixed bugs.
// v1.2: Added a lot new functions and fixed bugs.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

// Purpose: creates a new layer object and stores the passed effectobject in it (a pointer) so that effect is running
//			on the new layer. The layer gets the number lLayerNumber.
// Additional Info: if a layer already exists with the numner lLayerNumber, there will be NO new layer created, but infact
// the layer with the number lLayerNumber will be holding pEffectObject as the effect from now on.
void
CreateNewLayer(CEffect *pEffectObject, char *sEffectName, long lLayerNumber)
{
	CLayer		*pCurr, *pNew;
	long		lpCurrLayerNumber;

	pCurr = m_gpLayerHead;
	if(!pCurr)
	{
		// first entry
		pNew = new CLayer();
		pNew->SetLayerHidden(false);
		pNew->StoreEffectObject(pEffectObject,sEffectName,lLayerNumber);
		m_gpLayerHead = pNew;
		return;
	}

	// Find the layer that has a larger or equal number than lLayerNumber.
	// find the first event that has a timespot that is > than the timespot of the
	// to be inserted event. 
	for(;(pCurr->GetLayerNumber() < lLayerNumber)&&(pCurr->GetNext());)
	{
		if(pCurr->GetNext())
		{
			pCurr=pCurr->GetNext();
		}
	}

	// OR pcurr's layernumber is >= than lLayerNumber OR we're at the last one.
	// check if it's equal
	lpCurrLayerNumber=pCurr->GetLayerNumber();
	if(lpCurrLayerNumber==lLayerNumber)
	{
		// yup, use current object.
		pCurr->SetLayerHidden(false);
		pCurr->StoreEffectObject(pEffectObject,sEffectName,lLayerNumber);
		return;
	}

	// now, we HAVE to insert a NEW layer. pCurr is pointing to a layer that has a number OR < OR >. 
	// first create the layerobject
	pNew = new CLayer();

	pNew->SetLayerHidden(false);
	pNew->StoreEffectObject(pEffectObject,sEffectName,lLayerNumber);

	// now check where to insert it.
	if(lpCurrLayerNumber < lLayerNumber)
	{
		// insert AFTER pCurr
		pCurr->SetNext(pNew);
		pNew->SetPrev(pCurr);
	}
	else
	{
		// insert BEFORE pCurr. Check if pCurr is first one.
		if(!pCurr->GetPrev())
		{
			// first one.
			m_gpLayerHead=pNew;
			pNew->SetNext(pCurr);
			pCurr->SetPrev(pNew);
		}
		else
		{
			pCurr->GetPrev()->SetNext(pNew);
			pNew->SetNext(pCurr);
			pNew->SetPrev(pCurr->GetPrev());
			pCurr->SetPrev(pNew);
		}
	}
}


// Purpose: removes all the layerobjects that run the effect with the name 'sEffectname'
void
RemoveLayers(char *sEffectName)
{
	CLayer		*pCurr, *pDel;

	pCurr = m_gpLayerHead;
	while(pCurr)
	{
		if(!strcmp(sEffectName,pCurr->GetEffectName()))
		{
			// found one, remove it.
			if(!pCurr->GetPrev())
			{
			// pCurr is first one.
				if(pCurr->GetNext())
				{
					// there are more, set the next object's prev pointer to NULL
					pCurr->GetNext()->SetPrev(NULL);
					m_gpLayerHead = pCurr->GetNext();
				}
				else
				{
					m_gpLayerHead=NULL;
				}
			}
			else
			{
				pCurr->GetPrev()->SetNext(pCurr->GetNext());
				if(pCurr->GetNext())
				{
					pCurr->GetNext()->SetPrev(pCurr->GetPrev());
				}
			}
			// delete it.
			pDel = pCurr;
			pCurr=pCurr->GetNext();
			delete pDel;
			continue;
		}
		pCurr=pCurr->GetNext();
	}
}


// Purpose: displayes the currently created layers and the name of the effect running on it and if it's visible, in the console.
void
DisplayLayersInConsole()
{
	CLayer			*pCurrLayer;

	DEMOGL_ConsoleLogLine(true, " ");
	DEMOGL_ConsoleLogLine(true, "Layerno. | Effect name                              | Visible");
	DEMOGL_ConsoleLogLine(true, "---------+------------------------------------------+--------");

	// display the layers and effectnames.
	if(!m_gpLayerHead)
	{
		// no layers available
		DEMOGL_ConsoleLogLine(true, " No layers created at the moment");
	}
	else
	{
		// there are layers available. run the displaying from back to front, thus starting at the layer with the lowest number
		for(pCurrLayer=m_gpLayerHead;pCurrLayer;pCurrLayer=pCurrLayer->GetNext())
		{
			// formulate the string to log to the console.
			if(!pCurrLayer->GetLayerHidden())
			{
				DEMOGL_ConsoleLogLineV(true, "  %6d | %-40.40s |   yes  ",pCurrLayer->GetLayerNumber(),pCurrLayer->GetEffectName());
			}
			else
			{
				DEMOGL_ConsoleLogLineV(true, "  %6d | %-40.40s |   no   ",pCurrLayer->GetLayerNumber(),pCurrLayer->GetEffectName());
			}
		}
	}

	DEMOGL_ConsoleLogLine(true, "---------+------------------------------------------+--------");
}


// Purpose: sets the hideflag of the layer with number lLayerNumber
void
HideLayer(long lLayerNumber)
{
	CLayer	*pCurr;

	pCurr = m_gpLayerHead;
	while(pCurr)
	{
		if(pCurr->GetLayerNumber()==lLayerNumber)
		{	
			// found the layer.
			pCurr->SetLayerHidden(true);
			return;
		}
		pCurr=pCurr->GetNext();
	}
}


// Purpose: clears the hideflag of the layer with number lLayerNumber
void
ShowLayer(long lLayerNumber)
{
	CLayer	*pCurr;

	pCurr = m_gpLayerHead;
	while(pCurr)
	{
		if(pCurr->GetLayerNumber()==lLayerNumber)
		{	
			// found the layer.
			pCurr->SetLayerHidden(false);
			return;
		}
		pCurr=pCurr->GetNext();
	}
}


// Purpose: moves an object from one layer to the other. If this layer is not existing, it is created
//	        This is done using routines already available.
void
MoveEffectToLayer(char *sEffectName, long lDestLayerNumber)
{
	CEffect		*pEffect;
	CLayer		*pCurr;
	long		lCurrLayerNumber;

	pEffect = GetEffectObject(sEffectName);
	if(!pEffect)
	{
		// not found.
		return;
	}

	// find layer
	pCurr = m_gpLayerHead;
	lCurrLayerNumber = -1;

	while(pCurr)
	{
		if(!strcmp(pCurr->GetEffectName(),sEffectName))
		{	
			// found the layer.
			lCurrLayerNumber = pCurr->GetLayerNumber();
			break;
		}
		pCurr=pCurr->GetNext();
	}

	CreateNewLayer(pEffect,sEffectName,lDestLayerNumber);
	if(lCurrLayerNumber!=-1)
	{
		// it's on a layer now
		if(lDestLayerNumber!=lCurrLayerNumber)
		{
			// delete the current layer
			if(!pCurr->GetPrev())
			{
				// first one.
				m_gpLayerHead=pCurr->GetNext();
				pCurr->GetNext()->SetPrev(NULL);
			}
			else	
			{
				pCurr->GetPrev()->SetNext(pCurr->GetNext());
				if(pCurr->GetNext())
				{
					pCurr->GetNext()->SetPrev(pCurr->GetPrev());
				}
			}
			delete pCurr;
		}
	}
}


