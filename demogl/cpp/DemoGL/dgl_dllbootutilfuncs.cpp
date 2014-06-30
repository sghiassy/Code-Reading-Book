//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllbootutilfuncs.cpp
// PURPOSE: in here are all the utility function needed for the bootprocess.
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// The functions that do the boot of the DemoGL system use subfunctions which actually do
// the work. These functions are stored in here.
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
// v1.2: Added to codebase.
// v1.1: --
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////
// Purpose: parses commandline passed and fills in the structure m_sclpSSCLParams in DemoDat. 
//
// Returns: SYS_OK if succeeded or SYS_NOK if not.
//
// NOTE: can someone at Microsoft please kick the moron who designed the screensaver commandline parameter
// interface... geeezzzzz "whaddayamean.. consistency--what?"
int
ParseScreenSaverCL(char *pszCommandLine)
{
	SScreenSaverCLParams	*pSSCLParams;
	char					*pCur;
	int						iResult;
	bool					bQuit;

	pSSCLParams=m_gpDemoDat->GetSSCLParams();
	iResult=SYS_OK;

	for(pCur=pszCommandLine, bQuit=false;(*pCur!=NULL) && (!bQuit);pCur++)
	{
		switch(*pCur)
		{
			case '\"':
			{
				// application name with path. walk towards other '"' which is always there.
				for(pCur++;(*pCur!='\"') && (*pCur!=NULL);pCur++){};
				if(!*pCur)
				{
					// if we exit here with a NULL, the commandline parameters were bad formulated.
					bQuit=true;
					iResult=SYS_NOK;
				}
				else
				{
					// we've found the closing '"'. skip it.
					pCur++;
					// can be last char. 
					bQuit=(!*pCur);
				}
			}; break;
			case ' ':
			case ':':
			{
				// skip char, it's an intermediate character between params.
				continue;
			}; break;
			case '/':
			case '-':
			{
				// skip, it's a parameter prefix
				continue;
			}; break;
			case 's':
			case 'S':
			{
				// it's a flag that will result in normal run of the screensaver.
				pSSCLParams->bInitialized=true;
				pSSCLParams->hWnd=NULL;
				pSSCLParams->iSaverExecType=DGL_SSAVERET_NORMAL;
			}; break;
			case 'c':
			case 'C':
			{
				// Config mode.
				pSSCLParams->hWnd=NULL;
				pSSCLParams->bInitialized=true;
				pSSCLParams->iSaverExecType=DGL_SSAVERET_CONFIG;
				// continue to get an eventual hwnd passed on the commandline
				continue;
			}; break;
			case 'p':
			case 'P':
			case 'l':
			case 'L':
			{
				// Preview mode.
				pSSCLParams->hWnd=NULL;
				pSSCLParams->bInitialized=true;
				pSSCLParams->iSaverExecType=DGL_SSAVERET_PREVIEW;
				// continue to get an eventual hwnd passed on the commandline
				continue;
			}; break;
			case 'a':
			case 'A':
			{
				// Password mode. we don't support this.
				pSSCLParams->hWnd=NULL;
				pSSCLParams->bInitialized=true;
				pSSCLParams->iSaverExecType=DGL_SSAVERET_PASSWORD;
			}; break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			{
				// hWnd passed. grab it.
				pSSCLParams->hWnd=(HWND)atoi(pCur);
				// done
				bQuit=true;
			}; break;
			default:
			{
				// bad formulated commandline. 
				bQuit=true;
				iResult=SYS_NOK;
			}; break;
		}
	}
	if(iResult==SYS_OK && !pSSCLParams->bInitialized)
	{
		// no arguments were passed... which means we should do a config execution. (like /c [###])
		pSSCLParams->bInitialized=true;
		pSSCLParams->hWnd=NULL;
		pSSCLParams->iSaverExecType=DGL_SSAVERET_CONFIG;
	}

	return iResult;
}


