//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllkernel.cpp
// PURPOSE: in here are all the main kernel/controlfunctions needed to run the system.
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// this is the file with the main kernel/control functions for running the demo.
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
// v1.3: For 80% recoded.
// v1.2: Added major changes/fixes
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//						GLOBAL VARS FOR USAGE IN THIS FILE.
///////////////////////////////////////////////////////////////////////
// variables to hold the previous mousecoords when moving the mouse.
// Important for the screensaver. needed to determine if the threshold is
// reached so the screensaver should exit or not. Used in MainMsgHandler()
int						m_giMouseXVal = -1;
int						m_giMouseYVal = -1;

// Thread handles for init/prepare threads, started in MainMsghandler()
// Handles and other kernel internals are cleaned up in CleanUpKernelInternals()
static	unsigned long	m_glInitSystemThreadID = NULL;
static	unsigned long	m_glInitAppThreadID = NULL;
static	unsigned long	m_glPrepareSystemThreadID = NULL;
static	HANDLE			m_ghInitSystemThread=NULL;
static	HANDLE			m_ghInitAppThread=NULL;
static	HANDLE			m_ghPrepareSystemThread=NULL;

///////////////////////////////////////////////////////////////////////
//					           END GLOBAL VARS
///////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//                      Public routines exported by the library
////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//        DEPRECATED FUNCTIONS. HERE FOR COMPATIBILITY REASONS. 
//            DON'T USE THESE. USE DEMOGL_* VARIANTS INSTEAD
///////////////////////////////////////////////////////////////////////
// Purpose: deprecated function. Here for backwards compatibility. Use DEMOGL_RegisterEffect() instead.
void
RegisterEffect(CEffect *pEffectObject, const char *sEffectName)
{
	DEMOGL_EffectRegister(pEffectObject,sEffectName);
}

///////////////////////////////////////////////////////////////////////
//                     END DEPRECATED FUNCTIONS.                     //
///////////////////////////////////////////////////////////////////////

// Purpose: registers an effectobject with DemoGL. It will be placed in a linked list of EffectObjects.
void
DEMOGL_EffectRegister(CEffect * const pEffectObject, const char *pszEffectName)
{
	CEffectStore	*pEffectStoreObj;

	pEffectStoreObj = new CEffectStore();
	pEffectStoreObj->StoreEffectObject(pEffectObject,pszEffectName);

	if(!m_gpESHead)
	{
		// first object
		m_gpESHead = pEffectStoreObj;
		m_gpESTail = m_gpESHead;
	}
	else
	{
		// not first object.
		m_gpESTail->SetNext(pEffectStoreObj);
		m_gpESTail = pEffectStoreObj;
	}

	// Increase maximum value in loading progressbar with 1
	IncLoadingPBMaxValue(1);
}


////////////////////////////////////////////////////////////////////////
//                         Library ONLY routines
////////////////////////////////////////////////////////////////////////


// Purpose: the messagepump/kernelloop of the system. Won't return until WM_QUIT is posted 
// to the mainwindow. 
void
KernelLoop()
{
	bool	bQuitMainLoop;
	MSG		mMsg;

	// check if there are messages in the queue. If so: process them.
	for(bQuitMainLoop=false;!bQuitMainLoop;)
	{
		while(PeekMessage(&mMsg, NULL, 0, 0, PM_NOREMOVE)) 
		{
			if(GetMessage(&mMsg, NULL, 0, 0)) 
			{
				TranslateMessage(&mMsg);
				DispatchMessage(&mMsg);
			}
			else
			{
				bQuitMainLoop=true;
				break;
			}
		}
		// we're displaying content. That needs to be as smooth as possible so no messagewaitin'
		if(m_gpDemoDat->GetSystemState()==SSTATE_ARUN_KERNELLOOP)
		{
			ExecuteTimeLine((long)(m_gpDemoDat->GetfElapsedTimeInSecs() * 1000));
			m_gpDemoDat->CalcElapsedTimeInSeconds();
			RenderFrame();
		}
		else
		{
			WaitMessage();	
		}
	}
	//////////////////
	// a WM_QUIT message has been received.
	//////////////////

	// set new system state.
	m_gpDemoDat->SetSystemState(SSTATE_ARUN_POSTKERNELLOOP);

	// Control should be given back to the caller's main routine, but we first have to
	// do some cleanup of handles and windows. 
	CleanUpKernelInternals();
}


