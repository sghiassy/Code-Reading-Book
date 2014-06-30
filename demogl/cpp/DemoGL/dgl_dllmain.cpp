//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllmain.cpp
// PURPOSE: Implementation of the main dll structure. 
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
// v1.3: Added code to support new functionality.
// v1.2: Adjustments and fixes
// v1.1: Added to the codebase.
//////////////////////////////////////////////////////////////////////

#include "Demogl\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//
//                             GLOBAL DATA
//
// store global pointers to all the objects we like to have in this dll.
// we export functions to manipulate these, or get info from.
////////////////////////////////////////////////////////////////////////

	// Prepare queue for messagepump
	CFifo			*m_gpPrepareQueue;

	// texture store.
	CTexture		*m_garrpTextures[MAXTEXTURES];

	// the console
	CSysConsole		*m_gpConsole;

	// extensionsobject
	CExtensions		*m_gpExtensions;

	// timeline store
	CTimeLineEvent	*m_gpTimeLineHead;

	// effect store
	CEffectStore	*m_gpESHead;				// Effect Storage head
	CEffectStore	*m_gpESTail;				// Effect Storage Tail

	// Soundsystem
	CSoundSystem	*m_gpSoundSystem;

	// layer store
	CLayer			*m_gpLayerHead;			    // the head of the layers we need to render.

	// DLL instance handle
	HINSTANCE		m_ghDLLInstance;

	// DemoGL Data object
	CDemoDat		*m_gpDemoDat;

	// Loading progress bar
	CProgressBar	*m_gpLoadingPB;


////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

// Purpose: the main function. This is called every time the DLL is loaded and attached to a proces.
BOOL WINAPI 
DllMain(HINSTANCE hInstance, DWORD fdwreason,  LPVOID lpReserved)
{
	int			i;
	
	m_ghDLLInstance = hInstance;
	switch(fdwreason) 
	{
		case DLL_PROCESS_ATTACH:
		{
			// Open lowlevel debugger logfile. Will not be opened if dll is not compiled with
			// lowlevel debugger flag _DGLDEBUG.
			// UNCOMMENT THE LINE BELOW WHEN YOU'RE USING THE LOWLEVEL DEBUGGER IN THE LIBRARY 
			// _dgldbg_OpenLogFile();

			// The DLL is being mapped into process's address space
			// Do any required initialization on a per application basis,
			// return FALSE if failed

			// Initialize all the pointers.
			m_gpConsole = new CSysConsole();
			m_gpExtensions = new CExtensions();
			m_gpDemoDat = new CDemoDat();
			m_gpPrepareQueue = new CFifo();
			m_gpSoundSystem = new CSoundSystem();
			m_gpLoadingPB = new CProgressBar();

			// 5 current parts in the system which have to be reflected in the loading progress bar
			// Increase this value if necessary and don't forget to add calls to IncLoadingPBValue() to
			// move the progressbar further
			m_gpLoadingPB->SetMinMaxValues(0,5);

			m_gpTimeLineHead=NULL;
			m_gpESHead=NULL;
			m_gpESTail=NULL;

			for(i=0;i<MAXTEXTURES;i++)
			{
				m_garrpTextures[i]=NULL;
			}

			// Initialize the header in the console
			FillConsoleHeader();
			////////////////////////////
			// more init here...
			////////////////////////////
		}; break;
		case DLL_THREAD_ATTACH:
		{
			// A thread is created. Do any required initialization on a per
			// thread basis
		}; break;
	    case DLL_THREAD_DETACH:
		{
		    // Thread exits with  cleanup
		}; break ;
		case DLL_PROCESS_DETACH:
		{
			// The DLL unmapped from process's address space. Do necessary 
			// cleanup
			delete m_gpDemoDat;
			delete m_gpExtensions;
			delete m_gpConsole;
			delete m_gpPrepareQueue;
			delete m_gpSoundSystem;
			delete m_gpLoadingPB;

			// Close the lowlevel debugger logfile.
			// UNCOMMENT THE LINE BELOW WHEN YOU'RE USING THE LOWLEVEL DEBUGGER IN THE LIBRARY 
			// _dgldbg_CloseLogFile();
		}; break;
    }
    return TRUE;
}


