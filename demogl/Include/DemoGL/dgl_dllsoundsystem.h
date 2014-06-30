//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllsoundsystem.h
// PURPOSE: definition of the CSoundSystem class, the class for the universal sound producing object
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// Definition of the soundsystem class. DemoGL uses a universal soundsystem
// object which uses a library to to the actual music and sound actions.
//
// BASS soundsystem is used as soundproducing library.
//
// This is a new element in DemoGL v1.3. It's been written with DemoGL 2.0 in
// mind, and therefor more C++ than other parts of the DemoGL sourcecode. Also
// the placement of returnvalues in HERE is different from the rest of the DemoGL
// codedesign. Also the constants have a new namingscheme (with the DGL_ prefix),
// however are still done with #defines.
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
// v1.3: Added to codebase.
// v1.2: -- 
// v1.1: --
//////////////////////////////////////////////////////////////////////

#ifndef _DGL_DLLSOUNDSYSTEM_H
#define _DGL_DLLSOUNDSYSTEM_H

#include "Misc\CMP3Info.h"

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////

#define DGL_SS_AMLOADABLESOUNDELEMENTS		4096			// "4K should be enough for everybody" -- Otis/Infuse Project
#define DGL_SS_AMCHANNELS					2048			// maximum amount of concurrent playing sound elements.

#define DGL_SS_UNKNOWNELEMENT				-1
#define DGL_SS_UNKNOWNCHANNEL				-1
#define DGL_SS_MP3ELEMENT					1
#define DGL_SS_MODELEMENT					2
#define DGL_SS_SAMPLEELEMENT				3

#define DGL_SS_MAXLENDEVICESTRING			512
#define DGL_SS_MAXDEVICES					32
#define DGL_SS_MAXMP3SYNCS					0
#define DGL_SS_MAXMODSYNCS					256
#define DGL_SS_MAXSAMPLESYNCS				0

// Errorcodes
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

// SetAttributes flags WHAT to set
#define DGL_SS_SA_FREQUENCY					1
#define DGL_SS_SA_VOLUME					2
#define DGL_SS_SA_PAN						4


////////////////////////////////////////////////////////////////////////
//                          
//						TYPEDEFS
//
////////////////////////////////////////////////////////////////////////

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


// Purpose: Soundsystem sync structure. Per sync one structure is filled. 
typedef struct
{
	HSYNC					m_hBassSyncHandle;
	DWORD					m_dwSyncType;
	DWORD					m_dwSyncParam;
	DWORD					m_dwSyncFlag;
} SSoundSystemSync;


// Purpose: 3D vector structure for 3D sound attributes
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


// Purpose: soundelement 3D attributes structure. Filled by user.
typedef struct 
{
    DWORD					m_dw3DMode;							// BASS_3DMODE_* flags
    float					m_fMinimalDistance;					// Distance between sound element and listener
    float					m_fMaximalDistance;					// Distance between sound element and listerer
    int						m_iInsideProjectionConeAngle;		// e [0-360]. 0 == no cone, 360 == sphere
    int						m_iOutsideProjectionConeAngle;		// e [0-360]. 0 == no cone, 360 == sphere
    int						m_iDeltaVolumeOutsideOuterProjectionCone;	// e [0-100]. 0 == silent, 100 == as in inside Cone.
} SSoundElement3DAttributes; 


// Purpose: channel handle union for CSoundChannel object
typedef union
{
	HCHANNEL				m_hcSample;
	HMUSIC					m_hmMod;
	HSTREAM					m_hsMP3;
} UChannelHandle;


////////////////////////////////////////////////////////////////////////
//                          
//						CLASS DEFINITIONS
//
////////////////////////////////////////////////////////////////////////

// forward declaration of CSoundElement class.
class CSoundElement;

////////////////////////////////////////////////////
// Purpose: Channel class. When a sound element is bound to a BASS channel, a channel class
// instance (object) is created and filled with the BASS channel related information. Users refer to
// that object to affect the playback of the sound element. This way more than 1 play instance of a sample
// element is possible. (other soundelements like MP3 and MOD, just support 1 channel, but it's implemented
// this way to keep everything uniform.
class CSoundChannel
{
public:
	// Methods
								CSoundChannel(void);
	virtual						~CSoundChannel(void);
	int							AttachToElement(UChannelHandle *pchHandle, int iElementType, char *pszChannelScriptID, 
										CSoundElement *pseParentElement, bool bUses3D);
	int							Continue(void);
	int							CreateSync(DWORD dwSyncType, DWORD dwSyncParam, DWORD dwSyncFlag);
	int							DeleteSync(int iSyncID);
	int							DeleteAllSyncs(void);
	int							DetachFromElement(void);
	void						ExecuteSync(HSYNC hSyncHandle, DWORD dwSyncData);
	int							GetChannelCodeID(void);
	char						*GetChannelScriptID(void);
	bool						GetInUse(void);
	int							GetParentElementCodeID(void);
	int							Pause(void);
	int							Seek(long lStartPos);
	int							SetAttributes(int iVolume, int iPan, int iFrequency, int iWhat);
	int							Set3DAttributes(SSoundElement3DAttributes *p3DAttributes);	
	int							Set3DPosition(SSoundChannel3DPosition *p3DPosition);
	void						SetChannelCodeID(int iChannelCodeID);
	int							SetEAXMix(float fMixValue);
	void						SetInUse(bool bYN);
	int							Stop(void);

private:
	// Methods
	void						InitMembers(void);