// Purpose: cleans up kernel internals when the kernel loop exits.
void
CleanUpKernelInternals()
{
	HDC				hDC;
	HGLRC			hRC;

	// free the target DeviceContext
	wglMakeCurrent(NULL,NULL);
	hRC = m_gpDemoDat->GethRC();
	if(hRC)
	{
		wglDeleteContext(hRC); 
	}
	hDC = m_gpDemoDat->GethDC();
	if (hDC) 
	{
		ReleaseDC(m_gpDemoDat->GethWnd(), hDC); 
	}

	DestroyWindow(m_gpDemoDat->GethWnd());

	CloseHandle(m_ghInitSystemThread);
	CloseHandle(m_ghInitAppThread);
	CloseHandle(m_ghPrepareSystemThread);
	// done.
}


// Purpose: the draw function. This routine is used to render 1 frame. the current elapsed amount of seconds is passed.
//          so parts can time on this.
// NOTE: a user should create an object that runs on the layer with the lowest number AND that clears the buffers. 
void
RenderFrame()
{
	float		fFPS;
	bool		bDisplayOverlay, bDisplayFPS, bDisplayDebugInfo;

	// call console renderroutine if console is up.
	if(m_gpDemoDat->GetbConsoleVisible())
	{
		// Console is visible, so draw the console and nothing else.
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		m_gpConsole->RenderConsole(m_gpDemoDat->GetbInputPromptVisible(),m_gpDemoDat->GetInputBuffer(),
					(m_gpDemoDat->GetSystemState()!=SSTATE_ARUN_KERNELLOOP),
					(m_gpDemoDat->GetSystemState()!=SSTATE_ARUN_KERNELLOOP));
		// swap the buffers. 
		SwapBuffers(m_gpDemoDat->GethDC());
	}
	else
	{
		// calculate framespersecond
		fFPS=m_gpDemoDat->CalcFPS();

		// pass to console.
		m_gpConsole->SetFPS(fFPS);

		// if the console is switched off in the last frame, we have to clear the total colorbuffer first, this is
		// due to the fact that the console is screen filling and the effects can use glScissors() so we'll keep
		// parts of the console on screen, which we don't want. if it's switched off, we'll call this routine once.
		// the flag that signals this is cleared in DemoDat. This flag is set by ToggleConsole.
		if(m_gpDemoDat->GetbAnOverlayWasSwitchedOff())
		{
			glPushAttrib(GL_ENABLE_BIT);
				glDisable(GL_SCISSOR_TEST);
				glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			glPopAttrib();
		}

		// the console is NOT visible. Check if we're in the kernelloop. If so, we're allowed to
		// walk layers. if NOT, we're not allowed to walk layers and we don't do anything.
		if(m_gpDemoDat->GetSystemState()==SSTATE_ARUN_KERNELLOOP)
		{
			// for each registered effectobject currently playing, call render routine.
			// this is: walk all layerobjects and if visible, call renderframe routines.
			RenderLayerFrames();

			bDisplayFPS = m_gpDemoDat->GetbFPSVisible();
			bDisplayDebugInfo = m_gpDemoDat->GetbDebugInfoVisible();
			bDisplayOverlay = bDisplayFPS || bDisplayDebugInfo;
			if(bDisplayOverlay)
			{
				// display overlay
				m_gpConsole->RenderOverlay(bDisplayFPS,bDisplayDebugInfo,
							m_gpDemoDat->GetbInputPromptVisible(),(long)(m_gpDemoDat->GetfElapsedTimeInSecs() * 1000), 
							m_gpDemoDat->GetInputBuffer());
			}
		}
		if(m_gpDemoDat->GetbSwapBuffers())
		{
			// swap the buffers. 
			SwapBuffers(m_gpDemoDat->GethDC());
		}
	}
}


