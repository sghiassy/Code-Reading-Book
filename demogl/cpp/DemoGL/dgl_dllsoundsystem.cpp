//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllsoundsystem.cpp
// PURPOSE: Implementation of the CSoundSystem class
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// Definition of the soundsystem classes. DemoGL uses a universal soundsystem
// object which uses a library to execute the actual music and sound actions.
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
// v1.3: Added to codebase. Replaces all other soundsubsystems.
// v1.2: --
// v1.1: --
//////////////////////////////////////////////////////////////////////
#include "DemoGL\dgl_dllstdafx.h"
#include "Misc\CMP3Info.h"

////////////////////////////////////////////////////////////////////
/////////////////////////[Class CSoundChannel]//////////////////////
////////////////////////////////////////////////////////////////////

// Purpose: Constructor
CSoundChannel::CSoundChannel()
{
	InitMembers();
}

// Purpose: Destructor
CSoundChannel::~CSoundChannel()
{
	DetachFromElement();
}


// Purpose: inits the membervariables
void
CSoundChannel::InitMembers()
{
	m_iParentElementType=DGL_SS_UNKNOWNELEMENT;
	m_chHandle.m_hcSample=NULL;
	m_sChannelScriptID.Format("%s","");
	m_iChannelCodeID=DGL_SS_UNKNOWNCHANNEL;
	m_bInUse=false;
	m_pseParentElement=NULL;
	m_pssSyncStore=NULL;
	m_b3DEnabled=false;
}


// Purpose: Attaches this channelobject to the given element.
// Returns: SYS_OK if succeeded, errocode if not.
int
CSoundChannel::AttachToElement(UChannelHandle *pchHandle, int iElementType, 
			char *pszChannelScriptID, CSoundElement *pseParentElement, bool bUses3D)
{
	// check if passed info is correct
	if(!pseParentElement)
	{
		// fail.
		return DGL_SS_ERR_CHANNELACTIONFAILED;
	}

	if(m_bInUse)
	{
		// this channel is in use. stop it first.
		Stop();
	}

	switch(iElementType)
	{
		case DGL_SS_MP3ELEMENT:
		{
			m_chHandle.m_hsMP3=pchHandle->m_hsMP3;

			// no sync support implemented
		}; break;
		case DGL_SS_MODELEMENT:
		{
			m_chHandle.m_hmMod=pchHandle->m_hmMod;

			// create sync store.
			m_pssSyncStore=(SSoundSystemSync *)malloc(DGL_SS_MAXMODSYNCS * sizeof(SSoundSystemSync));

			// init to 0's
			memset(m_pssSyncStore,NULL,DGL_SS_MAXMODSYNCS * sizeof(SSoundSystemSync));

		}; break;
		case DGL_SS_SAMPLEELEMENT:
		{
			m_chHandle.m_hcSample=pchHandle->m_hcSample;

			// no syncsupport implemented
		}; break;
		default:
		{
			// unknown elementtype.
			return DGL_SS_ERR_ILLEGALELEMENTTYPE;
		}; break;
	}

	m_pseParentElement=pseParentElement;
	m_iParentElementCodeID=pseParentElement->GetElementCodeID();
	m_iParentElementType=iElementType;
	m_sChannelScriptID.Format("%s",pszChannelScriptID);
	m_sChannelScriptID.ToUpper();
	m_b3DEnabled=bUses3D;

	return SYS_OK;
}


// Purpose: sets the channelcodeID of this channel
void
CSoundChannel::SetChannelCodeID(int iChannelCodeID)
{
	m_iChannelCodeID=iChannelCodeID;
}


// Purpose: gets the channelcodeID of this channel
int
CSoundChannel::GetChannelCodeID()
{
	return m_iChannelCodeID;
}


// Purpose: returns the channelscriptID of this channel
char
*CSoundChannel::GetChannelScriptID()
{
	return &m_sChannelScriptID[0];
}


// Purpose: returns the ElementCodeID of the parent element, which is the
// element that started this channel and owns the data played through the
// channel.
int
CSoundChannel::GetParentElementCodeID()
{
	return m_iParentElementCodeID;
}


// Purpose: sets the in use flag
void
CSoundChannel::SetInUse(bool bYN)
{
	if(m_bInUse)
	{
		// already in use, ignore.
		return;
	}
	m_bInUse=bYN;
}


// Purpose: sets the attributes for this channel. (set frequency, pan and volume). 
// iWhat flags which values to set.
// Returns: SYS_OK if succeeded, SYS_NOK if not.
int
CSoundChannel::SetAttributes(int iVolume, int iPan, int iFrequency, int iWhat)
{
	int				iDefVolume, iDefPan, iDefFrequency;
	BOOL			bResult;

	if(!m_bInUse)
	{
		// not in use
		return SYS_NOK;
	}

	iDefVolume=-1;
	iDefPan=-101;
	iDefFrequency=-1;
	if(iWhat & DGL_SS_SA_FREQUENCY)
	{
		iDefFrequency=iFrequency;
	}
	if(iWhat & DGL_SS_SA_VOLUME)
	{
		iDefVolume=iVolume;
	}
	if(iWhat & DGL_SS_SA_PAN)
	{
		iDefPan=iPan;
	}

	switch(m_iParentElementType)
	{
		case DGL_SS_MODELEMENT:
		{
			// set attributes
			bResult=BASS_ChannelSetAttributes(m_chHandle.m_hmMod,iDefFrequency,iDefVolume,iDefPan);
		}; break;
		case DGL_SS_SAMPLEELEMENT:
		{
			// set attributes
			bResult=BASS_ChannelSetAttributes(m_chHandle.m_hcSample,iDefFrequency,iDefVolume,iDefPan);
		}; break;
		case DGL_SS_MP3ELEMENT:
		{
			// set attributes
			bResult=BASS_ChannelSetAttributes(m_chHandle.m_hsMP3,iDefFrequency,iDefVolume,iDefPan);
		}; break;
	}

	if(bResult)
	{
		return SYS_OK;
	}
	return SYS_NOK;
}


// Purpose: detaches this channel from the element it's attached to.
// Channel will be stopped, and marked as Not in use.
// returns errorcode if failed, else SYS_OK;
int
CSoundChannel::DetachFromElement()
{
	if(!m_bInUse)
	{
		// not in use, fail.
		return DGL_SS_ERR_CHANNELNOTINUSE;
	}

	// stop channel play.
	Stop();

	// Clean up syncs.
	switch(m_iParentElementType)
	{
		case DGL_SS_MP3ELEMENT:
		case DGL_SS_SAMPLEELEMENT:
		{
			// nothin'
		}; break;
		case DGL_SS_MODELEMENT:
		{
			free(m_pssSyncStore);
		}; break;
	}

	// Init the membervariables
	InitMembers();

	// done
	return SYS_OK;
}


// Purpose: creates a sync in the syncstore. if the syncstore is full, or an error occured, 
// DGL_SS_ERR_COULDNTCREATESYNC is returned, otherwise the 'iSyncID' of the sync.
int
CSoundChannel::CreateSync(DWORD dwSyncType, DWORD dwSyncParam, DWORD dwSyncFlag)
{
	HSYNC			hHandle;
	int				i, iSyncID;
	CStdString		sToLog;

	hHandle=NULL;

	// Get amount of syncs possible for this channel (depends on parentelementtype).
	switch(m_iParentElementType)
	{
		case DGL_SS_MP3ELEMENT:
		{
			// find empty slot and use it.
			for(i=0, iSyncID=-1;i<DGL_SS_MAXMP3SYNCS;i++)
			{
				if(!m_pssSyncStore[i].m_hBassSyncHandle)
				{
					// found empty slot.
					iSyncID=i;
					break;
				}
			}
			if(iSyncID>=0)
			{
				hHandle=BASS_ChannelSetSync(m_chHandle.m_hsMP3,dwSyncType,dwSyncParam, 
						&DEMOGL_SoundSystemSyncCallBack, (DWORD)m_iChannelCodeID);
			}
		}; break;
		case DGL_SS_SAMPLEELEMENT:
		{
			// Not implemented in BASS. fail.
			return DGL_SS_ERR_COULDNTCREATESYNC;
		}; break;
		case DGL_SS_MODELEMENT:
		{
			// find empty slot and use it.
			for(i=0, iSyncID=-1;i<DGL_SS_MAXMODSYNCS;i++)
			{
				if(!m_pssSyncStore[i].m_hBassSyncHandle)
				{
					// found empty slot.
					iSyncID=i;
					break;
				}
			}
			if(iSyncID>=0)
			{
				hHandle=BASS_ChannelSetSync(m_chHandle.m_hmMod,dwSyncType,dwSyncParam, 
						&DEMOGL_SoundSystemSyncCallBack, (DWORD)m_iChannelCodeID);
			}
		}; break;
	}

	if(!hHandle)
	{
		// error
		return DGL_SS_ERR_COULDNTCREATESYNC;
	}

	// fill object
	m_pssSyncStore[iSyncID].m_dwSyncFlag=dwSyncFlag;
	m_pssSyncStore[iSyncID].m_dwSyncParam=dwSyncParam;
	m_pssSyncStore[iSyncID].m_dwSyncType=dwSyncType;
	m_pssSyncStore[iSyncID].m_hBassSyncHandle=hHandle;

	// done, return iSyncID
	return iSyncID;
}


// Purpose: will remove the sync from the syncstore and this sync is removed from BASS.
// will return SYS_OK if everything went allright, SYS_NOK if failed. When failed, the
// sync is undefined
int
CSoundChannel::DeleteSync(int iSyncID)
{
	BOOL		bResult;

	if(!m_pssSyncStore[iSyncID].m_hBassSyncHandle)
	{
		// deleting a sync that's not created
		return SYS_NOK;
	}

	// get the handle of the channel. can only be MP3 or MOD, since BASS can't
	// handle CHANNEL handles for syncs (so no sample syncs)
	switch(m_iParentElementType)
	{
		case DGL_SS_MP3ELEMENT:
		{
			// not implemented.
			return SYS_OK;
		}; break;
		case DGL_SS_SAMPLEELEMENT:
		{
			// no can do. Not implemented in BASS. Fail.
			return SYS_NOK;
		}; break;
		case DGL_SS_MODELEMENT:
		{
			bResult=BASS_ChannelRemoveSync(m_chHandle.m_hmMod,m_pssSyncStore[iSyncID].m_hBassSyncHandle);
		}; break;
	}

	// clear the object, ALWAYS
	m_pssSyncStore[iSyncID].m_dwSyncFlag=NULL;
	m_pssSyncStore[iSyncID].m_dwSyncParam=NULL;
	m_pssSyncStore[iSyncID].m_dwSyncType=NULL;
	m_pssSyncStore[iSyncID].m_hBassSyncHandle=NULL;

	// Done
	if(!bResult)
	{
		return SYS_NOK;
	}
	return SYS_OK;
}


// Purpose: deletes ALL syncs from the syncstore and all these syncs are removed from bass.
// Returns SYS_OK if succeeded, SYS_NOK if failed. When failed all syncs are undefined.
int
CSoundChannel::DeleteAllSyncs()
{
	int			i, iMaxSyncs;
	BOOL		bResult, bReturnValFlag;

	// Get amount of syncs possible for this channel (depends on parentelementtype).
	switch(m_iParentElementType)
	{
		case DGL_SS_MP3ELEMENT:
		{
			iMaxSyncs=DGL_SS_MAXMP3SYNCS;
			for(i=0, bReturnValFlag=false;i<iMaxSyncs;i++)
			{
				if(m_pssSyncStore[i].m_hBassSyncHandle)
				{
					// remove it from bass.
					bResult=BASS_ChannelRemoveSync(m_chHandle.m_hsMP3,m_pssSyncStore[i].m_hBassSyncHandle);
					bReturnValFlag|=!bResult;
					m_pssSyncStore[i].m_dwSyncFlag=NULL;
					m_pssSyncStore[i].m_dwSyncParam=NULL;
					m_pssSyncStore[i].m_dwSyncType=NULL;
					m_pssSyncStore[i].m_hBassSyncHandle=NULL;
				}
			}
		}; break;
		case DGL_SS_SAMPLEELEMENT:
		{
			iMaxSyncs=DGL_SS_MAXSAMPLESYNCS;
			// Not implemented in BASS, fail. Set returnvalflag to true which will imply a fail.
			bReturnValFlag=true;
		}; break;
		case DGL_SS_MODELEMENT:
		{
			iMaxSyncs=DGL_SS_MAXMODSYNCS;
			for(i=0, bReturnValFlag=false;i<iMaxSyncs;i++)
			{
				if(m_pssSyncStore[i].m_hBassSyncHandle)
				{
					// remove it from bass.
					bResult=BASS_ChannelRemoveSync(m_chHandle.m_hmMod,m_pssSyncStore[i].m_hBassSyncHandle);
					bReturnValFlag|=!bResult;
					m_pssSyncStore[i].m_dwSyncFlag=NULL;
					m_pssSyncStore[i].m_dwSyncParam=NULL;
					m_pssSyncStore[i].m_dwSyncType=NULL;
					m_pssSyncStore[i].m_hBassSyncHandle=NULL;
				}
			}
		}; break;
	}

	if(bReturnValFlag)
	{
		// some error occured.
		return SYS_NOK;
	}
	return SYS_OK;
}


// Purpose: executes a sync which was triggered. It will search through the created syncs inside the
// syncstore and will send a WM_DEMOGL_SOUNDSYSTEMSYNC message to the mainwindow with LPARAM=dwData and
// WPARAM=the dwSyncFlag passed when the sync was created.
void
CSoundChannel::ExecuteSync(HSYNC hSyncHandle, DWORD dwSyncData)
{
	int			i, iMaxSyncs;

	// Get amount of syncs possible for this channel (depends on parentelementtype).
	switch(m_iParentElementType)
	{
		case DGL_SS_MP3ELEMENT:
		{
			iMaxSyncs=DGL_SS_MAXMP3SYNCS;
		}; break;
		case DGL_SS_SAMPLEELEMENT:
		{
			iMaxSyncs=DGL_SS_MAXSAMPLESYNCS;
		}; break;
		case DGL_SS_MODELEMENT:
		{
			iMaxSyncs=DGL_SS_MAXMODSYNCS;
		}; break;
	}

	// find sync
	for(i=0;i<iMaxSyncs;i++)
	{
		if(m_pssSyncStore[i].m_hBassSyncHandle==hSyncHandle)
		{
			// found it, send message
			PostMessage(m_gpDemoDat->GethWnd(),WM_DEMOGL_SOUNDSYSTEMSYNC,
						(WPARAM)m_pssSyncStore[i].m_dwSyncFlag, (LPARAM)dwSyncData);
			// done. return.
			return;
		}
	}
	// not found. 
}