// Purpose: initializes all the crypting parameters, stored in DemoDat. It reads the keys from the keyfile,
// calculates the decryption key and reports back if all went well (SYS_OK) or not: (SYS_NOK)
//
// Returns: SYS_OK if everything went ok, SYS_NOK if something went wrong.
int
InitCryptingVars()
{
	int				iResult;
	byte			byRnd1, byRnd2;
	long			lDecryptKey;

	// check keyfile. If correct, it constructs the Decryptkey
	iResult = ReadAndCheckKeyfile(&byRnd1, &byRnd2);
	switch(iResult)
	{
		case SYS_KEYSDONTMATCH:
		{
			MessageBox(NULL,"The keys supplied as decryptorkeys don't match the keys\nused to crypt the datafiles.\n\nAborting.",
							"DemoGL Fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
			return SYS_NOK;
		}; break;
		case SYS_ILLEGALVERSION:
		{
			MessageBox(NULL,"The datafiles were crypted with an incompatible version of the Cryde Crypt/Decrypt Engine.\n\nCannot decrypt the datafiles. Aborting.",
							"DemoGL Fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
			return SYS_NOK;
		}; break;
		case SYS_FILENOTFOUND:
		{
			MessageBox(NULL,"Cannot find the file keyfile to read the decryption information.\nCannot decrypt the datafiles.\n\nAborting.",
							"DemoGL Fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
			return SYS_NOK;
		}; break;
		case SYS_OK:
		{
			// all ok. construct decryption key
			lDecryptKey = ((((long)(m_gpDemoDat->GetKey1())) * 256) + (long)(m_gpDemoDat->GetKey2())) * ((((long)(byRnd1)) * 256) + (long)(byRnd2));
			m_gpDemoDat->SetDecryptionKey(lDecryptKey);
		}; break;
		default:
		{
			MessageBox(NULL,"Unknown error.\n\nAborting.",
							"DemoGL Fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
			return SYS_NOK;
		}; break;
	}
	return SYS_OK;
}


// Purpose: creates a popup window as child of the given hWnd, stored in the m_sclpSSCLParams struct in DemoDat, which
// were retrieved from the commandline. The width and height are retrieved using the hWnd passed.
//
// Returns: everything goes OK, SYS_CW_OK is returned, else SYS_CW_NOTRECOVERABLEERROR if not. This
// will result in a system exit.
int
CreateMainWindowForSaverPreview()
{
    WNDCLASS		wcWndClass; 
	HWND			hWnd;
	HICON			hiAppIcon;
	char			sAppName[]=APPNAME;		// change this to let the app reflect the name of the app.
	int				iResult;

	hiAppIcon=LoadIcon(m_gpDemoDat->GethInstance(), MAKEINTRESOURCE(IDI_ICON)); 
	if(!hiAppIcon)
	{
		// not specified in application. use default DemoGL Icon
		hiAppIcon=LoadIcon(m_ghDLLInstance, MAKEINTRESOURCE(IDI_ICON)); 
	}

	// create a windowclass
	wcWndClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcWndClass.lpfnWndProc   = (WNDPROC)MainMsgHandler; 
	wcWndClass.cbClsExtra    = 0; 
	wcWndClass.cbWndExtra    = 0; 
	wcWndClass.hInstance     = m_gpDemoDat->GethInstance(); 
	wcWndClass.hIcon         = hiAppIcon;
	wcWndClass.hCursor       = LoadCursor (NULL,IDC_ARROW); 
	wcWndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); 
	wcWndClass.lpszMenuName  = sAppName; 
	wcWndClass.lpszClassName = sAppName; 

	// register the windowclass
	if(!RegisterClass(&wcWndClass)) 
	{
		// no way back.. exit.
		return SYS_CW_NOTRECOVERABLEERROR; 
	}

	// create an always on top borderless window.
	hWnd = CreateWindowEx(WS_EX_TOPMOST, sAppName,sAppName,
					WS_CHILD | WS_VISIBLE, 0, 0, m_gpDemoDat->GetiWindowWidth(),
					m_gpDemoDat->GetiWindowHeight(), m_gpDemoDat->GetSSCLParams()->hWnd,
					NULL, m_gpDemoDat->GethInstance(), NULL);

	 // test if everything went all right
	if (!hWnd) 
	{
		// no. this error is not recoverable.
		return SYS_CW_NOTRECOVERABLEERROR; 
	}

	// Setup the window.
	iResult=SetupMainWindow(hWnd);
	if(iResult!=SYS_OK)
	{
		// not good. quit.
		DestroyWindow(hWnd);
		return SYS_CW_NOTRECOVERABLEERROR;
	}
	
	// show the window
	ShowWindow(hWnd, SW_SHOWNORMAL); 
	return SYS_CW_OK;
}


// Purpose: creates the window and switches resolution. If everything goes OK, SYS_CW_OK is returned, else 
//
// Returns: SYS_CW_RECOVERABLEERROR if the error can be solved or SYS_CW_NOTRECOVERABLEERROR if not. The latter
// will result in a system exit.
int
CreateMainWindow(const char *sWindowTitle)
{
    WNDCLASS		wcWndClass; 
	HWND			hWnd, hParentWnd;
	HICON			hiAppIcon;
	char			sAppName[]=APPNAME;		// change this to let the app reflect the name of the app.
	int				iResult;

	hiAppIcon=LoadIcon(m_gpDemoDat->GethInstance(), MAKEINTRESOURCE(IDI_ICON)); 
	if(!hiAppIcon)
	{
		// not specified in application. use default DemoGL Icon
		hiAppIcon=LoadIcon(m_ghDLLInstance, MAKEINTRESOURCE(IDI_ICON)); 
	}

	switch(m_gpDemoDat->GetRunType())
	{
		case RUNTYPE_NORMAL:
		case RUNTYPE_NODIALOG:
		{
			hParentWnd=NULL;
		}; break;
		case RUNTYPE_SAVER_CONFIG:
		case RUNTYPE_SAVER_NORMAL:
		case RUNTYPE_SAVER_PREVIEW:
		case RUNTYPE_SAVER_PASSWORD:
		{
			hParentWnd=m_gpDemoDat->GetSSCLParams()->hWnd;
		}; break;
		////////////////////////////////
		// ADD MORE RUNTYPES HERE
		////////////////////////////////
		default:
		{
			// unknown runtype. 
			return SYS_CW_NOTRECOVERABLEERROR;
		}; break;
	}

	// create a windowclass
	wcWndClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcWndClass.lpfnWndProc   = (WNDPROC)MainMsgHandler; 
	wcWndClass.cbClsExtra    = 0; 
	wcWndClass.cbWndExtra    = 0; 
	wcWndClass.hInstance     = m_gpDemoDat->GethInstance(); 
	wcWndClass.hIcon         = hiAppIcon;
	wcWndClass.hCursor       = LoadCursor (NULL,IDC_ARROW); 
	wcWndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); 
	wcWndClass.lpszMenuName  = sAppName; 
	wcWndClass.lpszClassName = sAppName; 

	// register the windowclass
	if(!RegisterClass(&wcWndClass)) 
	{
		// no way back.. exit.
		return SYS_CW_NOTRECOVERABLEERROR; 
	}

	// Create windowframe because all went well. 
	if(m_gpDemoDat->GetbFullScreen())
	{
		if(SwitchToFullScreen()==SYS_NOK)
		{
			// error occured...
			// let the user select another option. so don't quit.
			// unregister class.
			UnregisterClass(sAppName,m_gpDemoDat->GethInstance());
			// continue
			return SYS_CW_RECOVERABLEERROR;
		}
		else
		{
			// remove cursor
			ShowCursor(FALSE);
		}
		// create an always on top borderless window.
		// FIXED:
		// [FB] 29-mar-2001: removed WS_EX_TOPMOST
		hWnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_NOPARENTNOTIFY, sAppName,sWindowTitle,
						WS_POPUP , 0, 0, m_gpDemoDat->GetiWindowWidth(),
						m_gpDemoDat->GetiWindowHeight(), hParentWnd, NULL, m_gpDemoDat->GethInstance(), NULL);
	}
	else
	{
		// create a window with no system menubuttons or resizing capabilities.
		// FIXED:
		// [FB] 29-mar-2001: removed WS_EX_TOPMOST
		hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, sAppName,sWindowTitle, 
				WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, 
				CW_USEDEFAULT, CW_USEDEFAULT, m_gpDemoDat->GetiWindowWidth(),
				 m_gpDemoDat->GetiWindowHeight(), hParentWnd, NULL, m_gpDemoDat->GethInstance(), NULL); 
	}

	 // test if everything went all right
	if (!hWnd) 
	{
		// no. this error is not recoverable.
		return SYS_CW_NOTRECOVERABLEERROR; 
	}
	
	// Setup the window.
	iResult=SetupMainWindow(hWnd);
	if(iResult!=SYS_OK)
	{
		// not good. quit.
		DestroyWindow(hWnd);
		return SYS_CW_NOTRECOVERABLEERROR;
	}

	// show the window
	ShowWindow(hWnd, SW_SHOWNORMAL); 
	return SYS_CW_OK;
}


