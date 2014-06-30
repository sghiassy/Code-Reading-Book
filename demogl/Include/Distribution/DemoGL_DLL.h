//////////////////////////////////////////////////////////////////////
// FILE: DemoGL_DLL.h
// PURPOSE: include file of DemoGL_DLL
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// Include file for the demogl dll. 
//////////////////////////////////////////////////////////////////////
// Part of DemoGL Demo System. See version information
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
//////////////////////////////////////////////////////////////////////
// Contributers to the code:
//		- Frans Bouma [FB]
//////////////////////////////////////////////////////////////////////
// VERSION INFORMATION.
//
// v1.3: Added new API interface and definitions. 
// v1.2: Fixed some mistakes, added new functions to the interface
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////
// Last changed on: 19-may-2001.
// For library version: v1.31.
//////////////////////////////////////////////////////////////////////

#ifndef _DEMOGL_DLL_H
#define _DEMOGL_DLL_H

#include	<windows.h>
#include	<stdio.h>
#include	<GL/gl.h>

#include	"DemoGL_Effect.h"
#include	"DemoGL_Bass.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                  DEMOGL API #DEFINES. DO NOT ALTER.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DLLIMPORT							__declspec(dllimport)

// Textureupload hint ignore parameter for DEMOGL_TextureLoad()
#define DEMOGL_TEXHINT_USEDEFAULT			0		// DEPRECATED FROM v1.3
#define DGL_TEXHINT_USEDEFAULT				DEMOGL_TEXHINT_USEDEFAULT

// system return values
#define	SYS_OK								0		// DEPRECATED FROM v1.3
#define	SYS_NOK								1		// DEPRECATED FROM v1.3
#define DGL_RVAL_SYS_OK						SYS_OK
#define DGL_RVAL_SYS_NOK					SYS_NOK

// file type constants
#define JPGFILE								1		// DEPRECATED FROM v1.3
#define	BMPFILE								2		// DEPRECATED FROM v1.3
#define TGAFILE								3		// DEPRECATED FROM v1.3
#define DGL_FTYPE_JPGFILE					JPGFILE
#define DGL_FTYPE_BMPFILE					BMPFILE
#define DGL_FTYPE_TGAFILE					TGAFILE
#define DGL_FTYPE_DDSFILE					4

// Src read constants
#define SRC_LOADFROMDATAFILE				1		// DEPRECATED FROM v1.3
#define	SRC_LOADFROMDIR						2		// DEPRECATED FROM v1.3
#define DGL_FSRC_LOADFROMDATAFILE			SRC_LOADFROMDATAFILE
#define DGL_FSRC_LOADFROMDIR				SRC_LOADFROMDIR

// Effect's method Init() failure response flags
#define DGL_AIFF_DIE						1		// DemoGL will end when an effect fails in the Init() method
#define DGL_AIFF_IGNORE						2		// DemoGL will ignore the failure of an effect's Init() method
#define DGL_AIFF_REMOVEEFFECT				3		// DemoGL will remove the effect which Init() method failed.

// Resolution constants. Not renumbered for backward compatibility.
#define RES800x600							1		// DEPRECATED FROM v1.3
#define RES640x480							2		// DEPRECATED FROM v1.3
#define RES1024x768							3		// DEPRECATED FROM v1.3

#define DGL_RES_800x600						RES800x600
#define DGL_RES_640x480						RES640x480
#define DGL_RES_1024x768					RES1024x768
#define DGL_RES_320x240						4
#define DGL_RES_400x300						5
#define DGL_RES_512x384						6
#define DGL_RES_960x720						7
#define DGL_RES_1152x864					8
#define DGL_RES_1280x1024					9
#define DGL_RES_1600x1200					10
#define DGL_RES_2048x1536					11

// First private message number for your application.
// WM_USER + 0x100 are reserved. Define your own private messages as a number higher than DGL_MSGS_PRIVMSGLOW
#define APP_PRIVMESGLOW						0x500		// DEPRECATED FROM v1.3
#define DGL_MSGS_PRIVMSGLOW					APP_PRIVMESGLOW

