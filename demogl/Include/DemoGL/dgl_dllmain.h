//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllmain.h
// PURPOSE: include file of dgl_dllmain.cpp and others.
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
// v1.3: Added new constants and objects.
// v1.2: Moved all global stuff to 1 place and renamed them
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#ifndef _DGL_DLLMAIN_H
#define _DGL_DLLMAIN_H

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	// If line below is commented, the functionnames are not displayed when logging a line to console
	//#define _DEBUG_SHOWFUNCTIONNAMES
#endif

// Some defines to let you set some specifics about your application easy.
// These values are defaults for the dialog that pops up when the user starts the application.
#define	DOUBLEBUFFERING					1			// 1 = yes, 0 = no. 0 Not recommended.
#define	FULLSCREEN						1			// 1 = yes, 0 = no
#define	DISPLAY32BIT					1			// 1 = yes which gives 32 bpp, 0 = no, which implies 16bpp
#define	TEXTURE32BIT					1			// 1 = yes which gives 32 bit textures, 0=no which implies 16bit textures
#define	SOUND							1			// 1 = yes (MP3 player enabled) 0 = no (Silence)
// DONT ALTER THIS.
#define	ZBUFFERDEPTH					24			// when using 8 bit stencilbuffer, use 24bit Z buffer. 
// DONT ALTER THIS.
#define	STENCILBUFFERDEPTH				8			// in bits per pixel. 8 is normal. Not all cards support stencilbuffer.
// DONT ALTER THIS.
#define CRYPTKEYFILE					"keyfile"	// filename where the scrambled key is stored. This scrambled key is used 
													// to check the keys passed to DemoGL in DEMOGL_AppRun();

#define DEMOGLWEBSITE					"http://www.demogl.com/"


// General stuf
#define DEMOGL_VERSION					1.31f			// versionnr
#define DEMOGL_BUILD					923				// buildnr
#define DEMOGL_VERSIONTYPE				"Release"		// Alpha, Beta, RC(release candidate)x or nothing (release)
#define	DEMOGL_COPYRIGHT				"(c)1999-2001 Solutions Design. All rights reserved."
#define DEMOGL_AUTHOR					"Programmed by Frans 'Otis' Bouma."
#define DEMOGL_MSS_COPYRIGHT			"BASS Music subsystem programmed by Ian Luck. All rights reserved."

// SystemStates. If a state is added, also add a string to the
// lowlevel debugger routine _dgldbg_SystemState2String() in dgl_dlllowleveldebugger.cpp
#define SSTATE_PREBOOT					1
#define SSTATE_BOOT_STARTUPDIALOG		10
#define SSTATE_BOOT_MAINWNDCREATION		15
#define SSTATE_BOOT_MESSAGEPUMPSTART	16
#define SSTATE_BOOT_INITSTART			20
#define SSTATE_BOOT_SYSOBJECTSINIT		25
#define SSTATE_AINIT_SCRIPTPARSING		40
#define SSTATE_AINIT_EFFECTSINIT		45
#define SSTATE_AINIT_PRESTARTEVENTEXEC	50
#define SSTATE_ARUN_KERNELLOOP			60
#define SSTATE_ARUN_POSTKERNELLOOP		65
#define SSTATE_AEND_SYSTEMCLEANUP		80			

// define constants
#define	MAXTEXTURES						1024		// MAXTEXTURES amount of different textures can be loaded at a time. if every texture is 192KB this is LAAARGE (1GB of memory)
#define	MAXTEXWIDTH						2048		// maximum of texture width in pixels. 
#define	MAXTEXHEIGHT					2048		// maximum of texture width in pixels.

#define	BPPDEPTHHINT16BIT				GL_RGBA4	// the hint for the textureuploader for 16bpp texture choice
#define	BPPDEPTHHINT32BIT				GL_RGBA8	// the hint for the textureuploader for 32bpp texture choice.

#define SRC_LOADFROMDATAFILE			1			// loads from datafile. Deprecated in v1.3
#define	SRC_LOADFROMDIR					2			// loads from directory. Deprecated in v1.3
#define DGL_FSRC_LOADFROMDATAFILE		SRC_LOADFROMDATAFILE
#define DGL_FSRC_LOADFROMDIR			SRC_LOADFROMDIR

