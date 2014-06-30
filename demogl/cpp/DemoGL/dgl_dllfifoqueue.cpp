// File: dgl_dllfifoqueue.cpp. 
// Purpose: implements first in first out queue for prepare thread -> main system communication
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
// v1.2: Added to codebase
// v1.1: -
//////////////////////////////////////////////////////////////////////


#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////
// Purpose: constructor
CFifo::CFifo()
{
	int			i;

	for(i=0;i<FIFOQUEUESIZE;i++)
	{
		m_arrlIndices[i]=-1;		// mark slot it has no successor
		m_arrQueue[i]=NULL;
	}
	m_lHeadIndx=0;
	m_lTailIndx=0;
	m_lRealQueueLength=0;
}


// Purpose: destructor
CFifo::~CFifo()
{
}


// Purpose: adds a pointer to the FIFO queue
void
CFifo::Add(CEffect *pToAdd)
{
	long	lIndx;

	if(m_lRealQueueLength==FIFOQUEUESIZE)
	{
		// FULL!
		return;
	}
	// add pointer to array.
	// find first empty slot. There HAS TO be an empty slot or the code is buggy
	for(lIndx=0;m_arrQueue[lIndx];lIndx++);
	
	// lIdx is index of emtpy slot.
	if(IsEmpty())
	{
		// QUeue is empty. special case.
		m_lHeadIndx=lIndx;
		m_lTailIndx=lIndx;
	}
	else
	{
		m_arrlIndices[m_lTailIndx]=lIndx;
		m_lTailIndx = lIndx;
	}
	m_arrQueue[lIndx]=pToAdd;
	m_lRealQueueLength++;
}


// Purpose: gets the head of the queue by returing the CEffect pointer of the head of the queue. NULL if queue is emtpy.
CEffect
*CFifo::GetHead()
{
	if(IsEmpty())
	{
		return NULL;
	}
	else
	{
		return m_arrQueue[m_lHeadIndx];
	}
}


// Purpose: checks if queue is empty. returns true if so, otherwise false.
bool
CFifo::IsEmpty()
{
	return (m_lRealQueueLength==0);
}


// Purpose: Deletes the head of the queue.
void
CFifo::DelFirst()
{
	long	lNext;

	if(!IsEmpty())
	{
		m_arrQueue[m_lHeadIndx]=NULL;
		lNext=m_arrlIndices[m_lHeadIndx];
		m_arrlIndices[m_lHeadIndx]=-1;
		if(lNext<0)
		{
			m_lHeadIndx=m_lTailIndx=0;
		}
		else
		{
			m_lHeadIndx=lNext;
		}
		m_lRealQueueLength--;
	}
}