// Definition of message that will be sent when a musicsync is triggered. All registered messagehandlers will
// receive the message.
#define WM_DEMOGL_SOUNDSYSTEMSYNC			0x409

// For SSoundSystemOptionDat structure
#define DGL_SS_MAXLENDEVICESTRING			512
#define DGL_SS_MAXDEVICES					32

// SoundSystem supported elementtypes. Pass one of these to the several SoundSystem methods as 'iElementType'
#define DGL_SS_MP3ELEMENT					1
#define DGL_SS_MODELEMENT					2
#define DGL_SS_SAMPLEELEMENT				3

// Soundsystem specific defines for attribute setting (with the iWhat parameter: 'OR' one or more of these flags together)
#define DGL_SS_SA_FREQUENCY					1
#define DGL_SS_SA_VOLUME					2
#define DGL_SS_SA_PAN						4

// Soundsystem returnvalues (errorcodes)
#define DGL_SS_UNKNOWNELEMENT				-1
#define DGL_SS_UNKNOWNCHANNEL				-1
#define DGL_SS_ERR_COULDNTLOAD				-1
#define	DGL_SS_ERR_ILLEGALELEMENTID			-2
#define	DGL_SS_ERR_OPERATIONFAILED			-3
#define	DGL_SS_ERR_OPERATIONNOTSUPPORTED	-4
#define DGL_SS_ERR_ELEMENTSTOREFULL			-5
#define DGL_SS_ERR_ILLEGALELEMENTTYPE		-6
#define DGL_SS_ERR_INITOFELEMENTFAILED		-7
#define DGL_SS_ERR_COULDNTCREATESYNC		-8
#define DGL_SS_ERR_COULDNTATTACHTOCHANNEL	-9
#define DGL_SS_ERR_COULDNTCREATENEWELEMENT	-10
#define DGL_SS_ERR_CHANNELNOTINUSE			-1000
#define DGL_SS_ERR_CHANNELACTIONFAILED		-1001
#define DGL_SS_ERR_CHANNELSTOREFULL			-1002
#define DGL_SS_ERR_ILLEGALCHANNELID			-1003

// Stringlength definition for SStartupDat
#define DGL_SD_STRINGLENGTH					256

/////////////////////////////////////////
// VariableFlags for DEMOGL_SetVariable
/////////////////////////////////////////
// Defaults
#define DGL_VF_VSYNC						1
#define	DGL_VF_FILESRC						2
#define	DGL_VF_SRCDIR						3
#define	DGL_VF_DATAFILENAME					4
#define	DGL_VF_SCRIPTFILENAME				5
#define	DGL_VF_SHOWFPS						6
#define	DGL_VF_NEVERSHOWFPS					7
#define	DGL_VF_NEVERSHOWCONSOLE				8
#define	DGL_VF_SHOWDEBUGINFO				9
#define	DGL_VF_NEVERSHOWDEBUGINFO			10
#define	DGL_VF_NEVERRESCALETEXTURES			11
#define DGL_VF_QUICKBOOT					12
#define DGL_VF_USERENDERFRAMEEX				13
#define DGL_VF_INITFAILURERESPONSE			14

// Other flags settable
#define DGL_VF_SWAPBUFFERS					1000

// loading progressbar types:
#define	DGL_CNTRL_PGB_LINEAR				1
#define DGL_CNTRL_PGB_BLOCKS_2D				2

// Texture dimensions:
#define DGL_TEXTUREDIMENSION_1D				1
#define DGL_TEXTUREDIMENSION_2D				2
#define DGL_TEXTUREDIMENSION_3D				3		// currently not supported due to lack of opengl32.dll with OGL 1.2 functionality.