// Purpose: returns the InUse flag value. Use this to test if a channel is free.
// If the parentelementtype is a SAMPLEELEMENT and the channel is flagged as in
// use, BASS is consulted if the channel is already finished or still playing.
// if it's not in use in bass, the channel is freed and marked as not in use.
bool
CSoundChannel::GetInUse()
{
	if(!m_bInUse)
	{
		return m_bInUse;
	}

	if(m_iParentElementType==DGL_SS_SAMPLEELEMENT)
	{
		// test with BASS.
		if(BASS_ChannelIsActive(m_chHandle.m_hcSample))
		{
			// yes, do nothing.
			return m_bInUse;
		}
		// no, it's not in use anymore. free it.
		DetachFromElement();
	}
	return m_bInUse;
}


// Purpose: stops the play of the channel. The channel should be detached after this call.
// Returns: SYS_OK if succeeded, errorcode if not.
int
CSoundChannel::Stop()
{
	BOOL		bResult;

	if(!m_bInUse)
	{
		// not in use, fail.
		return DGL_SS_ERR_CHANNELNOTINUSE;
	}

	// stop the channel
	switch(m_iParentElementType)
	{
		case DGL_SS_MP3ELEMENT:
		{
			bResult=BASS_ChannelStop(m_chHandle.m_hsMP3);
		}; break;
		case DGL_SS_MODELEMENT:
		{
			bResult=BASS_ChannelStop(m_chHandle.m_hmMod);
		}; break;
		case DGL_SS_SAMPLEELEMENT:
		{
			bResult=BASS_ChannelStop(m_chHandle.m_hcSample);
		}; break;
		default:
		{
			// unknown elementtype.
			return DGL_SS_ERR_ILLEGALELEMENTTYPE;
		}; break;
	}

	if(bResult)
	{
		return SYS_OK;
	}
	// error
	return DGL_SS_ERR_CHANNELACTIONFAILED;
}


// Purpose: pauses the play of the channel. 
// Returns: SYS_OK if succeeded, errorcode if not.
int
CSoundChannel::Pause()
{
	BOOL		bResult;

	if(!m_bInUse)
	{
		// not in use, fail.
		return DGL_SS_ERR_CHANNELNOTINUSE;
	}

	// pause the channel
	switch(m_iParentElementType)
	{
		case DGL_SS_MP3ELEMENT:
		{
			bResult=BASS_ChannelPause(m_chHandle.m_hsMP3);
		}; break;
		case DGL_SS_MODELEMENT:
		{
			bResult=BASS_ChannelPause(m_chHandle.m_hmMod);
		}; break;
		case DGL_SS_SAMPLEELEMENT:
		{
			bResult=BASS_ChannelPause(m_chHandle.m_hcSample);
		}; break;
		default:
		{
			// unknown elementtype.
			return DGL_SS_ERR_ILLEGALELEMENTTYPE;
		}; break;
	}

	if(bResult)
	{
		return SYS_OK;
	}
	// error
	return DGL_SS_ERR_CHANNELACTIONFAILED;
}


// Purpose: continues the play of the paused channel. 
// Returns: SYS_OK if succeeded, errorcode if not.
int
CSoundChannel::Continue()
{
	BOOL		bResult;

	if(!m_bInUse)
	{
		// not in use, fail.
		return DGL_SS_ERR_CHANNELNOTINUSE;
	}

	// resume the channel
	switch(m_iParentElementType)
	{
		case DGL_SS_MP3ELEMENT:
		{
			bResult=BASS_ChannelResume(m_chHandle.m_hsMP3);
		}; break;
		case DGL_SS_MODELEMENT:
		{
			bResult=BASS_ChannelResume(m_chHandle.m_hmMod);
		}; break;
		case DGL_SS_SAMPLEELEMENT:
		{
			bResult=BASS_ChannelResume(m_chHandle.m_hcSample);
		}; break;
		default:
		{
			// unknown elementtype.
			return DGL_SS_ERR_ILLEGALELEMENTTYPE;
		}; break;
	}

	if(bResult)
	{
		return SYS_OK;
	}
	// error
	return DGL_SS_ERR_CHANNELACTIONFAILED;
}


// Purpose: Sets the EAXMix value of this channel
// Returns: SYS_OK if succeeded, errorcode if not.
int
CSoundChannel::SetEAXMix(float fMixValue)
{
	BOOL		bResult;

	if(!m_bInUse)
	{
		// not in use, fail.
		return DGL_SS_ERR_CHANNELNOTINUSE;
	}

	// set EAX mix value the channel
	switch(m_iParentElementType)
	{
		case DGL_SS_MP3ELEMENT:
		{
			bResult=BASS_ChannelSetEAXMix(m_chHandle.m_hsMP3, fMixValue);
		}; break;
		case DGL_SS_MODELEMENT:
		{
			bResult=BASS_ChannelSetEAXMix(m_chHandle.m_hmMod, fMixValue);
		}; break;
		case DGL_SS_SAMPLEELEMENT:
		{
			bResult=BASS_ChannelSetEAXMix(m_chHandle.m_hcSample, fMixValue);
		}; break;
		default:
		{
			// unknown elementtype.
			return DGL_SS_ERR_ILLEGALELEMENTTYPE;
		}; break;
	}

	if(bResult)
	{
		return SYS_OK;
	}
	// error
	return DGL_SS_ERR_CHANNELACTIONFAILED;
}


// Purpose: Seeks to the position lStartPos. It consults the parent element to provide the accurate
// data to pass to BASS.
// Returns: SYS_OK if succeeded, errorcode if not.
int
CSoundChannel::Seek(long lStartPos)
{
	BOOL		bResult;
	DWORD		dwSeekPos;

	if(!m_bInUse)
	{
		// not in use, fail.
		return DGL_SS_ERR_CHANNELNOTINUSE;
	}

	// calculate position to pass to BASS. Consult parent element's calculateseekvalue routine
	// to do that calculation. (overruled virtual method)
	dwSeekPos=m_pseParentElement->CalcSeekValue(lStartPos, &m_chHandle);

	// Seek into the channel
	switch(m_iParentElementType)
	{
		case DGL_SS_MP3ELEMENT:
		{
			bResult=BASS_ChannelSetPosition(m_chHandle.m_hsMP3, dwSeekPos);
		}; break;
		case DGL_SS_MODELEMENT:
		{
			bResult=BASS_ChannelSetPosition(m_chHandle.m_hmMod, dwSeekPos);
		}; break;
		case DGL_SS_SAMPLEELEMENT:
		{
			bResult=BASS_ChannelSetPosition(m_chHandle.m_hcSample, dwSeekPos);
		}; break;
		default:
		{
			// unknown elementtype.
			return DGL_SS_ERR_ILLEGALELEMENTTYPE;
		}; break;
	}

	if(bResult)
	{
		return SYS_OK;
	}
	// error
	return DGL_SS_ERR_CHANNELACTIONFAILED;
}


// Purpose: Sets the 3D attributes for this channel. Will only affect current channel,
// not element. So if another play instance of the element is started on another channel,
// it won't have this 3D attributes passed to this channel.
// Returns: SYS_OK if succeeded, errorcode if not.
int
CSoundChannel::Set3DAttributes(SSoundElement3DAttributes *p3DAttributes)
{
	bool		bFailed;

	if(!m_bInUse)
	{
		// not in use, fail.
		return DGL_SS_ERR_CHANNELNOTINUSE;
	}

	if(!m_b3DEnabled)
	{
		// not a channel that has 3D enabled, fail.
		return DGL_SS_ERR_CHANNELACTIONFAILED;
	}

	// Set 3D attributes of channel
	bFailed=false;
	switch(m_iParentElementType)
	{
		case DGL_SS_MP3ELEMENT:
		{
			if(!BASS_ChannelSet3DAttributes(m_chHandle.m_hsMP3,p3DAttributes->m_dw3DMode,p3DAttributes->m_fMinimalDistance,
								p3DAttributes->m_fMaximalDistance,p3DAttributes->m_iInsideProjectionConeAngle,
								p3DAttributes->m_iOutsideProjectionConeAngle,
								p3DAttributes->m_iDeltaVolumeOutsideOuterProjectionCone))
			{
				bFailed=true;
			}
		}; break;
		case DGL_SS_MODELEMENT:
		{
			if(!BASS_ChannelSet3DAttributes(m_chHandle.m_hmMod,p3DAttributes->m_dw3DMode,p3DAttributes->m_fMinimalDistance,
								p3DAttributes->m_fMaximalDistance,p3DAttributes->m_iInsideProjectionConeAngle,
								p3DAttributes->m_iOutsideProjectionConeAngle,
								p3DAttributes->m_iDeltaVolumeOutsideOuterProjectionCone))
			{
				bFailed=true;
			}
		}; break;
		case DGL_SS_SAMPLEELEMENT:
		{
			if(!BASS_ChannelSet3DAttributes(m_chHandle.m_hcSample,p3DAttributes->m_dw3DMode,p3DAttributes->m_fMinimalDistance,
								p3DAttributes->m_fMaximalDistance,p3DAttributes->m_iInsideProjectionConeAngle,
								p3DAttributes->m_iOutsideProjectionConeAngle,
								p3DAttributes->m_iDeltaVolumeOutsideOuterProjectionCone))
			{
				bFailed=true;
			}
		}; break;
		default:
		{
			// unknown elementtype.
			return DGL_SS_ERR_ILLEGALELEMENTTYPE;
		}; break;
	}

	if(!bFailed)
	{
		// set succeeded, finalize the set by calling Apply3D.
		if(!BASS_Apply3D())
		{
			// apply failed, fail.
			bFailed=true;
		}
		if(bFailed)
		{
			// error
			return DGL_SS_ERR_CHANNELACTIONFAILED;
		}
		return SYS_OK;
	}
	// error
	return DGL_SS_ERR_CHANNELACTIONFAILED;
}


// Purpose: Sets the 3D position for this channel. 
// Returns: SYS_OK if succeeded, errorcode if not.
int
CSoundChannel::Set3DPosition(SSoundChannel3DPosition *p3DPosition)
{
	bool		bFailed;

	if(!m_bInUse)
	{
		// not in use, fail.
		return DGL_SS_ERR_CHANNELNOTINUSE;
	}

	if(!m_b3DEnabled)
	{
		// not a channel that has 3D enabled, fail.
		return DGL_SS_ERR_CHANNELACTIONFAILED;
	}

	// Set 3D attributes of channel
	bFailed=false;
	switch(m_iParentElementType)
	{
		case DGL_SS_MP3ELEMENT:
		{
			if(!BASS_ChannelSet3DPosition(m_chHandle.m_hsMP3,(BASS_3DVECTOR *)&p3DPosition->m_ssvPosition,
						(BASS_3DVECTOR *)&p3DPosition->m_ssvOrientation,(BASS_3DVECTOR *)&p3DPosition->m_ssvVelocity))
			{
				bFailed=true;
			}
		}; break;
		case DGL_SS_MODELEMENT:
		{
			if(!BASS_ChannelSet3DPosition(m_chHandle.m_hmMod,(BASS_3DVECTOR *)&p3DPosition->m_ssvPosition,
						(BASS_3DVECTOR *)&p3DPosition->m_ssvOrientation,(BASS_3DVECTOR *)&p3DPosition->m_ssvVelocity))
			{
				bFailed=true;
			}
		}; break;
		case DGL_SS_SAMPLEELEMENT:
		{
			if(!BASS_ChannelSet3DPosition(m_chHandle.m_hcSample,(BASS_3DVECTOR *)&p3DPosition->m_ssvPosition,
						(BASS_3DVECTOR *)&p3DPosition->m_ssvOrientation,(BASS_3DVECTOR *)&p3DPosition->m_ssvVelocity))
			{
				bFailed=true;
			}
		}; break;
		default:
		{
			// unknown elementtype.
			return DGL_SS_ERR_ILLEGALELEMENTTYPE;
		}; break;
	}

	if(!bFailed)
	{
		// set succeeded, finalize the set by calling Apply3D.
		if(!BASS_Apply3D())
		{
			// apply failed, fail.
			bFailed=true;
		}
		if(bFailed)
		{
			// error
			return DGL_SS_ERR_CHANNELACTIONFAILED;
		}
		return SYS_OK;
	}
	// error
	return DGL_SS_ERR_CHANNELACTIONFAILED;
}


////////////////////////////////////////////////////////////////////
/////////////////////////[Class CSoundElement]//////////////////////
////////////////////////////////////////////////////////////////////

// Purpose: Constructor of sound element
CSoundElement::CSoundElement()
{
	// Init data
	InitMembers();
}


// Purpose: Destructor of sound element
CSoundElement::~CSoundElement()
{
	FreeElementData();
}


// Purpose: initializes the membervariables
void
CSoundElement::InitMembers()
{
	// Init data
	m_lElementLength=0;
	m_pbyElementDataBuffer=NULL;
	m_dwBassFlags=0;
	m_iElementCodeID=-1;
	m_iType=DGL_SS_UNKNOWNELEMENT;
	m_sElementScriptID.Format("%s","");
	// Initially, there are no references
	m_iReferences=0;

	m_bUses3D=false;
	m_sea3DAttributes.m_dw3DMode=0;
	m_sea3DAttributes.m_fMaximalDistance=0.0f;
	m_sea3DAttributes.m_fMinimalDistance=0.0f;
	m_sea3DAttributes.m_iDeltaVolumeOutsideOuterProjectionCone=100;
	m_sea3DAttributes.m_iInsideProjectionConeAngle=0;
	m_sea3DAttributes.m_iOutsideProjectionConeAngle=0;
}


// Purpose: clears the element and wipes all data from memory.
void
CSoundElement::ClearElement()
{
	FreeElementData();
	InitMembers();
}


// Purpose: increases the reference counter, which reflects the amount of references of
// to this soundelement. Should be executed only in a protected execution chain, i.e. from
// code that is protected using a mutex/critical section flag.
void
CSoundElement::IncReferences()
{
	m_iReferences++;
}


// Purpose: decreases the reference counter, which reflects the amount of references of
// to this soundelement. Should be executed only in a protected execution chain, i.e. from
// code that is protected using a mutex/critical section flag.
void
CSoundElement::DecReferences()
{
	m_iReferences--;
}


// Purpose: gets the references counter, which reflects the amount of references of
// to this soundelement. 
int
CSoundElement::GetReferences()
{
	return m_iReferences;
}


// Purpose: frees the element data
void
CSoundElement::FreeElementData()
{
	byte		*pDataBuffer;	

	// check if there is still a databuffer allocated
	if(m_lElementLength)
	{
		pDataBuffer=m_pbyElementDataBuffer;
		DEMOGL_FileFree(pDataBuffer);
		InitMembers();
	}
}


