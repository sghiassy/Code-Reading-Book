//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllendsystem.cpp
// PURPOSE: in here are all the end functions that are needed to get the system end itself correctly
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
// v1.3: Adapted new subsystems.
// v1.2: Fixed bugs.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////
#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//        DEPRECATED FUNCTIONS. HERE FOR COMPATIBILITY REASONS. 
//            DON'T USE THESE. USE DEMOGL_* VARIANTS INSTEAD
///////////////////////////////////////////////////////////////////////
// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_AppEnd() instead.
void
EndApp()
{
	DEMOGL_AppEnd();
}

///////////////////////////////////////////////////////////////////////
//                     END DEPRECATED FUNCTIONS.                     //
///////////////////////////////////////////////////////////////////////


// Purpose: this routine should be called when the application has returned from DEMOGL_AppRun*().
// NOTE: the window is destroyed by the kernelloop exit code. 
void
DEMOGL_AppEnd()
{
	int					i;
	CTimeLineEvent		*pCurr;
	CEffect				*pEffect;

	// test if there is something to end after all.
	if(m_gpDemoDat->GetSystemState()<=SSTATE_BOOT_MAINWNDCREATION)
	{
		// nothing to end. return
		return;
	}

	////////////////////////////
	// Start system clean up!
	////////////////////////////
	// set new system state.
	m_gpDemoDat->SetSystemState(SSTATE_AEND_SYSTEMCLEANUP);

	if(m_gpDemoDat->GetbSound())
	{
		m_gpSoundSystem->StopAll();
	}
	m_gpSoundSystem->FreeSystem();

	// delete all timelineevents
	while(m_gpTimeLineHead)
	{
		pCurr=m_gpTimeLineHead->GetNext();
		DeleteTimeLineEvent(m_gpTimeLineHead);
		m_gpTimeLineHead = pCurr;
	}

	// delete all effectstore objects.
	while(m_gpESHead)
	{
		m_gpESTail = m_gpESHead->GetNext();
		pEffect = m_gpESHead->GetEffectObject();
	 	// Tell Object to end.
		pEffect->End();
		RemoveLayers(m_gpESHead->GetEffectName());
		delete m_gpESHead;
		m_gpESHead = m_gpESTail;
	}

	// Tell Console it's over and to stop.
	m_gpConsole->End();

	// free all textureobjects that are left behind....
	for(i=0;i<MAXTEXTURES;i++)
	{
		if(m_garrpTextures[i]!=NULL)
		{
			DoDeleteTexture(i);
		}
	}

	if(m_gpDemoDat->GetbFullScreen())
	{
		// switchback to normal screen...
		ChangeDisplaySettings(NULL, 0);
		MessageBox(NULL,"Normal resolution reset.","Debug",MB_OK|MB_SYSTEMMODAL);

		// show cursor
		ShowCursor(TRUE);
	}

	// end the system. the callerapp should end now itself.
}