#define APPNAME							"DemoGLApp"

// file type constants
#define JPGFILE							1			// Deprecated in v1.3
#define	BMPFILE							2			// Deprecated in v1.3
#define TGAFILE							3			// Deprecated in v1.3
#define	DGL_FTYPE_JPGFILE				JPGFILE
#define DGL_FTYPE_BMPFILE				BMPFILE
#define DGL_FTYPE_TGAFILE				TGAFILE
#define DGL_FTYPE_DDSFILE				4

// Resolution constants
#define RES800x600						1
#define RES640x480						2
#define RES1024x768						3
#define RES320x240						4
#define RES400x300						5
#define RES512x384						6
#define RES960x720						7
#define RES1152x864						8
#define RES1280x1024					9
#define RES1600x1200					10
#define RES2048x1536					11

// Resolution strings for dropdownbox in startup dialog
#define STR_RES800x600					"800 x 600"
#define STR_RES640x480					"640 x 480"
#define STR_RES1024x768					"1024 x 768"
#define STR_RES320x240					"320 x 240"
#define STR_RES400x300					"400 x 300"
#define STR_RES512x384					"512 x 384"
#define STR_RES960x720					"960 x 720"
#define STR_RES1152x864					"1152 x 864"
#define STR_RES1280x1024				"1280 x 1024"
#define STR_RES1600x1200				"1600 x 1200 (Be careful)"
#define STR_RES2048x1536				"2048 x 1536 (Be careful)"

// Resolution indices in dropdownbox in startup dialog
#define INDX_RES320x240					0
#define INDX_RES400x300					1
#define INDX_RES512x384					2					
#define INDX_RES640x480					3					
#define INDX_RES800x600					4
#define INDX_RES960x720					5			
#define INDX_RES1024x768				6				
#define INDX_RES1152x864				7					
#define INDX_RES1280x1024				8					
#define INDX_RES1600x1200				9				
#define INDX_RES2048x1536				10					

// define system return values
#define	SYS_OK							0
#define	SYS_NOK							1
#define	SYS_STOP						2
#define SYS_WRONG_WINDOWSIZE			100
#define SYS_MEM_ALLOC_FAIL				1000
#define SYS_CONSOLE_FAILED				1001
#define	SYS_FILENOTFOUND				-100
#define	SYS_ERRORINZIP					-1
#define SYS_ILLEGALVERSION				-2
#define	SYS_KEYSDONTMATCH				-3

// Application run types
#define RUNTYPE_NORMAL					1		// default
#define RUNTYPE_NODIALOG				2
#define RUNTYPE_SAVER_CONFIG			3
#define RUNTYPE_SAVER_NORMAL			4
#define RUNTYPE_SAVER_PREVIEW			5
#define RUNTYPE_SAVER_PASSWORD			6		// not supported.

// Screensaver Execution types. Stored in SScreenSaverCLParams and returned by DEMOGL_ParseScreenSaverCL
#define DGL_SSAVERET_UNKNOWN			0
#define DGL_SSAVERET_CONFIG				1
#define	DGL_SSAVERET_NORMAL				2
#define DGL_SSAVERET_PREVIEW			3
#define DGL_SSAVERET_PASSWORD			4		// not supported.

// Screensaver mouse treshold, the amount of pixels moved before the screensaver will exit
#define SSAVER_MOUSE_TRESHOLD			10

// Registry key prefixes for the different runtypes (RUNTYPE_NORMAL and RUNTYPE_SAVER_*)
#define REGKEY_RT_NORMAL				"Application"
#define REGKEY_RT_SCREENSAVER			"Screensaver"

// Registry key names for startup data storage
#define REGKEY_CURRENTUSER_SUB			"DemoGL"
#define REGKEY_DATA_SUB					"AppData"

// Platform constants supported
#define OSTYPE_UNKNOWN					0
#define	OSTYPE_WINDOWS2000				1
#define OSTYPE_WINDOWSNT				2
#define OSTYPE_WINDOWS98				3
#define OSTYPE_WINDOWS95				4
#define OSTYPE_WIN32S					5