// Purpose: sets up the mainwindow. If error occures, all temporary retrieved objects are released on the fly.
// So no clean up of already retrieved objects has to be done when this routine fails.
//
// Returns: SYS_OK if everything went allright, SYS_NOK if not.
int
SetupMainWindow(HWND hWnd)
{
	HDC				hDC;
	HGLRC			hRC;

	hDC = GetDC(hWnd); 

	// setup pixelformat structure and get the pixelformat for OpenGL.
	if(SetupPixelFormat(hDC)!=SYS_OK)
	{
		// didn't succeed. exit
		return SYS_NOK;
	}

	// use Microsoft's wrappercalls to create a RenderContext and connect it to window
 	hRC = wglCreateContext(hDC); 

	if(!hRC)
	{
		// failed.
		ReleaseDC(hWnd, hDC);
		MessageBox(hWnd,"Couldn't create RenderContext for OpenGL","DemoGL Init Error",MB_ICONERROR|MB_APPLMODAL);
		return SYS_NOK;
	}

	// make the just created rendercontext current.
	wglMakeCurrent(hDC, hRC); 
	
	// store windowhandle, DC and RC in DemoDat...
	m_gpDemoDat->SetWindowVars(hWnd, hDC, hRC);

	// initialize OpenGL and set some states to default values.
	InitGL();

	// done. Send WM_INITSYSTEM message.
	PostMessage(m_gpDemoDat->GethWnd(),WM_DEMOGL_INITSYSTEM,0,0);
	return SYS_OK;
}