// Purpose: retrieves the Bass flags set for this element
DWORD
CSoundElement::GetBassFlags()
{
	return m_dwBassFlags;
}


// Purpose: Gets a pointer to the elementdata
byte
*CSoundElement::GetElementData()
{
	return m_pbyElementDataBuffer;
}


// Purpose: Gets the CodeID for this element. This CodeID is needed to perform actions via the soundsystem
// in CODE. Use the GetElementScriptID to use the ID assigned to the element in the script
int
CSoundElement::GetElementCodeID()
{
	return m_iElementCodeID;
}


// Purpose: Gets the ScriptID for this element. This ScriptID is needed to perform actions via the soundsystem
// in SCRIPT. Use the GetElementCodeID to use the ID assigned to the element using code calls.
char
*CSoundElement::GetElementScriptID()
{
	return &m_sElementScriptID[0];
}


// Purpose: gets the type of the element.
int
CSoundElement::GetElementType()
{
	return m_iType;
}


// Purpose: Gets the length in bytes of the stored buffer
long
CSoundElement::GetElementLength()
{
	return m_lElementLength;
}


// Purpose: gets the Uses3D flag for this element. Needed for selecting play routines (with/without 3D support)
bool
CSoundElement::GetUses3D()
{
	return m_bUses3D;
}


// Purpose: Sets element data
void
CSoundElement::SetElementData(byte *pElementBuffer, long lElementLength, DWORD dwBassFlags, 
				int iType, int iElementCodeID, char *pszElementScriptID, bool bUses3D)
{
	ClearElement();
	m_pbyElementDataBuffer=pElementBuffer;
	m_lElementLength=lElementLength;
	m_dwBassFlags=dwBassFlags;
	m_iElementCodeID=iElementCodeID;
	m_iType=iType;
	m_bUses3D=bUses3D;
	if(pszElementScriptID)
	{
		m_sElementScriptID.Format("%s",pszElementScriptID);
		m_sElementScriptID.ToUpper();
	}
}


// Purpose: gets the 3D attributes of this soundelement as a pointer to the internal struct.
SSoundElement3DAttributes
*CSoundElement::Get3DAttributes()
{
	return &m_sea3DAttributes;
}


// Purpose: sets the 3D attributes of this sound element. The contents of the stucture passed is copied,
// so the caller should free the structure
// Returns SYS_OK if succeeded, DGL_SS_ERR_OPERATIONFAILED if not. (wont happen here, but derived classes can report errors when
// effectuating the passed data.)
int
CSoundElement::Set3DAttributes(SSoundElement3DAttributes *p3DAttributes)
{
	// Copy contents.
	m_sea3DAttributes.m_dw3DMode=p3DAttributes->m_dw3DMode;
	m_sea3DAttributes.m_fMaximalDistance=p3DAttributes->m_fMaximalDistance;
	m_sea3DAttributes.m_fMinimalDistance=p3DAttributes->m_fMinimalDistance;
	m_sea3DAttributes.m_iDeltaVolumeOutsideOuterProjectionCone=p3DAttributes->m_iDeltaVolumeOutsideOuterProjectionCone;
	m_sea3DAttributes.m_iInsideProjectionConeAngle=p3DAttributes->m_iInsideProjectionConeAngle;
	m_sea3DAttributes.m_iOutsideProjectionConeAngle=p3DAttributes->m_iOutsideProjectionConeAngle;
	return SYS_OK;
}


///////////////////////////////////////////////////////////////////
///				 virtual function definitions.
///////////////////////////////////////////////////////////////////

DWORD
CSoundElement::CalcSeekValue(long lStartPos, UChannelHandle *pchHandle)
{
	return (DWORD)lStartPos;
}


int
CSoundElement::Init()
{
	return SYS_OK;
}


int
CSoundElement::Play(bool bFlush, bool bLoop, CSoundChannel *pscTargetChannel, char *pszChannelScriptID)
{
	return SYS_OK;
}


void
CSoundElement::SetStartPos(DWORD dwStartPos)
{
	// nothin'
}


// ADD MORE VIRTUAL FUNCTIONS HERE.

////////////////////////////////////////////////////////////////////
///////////////////////////[Class CMP3Element]//////////////////////
////////////////////////////////////////////////////////////////////

// Purpose: constructor
CMP3Element::CMP3Element()
{
	m_hsBassHandle=NULL;
}

// Purpose: destructor
CMP3Element::~CMP3Element()
{
	FreeElementData();
}


// Purpose: sets the BASS handle, used for this MP3 Element
void
CMP3Element::SetBassHandle(HSTREAM hsHandle)
{
	m_hsBassHandle=hsHandle;
}


// Purpose: returns the basshandle
HSTREAM
CMP3Element::GetBassHandle()
{
	return m_hsBassHandle;
}


// Purpose: frees up MP3 element's data. 
// All channels of this element are already deleted/closed, as also the stream.
void
CMP3Element::FreeElementData()
{
	m_hsBassHandle=NULL;
	CSoundElement::FreeElementData();
}


// Purpose: initializes the members concerning mp3 data.
int
CMP3Element::Init()
{
	// fill the mp3 info object.	
	m_miMP3Info.loadInfo(CSoundElement::GetElementData(),CSoundElement::GetElementLength());
	return SYS_OK;
}


// Purpose: calculates the new position of the pointer inside the streamdata, when seeking to
// lStartPos. Uses the mp3 info structure to move to the given dataposition.
// will return position in bytes if succeeded, NULL otherwise
DWORD
CMP3Element::CalcSeekValue(long lStartPos, UChannelHandle *pchHandle)
{
	float		fLengthOfMP3;
	long		lOffset;

	fLengthOfMP3 = m_miMP3Info.getLengthInSeconds();
	if(fLengthOfMP3 < (float)(lStartPos/1000))
	{
		// no can do.
		return NULL;
	}

	// ok now calculate the position of the pointer
	if(fLengthOfMP3<=0.0f)
	{
		lOffset=0;
	}
	else
	{
		lOffset = (long)(( ((float)(lStartPos/1000)) / fLengthOfMP3) * BASS_StreamGetLength(m_hsBassHandle));
	}

	return (DWORD)lOffset;
}


// Purpose: starts the play of the mp3 element's data. It will store the data in the passed
// SoundChannel object pscTargetChannel, if play start succeeded. 
// bFlush flags if the play should start from the beginning (true) or not.
// bLoop signals the playfunction to flag BASS to loop the element
//
// Returns SYS_OK if no error occured, DGL_SS_ERR_COULDNTATTACHTOCHANNEL otherwise.
int
CMP3Element::Play(bool bFlush, bool bLoop, CSoundChannel *pscTargetChannel, char *pszChannelScriptID)
{
	CStdString		sToLog;
	DWORD			dwFlags;
	int				iResult;
	UChannelHandle	chHandle;

	if(!pscTargetChannel)
	{
		// channel does not exist, can't play.
		sToLog.Format("Play on non existing channel executed. Ignored");
		LogFeedback(&sToLog[0],true,"CMP3Element::Play",false);
		return DGL_SS_ERR_COULDNTATTACHTOCHANNEL;
	}

	dwFlags=NULL;
	if(bLoop)
	{
		dwFlags|=BASS_SAMPLE_LOOP;
	}

	if(!BASS_StreamPlay(m_hsBassHandle, (BOOL)bFlush,dwFlags))
	{
		sToLog.Format("Couldn't play MP3 sound element with script ID %s. BASS Errorcode: %d.",
				CSoundElement::GetElementScriptID(), BASS_ErrorGetCode());
		LogFeedback(&sToLog[0],true,"CMP3Element::Play",false);
		return DGL_SS_ERR_COULDNTATTACHTOCHANNEL;
	}

	// Attach to channelobject. Because an MP3 stream can only be played on 1 channel and the
	// BASS channelhandle == the streamhandle, we just pass THAT as the handle to the channel.
	chHandle.m_hsMP3=m_hsBassHandle;
	iResult=pscTargetChannel->AttachToElement(&chHandle,DGL_SS_MP3ELEMENT,pszChannelScriptID,
						this,CSoundElement::GetUses3D());
	if(iResult==SYS_OK)
	{
		return SYS_OK;
	}

	// error
	sToLog.Format("Couldn't attach MP3 sound element with script ID %s to channel with script ID %s.",
			CSoundElement::GetElementScriptID(), pszChannelScriptID);
	LogFeedback(&sToLog[0],true,"CMP3Element::Play",false);
	return DGL_SS_ERR_COULDNTATTACHTOCHANNEL;
}



////////////////////////////////////////////////////////////////////
///////////////////////////[Class CMODElement]//////////////////////
////////////////////////////////////////////////////////////////////

// Purpose: constructor
CMODElement::CMODElement()
{
	m_hmBassHandle=NULL;
	m_dwStartPos=NULL;
}


// Purpose: destructor
CMODElement::~CMODElement()
{
	FreeElementData();
}


// Purpose: frees up MOD element's data.
// All channels of this element are already deleted/closed, as also the mod
void
CMODElement::FreeElementData()
{
	m_hmBassHandle=NULL;
	CSoundElement::FreeElementData();
}


// Purpose: sets the BASS handle used fore this MOD element
void
CMODElement::SetBassHandle(HMUSIC hsHandle)
{
	m_hmBassHandle=hsHandle;
}


// Purpose: returns the basshandle
HMUSIC
CMODElement::GetBassHandle()
{
	return m_hmBassHandle;
}


// Purpose: starts the play of the MOD element's data. It will store the data in the passed
// SoundChannel object pscTargetChannel, if play start succeeded. 
// bFlush flags if the play should start from the beginning (true) or not.
// bLoop signals the playfunction to flag BASS to loop the element
//
// Returns SYS_OK if no error occured, DGL_SS_ERR_COULDNTATTACHTOCHANNEL otherwise.
int
CMODElement::Play(bool bFlush, bool bLoop, CSoundChannel *pscTargetChannel, char *pszChannelScriptID)
{
	CStdString		sToLog;
	DWORD			dwFlags;
	int				iResult;
	UChannelHandle	chHandle;

	if(!pscTargetChannel)
	{
		// channel does not exist, can't play.
		sToLog.Format("Play on non existing channel executed. Ignored");
		LogFeedback(&sToLog[0],true,"CMODElement::Play",false);
		return DGL_SS_ERR_COULDNTATTACHTOCHANNEL;
	}

	dwFlags=CSoundElement::GetBassFlags();
	if(bLoop)
	{
		dwFlags|=BASS_MUSIC_LOOP;
	}

	if(!BASS_MusicPlayEx(m_hmBassHandle, m_dwStartPos,dwFlags,FALSE))
	{
		sToLog.Format("Couldn't play MOD sound element with script ID %s. BASS Errorcode: %d.",
				CSoundElement::GetElementScriptID(), BASS_ErrorGetCode());
		LogFeedback(&sToLog[0],true,"CMODElement::Play",false);
		return DGL_SS_ERR_COULDNTATTACHTOCHANNEL;
	}

	// Attach to channelobject. Because a MOD stream can only be played on 1 channel and the
	// BASS channelhandle == the streamhandle, we just pass THAT as the handle to the channel.
	chHandle.m_hmMod=m_hmBassHandle;
	iResult=pscTargetChannel->AttachToElement(&chHandle,DGL_SS_MODELEMENT,pszChannelScriptID,
						this,CSoundElement::GetUses3D());
	if(iResult==SYS_OK)
	{
		return SYS_OK;
	}

	// error
	sToLog.Format("Couldn't attach MOD sound element with script ID %s to channel with script ID %s.",
			CSoundElement::GetElementScriptID(), pszChannelScriptID);
	LogFeedback(&sToLog[0],true,"CMODElement::Play",false);
	return DGL_SS_ERR_COULDNTATTACHTOCHANNEL;
}


// Purpose: sets the startposition where Play() should start playing the moddata.
void
CMODElement::SetStartPos(DWORD dwStartPos)
{
	m_dwStartPos=dwStartPos;
}


////////////////////////////////////////////////////////////////////
///////////////////////////[Class CSampleElement]///////////////////
////////////////////////////////////////////////////////////////////

// Purpose: constructor
CSampleElement::CSampleElement()
{
	m_hsBassHandle=NULL;
	m_dwStartPos=NULL;
}


// Purpose: destructor
CSampleElement::~CSampleElement()
{
	FreeElementData();
}


// Purpose: sets the startposition where Play() should start playing the sample data.
void
CSampleElement::SetStartPos(DWORD dwStartPos)
{
	m_dwStartPos=dwStartPos;
}


// Purpose: frees up Sample element's data. BASS stream HAS TO BE CLOSED before calling this function.
void
CSampleElement::FreeElementData()
{
	m_hsBassHandle=NULL;
	CSoundElement::FreeElementData();
}


// Purpose: sets the BASS handle, used for this SMPL Element
void
CSampleElement::SetBassHandle(HSAMPLE hsHandle)
{
	m_hsBassHandle=hsHandle;
}


// Purpose: returns the basshandle
HSAMPLE
CSampleElement::GetBassHandle()
{
	return m_hsBassHandle;
}


// Purpose: starts the play of the sample element's data. It will store the data in the passed
// SoundChannel object pscTargetChannel, if play start succeeded. 
// bFlush flags if the play should start from the beginning (true) or not.
// bLoop signals the playfunction to flag BASS to loop the element.
//
// Returns SYS_OK if no error occured, DGL_SS_ERR_COULDNTATTACHTOCHANNEL otherwise.
int
CSampleElement::Play(bool bFlush, bool bLoop, CSoundChannel *pscTargetChannel, char *pszChannelScriptID)
{
	CStdString		sToLog;
	HCHANNEL		hcBassChannelHandle;
	int				iResult;
	UChannelHandle	chHandle;

	if(!pscTargetChannel)
	{
		// channel does not exist, can't play.
		sToLog.Format("Play on non existing channel executed. Ignored");
		LogFeedback(&sToLog[0],true,"CSampleElement::Play",false);
		return DGL_SS_ERR_COULDNTATTACHTOCHANNEL;
	}

	if(CSoundElement::GetUses3D())
	{
		// a 3D soundelement. use 3D playfunctions.
		hcBassChannelHandle=BASS_SamplePlay3DEx(m_hsBassHandle,NULL,NULL,NULL,-1,-1,-1,(BOOL)bLoop);
	}
	else
	{
		// a 2D soundelement. use 2D playfunctions.
		hcBassChannelHandle=BASS_SamplePlayEx(m_hsBassHandle, m_dwStartPos,-1,-1,-1,(BOOL)bLoop);
	}

	if(!hcBassChannelHandle)
	{
		sToLog.Format("Couldn't play sample sound element with script ID %s. BASS Errorcode: %d.",
				CSoundElement::GetElementScriptID(), BASS_ErrorGetCode());
		LogFeedback(&sToLog[0],true,"CSampleElement::Play",false);
		return DGL_SS_ERR_COULDNTATTACHTOCHANNEL;
	}

	// Attach to channelobject. Pass the just received basschannelhandle to the channelobject.
	chHandle.m_hcSample=hcBassChannelHandle;
	iResult=pscTargetChannel->AttachToElement(&chHandle,DGL_SS_SAMPLEELEMENT,pszChannelScriptID,
						this,CSoundElement::GetUses3D());
	if(iResult==SYS_OK)
	{
		return SYS_OK;
	}

	// error
	sToLog.Format("Couldn't attach sample sound element with script ID %s to channel with script ID %s.",
			CSoundElement::GetElementScriptID(), pszChannelScriptID);
	LogFeedback(&sToLog[0],true,"CSampleElement::Play",false);
	return DGL_SS_ERR_COULDNTATTACHTOCHANNEL;
}