// Application run types. Supply to DEMOGL_AppRun()
#define RUNTYPE_UNKNOWN						0
#define RUNTYPE_NORMAL						1		// default
#define RUNTYPE_NODIALOG					2
#define RUNTYPE_SAVER_CONFIG				3
#define RUNTYPE_SAVER_NORMAL				4
#define RUNTYPE_SAVER_PREVIEW				5
#define RUNTYPE_SAVER_PASSWORD				6		// not supported. Application will fail. Provided for completeness
													// (is in Plus! and win9x only. No password protection) DEMOGL_AppRun()
													// will exit immediately when called with this saver exec type.

// Screensaver Execution types. Returned by DEMOGL_ParseScreenSaverCL in the struct SScreenSaverCLParams.
// Use that struct to set the RUNTYPE when your application should run as a screensaver, thus RUNTYPE_SAVER_*
#define DGL_SSAVERET_UNKNOWN				0
#define DGL_SSAVERET_CONFIG					1
#define	DGL_SSAVERET_NORMAL					2
#define DGL_SSAVERET_PREVIEW				3
#define DGL_SSAVERET_PASSWORD				4		// not supported.

// Cubemap side flags, needed for dynamic cubemap generation functions.
#define DGL_CUBEMAPSIDE_POSX				1
#define DGL_CUBEMAPSIDE_NEGX				2
#define DGL_CUBEMAPSIDE_POSY				3
#define DGL_CUBEMAPSIDE_NEGY				4
#define DGL_CUBEMAPSIDE_POSZ				5
#define DGL_CUBEMAPSIDE_NEGZ				6

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
// 	OBSOLETE IN V1.3. DON'T USE THESE IN NEW APPLICATIONS. USE _SOUNDSYSTEM INSTEAD.
////////////////////////////////////////
// MusicSubSystemTypes.
#define	MP3SUBSYSTEM						1		// OBSOLETE IN V1.3
#define MODSUBSYSTEM						2		// OBSOLETE IN V1.3
////////////////////////////////////////
// 	END OF OBSOLETE IN V1.3
////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                        DEMOGL API TYPEDEFS. DO NOT ALTER.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// create a byte type
#ifndef byte
typedef unsigned char byte;
#endif //byte

// Purpose: soundsystem device structure. Needed for storing info about this device
// in the SSoundSystemOptionDat structure
typedef struct
{
	bool					m_b3DSound_A3DPossible;
	bool					m_b3DSound_EAXPossible;
	char					m_sDescription[DGL_SS_MAXLENDEVICESTRING];	
} SSoundDeviceOptions;


// Purpose: Soundsystem possibilities structure. Filled by a special routine
// and used by startup dialog or custom user dialog to be able to enable/disable
// Dialogobjects
typedef struct
{
	bool					m_bSound_Possible;
	int						m_iAmDevices;
	SSoundDeviceOptions		m_arrssdoDevices[DGL_SS_MAXDEVICES];
} SSoundSystemOptionDat;


// Purpose: 3D vector struct for 3D sound attributes
typedef struct
{
	float					m_fX;
	float					m_fY;
	float					m_fZ;
} SSoundSystem3DVector;


// Purpose: 3D position information structure of soundchannel
typedef struct
{
	SSoundSystem3DVector	m_ssvPosition;						// 3D worldposition of soundchannel
	SSoundSystem3DVector	m_ssvOrientation;					// 3D orientation of the soundchannel
	SSoundSystem3DVector	m_ssvVelocity;						// 3D velocity of the soundchannel. does not affect m_ssvPosition.
} SSoundChannel3DPosition;


// Purpose: 3D position information structure of listener
typedef struct
{
	SSoundSystem3DVector	m_ssvPosition;						// 3D worldposition of listener
	SSoundSystem3DVector	m_ssvVelocity;						// 3D velocity of the listener. does not affect m_ssvPosition.
	SSoundSystem3DVector	m_ssvFront;							// 3D vector poiting towards the same direction the listener's front is pointing to
	SSoundSystem3DVector	m_ssvTop;							// 3D vector poiting towards the same direction the listener's top is pointing to
} SListener3DPosition;