// Crypting constants
#define	KEYFILELENGTH					128
// this string is placed in front of the keyhash in the keyfile. DemoGL and the decrypt routine
// will check this version. if the version mentioned in the keyfile is NOT matching this string
// there can't be any decrypting. 
#define CRYPTENGINEVERSION				"0001"		

// Universal file/path stringlength constraint
#define MAXPATHNAMELENGTH				1024

// Effect's method Init() failure response flags
#define DGL_AIFF_DIE					1				// DemoGL will end when an effect fails in the Init() method
#define DGL_AIFF_IGNORE					2				// DemoGL will ignore the failure of an effect's Init() method
#define DGL_AIFF_REMOVEEFFECT			3				// DemoGL will remove the effect which Init() method failed.

/////////////////////////////////////////
// Flags for DEMOGL_SetVariables
/////////////////////////////////////////
// Defaults
#define DGL_VF_VSYNC					1
#define	DGL_VF_FILESRC					2
#define	DGL_VF_SRCDIR					3
#define	DGL_VF_DATAFILENAME				4
#define	DGL_VF_SCRIPTFILENAME			5
#define	DGL_VF_SHOWFPS					6
#define	DGL_VF_NEVERSHOWFPS				7
#define	DGL_VF_NEVERSHOWCONSOLE			8
#define	DGL_VF_SHOWDEBUGINFO			9
#define	DGL_VF_NEVERSHOWDEBUGINFO		10
#define	DGL_VF_NEVERRESCALETEXTURES		11
#define DGL_VF_QUICKBOOT				12
#define DGL_VF_USERENDERFRAMEEX			13
#define DGL_VF_INITFAILURERESPONSE		14

#define DGL_VF_MUSICSYS					20000		// OBSOLETE. For backwards compatibility. 
													// Not defined in Demogl_DLL.h

// Other flags, settable
#define DGL_VF_SWAPBUFFERS				1000

// DEMOGL Default values. These values are overrulable by the user using 
// DEMOGL_OverruleDefaults() calls.
#define	DGL_DEFAULT_VSYNC					false
#define	DGL_DEFAULT_FILESRC					DGL_FSRC_LOADFROMDATAFILE
#define	DGL_DEFAULT_SRCDIR					""
#define	DGL_DEFAULT_DATAFILENAME			"demodat.zip"
#define	DGL_DEFAULT_SCRIPTFILENAME			"demoflow"
#define	DGL_DEFAULT_SHOWFPS					false
#define	DGL_DEFAULT_NEVERSHOWFPS			false
#define	DGL_DEFAULT_NEVERSHOWCONSOLE		false
#define	DGL_DEFAULT_SHOWDEBUGINFO			false
#define	DGL_DEFAULT_NEVERSHOWDEBUGINFO		false
#define	DGL_DEFAULT_NEVERRESCALETEXTURES	true
#define	DGL_DEFAULT_QUICKBOOT				true
#define	DGL_DEFAULT_MUSICSYS				MP3SUBSYSTEM			// deprecated default.
#define DGL_DEFAULT_USERENDERFRAMEEX		false
#define DGL_DEFAULT_INITFAILURERESPONSE		DGL_AIFF_IGNORE
// ADD MORE USER OVERRULABLE DEFAULTS HERE.

// minimum values for console properties which can be set with DEMOGL_ConsoleSetAppearance()
// If you set a property that will result in a conflict with these values, DEMOGL_ConsoleSetAppearance() will fail.
#define CNSL_MINAMCHARSPERLINE			60
#define CNSL_MINAMLINESONSCREEN			7
#define CNSL_MINAMLINESINDBGINFO		0
#define CNSL_MINAMLINESINBUFFER			2

/////////////////////////////////////////////////////
// specific returnvalues to report more info
/////////////////////////////////////////////////////
// StartupDialog specific:
#define SYS_SUD_CANCELCLICKED			1
#define SYS_SUD_OKCLICKED				2

// CreateMainWindow specific:
#define SYS_CW_OK						SYS_OK
#define SYS_CW_RECOVERABLEERROR			1
#define SYS_CW_NOTRECOVERABLEERROR		2