////////////////////////////////////////////////////////////////////
//////////////////////////[Class CSoundSystem]//////////////////////
////////////////////////////////////////////////////////////////////

// Purpose: constructor
CSoundSystem::CSoundSystem()
{
	int			i;

	// clear soundelement store pointers.
	memset(m_pseSoundElementStore,NULL,(DGL_SS_AMLOADABLESOUNDELEMENTS * sizeof(CSoundElement *)));

	// Init soundchannelobjects.
	for(i=0;i<DGL_SS_AMCHANNELS;i++)
	{
		m_scSoundChannelStore[i].SetInUse(false);
		m_scSoundChannelStore[i].SetChannelCodeID(i);
	}

	// set internal vars
	m_bInitialized=false;
	m_dwBassSystemFlags=0;
	m_dwFrequency=0;
	m_iDevice=-1;
	m_b3DSoundEnabled=false;
	m_bEAXEnabled=false;
	m_bA3DEnabled=false;
	m_iMaxSoundElementsInUse=0;
	m_iMaxSoundChannelsInUse=0;

	// Initialize mutexes for multithreading support.
	InitializeCriticalSection(&m_csSoundSystemAccessMutex);
	InitializeCriticalSection(&m_csSoundChannelStoreMutex);
	InitializeCriticalSection(&m_csSoundElementStoreMutex);
}


// Purpose: destructor
CSoundSystem::~CSoundSystem()
{
	FreeSystem();
	DeleteCriticalSection(&m_csSoundSystemAccessMutex);
	DeleteCriticalSection(&m_csSoundChannelStoreMutex);
	DeleteCriticalSection(&m_csSoundElementStoreMutex);
}


// Purpose: Reserves a new Soundelement for the caller in the soundelement object store.
// It will create a new object and return the index to the new object in the store, if succeeded,
// otherwise errorcode. This routine is protected with a mutex so only 1 caller per time can 
// reserve new elements in the soundelementstore.
int
CSoundSystem::ReserveNewSoundElementInStore(int iElementType)
{
	int				i, iElementCodeID;

	EnterCriticalSection(&m_csSoundElementStoreMutex);
		for(i=0, iElementCodeID=DGL_SS_UNKNOWNELEMENT;i<DGL_SS_AMLOADABLESOUNDELEMENTS;i++)
		{
			if(!m_pseSoundElementStore[i])
			{
				// empty slot.
				if((i+1)>m_iMaxSoundElementsInUse)
				{
					// store this slot as the maximum occupied. We don't have
					// to check beyond this slot, no used slot above this one.
					m_iMaxSoundElementsInUse=(i+1);
				}
				// Use this as the new object
				iElementCodeID=i;
				break;
			}
		}
		
		if(iElementCodeID<0)
		{
			// store is full.
			LeaveCriticalSection(&m_csSoundElementStoreMutex);
			return DGL_SS_ERR_ELEMENTSTOREFULL;
		}

		// create new element and store it at the found spot.
		switch(iElementType)
		{
			case DGL_SS_MP3ELEMENT:
			{
				m_pseSoundElementStore[i]=new CMP3Element;
			}; break;
			case DGL_SS_MODELEMENT:
			{
				m_pseSoundElementStore[i]=new CMODElement;
			}; break;
			case DGL_SS_SAMPLEELEMENT:
			{
				m_pseSoundElementStore[i]=new CSampleElement;
			}; break;
			default:
			{
				// unknown element
				LeaveCriticalSection(&m_csSoundElementStoreMutex);
				return DGL_SS_UNKNOWNELEMENT;
			}; break;
		}
	LeaveCriticalSection(&m_csSoundElementStoreMutex);
	return iElementCodeID;
}


// Purpose: Reserves a new SoundChannel for the caller in the soundChannel object store.
// It will mark an unused object as used and return the index to that object in the store, if succeeded,
// otherwise errorcode. This routine is protected with a mutex so only 1 caller per time can 
// reserve new elements in the soundchannelstore.
int
CSoundSystem::ReserveNewSoundChannelInStore()
{
	int				i, iChannelCodeID;

	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		for(i=0, iChannelCodeID=DGL_SS_UNKNOWNELEMENT;i<DGL_SS_AMCHANNELS;i++)
		{
			if(!m_scSoundChannelStore[i].GetInUse())
			{
				// empty slot.
				if((i+1)>m_iMaxSoundChannelsInUse)
				{
					// store this slot as the maximum occupied. We don't have
					// to check beyond this slot, no used slot above this one.
					m_iMaxSoundChannelsInUse=(i+1);
				}
				// Use this as the new object
				iChannelCodeID=i;
				break;
			}
		}
		
		if(iChannelCodeID<0)
		{
			// store is full.
			LeaveCriticalSection(&m_csSoundChannelStoreMutex);
			return DGL_SS_ERR_CHANNELSTOREFULL;
		}

		// mark this channelobject ours.
		m_scSoundChannelStore[i].SetInUse(true);

	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
	return iChannelCodeID;
}


// Purpose: checks if the channel with the passed channelscriptID is in use. If not, the 
// channel is not valid, otherwise it's a valid channel.
// Returns true if valid, false otherwise
bool
CSoundSystem::CheckIfValidChannel(char *pszChannelScriptID)
{
	int			iChannelCodeID;

	iChannelCodeID=ConvertChannelScriptID2ChannelCodeID(pszChannelScriptID);

	if(iChannelCodeID<0)
	{
		// error occured.
		return false;
	}

	return m_scSoundChannelStore[iChannelCodeID].GetInUse();
}


// Purpose: checks if the channel with the passed channelcodeID is in use. If not, 
// the channel is not valid, otherwise it's a valid channel.
// Returns true if valid, false otherwise
bool
CSoundSystem::CheckIfValidChannel(int iChannelCodeID)
{
	if((iChannelCodeID<0)||(iChannelCodeID>DGL_SS_AMCHANNELS))
	{
		// invalid channelcodeid
		return false;
	}

	return m_scSoundChannelStore[iChannelCodeID].GetInUse();
}


// Purpose: converts an ElementscriptID to an ElementCodeID, the ID's that are used internally in DemoGL and
// with codefunctions in applicationcode. If a scriptID is not found, DGL_SS_ERR_ILLEGALELEMENTID
// is returned, otherwise the ElementCodeID is returned, which is usable with the soundsystem methods.
// ID's are case insensitive and stored uppercase.
int
CSoundSystem::ConvertElementScriptID2ElementCodeID(char *pszElementScriptID)
{
	int				i;
	CSoundElement	*pElement;
	CStdString		sUCaseElementScriptID;

	if(!pszElementScriptID)
	{
		return DGL_SS_ERR_ILLEGALELEMENTID;
	}

	sUCaseElementScriptID.Format("%s",pszElementScriptID);
	sUCaseElementScriptID.ToUpper();

	// search for the scriptid in all the loaded elements. Use m_iMaxSoundElementsInUse
	// to limit the amount of elements to search in. 
	for(i=0;i<m_iMaxSoundElementsInUse;i++)
	{
		pElement=m_pseSoundElementStore[i];
		if(!pElement)
		{
			continue;
		}
		if(!strcmp(pElement->GetElementScriptID(),&sUCaseElementScriptID[0]))
		{
			// found
			return pElement->GetElementCodeID();
		}
	}
	return DGL_SS_ERR_ILLEGALELEMENTID;
}


// Purpose: converts a ChannelScriptID to a ChannelCodeID, the ID's that are used internally in DemoGL and
// with codefunctions in applicationcode. If a scriptID is not found, DGL_SS_ERR_ILLEGALCHANNELID
// is returned, otherwise the ChannelCodeID is returned, which is usable with the soundsystem methods.
// ID's are case insensitive and stored uppercase.
int
CSoundSystem::ConvertChannelScriptID2ChannelCodeID(char *pszChannelScriptID)
{
	int				i;
	CStdString		sUCaseChannelScriptID;

	if(!pszChannelScriptID)
	{
		return DGL_SS_ERR_ILLEGALCHANNELID;
	}

	sUCaseChannelScriptID.Format("%s",pszChannelScriptID);
	sUCaseChannelScriptID.ToUpper();

	// search for the scriptid in all the in use channels.
	for(i=0;i<m_iMaxSoundChannelsInUse;i++)
	{
		if(!m_scSoundChannelStore[i].GetInUse())
		{
			// not in use.
			continue;
		}
		if(!strcmp(m_scSoundChannelStore[i].GetChannelScriptID(),&sUCaseChannelScriptID[0]))
		{
			// found
			return m_scSoundChannelStore[i].GetChannelCodeID();
		}
	}
	return DGL_SS_ERR_ILLEGALCHANNELID;
}