// Purpose: soundelement 3D attributes structure. Filled by user and passed to several 3D soundsystem functions.
// See DemoGL_Bass.h for BASS_3DMODE_* flag values.
typedef struct 
{
    DWORD					m_dw3DMode;							// BASS_3DMODE_* flags
    float					m_fMinimalDistance;					// Distance between sound element and listener
    float					m_fMaximalDistance;					// Distance between sound element and listerer
    int						m_iInsideProjectionConeAngle;		// e [0-360]. 0 == no cone, 360 == sphere
    int						m_iOutsideProjectionConeAngle;		// e [0-360]. 0 == no cone, 360 == sphere
    int						m_iDeltaVolumeOutsideOuterProjectionCone;	// e [0-100]. 0 == silent, 100 == as in inside Cone.
} SSoundElement3DAttributes; 


// Purpose: startup parameters structure. The contents of this structure is used to fill in the
// possible options of the startup dialog or use them for application execution when a DEMOGL_AppRun() variant is 
// used that doesnt show a startupdialog but just picks the settings supplied in this structure.
typedef struct
{
	bool					m_bFullScreen;			// false: windowed mode is used.
	int						m_iResolution;			// one of the resolution constants.
	bool					m_bDQ32bit;				// false: 16bit colorbuffer depth is used
	bool					m_bTQ32bit;				// false: 16bit texturedata hint is passed to OpenGL
	bool					m_bSound;				// false: no sound will be played. no soundsystem will be initialized
	char					m_sDemoName[DGL_SD_STRINGLENGTH];
	char					m_sReleasedBy[DGL_SD_STRINGLENGTH];
	bool					m_bSaveInRegistry;		// true: save dialog settings in registry
	bool					m_bRescaleTextures;		// true: rescale textures to fit maximum size. Needed for voodoo3 cards.
	bool					m_bSS_3DSound;			// true: 3D calculations are processed on 3D enabled sound elements.
	int						m_iSS_SoundDevice;		// device nr. See BASS documentation.
	bool					m_bSS_Stereo;			// false: mono
	bool					m_bSS_16bit;			// false: 8 bit
	bool					m_bSS_LowQuality;		// false: 44.1khz
	bool					m_bSS_A3D;				// false: no A3D support.
	bool					m_bSS_EAX;				// false: no EAX support.
	int						m_iSS_OverallVolume;	// the value of the overal volume of the output. 0-100.
} SStartupDat;


// Purpose: screensaver type struct. when the application should run as a RUNTYPE_SAVER_* variant the commandline should
// be parsed and on the commandline are parameters passed that will flag which variant of the screensaver type execution 
// should be ran: config dialog, preview on desktop properties dialog, fullscreen preview, normal saver execution.
// DEMOGL_ParseScreenSaverCL() will return a pointer to this structure, which will inform the caller how to call 
// DEMOGL_AppRun() correctly.
typedef struct
{
	HWND	hWnd;
	bool	bInitialized;
	int		iSaverExecType;
} SScreenSaverCLParams;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                        DEMOGL API FUNCTIONS.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEMOGL_AppEnd:
DLLIMPORT	void	DEMOGL_AppEnd(void);

// DEMOGL_AppRun:
DLLIMPORT	int		DEMOGL_AppRun(const HINSTANCE hInstance, const SStartupDat *psdStartupDat,
						const char *pszWindowTitle, const bool bUseCrypting, const byte byKey1, 
						const byte byKey2, const int iRunType);

// DEMOGL_ConsoleLogLine:
DLLIMPORT	void	DEMOGL_ConsoleLogLine(const bool bScroll, const char *pszLine);
DLLIMPORT	void	DEMOGL_ConsoleLogLineV(const bool bScroll, const char *pszFormat, ...);

// DEMOGL_ConsoleSetAppearance:
DLLIMPORT	int		DEMOGL_ConsoleSetAppearance(const int iAmCharsPerLine, const int iAmLinesInBuffer,
						const int iAmLinesOnScreen, const int iAmLinesInDebugInfo);