	// Members
	CSoundElement				*m_pseParentElement;
	int							m_iParentElementCodeID;
	int							m_iParentElementType;
	UChannelHandle				m_chHandle;
	CStdString					m_sChannelScriptID;
	int							m_iChannelCodeID;
	bool						m_bInUse;
	bool						m_b3DEnabled;
	SSoundSystemSync			*m_pssSyncStore;				// array of syncelements. amount depends on parentelementtype.
};


////////////////////////////////////////////////////
// Purpose: class of a sound element in demogl. Baseclass for different element types
class CSoundElement
{
public:
	// Methods
								CSoundElement();
	virtual						~CSoundElement();
	void						ClearElement(void);
	void						DecReferences(void);
	void						IncReferences(void);
	SSoundElement3DAttributes	*Get3DAttributes(void);
	DWORD						GetBassFlags(void);
	int							GetElementCodeID(void);
	byte						*GetElementData(void);
	long						GetElementLength(void);
	char						*GetElementScriptID(void);
	int							GetElementType(void);
	int							GetReferences(void);
	bool						GetUses3D(void);
	int							Set3DAttributes(SSoundElement3DAttributes *p3DAttributes);	
	void						SetElementData(byte *pElementBuffer, long lElementLength, DWORD dwBassFlags, 
										int iType, int iElementCodeID, char *pszElementScriptID, bool bUses3D);

	virtual	DWORD				CalcSeekValue(long lStartPos, UChannelHandle *pchHandle);
	virtual int					Init(void);
	virtual int					Play(bool bFlush, bool bLoop, CSoundChannel *pscTargetChannel, char *pszChannelScriptID);
	virtual	void				SetStartPos(DWORD dwStartPos);

protected:
	// Methods
	void						FreeElementData(void);
	void						InitMembers(void);

private:
	// Members
	byte						*m_pbyElementDataBuffer;
	long						m_lElementLength;
	DWORD						m_dwBassFlags;
	int							m_iElementCodeID;
	CStdString					m_sElementScriptID;
	int							m_iReferences;
	int							m_iType;
	bool						m_bUses3D;
	SSoundElement3DAttributes	m_sea3DAttributes;
};

////////////////////////////////////////////////
// Purpose: MP3 element
class CMP3Element:public CSoundElement
{
public:
								CMP3Element();
	virtual						~CMP3Element();
	DWORD						CalcSeekValue(long lStartPos, UChannelHandle *pchHandle);
	void						FreeElementData(void);
	HSTREAM						GetBassHandle(void);
	int							Init(void);
	int							Play(bool bFlush, bool bLoop, CSoundChannel *pscTargetChannel, char *pszChannelScriptID);
	void						SetBassHandle(HSTREAM hsHandle);

private:
	HSTREAM						m_hsBassHandle;
	CMP3Info					m_miMP3Info;
	bool						m_bPlaying;
};


////////////////////////////////////////////////
// Purpose: MOD element
class CMODElement:public CSoundElement
{
public:
								CMODElement();
	virtual						~CMODElement();
	void						FreeElementData(void);
	HMUSIC						GetBassHandle(void);
	int							Play(bool bFlush, bool bLoop, CSoundChannel *pscTargetChannel, char *pszChannelScriptID);
	void						SetBassHandle(HMUSIC hsHandle);
	void						SetStartPos(DWORD dwStartPos);

private:
	HMUSIC						m_hmBassHandle;
	DWORD						m_dwStartPos;
};


////////////////////////////////////////////////
// Purpose: Sample element
class CSampleElement:public CSoundElement
{
public:
								CSampleElement();
	virtual						~CSampleElement();
	void						FreeElementData(void);
	HSAMPLE						GetBassHandle(void);
	int							Play(bool bFlush, bool bLoop, CSoundChannel *pscTargetChannel, char *pszChannelScriptID);
	void						SetBassHandle(HSAMPLE hsHandle);
	void						SetStartPos(DWORD dwStartPos);

private:
	HSAMPLE						m_hsBassHandle;
	DWORD						m_dwStartPos;
};