// Purpose: initializes a structure for the pixelformat. In the pixelformat you define what the app will
// use: doublebuffering, all kinds of bufferdepths, colors per pixel etc.
//
// Returns: SYS_OK if setup succeeded, SYS_NOK otherwise.
int
SetupPixelFormat(HDC hDC) 
{ 
    int						iPixelFormat, iResult;
	PIXELFORMATDESCRIPTOR	pfdCheck;
	bool					bResult;

	// define default desired video mode (pixel format)
	PIXELFORMATDESCRIPTOR pfdSet = 
	{
        sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
        1,								// version number
        PFD_DRAW_TO_WINDOW |			// support window
        PFD_SUPPORT_OPENGL |			// support OpenGL
        PFD_DOUBLEBUFFER,				// double buffered
        PFD_TYPE_RGBA,                  // RGBA type
        24,                             // 24-bit color depth to start with
        8, 16, 8, 8, 8, 0,              // color bits ignored
        0,                              // no alpha buffer
        0,                              // shift bit ignored
        0,                              // no accumulation buffer
        16,16,16,0,		                // accum bits ignored
        24,                             // 24-bit z-buffer
        0,                              // no stencil buffer
        0,                              // no auxiliary buffer
        PFD_MAIN_PLANE,                 // main layer
        0,                              // reserved
        0, 0, 0                         // layer masks ignored
    };

	//set the changes you made through the #defines at the top.
	if(DOUBLEBUFFERING)
	{
		pfdSet.dwFlags=	PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL ;
	}
	else 
	{
		pfdSet.dwFlags=	PFD_SUPPORT_GDI| PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_NEED_SYSTEM_PALETTE;
	}
	
	// set colordepth
	pfdSet.cColorBits=(unsigned char)m_gpDemoDat->GetiDepth();

	// set Zbufferdepth
	pfdSet.cDepthBits=ZBUFFERDEPTH;

	// set stencilbuffer.
	pfdSet.cStencilBits = STENCILBUFFERDEPTH;

	// Get the pixelformat of this setup.
	if ((iPixelFormat = ChoosePixelFormat(hDC, &pfdSet)) == 0 )
    {
		// failed. try other pixeldepth.
		pfdSet.cColorBits = 16;
		if ((iPixelFormat = ChoosePixelFormat(hDC, &pfdSet)) == 0 )
		{
			MessageBox(NULL,"Your OpenGL driver reported it doesn't support 32 nor 16bit rendering. Cannot continue. Aborting.",
							"DemoGL Fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
	        return SYS_NOK;
		}
    }

	// Get a description of the pixelformat chosen
	iResult=DescribePixelFormat(hDC, iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfdCheck);
	if(!iResult)
	{
		MessageBox(NULL,"The OpenGL Pixeldescription couldn't be retrieved. Cannot continue. Aborting.",
						"DemoGL Fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
		return SYS_NOK;
	}

	// test the flags if there is any hardware acceleration. If not, display a warning that the visuals will be utterly slow :)
	bResult=((pfdCheck.dwFlags & PFD_GENERIC_ACCELERATED) == 0) && ((pfdCheck.dwFlags & PFD_GENERIC_FORMAT) > 0);
	if(bResult)
	{
		iResult=MessageBox(NULL,"The OpenGL driver that's currently active doesn't support hardware acceleration or there is no decent OpenGL driver installed.\n \
\nDemoGL will use the generic Microsoft OpenGL implementation, which uses single buffer software rendering and is extremely slow. \n\nContinue?",
						"DemoGL Error.",MB_ICONERROR|MB_YESNO|MB_SYSTEMMODAL);
		if(iResult==IDNO)
		{
			// doesn't want to continue
			return SYS_NOK;
		}
	}

	// try to set this video mode    
	if(!SetPixelFormat(hDC, iPixelFormat, &pfdSet))
    {
		// the requested video mode is not available so get a default one
        iPixelFormat = 1;	
		if(DescribePixelFormat(hDC, iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfdSet)==0)
		{
			// neither the requested nor the default are available: fail
			MessageBox(NULL,"No valid pixeldescriptor found. Cannot continue. Aborting.",
						"DemoGL Fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
			return SYS_NOK;
		}
		else
		{
			SetPixelFormat(hDC, iPixelFormat, &pfdSet);
		}
    }
	m_gpDemoDat->SetPixelFormatData(iPixelFormat, pfdCheck.cDepthBits, pfdCheck.cStencilBits,pfdCheck.cColorBits);
    return SYS_OK;
}


// Purpose: initialize some basic global OpenGL stuff. Most of the stuff is done by the effects though.
void
InitGL()
{
	GLuint	arr_iTexNames[MAXTEXTURES];
	
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glShadeModel(GL_SMOOTH);							// set default shading.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);	// set perspective correcthint

	// more things like these calls are not handy here, because effectobjects can suffer from settings
	// we set here. Effects should set and reset their proper OpenGL features before rendering stuff.
	// (these statechanges are a litlle overhead and not that hurting)

    glMatrixMode(GL_PROJECTION);    
    glLoadIdentity(); 
	// it's is done for starters. It can be changed once or whenever in every effect.
    glFrustum (-1.0, 1.0, -0.75, 0.75, 1.0, 20.0); 
    glMatrixMode(GL_MODELVIEW);  
    glViewport(0, 0, m_gpDemoDat->GetiWindowWidth(), m_gpDemoDat->GetiWindowHeight());

	// Generate MAXTEXTURES textureobjects inside OpenGL. 
	glGenTextures(MAXTEXTURES,&arr_iTexNames[0]);

	// get the delta between the first texture name and 0. This delta is then used to transform
	// texture ID's into indices in m_garrpTextures[] and back. Store this delta in DemoDat
	m_gpDemoDat->SetTexNameTexIDDelta(arr_iTexNames[0]);
}


// Purpose: switches the screenmode to fullscreen.
//
// Returns: SYS_OK if succeeded, SYS_NOK if not.
int 
SwitchToFullScreen()
{
	DEVMODE		dMode;
	int			iModeNo, iResult;
	bool		bModeSwitch;
	long		lChangeResult;

	// Walk all modes supported
	for(iModeNo=0, bModeSwitch=false;!bModeSwitch;)
	{
		bModeSwitch = !EnumDisplaySettings(NULL, iModeNo, &dMode );
		if(dMode.dmBitsPerPel == (unsigned int)m_gpDemoDat->GetiDepth()&& 
		   dMode.dmPelsWidth == (unsigned int)m_gpDemoDat->GetiWindowWidth() && 
		   dMode.dmPelsHeight == (unsigned int)m_gpDemoDat->GetiWindowHeight())
		{
			break;
		}
		iModeNo++;
	}
	if(bModeSwitch)
	{
		// didn't find mode
		ChangeDisplaySettings(NULL, 0);
		MessageBox(m_gpDemoDat->GethWnd(), "Couldn't switch to full screen. The resolution you've chosen isn't supported by your videocard. Choose another resolution or choose windowed mode.", "DemoGL Init Error",MB_ICONERROR|MB_APPLMODAL);
		return SYS_NOK;
	}

	dMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

	lChangeResult = ChangeDisplaySettings(&dMode, CDS_TEST);
	switch (lChangeResult)
	{
		case DISP_CHANGE_SUCCESSFUL:
		{
			ChangeDisplaySettings(&dMode, CDS_FULLSCREEN);
		};break;
		case DISP_CHANGE_FAILED:
		{
				MessageBox(NULL, 
				"DemoGL failed to change the screenresolution and desktop bitdepth to the desired settings. Try Windowed Appearance.", 
				"DemoGL Error occured",MB_ICONERROR|MB_APPLMODAL);
			return SYS_NOK;
		};break;
		case DISP_CHANGE_BADMODE:
		{
			MessageBox(NULL, 
				"The fullscreen mode you selected, with the options you selected, is not supported by your system. Try a lower bitdepth for Display Quality.", 
				"DemoGL Error occured",MB_ICONERROR|MB_APPLMODAL);
			return SYS_NOK;
		};break;
		case DISP_CHANGE_RESTART:
		{
			// the user has to reboot to get the fullscreen setting, because the drivers in the system and the system itself (win95
			// first release) are not able to switch the resolution using this call. (they are, but the nagging thing is.... we're not
			// allowed :(
			// The user doesn't have to reboot if the user matches the bitdepth and resolution of the desktop. 
			iResult=MessageBox(NULL, 
				"The fullscreen mode you selected requires a reboot, according to windows or your videocard drivers.\nA reboot can be avoided when you choose the same bitdepth at Display Quality as your desktop currently has\nor try Windowed Appearance.\n\nDo you want to ignore the reboot message and continue? (it might refuse to work)",
				"DemoGL Error occured",MB_ICONERROR|MB_APPLMODAL|MB_YESNO);
			if(iResult==IDYES)
			{
				// User wants to continue... 
				lChangeResult=ChangeDisplaySettings(&dMode, CDS_FULLSCREEN);
				if(lChangeResult==DISP_CHANGE_FAILED)
				{
					MessageBox(NULL, 
						"DemoGL failed to change the screenresolution and desktop bitdepth to the desired settings. Try Windowed Appearance.", 
						"DemoGL Error occured",MB_ICONERROR|MB_APPLMODAL);
					return SYS_NOK;
				}
				else
				{
					// successful
					break;
				}
			}
			else
			{
				// user doesn't want to continue... 
				return SYS_NOK;
			}
		};break;
	}
	return SYS_OK;
}


// Purpose: determines the width and height in pixels of a given resolution constant. a DEMOGL_AppRun helper routine
// Gets: iResolution, int, resolution constant, defined in dgl_dllmain.h
// Gets: *piWidth, int pointer, pointer to variable where width should be written
// Gets: *piHeight, int pointer, pointer to variable where height should be written
// returns: nothing
void
GetWidthHeightOfResolution(int iResolution, int *piWidth, int *piHeight)
{
	switch(iResolution)
	{
		case RES800x600:
		{
			*piWidth=800;
			*piHeight=600;
		}; break;
		case RES640x480:
		{
			*piWidth=640;
			*piHeight=480;
		}; break;
		case RES1024x768:
		{
			*piWidth=1024;
			*piHeight=768;
		}; break;
		case RES320x240:
		{
			*piWidth=320;
			*piHeight=240;
		}; break;
		case RES400x300:
		{
			*piWidth=400;
			*piHeight=300;
		}; break;
		case RES512x384:
		{
			*piWidth=512;
			*piHeight=384;
		}; break;
		case RES960x720:
		{
			*piWidth=960;
			*piHeight=720;
		}; break;
		case RES1152x864:	
		{
			*piWidth=1152;
			*piHeight=864;
		}; break;
		case RES1280x1024:
		{
			*piWidth=1280;
			*piHeight=1024;
		}; break;
		case RES1600x1200:
		{
			*piWidth=1600;
			*piHeight=1200;
		}; break;
		case RES2048x1536:
		{
			*piWidth=2048;
			*piHeight=1536;
		}; break;

		// ADD MORE RESOLUTION CONSTANTS HERE

		default:
		{
			// unknown resolution, choose 800x600
			*piWidth=800;
			*piHeight=600;
		}; break;
	}
}


// Purpose: checks if the keyfile is there, and ifso it reads it and checks if the keys given in the dialog
// are the ones used to crypt the files. if so, the two random numbers are returned plus SYS_OK
// failures: SYS_FILENOTFOUND (keyfile couldn't be read), SYS_ILLEGALVERSION (keyfile is of a different version
// than the current decrypter can handle) and SYS_KEYSDONTMATCH (keys mentioned in dialog are wrong.)
int 
ReadAndCheckKeyfile(byte *byRnd1, byte *byRnd2)
{
	byte		*byFile;
	byte		byKey1, byKey2;
	char		*sKeyData;
	char		sKey1[3], sKey2[3], sRnd1[3], sRnd2[3], sVersion[5];

	byFile = DEMOGL_FileLoad(CRYPTKEYFILE);
	if(!byFile)
	{
		// not found
		return SYS_FILENOTFOUND;
	}

	sKeyData = (char *)byFile;

	// check version.
	strncpy(&sVersion[0],sKeyData,4);
	sVersion[4] = '\0';
	if(strcmp(&sVersion[0],CRYPTENGINEVERSION))
	{
		// illegal version
		sKeyData=NULL;
		DEMOGL_FileFree(byFile);
		return SYS_ILLEGALVERSION;
	}

	// check keys.
	sKey1[0] = sKeyData[9+4];
	sKey1[1] = sKeyData[3+4];
	sKey1[2] = sKeyData[1+4];
	sKey2[0] = sKeyData[8+4];
	sKey2[1] = sKeyData[13+4];
	sKey2[2] = sKeyData[2+4];
	sRnd1[0] = sKeyData[12+4];
	sRnd1[1] = sKeyData[6+4];
	sRnd1[2] = sKeyData[14+4];
	sRnd2[0] = sKeyData[4+4];
	sRnd2[1] = sKeyData[10+4];
	sRnd2[2] = sKeyData[7+4];

	byKey1 = ((sKey1[0] - '0') * 100) + ((sKey1[1] - '0') * 10) + (sKey1[2] - '0');
	byKey2 = ((sKey2[0] - '0') * 100) + ((sKey2[1] - '0') * 10) + (sKey2[2] - '0');
	if(!((byKey1==m_gpDemoDat->GetKey1()) && (byKey2==m_gpDemoDat->GetKey2())))
	{
		// not valid keys.
		sKeyData=NULL;
		DEMOGL_FileFree(byFile);
		return SYS_KEYSDONTMATCH;
	}
	// keys match. fill Rnd numbers.
	*byRnd1 = ((sRnd1[0] - '0') * 100) + ((sRnd1[1] - '0') * 10) + (sRnd1[2] - '0');
	*byRnd2 = ((sRnd2[0] - '0') * 100) + ((sRnd2[1] - '0') * 10) + (sRnd2[2] - '0');
	sKeyData=NULL;
	DEMOGL_FileFree(byFile);
	return SYS_OK;
}