// DEMOGL_EffectRegister: 
DLLIMPORT	void	DEMOGL_EffectRegister(CEffect * const pEffectObject, const char *pszEffectName);

// DEMOGL_ExecuteCommand:
DLLIMPORT	void	DEMOGL_ExecuteCommand(const char *pszCommand);

// DEMOGL_ExtensionCheckIfAvailable:
					// will check internal database
DLLIMPORT	bool	DEMOGL_ExtensionCheckIfAvailable(const int iExtension);
					// will check with string from driver
DLLIMPORT	bool	DEMOGL_ExtensionCheckIfAvailable(const char *pszExtensionName);	

// DEMOGL_FileFree:
DLLIMPORT	void	DEMOGL_FileFree(const byte *pbyFile);

// DEMOGL_FileGetLength:
DLLIMPORT	long	DEMOGL_FileGetLength(const char *pszFilename);

// DEMOGL_FileLoad:
DLLIMPORT	byte	*DEMOGL_FileLoad(const char *pszFilename);

// DEMOGL_GetVersion:
DLLIMPORT	void	DEMOGL_GetVersion(const float *fVersion, const long *lBuildnr);

// DEMOGL_GetWindowSpecificHandles:
DLLIMPORT	void	DEMOGL_GetWindowSpecificHandles(HWND * const hWndMainWindow, HDC * const hDCMainWindow);

// DEMOGL_LoadingPBEnable:
DLLIMPORT	void	DEMOGL_LoadingPBEnable(const int iType, const float fWidth, const float fHeight,
						const float fBottomLeftX, const float fBottomLeftY);

// DEMOGL_LoadingPBSetAppearance:
DLLIMPORT	void	DEMOGL_LoadingPBSetAppearance(const bool bHorizontal, const bool bBlend, 
						const GLenum iBlendSrc, const GLenum iBlendDst, const float fOpacity,
						const float fBGColor_R, const float fBGColor_G,	const float fBGColor_B, 
						const float fBarColor_R, const float fBarColor_G, const float fBarColor_B);

// DEMOGL_LoadingSplashEnable:
DLLIMPORT	void	DEMOGL_LoadingSplashEnable(const char *pszSplashScreenTexture, const int iFileType, 
						const float fOpacity);

// DEMOGL_MessageHandlerDisable:
DLLIMPORT	void	DEMOGL_MessageHandlerDisable(const char *pszEffectName);

// DEMOGL_MessageHandlerEnable:
DLLIMPORT	void	DEMOGL_MessageHandlerEnable(const char *pszEffectName);

// DEMOGL_ParseScreenSaverCL:
DLLIMPORT	SScreenSaverCLParams	*DEMOGL_ParseScreenSaverCL(const LPSTR lpCmdLine);

// DEMOGL_SetVariable:
DLLIMPORT	void	DEMOGL_SetVariable(const int iFlag, const int iValue);
DLLIMPORT	void	DEMOGL_SetVariable(const int iFlag, const bool bValue);
DLLIMPORT	void	DEMOGL_SetVariable(const int iFlag, const char *pszValue);

// DEMOGL_SoundSystemChannelScriptID2ChannelCodeID:
DLLIMPORT	int		DEMOGL_SoundSystemChannelScriptID2ChannelCodeID(const char *pszChannelScriptID);

// DEMOGL_SoundSystemContinueAll:
DLLIMPORT	int		DEMOGL_SoundSystemContinueAll(void);

// DEMOGL_SoundSystemContinueChannel:
DLLIMPORT	int		DEMOGL_SoundSystemContinueChannel(const int iChannelCodeID);

// DEMOGL_SoundSystemDetermineOptionsData:
DLLIMPORT	SSoundSystemOptionDat	*DEMOGL_SoundSystemDetermineOptionsData(void);

// DEMOGL_SoundSystemElementScriptID2ElementCodeID:
DLLIMPORT	int		DEMOGL_SoundSystemElementScriptID2ElementCodeID(const char *pszElementScriptID);