////////////////////////////////////////////////
// Purpose: Sound system class. Uses BASS to produce sound
class CSoundSystem  
{
public:
	// Methods
								CSoundSystem();
	virtual						~CSoundSystem();
	void						AddBassSystemFlags(DWORD dwFlags);
	bool						CheckIfValidChannel(char *pszChannelScriptID);
	int							CreateSyncChannel(int iChannelCodeID, DWORD dwSyncType, DWORD dwSyncParam, DWORD dwSyncFlag);
	int							ConvertElementScriptID2ElementCodeID(char *pszElementScriptID);
	int							ConvertChannelScriptID2ChannelCodeID(char *pszChannelScriptID);
	int							DeleteSyncChannel(int iChannelCodeID, int iSyncID);
	int							DeleteAllSyncsChannel(int iChannelCodeID);
	void						ExecuteSyncChannel(int iChannelCodeID, HSYNC hSyncHandle, DWORD dwSyncData);
	int							ContinueAll(void);
	int							ContinueChannel(int iChannelCodeID);
	int							FreeSoundElement(int iElementCodeID, int iElementType, bool bAlwaysFree);
	int							FreeSystem(void);
	bool						Get3DSoundEnabled(void);
	bool						GetA3DEnabled(void);
	DWORD						GetBassSystemFlags(void);
	bool						GetEAXEnabled(void);
	int							InitSystem(void);
	int							LoadSoundElement(char *pszFilename, int iElementType,char *pszElementScriptID, bool bUses3D);
	int							PauseAll(void);
	int							PauseChannel(int iChannelCodeID);
	void						RemoveBassSystemFlags(DWORD dwFlags);
	int							SeekInChannel(int iChannelCodeID, long lStartPos);
	int							Set3DFactors(float fDistanceFactor, float fRollOffFactor, float fDopplerFactor);
	void						Set3DSoundEnabled(bool bYN);
	void						SetA3DEnabled(bool bYN);
	void						SetBassSystemFlags(DWORD dwFlags);
	int							SetChannel3DAttributes(int iChannelCodeID, SSoundElement3DAttributes *p3DAttributes);	
	int							SetChannel3DPosition(int iChannelCodeID, SSoundChannel3DPosition *p3DPosition);	
	int							SetChannelAttributes(int iChannelCodeID, int iVolume, int iPan, int iFrequency, int iWhat);
	void						SetDeviceAndFrequency(int iDevice, DWORD dwFrequency);
	void						SetEAXEnabled(bool bYN);
	int							SetEAXEnv(int iEAXEnvironment, float fReverbVolume, float fDecay, float fDamp);
	int							SetEAXMixChannel(int iChannelCodeID, float fMixValue);
	int							SetElement3DAttributes(int iElementCodeID, int iElementType, SSoundElement3DAttributes *p3DAttributes);	
	int							SetListener3DPosition(SListener3DPosition *p3DPosition);	
	int							SetVolume(int iVolume);
	int							StartElement(int iElementCodeID, int iElementType, long lStartPos, 
										bool bLoop, char *pszChannelScriptID, SSoundChannel3DPosition *p3DPosition);
	int							StopAll(void);
	int							StopChannel(int iChannelCodeID);

	// NO PUBLIC MEMBERS ALLOWED
private:
	// Methods
	bool						CheckIfValidChannel(int iChannelCodeID);
	void						DetachAllChannelsFromElement(int iElementCodeID);
	int							ReserveNewSoundElementInStore(int iElementType);
	int							ReserveNewSoundChannelInStore(void);

private:
	// Members
	// Element store
	CSoundElement				*m_pseSoundElementStore[DGL_SS_AMLOADABLESOUNDELEMENTS];

	// Channel store
	CSoundChannel				m_scSoundChannelStore[DGL_SS_AMCHANNELS];

	DWORD						m_dwBassSystemFlags;
	DWORD						m_dwFrequency;
	int							m_iDevice;
	bool						m_b3DSoundEnabled;			// redundant, also determenable from m_dwBassSystemFlags, but stored here for simplyness
	bool						m_bEAXEnabled;				
	bool						m_bA3DEnabled;				// redundant, also determenable from m_dwBassSystemFlags, but stored here for simplyness
	bool						m_bInitialized;
	CRITICAL_SECTION			m_csSoundSystemAccessMutex;
	CRITICAL_SECTION			m_csSoundChannelStoreMutex;
	CRITICAL_SECTION			m_csSoundElementStoreMutex;
	int							m_iMaxSoundElementsInUse;
	int							m_iMaxSoundChannelsInUse;
};