// Purpose: walk all layers and call RenderFrame routines of the effects enclosed IF layer is visible
void
RenderLayerFrames()
{
	long		lElapsedTime;
	CLayer		*pCurr;
	bool		bUseRenderFrameEx;

	lElapsedTime=(long)(m_gpDemoDat->GetfElapsedTimeInSecs() * 1000.0f);

	if(!m_gpLayerHead)
	{	
		// no layers to walk
		return;
	}

	// Get the flag which will signal which renderroutine to call.
	bUseRenderFrameEx=m_gpDemoDat->GetbUseRenderFrameEx();
	for(pCurr=m_gpLayerHead;pCurr;pCurr=pCurr->GetNext())
	{
		// check if this layer is hidden
		if(!pCurr->GetLayerHidden())
		{
			// it's visible. call renderroutine
			if(bUseRenderFrameEx)
			{
				pCurr->GetEffectObject()->RenderFrameEx(lElapsedTime, pCurr->GetLayerNumber());
			}
			else
			{
				pCurr->GetEffectObject()->RenderFrame(lElapsedTime);
			}
		}
	}
}


// Purpose: mainmessagehandler. All messages received by the application are processed here. Most of
//          the messages will be handled by DemoGL further.
long WINAPI 
MainMsgHandler(HWND hWnd, UINT uMsg, WPARAM  wParam, LPARAM  lParam) 
{ 
	long							lReturnValue;
	RECT							rectCanvas;
	CStdString						sError;
	int								iResult, iMaxTextureSize;
	CEffect							*pEffect;
	HANDLE							hOwnThread;
	BOOL							bResult;

	lReturnValue = 1;

    switch (uMsg) 
	{ 
		// Posted by SetupMainWindow(), when the setup of that window was succesful.
		case WM_DEMOGL_INITSYSTEM:
		{

			// Boost foreground thread (that is.. this thread), according to the OS we're on.
			// Windows9x has bad thread scheduling, so we need to give the renderthread (this thread) the highest
			// priority without system starvation so the messages reported by worker threads get logged (rendered
			// to the console) as fast as possible. If we don't do this, the renderthread (the mainthread, this
			// thread) will choke in the amount of messages waiting when a worker thread ends. (resulting in a
			// pause between InitSystem() and InitApp() worker threads.). NT/Windows2k doesn't have this
			// nasty crap, so we just boost it a little less on that OS.
			hOwnThread=GetCurrentThread();
			if(CheckOSType(OSTYPE_WINDOWS98)||CheckOSType(OSTYPE_WINDOWS95))
			{
				bResult = SetThreadPriority(hOwnThread,THREAD_PRIORITY_HIGHEST);
			}
			else
			{
				bResult = SetThreadPriority(hOwnThread,THREAD_PRIORITY_ABOVE_NORMAL);
			}

			// set system state
			m_gpDemoDat->SetSystemState(SSTATE_BOOT_INITSTART);

			// set up a highres timer.
			iResult = m_gpDemoDat->SetupTicker();
			if(iResult==SYS_NOK)
			{
				MessageBox(hWnd,"No high resolution timer available. Can't continue.","DemoGL Init Error",MB_ICONERROR|MB_APPLMODAL);
				PostQuitMessage(0);
				return 0;
			}

			m_gpDemoDat->SetSystemState(SSTATE_BOOT_SYSOBJECTSINIT);

			// initialize console.
			iResult = m_gpConsole->Init(m_gpDemoDat->GetiWindowWidth(), m_gpDemoDat->GetiWindowHeight(), 
							m_gpDemoDat->GetLoadingSplashTextureName(), m_gpDemoDat->GetLoadingSplashFileType(),
							m_gpDemoDat->GetLoadingSplashOpacity());
			if(iResult !=SYS_OK)
			{
				// bubble resultcode.
				PostMessage(m_gpDemoDat->GethWnd(),WM_DEMOGL_ABORT,NULL,iResult);
				return 0;
			}

			// prepare console
			m_gpConsole->Prepare();
			LogFeedback("Console prepared.",true,"MainMsgHandler:WM_DEMOGL_INITSYSTEM",false);

			// Start the soundsystem, if available / requested. Has to be done in this thread to 
			// avoid parameter storage per thread mess.
			iResult=InitSoundSystem();
			//////////////////////////////////////
			// Ignore resultcode for now. Sound is disabled when failure of init.
			//////////////////////////////////////

			// Increase Current Value in loading progressbar.
			IncLoadingPBValue(1);

			// Get ICD information and store that into the demodat object. Done here to avoid having to create
			// another rendercontext in another thread, which can be halt the system with crappy drivers.
			// We are not here if there is no valid window plus rendercontext. Store the info here so the
			// worker thread can parse the info.
			m_gpDemoDat->SetOGLVendor((char *)glGetString(GL_VENDOR));
			m_gpDemoDat->SetOGLRenderer((char *)glGetString(GL_RENDERER));
			m_gpDemoDat->SetOGLVersion((char *)glGetString(GL_VERSION));
			m_gpDemoDat->SetOGLExtensions((char *)glGetString(GL_EXTENSIONS));
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iMaxTextureSize);
			m_gpDemoDat->SetOGLMaxTextureSize(iMaxTextureSize);

			// display the info gathered from the ICD
			ListICDInfo();

			// Create System Init worker Thread.
			/////////////////////////////////////
			// UNCOMMENT NEXT LINE AND COMMENT OUT _beginthreadex LINE IF YOU WANT TO USE CreateThread instead of the CRT equivalent.
			// (which is safer than CreateThread)
			/////////////////////////////////////
			//m_ghInitSystemThread=CreateThread(NULL,0,&InitSystem,NULL,0,&m_glInitSystemThreadID);
			m_ghInitSystemThread=(void *)_beginthreadex(NULL,0,(PTHREAD_START)&InitSystem,NULL,0,(unsigned int *)&m_glInitSystemThreadID);
			if(!m_glInitSystemThreadID)
			{
				MessageBox(hWnd,"DemoGL couldn't create a new thread for the system initialisation routine. Can't continue.","DemoGL Init Error",MB_ICONERROR|MB_APPLMODAL);
				// flag the system we want to quit.
				PostQuitMessage(0); 
				return 0;
			}
		}; break;

		// Posted by RedrawTheWindow(), which is called when a workerthread causes a redraw of the window.
		case WM_DEMOGL_PAINT:
		{
			RenderFrame();
		}; break;

		// Posted when WM_DEMOGL_INITSYSTEM has finished. (Posted by InitSystem())
		case WM_DEMOGL_INITAPP:
		{
			// Create Init App Thread
			/////////////////////////////////////
			// UNCOMMENT NEXT LINE AND COMMENT OUT _beginthreadex LINE IF YOU WANT TO USE CreateThread instead of the CRT equivalent.
			// (which is safer than CreateThread)
			/////////////////////////////////////
			// m_ghInitAppThread=CreateThread(NULL,0,&InitApp,NULL,0,&m_glInitAppThreadID);
			m_ghInitAppThread=(void *)_beginthreadex(NULL,0,(PTHREAD_START)&InitApp,NULL,0,(unsigned int *)&m_glInitAppThreadID);
			if(!m_glInitAppThreadID)
			{
				MessageBox(hWnd,"DemoGL couldn't create a new thread for the application initialisation routine. Can't continue.","DemoGL Init Error",MB_ICONERROR|MB_APPLMODAL);
				// flag the system we want to quit.
				PostQuitMessage(0); 
				return 0;
			}
		}; break;

		// Posted by InitApp() routine (On success)
		case WM_DEMOGL_PREPARE:
		{
			////////////////////////
			// Apply last minute system variables. Done here because OpenGL related variables HAVE to be executed
			// here and not in a workerthread.
			////////////////////////
				// effectuate the value set 
				EffectuateVariable_VSYNC();

				//////////// ADD MORE Systemvariables that have to be applied HERE. Every restart these variables are
				//////////// applied. Be aware of that.

			////////////////////////
			// End last minute system variables
			////////////////////////

			// Set startup timer refresh value
			m_gpDemoDat->SetCurrentTimeAtTickerReset(0);

			// Set CurrentTimeLineEvent pointer (CurrentTLE) to timeline head
			m_gpDemoDat->SetCurrentTLE(m_gpTimeLineHead);

			// Create Prepare Thread
			/////////////////////////////////////
			// UNCOMMENT NEXT LINE AND COMMENT OUT _beginthreadex LINE IF YOU WANT TO USE CreateThread instead of the CRT equivalent.
			// (which is safer than CreateThread)
			/////////////////////////////////////
			// m_ghPrepareSystemThread=CreateThread(NULL,0,&Prepare,NULL,0,&m_glPrepareSystemThreadID);
			m_ghPrepareSystemThread=(void *)_beginthreadex(NULL,0,(PTHREAD_START)&Prepare,NULL,0,(unsigned int *)&m_glPrepareSystemThreadID);
			if(!m_glPrepareSystemThreadID)
			{
				MessageBox(hWnd,"DemoGL couldn't create a new thread for the application prepare routine. Can't continue.","DemoGL Init Error",MB_ICONERROR|MB_APPLMODAL);
				// flag the system we want to quit.
				PostQuitMessage(0); 
				return 0;
			}
		}; break;

		// Posted by windows when user kills application or windows kills application with Alt-f4 or nasty killutils.
		case WM_CLOSE:
		{
			// flag the system we want to quit.
			PostQuitMessage(0); 
		}; break;

		// This message is posted when there was an unrecoverable error in some workerthread so the system
		// should abort. Posted by InitApp() (on failure)
		case WM_DEMOGL_ABORT:
		{
			// check if wParam is 0. If so, an error messagebox should be displayed.
			if(!wParam)
			{
				sError.Format("An unrecoverable error has occured: %d. The application will now exit.",lParam);
				MessageBox(hWnd,&sError[0],"DemoGL Unrecoverable Error",MB_ICONERROR|MB_SYSTEMMODAL);
			}
			// flag the system we want to quit.
			PostQuitMessage(0); 
			return 0;
		}; break;

		// Posted when the initphases are all done and the system should start. Posted by Prepare() (another thread,
		// started in WM_DEMOGL_PREPARE handler)
		case WM_DEMOGL_STARTKERNELLOOP:
		{
			// switch back foreground thread
			hOwnThread=GetCurrentThread();
			SetThreadPriority(hOwnThread,THREAD_PRIORITY_NORMAL);

			LogFeedback("Resetting timerticker.",true,"MainMsgHandler:WM_DEMOGL_STARTKERNELLOOP",false);
			iResult = m_gpDemoDat->SetupTicker();
			if(iResult==SYS_NOK)
			{
				MessageBox(hWnd,"No high resolution timer available. Can't continue.","DemoGL Init Error",MB_ICONERROR|MB_APPLMODAL);
				// flag the system we want to quit.
				PostQuitMessage(0);
				return 0;
			}
			LogFeedback("Timerticker reset.",true,"MainMsgHandler:WM_DEMOGL_STARTKERNELLOOP",true);

			// execute TLE's prior to the timespot if the flag to do so is set. This flag is ONLY set by
			// a restart command. first time boots will always execute prior starttime TLE's in the prepare thread.
			// the flag that will cause an execution NOW when true is false at startup. (see dgl_dlldemodat.cpp,
			// CDemoDat constructor)
			if(m_gpDemoDat->GetbExecPriorStartTimeTLEsAtReset())
			{
				m_gpDemoDat->SetCurrentTLE(m_gpTimeLineHead);
				// an execution is requested.
				ExecuteTimeLine((long)m_gpDemoDat->GetCurrentTimeAtTickerReset()-1);
				m_gpDemoDat->SetbExecPriorStartTimeTLEsAtReset(false);
			}

			// Set the CurrentTLE pointer to the timespot with which we've refreshed the ticker with
			// (usually 0, unless the app is restarted with a command and a timespot other than 0 is
			// passed)
			MoveCurrentTLEToTimeSpot(m_gpDemoDat->GetCurrentTimeAtTickerReset());

			// execute TLE's prior to the timespot if the flag to do so is set. This flag is ONLY set by
			// a restart command. first time boots will always execute prior starttime TLE's in the prepare thread.
			// the flag that will cause an execution NOW when true is false at startup. (see dgl_dlldemodat.cpp,
			// CDemoDat constructor)
			if(m_gpDemoDat->GetbExecPriorStartTimeTLEsAtReset())
			{
				// an execution is requested.
				ExecuteTimeLine((long)m_gpDemoDat->GetCurrentTimeAtTickerReset()-1);
				m_gpDemoDat->SetbExecPriorStartTimeTLEsAtReset(false);
			}

			// start.
			m_gpDemoDat->SetbConsoleVisible(false);
			// switch on the rendercycle.
			m_gpDemoDat->SetSystemState(SSTATE_ARUN_KERNELLOOP);
			LogFeedback("Boot complete. Invoking kernelloop.",true,"MainMsgHandler:WM_DEMOGL_STARTKERNELLOOP",false);
			// wakeup the mainmsg loop
			PostMessage(m_gpDemoDat->GethWnd(),WM_DEMOGL_PAINT,NULL,NULL);
		}; break;

		// Posted by windows. Is result of a keypress of a normal characterkey
		case WM_CHAR:
		{
			// If we're running in RUNTYPE_SAVER_NORMAL we should exit.
			if(m_gpDemoDat->GetRunType()==RUNTYPE_SAVER_NORMAL)
			{
				// yup. exit.
				// flag the system we want to quit.
				PostQuitMessage(0);
				break;
			}

			switch(wParam)
			{
				case 0x08:
				{
					// backspace. if inputprompt is visible, do backspace
					if(m_gpDemoDat->GetbInputPromptVisible() && m_gpDemoDat->GetbDebugInfoVisible())
					{
						m_gpDemoDat->DelLastCharFromInputBuffer();
					}
				}; break;
				case 0x0D:
				{
					// user pressed return. if inputprompt is visible, execute 
					if(m_gpDemoDat->GetbInputPromptVisible() && m_gpDemoDat->GetbDebugInfoVisible())
					{
						if(m_gpDemoDat->GetInputBufferLength() > 0)
						{
							// there is something typed in. execute it.
							ExecuteInputBuffer();
							m_gpDemoDat->ClearInputBuffer();
						}
					}
				}; break;
				case 0x1B:
				{
					// user pressed ESC -> exit.
					// flag the system we want to quit.
					PostQuitMessage(0);
				}; break;
				case 0x60:
				case 0x7e:
				{
					// user pressed '`' or '~'
					ToggleConsole();
				}; break;

				// ADD MORE CONTROLKEYS HERE

				default:
				{
					if(wParam>=32)
					{
						// printable char. add it if inputbuffer is visible
						if(m_gpDemoDat->GetbInputPromptVisible() && m_gpDemoDat->GetbDebugInfoVisible())
						{
							m_gpDemoDat->AddCharToInputBuffer((char)wParam);
						}
					}
				}; break;
			};
		}; break;

		// Posted by by ExecuteTimeline, when that routine was executed by the Prepare worker thread.
		// The Prepare() method of an effect has to be executed by the window owning thread (i.e. the mainthread)
		// because only there is opengl available. (i.e. the rendercontext), so actions done by the effects are
		// also effective.
		case WM_DEMOGL_PREPAREEFFECT:
		{
			pEffect = m_gpPrepareQueue->GetHead();
			if(pEffect)
			{
				m_gpPrepareQueue->DelFirst();
				pEffect->Prepare();
			}
		}; break;

		// Posted by windows when the user resizes the window, or a routine resized the window
		case WM_SIZE: 
		{
			GetClientRect(hWnd, &rectCanvas); 

			// tell DemoGL to reshape the projectionmatrices, and to tell every running effectobject
			// the new width and height, so every effect can reshape the projection matrices too.
			ReshapeCallBack(rectCanvas.right, rectCanvas.bottom);
		};break; 

		// Posted by windows when a user presses alt-f4 or clicks the closebutton
	    case WM_DESTROY: 
		{
			// flag the system we want to quit.
 			PostQuitMessage (0); 
		};break; 
	
		// Posted by windows when the application's window is active and the user presses a key
		case WM_KEYDOWN: 
		{
			// If we're running in RUNTYPE_SAVER_NORMAL, we should exit.
			if(m_gpDemoDat->GetRunType()==RUNTYPE_SAVER_NORMAL)
			{
				// yup. exit.
				// flag the system we want to quit.
				PostQuitMessage(0);
				break;
			}
			switch(wParam)
			{
				case VK_F1:
				{
					// display help
					DisplayHelp();
				}; break;
				case VK_F2:
				{
					// toggle FPS display
					ToggleFPSDisplay();
				}; break;
				case VK_F3:
				{
					// display all layers and running effects and visibility
					DisplayLayersInConsole();
				}; break;
				case VK_F4:
				{
					// display all registered objects.
					DisplayRegisteredObjectsInConsole();
				}; break;
				case VK_F5:
				{
					// toggle display debuginfo in overlay
					ToggleDebugInfoDisplay();
				};break;
				case VK_F6:
				{
					// toggle Inputprompt in overlay
					ToggleInputPrompt();
				};break;
				case VK_F7:
				{
					// display complete timeline with all events
					DisplayTimeLineInConsole();
				};break;
				case VK_TAB:
				case VK_F12:
				{
					// toggle console
					ToggleConsole();
				}; break;
				case VK_UP:
				{
					// user pressed UP arrow key
					m_gpConsole->MoveConsoleViewWindow(CONSOLE_MOVE1LINEUP);
				}; break;
				case VK_DOWN:
				{
					// user pressed DOWN arrow key
					m_gpConsole->MoveConsoleViewWindow(CONSOLE_MOVE1LINEDOWN);
				}; break;
				case VK_PRIOR:
				{
					// user pressed pagedown key
					m_gpConsole->MoveConsoleViewWindow(CONSOLE_MOVE1PAGEUP);
				}; break;
				case VK_NEXT:
				{
					// user pressed pagedown key
					m_gpConsole->MoveConsoleViewWindow(CONSOLE_MOVE1PAGEDOWN);
				}; break;
			}
        };break;

		// Posted by windows
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		{
			// If we're running in RUNTYPE_SAVER_NORMAL we should
			// exit. THis should ONLY be done if we're in kernel loop.
			if((m_gpDemoDat->GetRunType()==RUNTYPE_SAVER_NORMAL)&&
						(m_gpDemoDat->GetSystemState()==SSTATE_ARUN_KERNELLOOP))
			{
				if(m_giMouseXVal==-1 )
				{
					m_giMouseXVal = LOWORD( lParam );
					m_giMouseYVal = HIWORD( lParam );
				}
				if((abs(m_giMouseXVal-LOWORD(lParam)) > SSAVER_MOUSE_TRESHOLD ) ||
					( abs(m_giMouseYVal-HIWORD(lParam)) > SSAVER_MOUSE_TRESHOLD ))
				{
					// flag the system we want to quit.
					PostQuitMessage(0);
					break;
				}
			}
		}; break;

		// Posted by windows
		case WM_ACTIVATE:
		{
			// If we're running in RUNTYPE_SAVER_NORMAL we should
			// exit. THis should ONLY be done if we're in kernel loop.
			if((m_gpDemoDat->GetRunType()==RUNTYPE_SAVER_NORMAL)&&
						(BOOL)wParam==FALSE)
			{
				return TRUE;
			}
			else
			{
				lReturnValue = DefWindowProc (hWnd, uMsg, wParam, lParam); 
			}
		}; break;

		// Posted by windows
		case WM_SYSCOMMAND:
		{
			switch(wParam)
			{
				case SC_SCREENSAVE:
				{
					// do nothing. this way the system won't start (another) screensaver.
				}; break;
				default:
				{
					// nope, bubble the message
					lReturnValue = DefWindowProc (hWnd, uMsg, wParam, lParam); 
				}
			}
		}; break;

		// ADD MORE EVENTHANDLERS HERE

	    default: 
		{
			lReturnValue = DefWindowProc (hWnd, uMsg, wParam, lParam); 
		};break; 
    }

	// call all enabled eventhandlers of active objects 
	CallEnabledEventHandlers(hWnd, uMsg, wParam, lParam);
    return lReturnValue; 
}


// Purpose: this routine is called when the window is resized. Only done once, (when the windowtitle bar is added
// to the window, Windows will post a WM_SIZE message.
void
ReshapeCallBack(int iWidth, int iHeight)
{
	m_gpDemoDat->SetiWindowWidth(iWidth);
	m_gpDemoDat->SetiWindowHeight(iHeight);

    glViewport (0, 0, m_gpDemoDat->GetiWindowWidth(), m_gpDemoDat->GetiWindowHeight());      
	m_gpConsole->Reshape(m_gpDemoDat->GetiWindowWidth(),m_gpDemoDat->GetiWindowHeight());
}