// DEMOGL_SoundSystemFreeSoundElement:
DLLIMPORT	int		DEMOGL_SoundSystemFreeElement(const int iElementCodeID, const int iElementType);

// DEMOGL_SoundSystemLoadSoundElement:
DLLIMPORT	int		DEMOGL_SoundSystemLoadElement(const char *pszFilename, const int iElementType,
						const char *pszElementScriptID, const bool bUses3D);

// DEMOGL_SoundSystemPauseAll:
DLLIMPORT	int		DEMOGL_SoundSystemPauseAll(void);

// DEMOGL_SoundSystemPauseChannel:
DLLIMPORT	int		DEMOGL_SoundSystemPauseChannel(const int iChannelCodeID);

// DEMOGL_SoundSystemSeekInChannel:
DLLIMPORT	int		DEMOGL_SoundSystemSeekInChannel(const int iChannelCodeID, const long lStartPos);

// DEMOGL_SoundSystemSet3DFactors:
DLLIMPORT	int		DEMOGL_SoundSystemSet3DFactors(const float fDistanceFactor, const float fRollOffFactor, 
						const float fDopplerFactor);

// DEMOGL_SoundSystemSetChannel3DAttributes:
DLLIMPORT	int		DEMOGL_SoundSystemSetChannel3DAttributes(const int iChannelCodeID, 
						SSoundElement3DAttributes * const p3DAttributes);

// DEMOGL_SoundSystemSetChannel3DPosition:
DLLIMPORT	int		DEMOGL_SoundSystemSetChannel3DPosition(const int iChannelCodeID, 
						SSoundChannel3DPosition * const p3DPosition);

// DEMOGL_SoundSystemSetChannelAttributes:
DLLIMPORT	int		DEMOGL_SoundSystemSetChannelAttributes(const int iChannelCodeID, const int iVolume, 
						const int iPan, const int iFrequency, const int iWhat);

// DEMOGL_SoundSystemSetEAXEnv:
DLLIMPORT	int		DEMOGL_SoundSystemSetEAXEnv(const int iEAXEnvironment, const float fReverbVolume, 
						const float fDecay, const float fDamp);

// DEMOGL_SoundSystemSetEAXMixChannel:
DLLIMPORT	int		DEMOGL_SoundSystemSetEAXMixChannel(const int iChannelCodeID, const float fMixValue);

// DEMOGL_SoundSystemSetElement3DAttributes:
DLLIMPORT	int		DEMOGL_SoundSystemSetElement3DAttributes(const int iElementCodeID, const int iElementType,
						SSoundElement3DAttributes * const p3DAttributes);

// DEMOGL_SoundSystemSetListener3DPosition:
DLLIMPORT	int		DEMOGL_SoundSystemSetListener3DPosition(SListener3DPosition * const p3DPosition);

// DEMOGL_SoundSystemSetVolume:
DLLIMPORT	int		DEMOGL_SoundSystemSetVolume(const int iVolume);

// DEMOGL_SoundSystemStartElement:
DLLIMPORT	int		DEMOGL_SoundSystemStartElement(const int iElementCodeID, const int iElementType, 
						const long lStartPos, const bool bLoop,	const int iVolume, const int iPan, 
						const int iFrequency, SSoundChannel3DPosition * const p3DPosition, 
						const char *pszChannelScriptID);

// DEMOGL_SoundSystemStopAll:
DLLIMPORT	int		DEMOGL_SoundSystemStopAll(void);

// DEMOGL_SoundSystemStopChannel:
DLLIMPORT	int		DEMOGL_SoundSystemStopChannel(const int iChannelCodeID);

// DEMOGL_SoundSystemSyncCreate:
DLLIMPORT	int		DEMOGL_SoundSystemSyncCreate(const int iChannelCodeID, const DWORD dwSyncType, 
						const DWORD dwSyncParam, const DWORD dwSyncFlag);