////////////////////////////////////////////////////////////////////////
//                          
//						FUNCTION DEFINITIONS
//
////////////////////////////////////////////////////////////////////////

void	CALLBACK	DEMOGL_SoundSystemSyncCallBack(HSYNC hSyncHandle, DWORD dwChannel, DWORD dwData, DWORD dwChannelCodeID);

//////////////////////////////////////////////////////////////////////////////////
// Misc soundsystem related functions not embedded as methods in the soundsystemobject.
// C-style functions instead of methods because the soundsystem object isn't created most of the time when
// one of these functions is needed.
//////////////////////////////////////////////////////////////////////////////////

extern	int	InitSoundSystem(void);

//////////////////////////////////////////////////////////////////////////////////
//                   functions also exported from the dll.
//////////////////////////////////////////////////////////////////////////////////
extern	DLLEXPORT	int		DEMOGL_SoundSystemChannelScriptID2ChannelCodeID(const char *pszChannelScriptID);
extern	DLLEXPORT	int		DEMOGL_SoundSystemContinueAll(void);
extern	DLLEXPORT	int		DEMOGL_SoundSystemContinueChannel(const int iChannelCodeID);
extern	DLLEXPORT	SSoundSystemOptionDat	*DEMOGL_SoundSystemDetermineOptionsData(void);
extern	DLLEXPORT	int		DEMOGL_SoundSystemElementScriptID2ElementCodeID(const char *pszElementScriptID);
extern	DLLEXPORT	int		DEMOGL_SoundSystemFreeElement(const int iElementCodeID, const int iElementType);
extern	DLLEXPORT	int		DEMOGL_SoundSystemLoadElement(const char *pszFilename, const int iElementType,
								const char *pszElementScriptID, const bool bUses3D);
extern	DLLEXPORT	int		DEMOGL_SoundSystemPauseAll(void);
extern	DLLEXPORT	int		DEMOGL_SoundSystemPauseChannel(const int iChannelCodeID);
extern	DLLEXPORT	int		DEMOGL_SoundSystemSeekInChannel(const int iChannelCodeID, const long lStartPos);
extern	DLLEXPORT	int		DEMOGL_SoundSystemSet3DFactors(const float fDistanceFactor, const float fRollOffFactor,
								const float fDopplerFactor);
extern	DLLEXPORT	int		DEMOGL_SoundSystemSetChannel3DAttributes(const int iChannelCodeID, 
								SSoundElement3DAttributes * const p3DAttributes);	
extern	DLLEXPORT	int		DEMOGL_SoundSystemSetChannel3DPosition(int iChannelCodeID, SSoundChannel3DPosition * const p3DPosition);	
extern	DLLEXPORT	int		DEMOGL_SoundSystemSetChannelAttributes(const int iChannelCodeID, const int iVolume, const int iPan, 
								const int iFrequency, int iWhat);
extern	DLLEXPORT	int		DEMOGL_SoundSystemSetEAXEnv(const int iEAXEnvironment, const float fReverbVolume, float fDecay, 
								const float fDamp);
extern	DLLEXPORT	int		DEMOGL_SoundSystemSetEAXMixChannel(const int iChannelCodeID, const float fMixValue);
extern	DLLEXPORT	int		DEMOGL_SoundSystemSetElement3DAttributes(const int iElementCodeID, const int iElementType,
								SSoundElement3DAttributes * const p3DAttributes);	
extern	DLLEXPORT	int		DEMOGL_SoundSystemSetListener3DPosition(SListener3DPosition * const p3DPosition);	
extern	DLLEXPORT	int		DEMOGL_SoundSystemSetVolume(const int iVolume);
extern	DLLEXPORT	int		DEMOGL_SoundSystemStartElement(const int iElementCodeID, const int iElementType, const long lStartPos,
								const bool bLoop, const int iVolume, const int iPan, const int iFrequency, 
								SSoundChannel3DPosition * const p3DPosition, char *pszChannelScriptID);
extern	DLLEXPORT	int		DEMOGL_SoundSystemStopAll(void);
extern	DLLEXPORT	int		DEMOGL_SoundSystemStopChannel(int const iChannelCodeID);
extern	DLLEXPORT	int		DEMOGL_SoundSystemSyncCreate(int const iChannelCodeID, const DWORD dwSyncType, 
								const DWORD dwSyncParam, const DWORD dwSyncFlag);
extern	DLLEXPORT	int		DEMOGL_SoundSystemSyncDelete(const int iChannelCodeID, const int iSyncID);
extern	DLLEXPORT	int		DEMOGL_SoundSystemSyncDeleteAll(const int iChannelCodeID);

#endif	// _DGL_DLLSOUNDSYSTEM_H