// DoImportBMP specific:
#define	SYS_BMP_LOAD_FAILED				2
#define SYS_BMP_INVALID_HEADER			3
#define SYS_BMP_INVALID_COLORS			4
#define SYS_BMP_INVALID_COMPRESSED		5
#define SYS_BMP_INVALID_SIZE			6

// DoImportTGA specific:
#define SYS_TGA_LOAD_FAILED				11
#define	SYS_TGA_EMPTY_FILE				12
#define SYS_TGA_INVALID_FORMAT			13
#define	SYS_TGA_INVALID_SIZE			14

// DoImportJPG specific
#define	SYS_JPG_LOAD_FAILED				20
#define SYS_JPG_CONVERT_FAILED			21

// DoImportDDS specific
#define SYS_DDS_LOAD_FAILED				30
#define SYS_DDS_UNSUPPORTEDFORMAT		31
#define SYS_DDS_NODDSFILE				32

// UploadTexture specific
#define SYS_UT_NO_TEXTURE				11
#define SYS_UT_INVALID_TEXTURE			13

// LoadAndParseScript specific
#define SYS_LPS_FILENOTFOUND			20

// Lex specific
#define LA_ISCOMMENT					100
#define	LA_ISEMPTY						101
#define LA_TOKENIZED					103

// Parser specific
#define TP_OK							200
#define TP_SYNTAXERROR					201

// Console window move defines
#define CONSOLE_MOVE1LINEUP				1
#define CONSOLE_MOVE1LINEDOWN			2
#define CONSOLE_MOVE1PAGEUP				3
#define CONSOLE_MOVE1PAGEDOWN			4

// MessageDefinitions
#define WM_DEMOGL_INITSYSTEM			0x401			// WM_USER + 1. Sent when WM_CREATE is finished.
#define WM_DEMOGL_INITAPP				0x402			// Sent when thread started by WM_DEMOGL_INITSYSTEM is finished.
#define WM_DEMOGL_ABORT					0x403			// unrecoverable error: abort system
#define WM_DEMOGL_STARTKERNELLOOP		0x404			// starts the kernelloop
#define WM_DEMOGL_PREPARE				0x405			// prepares the system for the big start.
#define WM_DEMOGL_PREPAREEFFECT			0x407			// prepare effect in gpPrepareQueue;
#define WM_DEMOGL_PAINT					0x408			// will render the screen. only used in bootphase to render the console
#define WM_DEMOGL_SOUNDSYSTEMSYNC		0x409			// sent to every effect when a sound sync is triggered by BASS.

// Texture dimensions:
#define DGL_TEXTUREDIMENSION_1D			1
#define DGL_TEXTUREDIMENSION_2D			2
#define DGL_TEXTUREDIMENSION_3D			3			// currently not supported due to lack of opengl32.dll with OGL 1.2 functionality.

////////////////////////////////////////
// 	OBSOLETE IN V1.3
////////////////////////////////////////
// MusicSubSystemTypes.
#define	MP3SUBSYSTEM					1		// OBSOLETE IN V1.3
#define MODSUBSYSTEM					2		// OBSOLETE IN V1.3
////////////////////////////////////////
// 	END OF OBSOLETE IN V1.3
////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//                          
//						TYPEDEFS
//
////////////////////////////////////////////////////////////////////////

// create a byte type
typedef unsigned char byte;

////////////////////////////////////////////////////////////////////////
//                          
//						FUNCTION DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
extern BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwreason,  LPVOID lpReserved);

////////////////////////////////////////////////////////////////////////
//                          
//						GLOBALS
//
////////////////////////////////////////////////////////////////////////
extern	CTexture			*m_garrpTextures[MAXTEXTURES];
extern	CSysConsole			*m_gpConsole;
extern	CTimeLineEvent		*m_gpTimeLineHead;
extern	CEffectStore		*m_gpESHead;
extern	CEffectStore		*m_gpESTail;
extern	CLayer				*m_gpLayerHead;
extern	HINSTANCE			m_ghDLLInstance;
extern	CDemoDat			*m_gpDemoDat;
extern	CFifo				*m_gpPrepareQueue;
extern	CExtensions			*m_gpExtensions;
extern	CSoundSystem		*m_gpSoundSystem;
extern	CProgressBar		*m_gpLoadingPB;

#endif	// _DGL_DLLMAIN_H