// DEMOGL_SoundSystemSyncDelete:
DLLIMPORT	int		DEMOGL_SoundSystemSyncDelete(const int iChannelCodeID, const int iSyncID);

// DEMOGL_SoundSystemSyncDeleteAll:
DLLIMPORT	int		DEMOGL_SoundSystemSyncDeleteAll(const int iChannelCodeID);

// DEMOGL_TextureCreateFromBuffer:
DLLIMPORT	GLuint	DEMOGL_TextureCreateFromBuffer(const int iWidth, const int iHeight, const int iDepth,
						const byte *pbyBuffer, const char *pszIdentName, const int iBorder, const GLint iWrapS, 
						const GLint iWrapT, const GLint iMinFilter, const GLint iMagFilter, const bool bMipMaps,	
						const GLint iTexUploadHint, const int iDimensions);

// DEMOGL_TextureCreateFromFBRegion:
DLLIMPORT	GLuint	DEMOGL_TextureCreateFromFBRegion(const int iX, const int iY, const int iWidth, const int iHeight, 
						const char *pszIdentName, const int iBorder, const GLint iWrapS, const GLint iWrapT, 
						const GLint iMinFilter, const GLint iMagFilter, const GLint iTexUploadHint, 
						const int iDimensions);

// DEMOGL_TextureCubeMapCreateFromFBRegion
DLLIMPORT	GLuint	DEMOGL_TextureCubeMapCreateDynamic(const int iWidth, const int iHeight, const char *pszIdentName, 
						const int iBorder, const GLint iMinFilter, const GLint iMagFilter, const GLint iTexUploadHint);

// DEMOGL_TextureCubeMapLoad
DLLIMPORT	GLuint	DEMOGL_TextureCubeMapLoad(const char *pszFilename, const int iFileType, const GLint iMinFilter, 
						const GLint iMagFilter, const bool bMipMaps, const bool bCreateAlphaFromColor, const int iBorder, 
						const GLint iTexUploadHint);

// DEMOGL_TextureCubeMapUpdateWithFBRegion
DLLIMPORT	int		DEMOGL_TextureCubeMapUpdateWithFBRegion(const GLuint iTextureID, const int iCubeSide, const int iX, 
						const int iY, const int iWidth, const int iHeight, const int iXDest, const int iYDest);

// DEMOGL_TextureDelete:
DLLIMPORT	void	DEMOGL_TextureDelete(const GLuint iTextureID);

// DEMOGL_TextureGetData:
DLLIMPORT	byte	*DEMOGL_TextureGetData(const GLuint iTextureID, int * const iWidth, int * const iHeight, 
						int * const iDepth, int * const iBitsPerPixel, int * const iDimensions, 
						bool * const bHasAlpha);

// DEMOGL_TextureLoad:
DLLIMPORT	GLuint	DEMOGL_TextureLoad(const char *pszFilename, const int iFileType, const int iDepth, const GLint iWrapS,
						const GLint iWrapT, const GLint iMinFilter, const GLint iMagFilter, const bool bMipMaps, 
						const bool bCreateAlphaFromColor, const int iBorder, const GLint iTexUploadHint, 
						const int iDimensions);

// DEMOGL_TextureUnUpload:
DLLIMPORT	void	DEMOGL_TextureUnUpload(const GLuint iTextureID);

// DEMOGL_TextureUpdateWithFBRegion:
DLLIMPORT	int		DEMOGL_TextureUpdateWithFBRegion(const int iX, const int iY, const int iWidth, const int iHeight, 
						const int iXDest, const int iYDest, const GLuint iTextureID, const int iDimensions);

// DEMOGL_TextureUpload:
DLLIMPORT	int		DEMOGL_TextureUpload(const GLuint iTextureID);