// Purpose: sets several attributes of the Channel with ID passed.
// iWhat is the OR of 3 flags indicating what to set. (you can use this function to just set volume, 
// or just set panning) iWhat is the OR of DGL_SS_SA_FREQUENCY, DGL_SS_SA_VOLUME, DGL_SS_SA_PAN
// Returns: SYS_OK if succeeded, SYS_NOK otherwise
int
CSoundSystem::SetChannelAttributes(int iChannelCodeID, int iVolume, int iPan, int iFrequency, int iWhat)
{
	CStdString		sToLog;
	int				iResult;

	// use a mutex so more than 1 thread can simultaneously access soundchannels.
	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		if(CheckIfValidChannel(iChannelCodeID))
		{
			// legal iChannelCodeID. Set Attributes
			iResult=m_scSoundChannelStore[iChannelCodeID].SetAttributes(iVolume, iPan, iFrequency, iWhat);
		}
		else
		{
			// error, not known channel. abort.
			sToLog.Format("Couldn't set attributes on sound channel %d. Channel not found.",iChannelCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::SetChannelAttributes",false);
			LeaveCriticalSection(&m_csSoundChannelStoreMutex);
			return SYS_NOK;
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
	if(iResult<0)
	{
		sToLog.Format("Couldn't set attributes on sound channel %d. Errorcode: %d.",iChannelCodeID,iResult);
		LogFeedback(&sToLog[0],true,"CSoundSystem::SetChannelAttributes",false);
		return SYS_NOK;
	}
	return SYS_OK;
}


// Purpose: sets the EAXMix value for the given channel
// Returns: SYS_OK if succeeded, SYS_NOK Otherwise
int
CSoundSystem::SetEAXMixChannel(int iChannelCodeID, float fMixValue)
{
	CStdString		sToLog;
	int				iResult;

	// use a mutex so more than 1 thread can simultaneously access soundchannels.
	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		if(CheckIfValidChannel(iChannelCodeID))
		{
			iResult=m_scSoundChannelStore[iChannelCodeID].SetEAXMix(fMixValue);
		}
		else
		{
			// error, not known channel. abort.
			sToLog.Format("Couldn't set EAX Mix value for sound channel %d. Channel not found.",iChannelCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::SetEAXMixChannel",false);
			LeaveCriticalSection(&m_csSoundChannelStoreMutex);
			return SYS_NOK;
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
	if(iResult<0)
	{
		sToLog.Format("Couldn't set EAX Mix value for sound channel %d. Errorcode: %d.",iChannelCodeID, iResult);
		LogFeedback(&sToLog[0],true,"CSoundSystem::SetEAXMixChannel",false);
		return SYS_NOK;
	}
	return SYS_OK;
}


// Purpose: sets the 3D attributes for the given element
// Returns: SYS_OK if succeeded, SYS_NOK Otherwise
int
CSoundSystem::SetElement3DAttributes(int iElementCodeID, int iElementType, SSoundElement3DAttributes *p3DAttributes)
{
	CStdString		sToLog;
	int				iResult;

	// use a mutex so more than 1 thread can simultaneously access soundelements.
	EnterCriticalSection(&m_csSoundElementStoreMutex);
		if((iElementCodeID<DGL_SS_AMLOADABLESOUNDELEMENTS)&&(iElementCodeID>=0)&&
				m_pseSoundElementStore[iElementCodeID])
		{
			switch(iElementType)
			{
				case DGL_SS_MODELEMENT:
				case DGL_SS_SAMPLEELEMENT:
				case DGL_SS_MP3ELEMENT:
				{
					if(!p3DAttributes)
					{
						iResult=DGL_SS_ERR_OPERATIONFAILED;
					}
					else
					{
						iResult=m_pseSoundElementStore[iElementCodeID]->Set3DAttributes(p3DAttributes);
					}
				}; break;
				default:
				{
					// passed unknown element type. 
					sToLog.Format("Couldn't set 3D attributes for sound element %d. Unknown type: %d",iElementCodeID, iElementType);
					LogFeedback(&sToLog[0],true,"CSoundSystem::SetElement3DAttributes",false);
					LeaveCriticalSection(&m_csSoundElementStoreMutex);
					return DGL_SS_ERR_ILLEGALELEMENTTYPE;
				}; break;
			}
		}
		else
		{
			// error, not known element. abort.
			sToLog.Format("Couldn't set 3D attributes for sound element %d. Element not found/not loaded.",iElementCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::SetElement3DAttributes",false);
			LeaveCriticalSection(&m_csSoundElementStoreMutex);
			return DGL_SS_ERR_ILLEGALELEMENTID;
		}
	LeaveCriticalSection(&m_csSoundElementStoreMutex);
	if(iResult<0)
	{
		sToLog.Format("Couldn't set 3D attributes for sound element %d. Errorcode: %d.",iElementCodeID, iResult);
		LogFeedback(&sToLog[0],true,"CSoundSystem::SetElement3DAttributes",false);
		return DGL_SS_ERR_OPERATIONFAILED;
	}
	return SYS_OK;
}


// Purpose: sets the 3D attributes for the given channel
// Returns: SYS_OK if succeeded, SYS_NOK Otherwise
int
CSoundSystem::SetChannel3DAttributes(int iChannelCodeID, SSoundElement3DAttributes *p3DAttributes)
{
	CStdString		sToLog;
	int				iResult;

	// use a mutex so more than 1 thread can simultaneously access soundchannels.
	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		if(CheckIfValidChannel(iChannelCodeID))
		{
			iResult=m_scSoundChannelStore[iChannelCodeID].Set3DAttributes(p3DAttributes);
		}
		else
		{
			// error, not known channel. abort.
			sToLog.Format("Couldn't set 3D attributes for sound channel %d. Channel not found.",iChannelCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::SetChannel3DAttributes",false);
			LeaveCriticalSection(&m_csSoundChannelStoreMutex);
			return SYS_NOK;
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
	if(iResult<0)
	{
		sToLog.Format("Couldn't set 3D attributes for sound channel %d. Errorcode: %d.",iChannelCodeID, iResult);
		LogFeedback(&sToLog[0],true,"CSoundSystem::SetChannel3DAttributes",false);
		return SYS_NOK;
	}
	return SYS_OK;
}


// Purpose: sets the 3D Position for the given channel
// Returns: SYS_OK if succeeded, SYS_NOK Otherwise
int
CSoundSystem::SetChannel3DPosition(int iChannelCodeID, SSoundChannel3DPosition *p3DPosition)
{
	CStdString		sToLog;
	int				iResult;

	// use a mutex so more than 1 thread can simultaneously access soundchannels.
	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		if(CheckIfValidChannel(iChannelCodeID))
		{
			iResult=m_scSoundChannelStore[iChannelCodeID].Set3DPosition(p3DPosition);
		}
		else
		{
			// error, not known channel. abort.
			sToLog.Format("Couldn't set 3D position for sound channel %d. Channel not found.",iChannelCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::SetChannel3DPosition",false);
			LeaveCriticalSection(&m_csSoundChannelStoreMutex);
			return SYS_NOK;
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
	if(iResult<0)
	{
		sToLog.Format("Couldn't set 3D position for sound channel %d. Errorcode: %d.",iChannelCodeID, iResult);
		LogFeedback(&sToLog[0],true,"CSoundSystem::SetChannel3DPosition",false);
		return SYS_NOK;
	}
	return SYS_OK;
}


// Purpose: starts a given soundelement at the passed position and sets after that the 3D position and attributes
// if needed.
// Returns: iChannelCodeID of channel the play is started on if succeeded, errorcode if not.
int
CSoundSystem::StartElement(int iElementCodeID, int iElementType, long lStartPos, 
			bool bLoop, char *pszChannelScriptID, SSoundChannel3DPosition *p3DPosition)
{
	CStdString		sToLog;
	int				iResult, iChannelCodeID;

	// use a mutex so more than 1 thread can simultaneously access soundelements.
	EnterCriticalSection(&m_csSoundElementStoreMutex);
		if((iElementCodeID<DGL_SS_AMLOADABLESOUNDELEMENTS)&&(iElementCodeID>=0)&&
				m_pseSoundElementStore[iElementCodeID])
		{
			// legal elementID, reserve channel
			iChannelCodeID=ReserveNewSoundChannelInStore();
			if(iChannelCodeID<0)
			{
				// failed.
				sToLog.Format("Couldn't start sound element %d. Couldn't reserve new channel. ",iElementCodeID, iElementType);
				LogFeedback(&sToLog[0],true,"CSoundSystem::StartElement",false);
				LeaveCriticalSection(&m_csSoundElementStoreMutex);
				return DGL_SS_ERR_CHANNELSTOREFULL;
			}

			switch(iElementType)
			{
				case DGL_SS_MODELEMENT:
				case DGL_SS_SAMPLEELEMENT:
				{
					if(lStartPos>0)
					{
						// if there was supplied a startposition set this first.
						m_pseSoundElementStore[iElementCodeID]->SetStartPos((DWORD)lStartPos);
					}
					// play the element on the given channel
					iResult=m_pseSoundElementStore[iElementCodeID]->Play(true, bLoop,
								&m_scSoundChannelStore[iChannelCodeID],pszChannelScriptID);
				}; break;
				case DGL_SS_MP3ELEMENT:
				{
					// play the element on the given channel
					iResult=m_pseSoundElementStore[iElementCodeID]->Play(true, bLoop,
								&m_scSoundChannelStore[iChannelCodeID],pszChannelScriptID);

					if(iResult==SYS_OK)
					{
						// seek in channel
						if(lStartPos>0)
						{
							iResult=SeekInChannel(iChannelCodeID, lStartPos);
						}
					}
				}; break;
				default:
				{
					// passed unknown element type. 
					sToLog.Format("Couldn't start sound element %d. Unknown type: %d",iElementCodeID, iElementType);
					LogFeedback(&sToLog[0],true,"CSoundSystem::StartElement",false);
					LeaveCriticalSection(&m_csSoundElementStoreMutex);
					return DGL_SS_ERR_ILLEGALELEMENTTYPE;
				}; break;
			}

			if(iResult==SYS_OK)
			{
				// succeeded, set 3D attributes into channel if channel uses 3D.
				if(m_pseSoundElementStore[iElementCodeID]->GetUses3D())
				{
					// set 3D attributes
					iResult=m_scSoundChannelStore[iChannelCodeID].Set3DAttributes(
											m_pseSoundElementStore[iElementCodeID]->Get3DAttributes());
					if((iResult==SYS_OK)&&(p3DPosition))
					{
						// set 3D position
						iResult=m_scSoundChannelStore[iChannelCodeID].Set3DPosition(p3DPosition);
					}
					if(iResult!=SYS_OK)
					{
						// error occured.
						sToLog.Format("Couldn't set sound element %d's 3D attributes.",iElementCodeID);
						LogFeedback(&sToLog[0],true,"CSoundSystem::StartElement",false);
						LeaveCriticalSection(&m_csSoundElementStoreMutex);
						return DGL_SS_ERR_CHANNELACTIONFAILED;
					}
				}
			}
			else
			{
				sToLog.Format("Couldn't start sound element %d. Play and/or seek failed.",iElementCodeID);
				LogFeedback(&sToLog[0],true,"CSoundSystem::StartElement",false);
				LeaveCriticalSection(&m_csSoundElementStoreMutex);
				return DGL_SS_ERR_OPERATIONFAILED;
			}
		}
		else
		{
			// error, not known element. abort.
			sToLog.Format("Couldn't start sound element %d. Element not found/not loaded",iElementCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::StartElement",false);
			LeaveCriticalSection(&m_csSoundElementStoreMutex);
			return DGL_SS_ERR_ILLEGALELEMENTID;
		}
	LeaveCriticalSection(&m_csSoundElementStoreMutex);

	// return channelcodeid
	return iChannelCodeID;
}


// Purpose: stops the playing of a given soundchannel. Because stopped channels can't be resumed (only
// paused channels can), the channel will be detached
// Returns: SYS_OK if succeeded, SYS_NOK otherwise
int
CSoundSystem::StopChannel(int iChannelCodeID)
{
	CStdString		sToLog;
	int				iResult;

	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		if(CheckIfValidChannel(iChannelCodeID))
		{
			// always detach from element, so just call detach, which will stop the sound.
			iResult=m_scSoundChannelStore[iChannelCodeID].DetachFromElement();
		}
		else
		{
			// error, not known channel. abort.
			sToLog.Format("Couldn't stop sound channel %d. Channel not found.",iChannelCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::StopChannel",false);
			LeaveCriticalSection(&m_csSoundChannelStoreMutex);
			return SYS_NOK;
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
	if(iResult<0)
	{
		sToLog.Format("Couldn't stop sound channel %d. Errorcode: %d.",iChannelCodeID, iResult);
		LogFeedback(&sToLog[0],true,"CSoundSystem::StopChannel",false);
		return SYS_NOK;
	}
	return SYS_OK;
}


// Purpose: stops all playing soundchannels.
// Returns: SYS_OK if succeeded, SYS_NOK otherwise
int
CSoundSystem::StopAll()
{
	CStdString		sToLog;
	int				i, iReturnValue, iResult;

	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		if(m_bInitialized)
		{
			iReturnValue=SYS_OK;

			// detach all channels
			for(i=0;i<m_iMaxSoundChannelsInUse;i++)
			{
				if(m_scSoundChannelStore[i].GetInUse())
				{
					iResult=m_scSoundChannelStore[i].Stop();
					if(iResult < 0)
					{
						// Something went wrong. log error.
						sToLog.Format("Couldn't stop channel %d (%s). Errorcode: %d",
							m_scSoundChannelStore[i].GetChannelCodeID(),m_scSoundChannelStore[i].GetChannelScriptID(),
							iResult);
						LogFeedback(&sToLog[0],true,"CSoundSystem::StopAll",false);
						iReturnValue=SYS_NOK;
					}
					m_scSoundChannelStore[i].DetachFromElement();
				}
			}
		}
		else
		{
			iReturnValue=SYS_NOK;
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
	return iReturnValue;
}


// Purpose: pauses all playing soundchannels
// Returns: SYS_OK if succeeded, SYS_NOK otherwise
int
CSoundSystem::PauseAll()
{
	BOOL		bResult;
	CStdString	sToLog;
	int			iReturnValue;

	if(m_bInitialized)
	{
		// Call BASS routine to pause
		bResult=BASS_Pause();
		if(!bResult)
		{
			// Something went wrong. log error.
			sToLog.Format("Couldn't pause all playing channels. BASS Errorcode: %d",BASS_ErrorGetCode());
			LogFeedback(&sToLog[0],true,"CSoundSystem::PauseAll",false);
			iReturnValue=SYS_NOK;
		}
		else
		{
			iReturnValue=SYS_OK;
		}
	}
	else
	{
		iReturnValue=SYS_NOK;
	}
	return iReturnValue;
}


// Purpose: continues all PAUSED soundchannels
// Returns: SYS_OK if succeeded, SYS_NOK otherwise
int
CSoundSystem::ContinueAll()
{
	BOOL		bResult;
	CStdString	sToLog;
	int			iReturnValue;

	if(m_bInitialized)
	{
		// Call BASS routine to continu
		bResult=BASS_Start();
		if(!bResult)
		{
			// Something went wrong. log error.
			sToLog.Format("Couldn't continue all paused channels. BASS Errorcode: %d",BASS_ErrorGetCode());
			LogFeedback(&sToLog[0],true,"CSoundSystem::ContinueAll",false);
			iReturnValue=SYS_NOK;
		}
		else
		{
			iReturnValue=SYS_OK;
		}
	}
	else
	{
		iReturnValue=SYS_NOK;
	}
	return iReturnValue;
}


// Purpose: sets overall volume. Will affect all playing elements
// Returns: SYS_OK if succeeded, SYS_NOK otherwise
int
CSoundSystem::SetVolume(int iVolume)
{
	BOOL		bResult;
	CStdString	sToLog;
	int			iReturnValue;

	EnterCriticalSection(&m_csSoundSystemAccessMutex);
		if(m_bInitialized)
		{
			// Call BASS routine to set volume
			bResult=BASS_SetVolume((DWORD)iVolume);
			if(!bResult)
			{
				// Something went wrong. log error.
				sToLog.Format("Couldn't set overall volume. BASS Errorcode: %d",BASS_ErrorGetCode());
				LogFeedback(&sToLog[0],true,"CSoundSystem::SetVolume",false);
				iReturnValue=SYS_NOK;
			}
			else
			{
				iReturnValue=SYS_OK;
			}
		}
		else
		{
			iReturnValue=SYS_NOK;
		}
	LeaveCriticalSection(&m_csSoundSystemAccessMutex);
	return iReturnValue;
}


// Purpose: Pauses the playing of a given soundchannel
// Returns: SYS_OK if succeeded, SYS_NOK otherwise
int
CSoundSystem::PauseChannel(int iChannelCodeID)
{
	CStdString		sToLog;
	int				iResult;

	// use a mutex so more than 1 thread can simultaneously access soundchannels.
	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		if(CheckIfValidChannel(iChannelCodeID))
		{
			iResult=m_scSoundChannelStore[iChannelCodeID].Pause();
		}
		else
		{
			// error, not known channel. abort.
			sToLog.Format("Couldn't pause sound channel %d. Channel not found.",iChannelCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::PauseChannel",false);
			LeaveCriticalSection(&m_csSoundChannelStoreMutex);
			return SYS_NOK;
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
	if(iResult<0)
	{
		sToLog.Format("Couldn't pause sound channel %d. Errorcode: %d.",iChannelCodeID, iResult);
		LogFeedback(&sToLog[0],true,"CSoundSystem::PauseChannel",false);
		return SYS_NOK;
	}
	return SYS_OK;
}


// Purpose: Continues the playing of a paused soundchannel
// Returns: SYS_OK if succeeded, SYS_NOK otherwise
int
CSoundSystem::ContinueChannel(int iChannelCodeID)
{
	CStdString		sToLog;
	int				iResult;

	// use a mutex so more than 1 thread can simultaneously access soundchannels.
	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		if(CheckIfValidChannel(iChannelCodeID))
		{
			iResult=m_scSoundChannelStore[iChannelCodeID].Continue();
		}
		else
		{
			// error, not known channel. abort.
			sToLog.Format("Couldn't continue sound channel %d. Channel not found.",iChannelCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::ContinueChannel",false);
			LeaveCriticalSection(&m_csSoundChannelStoreMutex);
			return SYS_NOK;
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
	if(iResult<0)
	{
		sToLog.Format("Couldn't continue sound channel %d. Errorcode: %d.",iChannelCodeID, iResult);
		LogFeedback(&sToLog[0],true,"CSoundSystem::ContinueChannel",false);
		return SYS_NOK;
	}
	return SYS_OK;
}


// Purpose: seeks to the specified position in the given soundelement.
// Returns: SYS_OK if succeeded, SYS_NOK otherwise, like when the seeking is notpossible.
int
CSoundSystem::SeekInChannel(int iChannelCodeID, long lStartPos)
{
	CStdString		sToLog;
	int				iResult;

	// use a mutex so more than 1 thread can simultaneously access soundchannels.
	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		if(CheckIfValidChannel(iChannelCodeID))
		{
			iResult=m_scSoundChannelStore[iChannelCodeID].Seek(lStartPos);
		}
		else
		{
			// error, not known channel. abort.
			sToLog.Format("Couldn't seek in sound channel %d. Channel not found.",iChannelCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::SeekInChannel",false);
			LeaveCriticalSection(&m_csSoundChannelStoreMutex);
			return SYS_NOK;
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
	if(iResult<0)
	{
		sToLog.Format("Couldn't seek in sound channel %d. Errorcode: %d.",iChannelCodeID, iResult);
		LogFeedback(&sToLog[0],true,"CSoundSystem::SeekInChannel",false);
		return SYS_NOK;
	}
	return SYS_OK;
}


// Purpose: will create a sync in the channel given with the parameters passed
// Will return the syncID usable with the channel given as reference to this sync.
// When error, DGL_SS_ERR_CHANNELNOTINUSE is returned, or other error reported by channel.
int
CSoundSystem::CreateSyncChannel(int iChannelCodeID, DWORD dwSyncType, DWORD dwSyncParam, DWORD dwSyncFlag)
{
	CStdString		sToLog;
	int				iSyncID;

	// use a mutex so more than 1 thread can simultaneously access soundchannels.
	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		if(CheckIfValidChannel(iChannelCodeID))
		{
			iSyncID=m_scSoundChannelStore[iChannelCodeID].CreateSync(dwSyncType, dwSyncParam, dwSyncFlag);
		}
		else
		{
			// error, not known channel. abort.
			sToLog.Format("Couldn't create sync in sound channel %d. Channel not found.",iChannelCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::CreateSyncChannel",false);
			LeaveCriticalSection(&m_csSoundChannelStoreMutex);
			return DGL_SS_ERR_CHANNELNOTINUSE;
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);

	// return iSyncID
	return iSyncID;
}


// Purpose: will delete the sync with id iSyncID from the given channel
// Returns SYS_OK if succeeded, SYS_NOK otherwise
int
CSoundSystem::DeleteSyncChannel(int iChannelCodeID, int iSyncID)
{
	CStdString		sToLog;
	int				iResult;

	// use a mutex so more than 1 thread can simultaneously access soundchannels.
	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		if(CheckIfValidChannel(iChannelCodeID))
		{
			iResult=m_scSoundChannelStore[iChannelCodeID].DeleteSync(iSyncID);
		}
		else
		{
			// error, not known channel. abort.
			sToLog.Format("Couldn't delete sync %d in sound channel %d. Channel not found.", iSyncID, iChannelCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::DeleteSyncChannel",false);
			LeaveCriticalSection(&m_csSoundChannelStoreMutex);
			return SYS_NOK;
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
	if(iResult<0)
	{
		sToLog.Format("Couldn't delete sync %d in sound channel %d. Errorcode: %d.",iSyncID, iChannelCodeID, iResult);
		LogFeedback(&sToLog[0],true,"CSoundSystem::DeleteSyncChannel",false);
		return SYS_NOK;
	}
	return SYS_OK;
}


// Purpose: will delete all the syncs from the given channel
// Returns SYS_OK if succeeded, SYS_NOK otherwise
int
CSoundSystem::DeleteAllSyncsChannel(int iChannelCodeID)
{
	CStdString		sToLog;
	int				iResult;

	// use a mutex so more than 1 thread can simultaneously access soundchannels.
	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		if(CheckIfValidChannel(iChannelCodeID))
		{
			iResult=m_scSoundChannelStore[iChannelCodeID].DeleteAllSyncs();
		}
		else
		{
			// error, not known element. abort.
			sToLog.Format("Couldn't delete all syncs in sound channel %d. Channel not found.", iChannelCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::DeleteAllSyncsChannel",false);
			LeaveCriticalSection(&m_csSoundChannelStoreMutex);
			return SYS_NOK;
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
	if(iResult<0)
	{
		sToLog.Format("Couldn't delete all syncs in sound channel %d. Errorcode: %d.", iChannelCodeID, iResult);
		LogFeedback(&sToLog[0],true,"CSoundSystem::DeleteAllSyncsChannel",false);
		return SYS_NOK;
	}
	return SYS_OK;
}


// Purpose: will execute the sync in the given channel. Because this routine is called from the
// sync callback, that routine receives just the ChannelCodeID from BASS. 
// There is no errorlogging for performance reasons.
void
CSoundSystem::ExecuteSyncChannel(int iChannelCodeID, HSYNC hSyncHandle, DWORD dwSyncData)
{
	// use a mutex so more than 1 thread can simultaneously access soundchannels.
	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		if(CheckIfValidChannel(iChannelCodeID))
		{
			// execute it
			m_scSoundChannelStore[iChannelCodeID].ExecuteSync(hSyncHandle, dwSyncData);
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
}


// Purpose: Initialises the soundsystem. It uses the flags and options set with methods
// of the soundsystem object to initialise the BASS subsystem. If succeeded, SYS_OK is
// returned, otherwise: SYS_NOK. There is no error recovery in this function. A failure
// of the initialisation is not recovered.
int
CSoundSystem::InitSystem()
{
	if(!BASS_Init(m_iDevice,m_dwFrequency,m_dwBassSystemFlags,m_gpDemoDat->GethWnd()))
	{
		// didn't succeed.
		m_bInitialized=false;
		return SYS_NOK;
	}
	if(!BASS_Start())
	{
		m_bInitialized=false;
		return SYS_NOK;
	}
	// Set volume curves to logaritmic. DSound uses Logaritmic curves for panning and volume.
	BASS_SetLogCurves(TRUE,FALSE);

	m_bInitialized=true;
	return SYS_OK;
}


// Purpose: frees all resources and stops all sounds. It also frees BASS subsystem.
// Returns: SYS_OK if succeeded, SYS_NOK otherwise
int
CSoundSystem::FreeSystem()
{
	int				i;

	if(m_bInitialized)
	{
		// system was initialized, so end all current playing audio if that's not
		// shut off already, and end BASS.
		StopAll();
		m_bInitialized=false;
		m_dwBassSystemFlags=0;
		m_dwFrequency=0;
		m_iDevice=-1;

		// free all Soundelement objects.
		for(i=0;i<DGL_SS_AMLOADABLESOUNDELEMENTS;i++)
		{
			if(m_pseSoundElementStore[i])
			{
				FreeSoundElement(m_pseSoundElementStore[i]->GetElementCodeID(),m_pseSoundElementStore[i]->GetElementType(),true);
			}
		}

		// Detach all soundchannels
		for(i=0;i<DGL_SS_AMCHANNELS;i++)
		{
			m_scSoundChannelStore[i].DetachFromElement();
		}

		// free resources etc.
		BASS_Free();
	}
	// Done.
	return SYS_OK;
}


// Purpose: adds a set of flags to the already stored basssystem flags.
void
CSoundSystem::AddBassSystemFlags(DWORD dwFlags)
{
	m_dwBassSystemFlags|=dwFlags;
}


// Purpose: Returns the current stored bass system flags.
DWORD
CSoundSystem::GetBassSystemFlags()
{
	return m_dwBassSystemFlags;
}


// Purpose: Removes the specified flags from the stored set of systemflags
void
CSoundSystem::RemoveBassSystemFlags(DWORD dwFlags)
{
	m_dwBassSystemFlags&=!dwFlags;
}


// Purpose: sets the set of systemflags. Overwrites any current set.
void
CSoundSystem::SetBassSystemFlags(DWORD dwFlags)
{
	m_dwBassSystemFlags=dwFlags;
}


// Purpose: sets the devicenumber and frequency to use. If already initialized,
// the function will ignore the passed data.
void
CSoundSystem::SetDeviceAndFrequency(int iDevice, DWORD dwFrequency)
{
	if(!m_bInitialized)
	{
		m_iDevice=iDevice;
		m_dwFrequency=dwFrequency;
	}
}


// Purpose: Loads a soundelement and stores it in the appropriate object inside the SoundSystem.
// If succeeded, the ElementCodeID is returned. If failed DGL_SS_ERR_COULDNTLOAD is returned, or other
// appropriate error. No channels are attached now, that is done when the element is started.
// When an element is already loaded, it just increases the reference counter to the element. 
int
CSoundSystem::LoadSoundElement(char *pszFilename, int iElementType, char *pszElementScriptID, bool bUses3D)
{
	int					iElementCodeID, iResult;
	byte				*pFileData;
	long				lFileLength;
	CStdString			sToLog;
	HSTREAM				hsMP3;
	HMUSIC				hsMOD;
	HSAMPLE				hsSMPL;
	DWORD				dwBassFlags;
	CMP3Element			*pMP3Element;
	CSampleElement		*pSampleElement;
	CMODElement			*pMODElement;

	// use a mutex so more than 1 thread can simultaneously load soundelements.
	EnterCriticalSection(&m_csSoundSystemAccessMutex);
		// check if there is already an element with this ID loaded. If so increase reference counter.
		if(pszElementScriptID)
		{
			iElementCodeID=ConvertElementScriptID2ElementCodeID(pszElementScriptID);
			if(iElementCodeID>=0)
			{
				// didn't report an error, so it is found in the base.
				// already loaded. just increase the reference counter and return iELementCodeID
				m_pseSoundElementStore[iElementCodeID]->IncReferences();
				sToLog.Format("Sound element with ID %s already loaded. Increased reference.", pszElementScriptID);
				LogFeedback(&sToLog[0],true,"CSoundSystem::LoadSoundElement",false);
				LeaveCriticalSection(&m_csSoundSystemAccessMutex);
				return iElementCodeID;
			}
		}

		// Load the elementdata
		pFileData=DEMOGL_FileLoad(pszFilename);
		if(!pFileData)
		{
			// not succeeded.
			sToLog.Format("Couldn't load sound element %s.",pszFilename);
			LogFeedback(&sToLog[0],true,"CSoundSystem::LoadSoundElement",false);
			LeaveCriticalSection(&m_csSoundSystemAccessMutex);
			return DGL_SS_ERR_COULDNTLOAD;
		}
		// file loaded. get length of file
		lFileLength=DEMOGL_FileGetLength(pszFilename);

		// Reserve new soundelement in the store.
		iElementCodeID=ReserveNewSoundElementInStore(iElementType);

		if(iElementCodeID<0)
		{
			// no empty slot found. slots are full. 
			DEMOGL_FileFree(pFileData);
			sToLog.Format("Couldn't load sound element %s. All available slots are full. Clear some elements.",pszFilename, iElementType);
			LogFeedback(&sToLog[0],true,"CSoundSystem::LoadSoundElement",false);
			LeaveCriticalSection(&m_csSoundSystemAccessMutex);
			return DGL_SS_ERR_ELEMENTSTOREFULL;
		}

		// store data in object of type iElementType
		switch(iElementType)
		{
			case DGL_SS_MP3ELEMENT:
			{
				pMP3Element=dynamic_cast<CMP3Element*>(m_pseSoundElementStore[iElementCodeID]);

				// Select flags.
				dwBassFlags=BASS_MP3_SETPOS;
				if(bUses3D)
				{
					dwBassFlags|=BASS_SAMPLE_3D;
				}
				// Create stream.
				hsMP3=BASS_StreamCreateFile(TRUE,pFileData,0,(DWORD)lFileLength,dwBassFlags);
				if(!hsMP3)
				{
					// Couldn't init the stream. 
					EnterCriticalSection(&m_csSoundElementStoreMutex);
						delete m_pseSoundElementStore[iElementCodeID];
						m_pseSoundElementStore[iElementCodeID]=NULL;
					LeaveCriticalSection(&m_csSoundElementStoreMutex);

					DEMOGL_FileFree(pFileData);
					sToLog.Format("Couldn't load MP3 sound element %s. Initialisation in BASS failed. Errorcode: %d",
									pszFilename, BASS_ErrorGetCode());
					LogFeedback(&sToLog[0],true,"CSoundSystem::LoadSoundElement",false);
					LeaveCriticalSection(&m_csSoundSystemAccessMutex);
					return DGL_SS_ERR_COULDNTLOAD;
				}
				// store streamhandle plus other data in the object
				pMP3Element->SetBassHandle(hsMP3);
				pMP3Element->SetElementData(pFileData,lFileLength,dwBassFlags, iElementType,iElementCodeID,pszElementScriptID, bUses3D);
			}; break;
			case DGL_SS_MODELEMENT:
			{
				pMODElement=dynamic_cast<CMODElement*>(m_pseSoundElementStore[iElementCodeID]);

				// Select flags.
				dwBassFlags=BASS_MUSIC_RAMPS|BASS_MUSIC_PT1MOD;
				if(bUses3D)
				{
					dwBassFlags|=BASS_MUSIC_3D;
				}
				// Create music
				hsMOD=BASS_MusicLoad(TRUE,pFileData,0,(DWORD)lFileLength,dwBassFlags);
				if(!hsMOD)
				{
					// Couldn't init the music 
					EnterCriticalSection(&m_csSoundElementStoreMutex);
						delete m_pseSoundElementStore[iElementCodeID];
						m_pseSoundElementStore[iElementCodeID]=NULL;
					LeaveCriticalSection(&m_csSoundElementStoreMutex);

					DEMOGL_FileFree(pFileData);
					sToLog.Format("Couldn't load MOD sound element %s. Initialisation in BASS failed. Errorcode: %d",
									pszFilename, BASS_ErrorGetCode());
					LogFeedback(&sToLog[0],true,"CSoundSystem::LoadSoundElement",false);
					LeaveCriticalSection(&m_csSoundSystemAccessMutex);
					return DGL_SS_ERR_COULDNTLOAD;
				}
				// store musichandle plus other data in the object
				pMODElement->SetBassHandle(hsMOD);
				pMODElement->SetElementData(pFileData,lFileLength,dwBassFlags, iElementType,iElementCodeID,pszElementScriptID, bUses3D);
			}; break;
			case DGL_SS_SAMPLEELEMENT:
			{
				pSampleElement=dynamic_cast<CSampleElement*>(m_pseSoundElementStore[iElementCodeID]);

				// Select flags.
				dwBassFlags=BASS_SAMPLE_VAM;
				if(bUses3D)
				{
					dwBassFlags|=BASS_SAMPLE_3D|BASS_SAMPLE_MUTEMAX|BASS_SAMPLE_OVER_DIST;
				}
				// Create sample. use max concurrent replay value (which is 65535, see BASS Docs)
				hsSMPL=BASS_SampleLoad(TRUE,pFileData,0,(DWORD)lFileLength,65535,dwBassFlags);

				if(!hsSMPL)
				{
					// Couldn't init the sample. 
					EnterCriticalSection(&m_csSoundElementStoreMutex);
						delete m_pseSoundElementStore[iElementCodeID];
						m_pseSoundElementStore[iElementCodeID]=NULL;
					LeaveCriticalSection(&m_csSoundElementStoreMutex);

					DEMOGL_FileFree(pFileData);
					sToLog.Format("Couldn't load sample sound element %s. Initialisation in BASS failed. Errorcode: %d",
									pszFilename, BASS_ErrorGetCode());
					LogFeedback(&sToLog[0],true,"CSoundSystem::LoadSoundElement",false);
					LeaveCriticalSection(&m_csSoundSystemAccessMutex);
					return DGL_SS_ERR_COULDNTLOAD;
				}
				// store samplehandle plus other data in the object
				pSampleElement->SetBassHandle(hsSMPL);
				pSampleElement->SetElementData(pFileData,lFileLength,dwBassFlags, iElementType,iElementCodeID,pszElementScriptID, bUses3D);
			}; break;
			default:
			{
				// passed unknown element type. free data and return error
				EnterCriticalSection(&m_csSoundElementStoreMutex);
					delete m_pseSoundElementStore[iElementCodeID];
					m_pseSoundElementStore[iElementCodeID]=NULL;
				LeaveCriticalSection(&m_csSoundElementStoreMutex);

				DEMOGL_FileFree(pFileData);
				sToLog.Format("Couldn't load sound element %s. Unknown type: %d",pszFilename, iElementType);
				LogFeedback(&sToLog[0],true,"CSoundSystem::LoadSoundElement",false);
				LeaveCriticalSection(&m_csSoundSystemAccessMutex);
				return DGL_SS_ERR_COULDNTLOAD;
			}; break;
		}

		// Initialize the element.
		iResult=m_pseSoundElementStore[iElementCodeID]->Init();
		if(iResult!=SYS_OK)
		{
			// error.
			EnterCriticalSection(&m_csSoundElementStoreMutex);
				delete m_pseSoundElementStore[iElementCodeID];
				m_pseSoundElementStore[iElementCodeID]=NULL;
			LeaveCriticalSection(&m_csSoundElementStoreMutex);

			DEMOGL_FileFree(pFileData);
			sToLog.Format("Couldn't load sound element %s. Initialisation in BASS failed. Errorcode: %d",
							pszFilename, BASS_ErrorGetCode());
			LogFeedback(&sToLog[0],true,"CSoundSystem::LoadSoundElement",false);
			LeaveCriticalSection(&m_csSoundSystemAccessMutex);
			return DGL_SS_ERR_INITOFELEMENTFAILED;
		}
		// Done ok, increase the reference to this element, we can do this now safely.
		m_pseSoundElementStore[iElementCodeID]->IncReferences();
	LeaveCriticalSection(&m_csSoundSystemAccessMutex);

	// all clear, return iElementCodeID
	return iElementCodeID;
}


// Purpose: Detaches all channels from the given element. 
void
CSoundSystem::DetachAllChannelsFromElement(int iElementCodeID)
{
	int			i;

	EnterCriticalSection(&m_csSoundChannelStoreMutex);
		// walk all soundchannels currently in use.
		for(i=0;i<m_iMaxSoundChannelsInUse;i++)
		{
			if(m_scSoundChannelStore[i].GetParentElementCodeID()==iElementCodeID)
			{
				// the given element is playing on this channel, detach it.
				m_scSoundChannelStore[i].DetachFromElement();
			}
		}
	LeaveCriticalSection(&m_csSoundChannelStoreMutex);
}


// Purpose: frees a loaded soundelement. if bAlwaysFree is true, the element is always freed, no matter how many references
// there are to this element. If it's false, it's only freed when there are no more references to the element.
// When there is more than 1 reference to the element, and bAlwaysFree is false, the reference counter is just decreased and
// nothing is freed nor stopped. The caller should have stopped the play of the channels it started beforehand.
// When the element is indeed freed, channels where this element is playing on are also detached.
// Returns SYS_OK if everything was allright, or an errorvalue if an error occured. For the caller it's as if the element
// is always freed, even if the reference is just decreased.
int
CSoundSystem::FreeSoundElement(int iElementCodeID, int iElementType, bool bAlwaysFree)
{
	CStdString			sToLog;
	CMP3Element			*pMP3Element;
	CSampleElement		*pSampleElement;
	CMODElement			*pMODElement;

	// use a mutex so more than 1 thread can simultaneously access soundelements.
	EnterCriticalSection(&m_csSoundElementStoreMutex);
		if((iElementCodeID<DGL_SS_AMLOADABLESOUNDELEMENTS)&&(iElementCodeID>=0)&&
				m_pseSoundElementStore[iElementCodeID])
		{
			// decrease references.
			m_pseSoundElementStore[iElementCodeID]->DecReferences();

			if((m_pseSoundElementStore[iElementCodeID]->GetReferences()>0) && !bAlwaysFree)
			{
				// still references to this element. return SYS_OK, like everything went ok.
				LeaveCriticalSection(&m_csSoundElementStoreMutex);
				return SYS_OK;
			}

			// Detach element from all channels it's currently playing on.
			DetachAllChannelsFromElement(iElementCodeID);

			switch(iElementType)
			{
				case DGL_SS_MP3ELEMENT:
				{
					pMP3Element=dynamic_cast<CMP3Element*>(m_pseSoundElementStore[iElementCodeID]);
					// Close stream
					BASS_StreamFree(pMP3Element->GetBassHandle());
					pMP3Element->SetBassHandle(NULL);
					pMP3Element->FreeElementData();
					delete pMP3Element;
					m_pseSoundElementStore[iElementCodeID]=NULL;
				}; break;
				case DGL_SS_MODELEMENT:
				{
					pMODElement=dynamic_cast<CMODElement*>(m_pseSoundElementStore[iElementCodeID]);
					// Close stream
					BASS_MusicFree(pMODElement->GetBassHandle());
					pMODElement->SetBassHandle(NULL);
					pMODElement->FreeElementData();
					delete pMODElement;
					m_pseSoundElementStore[iElementCodeID]=NULL;
				}; break;
				case DGL_SS_SAMPLEELEMENT:
				{
					pSampleElement=dynamic_cast<CSampleElement*>(m_pseSoundElementStore[iElementCodeID]);
					// Close stream
					BASS_MusicFree(pSampleElement->GetBassHandle());
					pSampleElement->SetBassHandle(NULL);
					pSampleElement->FreeElementData();
					delete pSampleElement;
					m_pseSoundElementStore[iElementCodeID]=NULL;
				}; break;
				default:
				{
					// passed unknown element type. 
					sToLog.Format("Couldn't free sound element %d. Unknown type: %d",iElementCodeID, iElementType);
					LogFeedback(&sToLog[0],true,"CSoundSystem::FreeSoundElement",false);
					LeaveCriticalSection(&m_csSoundElementStoreMutex);
					return DGL_SS_ERR_ILLEGALELEMENTTYPE;
				}; break;
			}
		}
		else
		{
			// error, not known element. abort.
			sToLog.Format("Couldn't free sound element %d. Element not found/not loaded.",iElementCodeID);
			LogFeedback(&sToLog[0],true,"CSoundSystem::FreeSoundElement",false);
			LeaveCriticalSection(&m_csSoundElementStoreMutex);
			return DGL_SS_ERR_ILLEGALELEMENTID;
		}
	LeaveCriticalSection(&m_csSoundElementStoreMutex);
	return SYS_OK;
}


// Purpose: sets the overall system EAX environment settings. This will affect ALL currently playing sounds.
// Returns: SYS_OK if succeeded, SYS_NOK otherwise
int
CSoundSystem::SetEAXEnv(int iEAXEnvironment, float fReverbVolume, float fDecay,float fDamp)
{
	BOOL			bResult;
	CStdString		sToLog;
	int				iReturnValue;

	EnterCriticalSection(&m_csSoundSystemAccessMutex);
		if(m_bInitialized)
		{
			iReturnValue=SYS_OK;
			// Call BASS routine to set the parameters.
			bResult=BASS_SetEAXParameters(iEAXEnvironment, fReverbVolume, fDecay, fDamp);
			if(!bResult)
			{
				// Something went wrong. log error.
				sToLog.Format("Couldn't set EAXEnvironment. BASS Errorcode: %d",BASS_ErrorGetCode());
				LogFeedback(&sToLog[0],true,"CSoundSystem::SetEAXEnv",false);
				iReturnValue=SYS_NOK;
			}
		}
		else
		{
			iReturnValue=SYS_NOK;
		}
	LeaveCriticalSection(&m_csSoundSystemAccessMutex);
	return iReturnValue;
}


// Purpose: gets 3DSound enabled flag
bool
CSoundSystem::Get3DSoundEnabled()
{
	return m_b3DSoundEnabled;
}


// Purpose: gets A3D enabled flag
bool
CSoundSystem::GetA3DEnabled()
{
	return m_bA3DEnabled;
}


// Purpose: gets EAX enabled flag
bool
CSoundSystem::GetEAXEnabled()
{
	return m_bEAXEnabled;
}


// Purpose: sets 3Dsound enabled flag to the value passed
void
CSoundSystem::Set3DSoundEnabled(bool bYN)
{
	EnterCriticalSection(&m_csSoundSystemAccessMutex);
		m_b3DSoundEnabled=bYN;
	LeaveCriticalSection(&m_csSoundSystemAccessMutex);
}


// Purpose: sets A3D enabled flag to the value passed
void
CSoundSystem::SetA3DEnabled(bool bYN)
{
	EnterCriticalSection(&m_csSoundSystemAccessMutex);
		m_bA3DEnabled=bYN;
	LeaveCriticalSection(&m_csSoundSystemAccessMutex);
}


// Purpose: sets EAX enabled flag to the value passed
void
CSoundSystem::SetEAXEnabled(bool bYN)
{
	EnterCriticalSection(&m_csSoundSystemAccessMutex);
		m_bEAXEnabled=bYN;
	LeaveCriticalSection(&m_csSoundSystemAccessMutex);
}


// Purpose: sets the 3D position of the listener, directly into BASS.
// This set of position will influence ALL playing channels.
// returns SYS_OK if succeeded, SYS_NOK if failed.
int
CSoundSystem::SetListener3DPosition(SListener3DPosition *p3DPosition)
{	
	BOOL	bResult;

	if(m_bInitialized)
	{
		EnterCriticalSection(&m_csSoundSystemAccessMutex);
			bResult=BASS_Set3DPosition((BASS_3DVECTOR *)&p3DPosition->m_ssvPosition,
						(BASS_3DVECTOR *)&p3DPosition->m_ssvVelocity,
						(BASS_3DVECTOR *)&p3DPosition->m_ssvFront,
						(BASS_3DVECTOR *)&p3DPosition->m_ssvTop);
			if(bResult)
			{
				bResult=BASS_Apply3D();
				if(bResult)
				{
					return SYS_OK;
				}
			}
		LeaveCriticalSection(&m_csSoundSystemAccessMutex);
	}
	return SYS_NOK;
}


// Purpose: sets the 3D factors in BASS. These factors are influencing ALL playing channels with 3D functionality.
// returns SYS_OK if succeeded, SYS_NOK if failed.
int
CSoundSystem::Set3DFactors(float fDistanceFactor, float fRollOffFactor, float fDopplerFactor)
{
	BOOL	bResult;

	if(m_bInitialized)
	{
		EnterCriticalSection(&m_csSoundSystemAccessMutex);
			bResult=BASS_Set3DFactors(fDistanceFactor, fRollOffFactor, fDopplerFactor);
			if(bResult)
			{
				bResult=BASS_Apply3D();
				if(bResult)
				{
					return SYS_OK;
				}
			}
		LeaveCriticalSection(&m_csSoundSystemAccessMutex);
	}
	return SYS_NOK;
}


//////////////////////////////////////////////////////////////////////////////////////
//  Misc non class related, but soundsystem related functions (most utility functions)
//////////////////////////////////////////////////////////////////////////////////////


// Purpose: wrapper around CSoundSystem::ConvertElementScriptID2ElementCodeID routine to export that routine 
// as an API function. See CSoundSystem::ConvertElementScriptID2ElementCodeID for details.
int		
DEMOGL_SoundSystemElementScriptID2ElementCodeID(const char *pszElementScriptID)
{
	CStdString		sElementScriptID;

	sElementScriptID.Format("%s",pszElementScriptID);
	return m_gpSoundSystem->ConvertElementScriptID2ElementCodeID(&sElementScriptID[0]);
}


// Purpose: wrapper around CSoundSystem::ConvertChannelScriptID2ChannelCodeID routine to export that routine 
// as an API function. See CSoundSystem::ConvertChannelScriptID2ChannelCodeID for details.
int		
DEMOGL_SoundSystemChannelScriptID2ChannelCodeID(const char *pszChannelScriptID)
{
	CStdString		sChannelScriptID;

	sChannelScriptID.Format("%s", pszChannelScriptID);
	return m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(&sChannelScriptID[0]);
}


// Purpose: wrapper around CSoundSystem::LoadSoundElement routine to export that routine
// as an API function. See CSoundSystem::LoadSoundElement for details.
// Returns: ElementCodeID of soundelement loaded, or errorcode from LoadSoundElement.
int
DEMOGL_SoundSystemLoadElement(const char *pszFilename, const int iElementType,
				const char *pszElementScriptID, const bool bUses3D)
{
	CStdString		sFilename, sElementScriptID;

	sElementScriptID.Format("%s",pszElementScriptID);
	sFilename.Format("%s",pszFilename);
	return m_gpSoundSystem->LoadSoundElement(&sFilename[0], iElementType, 
						&sElementScriptID[0], bUses3D);
}


// Purpose: wrapper around CSoundSystem::FreeSoundElement routine to export that routine
// as an API function. See CSoundSystem::FreeSoundElement for details.
// Returns FreeSoundElement resultcode
int
DEMOGL_SoundSystemFreeElement(const int iElementCodeID, const int iElementType)
{
	return m_gpSoundSystem->FreeSoundElement(iElementCodeID, iElementType, false);
}


// Purpose: wrapper around CSoundSystem::ContinueAll routine to export that routine
// as an API function. See CSoundSystem::ContinueAll for details.
// Returns ContinueAll resultcode
int
DEMOGL_SoundSystemContinueAll()
{
	return m_gpSoundSystem->ContinueAll();
}


// Purpose: wrapper around CSoundSystem::ContinueChannel routine to export that routine
// as an API function. See CSoundSystem::ContinueChannel for details.
// Returns ContinueChannel resultcode
int
DEMOGL_SoundSystemContinueChannel(const int iChannelCodeID)
{
	return m_gpSoundSystem->ContinueChannel(iChannelCodeID);
}


// Purpose: wrapper around CSoundSystem::PauseAll routine to export that routine
// as an API function. See CSoundSystem::PauseAll for details.
// Returns PauseAll resultcode
int
DEMOGL_SoundSystemPauseAll()
{
	return m_gpSoundSystem->PauseAll();
}


// Purpose: wrapper around CSoundSystem::PauseChannel routine to export that routine
// as an API function. See CSoundSystem::PauseChannel for details.
// Returns PauseChannel resultcode
int
DEMOGL_SoundSystemPauseChannel(const int iChannelCodeID)
{
	return m_gpSoundSystem->PauseChannel(iChannelCodeID);
}


// Purpose: wrapper around CSoundSystem::SeekInChannel routine to export that routine
// as an API function. See CSoundSystem::SeekInChannel for details.
// Returns SeekInChannel resultcode. Not all elementtypes support seeking.
int
DEMOGL_SoundSystemSeekInChannel(const int iChannelCodeID, const long lStartPos)
{
	return m_gpSoundSystem->SeekInChannel(iChannelCodeID, lStartPos);
}


// Purpose: wrapper around CSoundSystem::SetEAXMixChannel routine to export that routine
// as an API function. See CSoundSystem::SetEAXMixChannel for details.
// Returns SetEAXMixChannel resultcode
int
DEMOGL_SoundSystemSetEAXMixChannel(const int iChannelCodeID, const float fMixValue)
{
	return m_gpSoundSystem->SetEAXMixChannel(iChannelCodeID, fMixValue);
}


// Purpose: wrapper around CSoundSystem::Set3DFactors routine to export that routine
// as an API function. See CSoundSystem::Set3DFactors for details.
// Returns Set3DFactors resultcode
int
DEMOGL_SoundSystemSet3DFactors(const float fDistanceFactor, const float fRollOffFactor,
		const float fDopplerFactor)
{
	return m_gpSoundSystem->Set3DFactors(fDistanceFactor, fRollOffFactor, fDopplerFactor);
}


// Purpose: wrapper around CSoundSystem::SetChannel3DAttributes routine to export that routine
// as an API function. See CSoundSystem::SetChannel3DAttributes for details.
// Returns SetChannel3DAttributes resultcode
int
DEMOGL_SoundSystemSetChannel3DAttributes(const int iChannelCodeID, 
		SSoundElement3DAttributes * const p3DAttributes)
{
	return m_gpSoundSystem->SetChannel3DAttributes(iChannelCodeID, p3DAttributes);
}


// Purpose: wrapper around CSoundSystem::SetChannel3DPosition routine to export that routine
// as an API function. See CSoundSystem::SetChannel3DPosition for details.
// Returns SetChannel3DPosition resultcode
int
DEMOGL_SoundSystemSetChannel3DPosition(const int iChannelCodeID, SSoundChannel3DPosition * const p3DPosition)
{
	return m_gpSoundSystem->SetChannel3DPosition(iChannelCodeID, p3DPosition);
}


// Purpose: wrapper around CSoundSystem::SetChannelAttributes routine to export that routine
// as an API function. See CSoundSystem::SetChannelAttributes for details.
// Returns SetChannelAttributes resultcode
int
DEMOGL_SoundSystemSetChannelAttributes(const int iChannelCodeID, const int iVolume, const int iPan, 
		const int iFrequency, const int iWhat)
{
	return m_gpSoundSystem->SetChannelAttributes(iChannelCodeID, iVolume, iPan, iFrequency, iWhat);
}


// Purpose: wrapper around CSoundSystem::SetElement3DAttributes routine to export that routine
// as an API function. See CSoundSystem::SetElement3DAttributes for details.
// Returns SetElement3DAttributes resultcode
int
DEMOGL_SoundSystemSetElement3DAttributes(const int iElementCodeID, const int iElementType, 
		SSoundElement3DAttributes * const p3DAttributes)
{
	return m_gpSoundSystem->SetElement3DAttributes(iElementCodeID, iElementType, p3DAttributes);
}


// Purpose: wrapper around CSoundSystem::SetEAXEnv routine to export that routine
// as an API function. See CSoundSystem::SetEAXEnv for details.
// Returns SetEAXEnv resultcode
int
DEMOGL_SoundSystemSetEAXEnv(const int iEAXEnvironment, const float fReverbVolume, 
		const float fDecay, const float fDamp)
{
	return m_gpSoundSystem->SetEAXEnv(iEAXEnvironment, fReverbVolume, fDecay, fDamp);
}


// Purpose: wrapper around CSoundSystem::SetListener3DPosition routine to export that routine
// as an API function. See CSoundSystem::SetListener3DPosition for details.
// Returns SetListener3DPosition resultcode
int
DEMOGL_SoundSystemSetListener3DPosition(SListener3DPosition * const p3DPosition)
{
	return m_gpSoundSystem->SetListener3DPosition(p3DPosition);
}


// Purpose: wrapper around CSoundSystem::SetVolume routine to export that routine
// as an API function. See CSoundSystem::SetVolume for details.
// Returns SetVolume resultcode
int
DEMOGL_SoundSystemSetVolume(int iVolume)
{
	return m_gpSoundSystem->SetVolume(iVolume);
}


// Purpose: wrapper around CSoundSystem::StartElement routine to export that routine
// as an API function. See CSoundSystem::StartElement for details.
// Returns ChannelCodeID if succeeded, DGL_SS_ERR_OPERATIONFAILED if not. 
// Receives additional info to represent the PLAY* command in code.
int
DEMOGL_SoundSystemStartElement(const int iElementCodeID, const int iElementType, const long lStartPos, const bool bLoop,
		const int iVolume, const int iPan, const int iFrequency, SSoundChannel3DPosition * const p3DPosition, 
		const char *pszChannelScriptID)
{
	int				iChannelCodeID, iWhat, iResult;
	CStdString		sChannelScriptID;

	sChannelScriptID.Format("%s",pszChannelScriptID);

	iResult=m_gpSoundSystem->StartElement(iElementCodeID, iElementType,lStartPos,bLoop,
								&sChannelScriptID[0], p3DPosition);
	if(iResult>=0)
	{
		// start went ok, set attributes.
		iChannelCodeID=iResult;
		switch(iElementType)
		{
			case DGL_SS_MODELEMENT:
			case DGL_SS_MP3ELEMENT:
			{
				iWhat=DGL_SS_SA_PAN | DGL_SS_SA_VOLUME;
			}; break;
			case DGL_SS_SAMPLEELEMENT:
			{
				iWhat=DGL_SS_SA_PAN | DGL_SS_SA_VOLUME | DGL_SS_SA_FREQUENCY;
			}; break;
			default:
			{
				// unknown elementtype. can't happen
				return SYS_NOK;
			}; break;
		}
		iResult=m_gpSoundSystem->SetChannelAttributes(iChannelCodeID, iVolume, iPan, iFrequency,iWhat);
	}
	if(iResult==SYS_OK)
	{
		return iChannelCodeID;
	}
	return DGL_SS_ERR_OPERATIONFAILED;
}


// Purpose: wrapper around CSoundSystem::StopAll routine to export that routine
// as an API function. See CSoundSystem::StopAll for details.
// Returns StopAll resultcode
int
DEMOGL_SoundSystemStopAll()
{
	return m_gpSoundSystem->StopAll();
}


// Purpose: wrapper around CSoundSystem::StopChannel routine to export that routine
// as an API function. See CSoundSystem::StopChannel for details.
// Returns StopChannel resultcode
int
DEMOGL_SoundSystemStopChannel(const int iChannelCodeID)
{
	return m_gpSoundSystem->StopChannel(iChannelCodeID);
}


// Purpose: wrapper around CSoundSystem::CreateSyncChannel routine to export that routine
// as an API function. See CSoundSystem::CreateSyncChannel for details.
// Returns CreateSyncChannel resultcode
int
DEMOGL_SoundSystemSyncCreate(const int iChannelCodeID, const DWORD dwSyncType, 
		const DWORD dwSyncParam, const DWORD dwSyncFlag)
{
	return m_gpSoundSystem->CreateSyncChannel(iChannelCodeID, dwSyncType, dwSyncParam, dwSyncFlag);
}


// Purpose: wrapper around CSoundSystem::DeleteSyncChannel routine to export that routine
// as an API function. See CSoundSystem::DeleteSyncChannel for details.
// Returns DeleteSyncChannel resultcode
int
DEMOGL_SoundSystemSyncDelete(const int iChannelCodeID, const int iSyncID)
{
	return m_gpSoundSystem->DeleteSyncChannel(iChannelCodeID, iSyncID);
}


// Purpose: wrapper around CSoundSystem::DeleteAllSyncsChannel routine to export that routine
// as an API function. See CSoundSystem::DeleteAllSyncsChannel for details.
// Returns DeleteAllSyncsChannel resultcode
int
DEMOGL_SoundSystemSyncDeleteAll(const int iChannelCodeID)
{
	return m_gpSoundSystem->DeleteAllSyncsChannel(iChannelCodeID);
}


// Purpose: creates a SSoundSystemOptionDat structure and fills it with data, retrieved from
// the BASS library. This structure is part of CDemoDat object. Users shouldn't free this structure
// themselves.
// Returns: NULL if failed, pointer to created and filled SSoundSystemOptionDat structure otherwise.
SSoundSystemOptionDat	
*DEMOGL_SoundSystemDetermineOptionsData()
{
	SSoundSystemOptionDat	*pSSODat;
	int						i;
	char					*pDeviceDesc;
	BOOL					bResult;
	HWND					hWnd;

	// get forgroundwindow. Any window will do. Since we could call this routine when there isn't
	// any window created for our application, there is no other way.
	hWnd = GetForegroundWindow();

	// get pointer to object. 
	pSSODat = m_gpDemoDat->GetSSODat();

	pSSODat->m_bSound_Possible=false;
	pSSODat->m_iAmDevices=0;

	for(i=0;i < DGL_SS_MAXDEVICES;i++)
	{
		// Use BASS_Init() to determine the possibilities of the soundsystem installed.
		bResult=BASS_GetDeviceDescription(i,&pDeviceDesc);
		if(!bResult)
		{
			// we're done
			break;
		}

		pSSODat->m_bSound_Possible=true;

		// init device.
		pSSODat->m_arrssdoDevices[i].m_b3DSound_A3DPossible=false;
		pSSODat->m_arrssdoDevices[i].m_b3DSound_EAXPossible=false;
		// store description
		strncpy(pSSODat->m_arrssdoDevices[i].m_sDescription,pDeviceDesc,DGL_SS_MAXLENDEVICESTRING);

		pSSODat->m_iAmDevices++;

		// determine if this device does EAX and / or A3D
		if(!BASS_Init(i,44100,BASS_DEVICE_A3D,hWnd)) 
		{
			if (!BASS_Init(i,44100,BASS_DEVICE_3D,hWnd))
			{
				// no 3D support
				pSSODat->m_arrssdoDevices[i].m_b3DSound_A3DPossible=false;
				pSSODat->m_arrssdoDevices[i].m_b3DSound_EAXPossible=false;
				continue;
			}
		}
		else
		{
			// it has A3D Support.
			strcat(pSSODat->m_arrssdoDevices[i].m_sDescription," [A3D]");
			pSSODat->m_arrssdoDevices[i].m_b3DSound_A3DPossible=true;
		}
		if (BASS_GetEAXParameters(NULL,NULL,NULL,NULL))
		{
			// it has EAX support
			strcat(pSSODat->m_arrssdoDevices[i].m_sDescription," [EAX]");
			pSSODat->m_arrssdoDevices[i].m_b3DSound_EAXPossible=true;
		}
		BASS_Free();
	}
	return pSSODat;
}


// Purpose: Initializes the soundsystem. If the specs as specified in the SStartupDat structure in
// demodat are correct, the initialisation of the soundsystem will succeed. if it fails, it will display
// an error and will return SYS_NOK, otherwise it will return SYS_OK
int
InitSoundSystem()
{
	SStartupDat		*pStartupDat;
	DWORD			dwFlags;
	int				iResult;

	pStartupDat=m_gpDemoDat->GetStartupDat();
	dwFlags=0;
	if(!pStartupDat->m_bSound)
	{
		// no sound requested. 
		return SYS_OK;
	}

	if(!pStartupDat->m_bSS_16bit)
	{
		dwFlags|=BASS_DEVICE_8BITS;
	}
	if(!pStartupDat->m_bSS_Stereo)
	{
		dwFlags|=BASS_DEVICE_MONO;
	}
	if(pStartupDat->m_bSS_3DSound)
	{
		dwFlags|=BASS_DEVICE_3D;
		m_gpSoundSystem->Set3DSoundEnabled(true);
	}
	if(pStartupDat->m_bSS_A3D)
	{
		dwFlags|=BASS_DEVICE_A3D;
		m_gpSoundSystem->SetA3DEnabled(true);
	}
	if(pStartupDat->m_bSS_EAX)
	{
		m_gpSoundSystem->SetEAXEnabled(true);
	}
	
	// Leave the current volume set in windows for wave audio when calling BASS_Init()
	// (Only available in Bass 0.8b and higher)
	//dwFlags|=BASS_DEVICE_LEAVEVOL;
	
	// Store properties in soundsystem
	m_gpSoundSystem->AddBassSystemFlags(dwFlags);
	if(pStartupDat->m_bSS_LowQuality)
	{
		m_gpSoundSystem->SetDeviceAndFrequency(pStartupDat->m_iSS_SoundDevice, 22050);
	}
	else
	{
		m_gpSoundSystem->SetDeviceAndFrequency(pStartupDat->m_iSS_SoundDevice, 44100);
	}

	// Initializes it
	iResult=m_gpSoundSystem->InitSystem();
	if(iResult==SYS_OK)
	{
		// Set overall volume
		m_gpSoundSystem->SetVolume(pStartupDat->m_iSS_OverallVolume);
		LogFeedback("Initialisation of soundsystem succeeded.",true,"InitSoundSystem", true);
		return SYS_OK;
	}
	// something went wrong, retry with 8bit, mono 22050 khz
	LogFeedback("Initialisation of soundsystem failed. Retrying with 8bit/mono/22050khz.",true,"InitSoundSystem",false);
	m_gpSoundSystem->SetBassSystemFlags(BASS_DEVICE_8BITS|BASS_DEVICE_MONO);
	m_gpSoundSystem->SetDeviceAndFrequency(pStartupDat->m_iSS_SoundDevice,22050);
	iResult=m_gpSoundSystem->InitSystem();
	if(iResult==SYS_OK)
	{
		// Set overall volume
		m_gpSoundSystem->SetVolume(pStartupDat->m_iSS_OverallVolume);
		LogFeedback("Initialisation of soundsystem succeeded with 8bit/mono/22050khz",true,"InitSoundSystem", true);
		return SYS_OK;
	}
	// something went wrong. Perhaps another application has locked the audio.
	MessageBox(NULL,"Initialisation of soundsystem failed, also after using lowquality settings. Sound will be disabled.",
			"DemoGL Non-fatal Error.",MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
	// disable sound.
	m_gpDemoDat->SetbSound(false);
	return SYS_NOK;
}


// Purpose: callback routine that is called by BASS when a set sync is triggered. 
// dwChannelCodeID is the Userdata passed to the sync when created inside BASS. 
void	CALLBACK
DEMOGL_SoundSystemSyncCallBack(HSYNC hSyncHandle, DWORD dwChannel, DWORD dwData, DWORD dwChannelCodeID)
{
	m_gpSoundSystem->ExecuteSyncChannel((int)dwChannelCodeID, hSyncHandle, dwData);
}