/////////////////////////////////[IMPORTANT NOTICE]/////////////////////////////////////////////////////
// The functions below are marked 'deprecated' starting with version 1.3 but still supported for backwards compatibility
// They are just wrappers around the new DEMOGL_ versions so they don't take much DLL space.
//
//				    >>> Please use the new DEMOGL_* functions in your new code <<<
//
/////////////////////////////////[IMPORTANT NOTICE]/////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
	DLLIMPORT	void	RunApp(HINSTANCE hInstance, const char *sWindowTitle, bool bUseCrypting, byte byKey1, byte byKey2);		// Deprecated from v1.3
	DLLIMPORT	void	OverruleDefaults(bool bVSYNC, int iFileSRC, const char *sSRCDir, const char *sDataFilename, 
								const char *sScriptFilename, bool bShowFPS, bool bNeverShowFPS, bool bNeverShowConsole, 
								bool bShowDebugInfo, bool bNeverShowDebugInfo, int iMusicSys);		// Deprecated from v1.3
	DLLIMPORT	void	EndApp(void);		// Deprecated from v1.3
	DLLIMPORT	void	RegisterEffect(CEffect *pEffectObject, const char *sEffectName);		// Deprecated from v1.3
	DLLIMPORT	void	DeleteTexture(int iTextureName);		// Deprecated from v1.3
	DLLIMPORT	void	UnUploadTexture(int iTexureName);		// Deprecated from v1.3
	DLLIMPORT	GLuint	LoadTexture(const char *sFilename, int iFileType, GLint iWrapS, GLint iWrapT, GLint iMinFilter, 
								GLint iMagFilter, bool bMipMaps, bool bCreateAlphaFromColor, int iBorder, GLint iTexUploadHint);		// Deprecated from v1.3
	DLLIMPORT	int		UploadTexture(int iTexureName);		// Deprecated from v1.3
	DLLIMPORT	void	SetbSwapBuffers(bool bYN);		// Deprecated from v1.3
	DLLIMPORT	long	GetLengthOfFile(const char *sFilename);		// Deprecated from v1.3
	DLLIMPORT	bool	IsExtensionAvailable(int iExtension);		// Deprecated from v1.3
	DLLIMPORT	GLuint	CreateTextureFromBuffer(int iWidth, int iHeight, byte *pBuffer, const char *sIdentName, int iBorder, 
								GLint iWrapS, GLint iWrapT, GLint iMinFilter, GLint iMagFilter, bool bMipMaps, GLint iTexUploadHint);		// Deprecated from v1.3
	DLLIMPORT	GLuint	CreateTextureFromFBRegion(int iX, int iY, int iWidth, int iHeight, const char *sIdentName, int iBorder, 
								GLint iWrapS, GLint iWrapT, GLint iMinFilter, GLint iMagFilter, GLint iTexUploadHint);		// Deprecated from v1.3
	DLLIMPORT	void	LogLineToConsole(const char *sLine, bool bScroll);		// Deprecated from v1.3
	DLLIMPORT	byte	*LoadFile(const char *sFilename);		// Deprecated from v1.3
	DLLIMPORT	void	SetStartupDialogVars(const char *sDemoName, const char *sCreators, bool bFullScreen, int iResolution, 
								bool bDisplay32bpp, bool bTexture32bpp, bool bSound);		// Deprecated from v1.3
	DLLIMPORT	void	GetDemoGLVersion(float *fVersion, long *lBuildnr);		// Deprecated from v1.3
	DLLIMPORT	void	EnableMessageHandler(const char *sEffectName);		// Deprecated from v1.3
	DLLIMPORT	void	DisableMessageHandler(const char *sEffectName);		// Deprecated from v1.3
	DLLIMPORT	void	EnableLoadingSplash(const char *sSplashScreenTexture, int iFileType, float fOpacity);		// Deprecated from v1.3
	DLLIMPORT	void	FreeFile(byte *byFile);		// Deprecated from v1.3
	DLLIMPORT	void	ExecuteCommand(const char *sCommand);		// Deprecated from v1.3
#ifdef __cplusplus
}
#endif // __cplusplus
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	// _DEMOGL_DLL_H