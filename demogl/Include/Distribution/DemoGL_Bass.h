//////////////////////////////////////////////////////////////////////
// FILE: DemoGL_Bass.h
// PURPOSE: Include file for DemoGL powered application to use BASS specific
// structures and constants more easily. DemoGL 1.3 and higher only. 
// From v1.3, DemoGL uses BASS soundlibrary to produce sound and playback
// music in a variaty of formats. 
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// BASS specific definitions, constants and structures for usage with
// DemoGL's soundsystem functions.
// 
// Based on BASS 1.1a's include file bass.h
// NOTE: Naming scheme for variables and structures has been kept the same, to avoid
// confusion when people already know how to use bass.
//
//////////////////////////////////////////////////////////////////////
// Part of DemoGL Demo System sourcecode. See version information
//////////////////////////////////////////////////////////////////////
// COPYRIGHTS:
// BASS is written by Ian Luck (c)2000 Ian Luck.
// Code below is based on Ian Luck's bass.h include file and enhanced for
// DemoGL usage by Frans Bouma. 
// 
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
//		- Ian Luck (main)
//		- Frans Bouma [FB] (recoding some parts, comments)
//////////////////////////////////////////////////////////////////////
// VERSION INFORMATION.
//
// v1.3: Added to codebase.
// v1.2: --
// v1.1: --
//////////////////////////////////////////////////////////////////////

#ifndef _DEMOGL_BASS_H
#define _DEMOGL_BASS_H

////////////////////////////////////////////////////////////////////////
//                          
//						ENUMS
//
////////////////////////////////////////////////////////////////////////
// EAX environments, useful to set a playback environment.
enum
{
    EAX_ENVIRONMENT_GENERIC,
    EAX_ENVIRONMENT_PADDEDCELL,
    EAX_ENVIRONMENT_ROOM,
    EAX_ENVIRONMENT_BATHROOM,
    EAX_ENVIRONMENT_LIVINGROOM,
    EAX_ENVIRONMENT_STONEROOM,
    EAX_ENVIRONMENT_AUDITORIUM,
    EAX_ENVIRONMENT_CONCERTHALL,
    EAX_ENVIRONMENT_CAVE,
    EAX_ENVIRONMENT_ARENA,
    EAX_ENVIRONMENT_HANGAR,
    EAX_ENVIRONMENT_CARPETEDHALLWAY,
    EAX_ENVIRONMENT_HALLWAY,
    EAX_ENVIRONMENT_STONECORRIDOR,
    EAX_ENVIRONMENT_ALLEY,
    EAX_ENVIRONMENT_FOREST,
    EAX_ENVIRONMENT_CITY,
    EAX_ENVIRONMENT_MOUNTAINS,
    EAX_ENVIRONMENT_QUARRY,
    EAX_ENVIRONMENT_PLAIN,
    EAX_ENVIRONMENT_PARKINGLOT,
    EAX_ENVIRONMENT_SEWERPIPE,
    EAX_ENVIRONMENT_UNDERWATER,
    EAX_ENVIRONMENT_DRUGGED,
    EAX_ENVIRONMENT_DIZZY,
    EAX_ENVIRONMENT_PSYCHOTIC,

	// Add your own presets here

    EAX_ENVIRONMENT_COUNT			// total number of environments

};

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////

// Errorcodes BASS will return and which are logged on the system console when available.
// Not all errorcodes can happen in DemoGL. 
#define BASS_OK					0	// all is OK
#define BASS_ERROR_MEM			1	// memory error
#define BASS_ERROR_FILEOPEN		2	// can't open the file
#define BASS_ERROR_DRIVER		3	// can't find a free/valid driver
#define BASS_ERROR_BUFLOST		4	// the sample buffer was lost - please report this!
#define BASS_ERROR_HANDLE		5	// invalid handle
#define BASS_ERROR_FORMAT		6	// unsupported format
#define BASS_ERROR_POSITION		7	// invalid playback position
#define BASS_ERROR_INIT			8	// BASS_Init has not been successfully called
#define BASS_ERROR_START		9	// BASS_Start has not been successfully called
#define BASS_ERROR_INITCD		10	// can't initialize CD
#define BASS_ERROR_CDINIT		11	// BASS_CDInit has not been successfully called
#define BASS_ERROR_NOCD			12	// no CD in drive
#define BASS_ERROR_CDTRACK		13	// can't play the selected CD track
#define BASS_ERROR_ALREADY		14	// already initialized
#define BASS_ERROR_CDVOL		15	// CD has no volume control
#define BASS_ERROR_NOPAUSE		16	// not paused
#define BASS_ERROR_NOTAUDIO		17	// not an audio track
#define BASS_ERROR_NOCHAN		18	// can't get a free channel
#define BASS_ERROR_ILLTYPE		19	// an illegal type was specified
#define BASS_ERROR_ILLPARAM		20	// an illegal parameter was specified
#define BASS_ERROR_NO3D			21	// no 3D support
#define BASS_ERROR_NOEAX		22	// no EAX support
#define BASS_ERROR_DEVICE		23	// illegal device number
#define BASS_ERROR_NOPLAY		24	// not playing
#define BASS_ERROR_FREQ			25	// illegal sample rate
#define BASS_ERROR_NOA3D		26	// A3D.DLL is not installed
#define BASS_ERROR_NOTFILE		27	// the stream is not a file stream (WAV/MP3)
#define BASS_ERROR_NOHW			29	// no hardware voices available
#define BASS_ERROR_NOSYNC		30	// synchronizers have been disabled
#define BASS_ERROR_EMPTY		31	// the MOD music has no sequence data
#define BASS_ERROR_NONET		32	// no internet connection could be opened
#define BASS_ERROR_CREATE		33	// couldn't create the file
#define BASS_ERROR_NOFX			34	// effects are not enabled
#define BASS_ERROR_PLAYING		35	// the channel is playing
#define BASS_ERROR_UNKNOWN		-1	// some other mystery error

// Sync flags (pass as dwSyncType to the SoundSystem sync methods)
#define BASS_SYNC_MUSICPOS		0
#define BASS_SYNC_POS			0
#define BASS_SYNC_MUSICINST		1
#define BASS_SYNC_END			2
#define BASS_SYNC_MUSICFX		3
#define BASS_SYNC_MIXTIME		0x40000000	// FLAG: sync at mixtime, else at playtime
#define BASS_SYNC_ONETIME		0x80000000	// FLAG: sync only once, else continuously

// defines needed for BASS_SAMPLE struct.
#define BASS_SAMPLE_8BITS		1			// 8 bit, else 16 bit
#define BASS_SAMPLE_MONO		2			// mono, else stereo
#define BASS_SAMPLE_LOOP		4			// looped
#define BASS_SAMPLE_3D			8			// 3D functionality enabled
#define BASS_SAMPLE_SOFTWARE	16			// it's NOT using hardware mixing
#define BASS_SAMPLE_MUTEMAX		32			// muted at max distance (3D only)
#define BASS_SAMPLE_VAM			64			// uses the DX7 voice allocation & management
#define BASS_SAMPLE_OVER_VOL	0x10000		// override lowest volume
#define BASS_SAMPLE_OVER_POS	0x20000		// override longest playing
#define BASS_SAMPLE_OVER_DIST	0x30000		// override furthest from listener (3D only)

// DX7 voice allocation flags. Used with BASS_SAMPLE. Ignored if DX7 is not available
#define BASS_VAM_HARDWARE		1			// Play the sample in hardware only.
#define BASS_VAM_SOFTWARE		2			// Play the sample using software mixing.

// DX7 voice management flags. Used with BASS_SAMPLE. Ignored if DX7 is not available
// These VAM flags are usable for hw voice management when no voices are free for your sample.
#define BASS_VAM_TERM_TIME		4			// Use the hw buffer with the least time left to play
#define BASS_VAM_TERM_DIST		8			// Use the hw buffer which is beyond it's max distance (3D)
#define BASS_VAM_TERM_PRIO		16			// Use the hw buffer with the lowest priority

// 3D channel modes. For the 3D replay mode of a sound element.
#define BASS_3DMODE_NORMAL		0			// normal 3D processing 
#define BASS_3DMODE_RELATIVE	1			// the 3D position is relative to the listener. if
											// the listener position is changed, the sound position isn't changed
#define BASS_3DMODE_OFF			2			// Turn off 3D processing. Play will continue in the center.

// EAX environment preset values. Change these to adjust a preset used in your code.
#define EAX_PRESET_GENERIC         EAX_ENVIRONMENT_GENERIC,0.5F,1.493F,0.5F
#define EAX_PRESET_PADDEDCELL      EAX_ENVIRONMENT_PADDEDCELL,0.25F,0.1F,0.0F
#define EAX_PRESET_ROOM            EAX_ENVIRONMENT_ROOM,0.417F,0.4F,0.666F
#define EAX_PRESET_BATHROOM        EAX_ENVIRONMENT_BATHROOM,0.653F,1.499F,0.166F
#define EAX_PRESET_LIVINGROOM      EAX_ENVIRONMENT_LIVINGROOM,0.208F,0.478F,0.0F
#define EAX_PRESET_STONEROOM       EAX_ENVIRONMENT_STONEROOM,0.5F,2.309F,0.888F
#define EAX_PRESET_AUDITORIUM      EAX_ENVIRONMENT_AUDITORIUM,0.403F,4.279F,0.5F
#define EAX_PRESET_CONCERTHALL     EAX_ENVIRONMENT_CONCERTHALL,0.5F,3.961F,0.5F
#define EAX_PRESET_CAVE            EAX_ENVIRONMENT_CAVE,0.5F,2.886F,1.304F
#define EAX_PRESET_ARENA           EAX_ENVIRONMENT_ARENA,0.361F,7.284F,0.332F
#define EAX_PRESET_HANGAR          EAX_ENVIRONMENT_HANGAR,0.5F,10.0F,0.3F
#define EAX_PRESET_CARPETEDHALLWAY EAX_ENVIRONMENT_CARPETEDHALLWAY,0.153F,0.259F,2.0F
#define EAX_PRESET_HALLWAY         EAX_ENVIRONMENT_HALLWAY,0.361F,1.493F,0.0F
#define EAX_PRESET_STONECORRIDOR   EAX_ENVIRONMENT_STONECORRIDOR,0.444F,2.697F,0.638F
#define EAX_PRESET_ALLEY           EAX_ENVIRONMENT_ALLEY,0.25F,1.752F,0.776F
#define EAX_PRESET_FOREST          EAX_ENVIRONMENT_FOREST,0.111F,3.145F,0.472F
#define EAX_PRESET_CITY            EAX_ENVIRONMENT_CITY,0.111F,2.767F,0.224F
#define EAX_PRESET_MOUNTAINS       EAX_ENVIRONMENT_MOUNTAINS,0.194F,7.841F,0.472F
#define EAX_PRESET_QUARRY          EAX_ENVIRONMENT_QUARRY,1.0F,1.499F,0.5F
#define EAX_PRESET_PLAIN           EAX_ENVIRONMENT_PLAIN,0.097F,2.767F,0.224F
#define EAX_PRESET_PARKINGLOT      EAX_ENVIRONMENT_PARKINGLOT,0.208F,1.652F,1.5F
#define EAX_PRESET_SEWERPIPE       EAX_ENVIRONMENT_SEWERPIPE,0.652F,2.886F,0.25F
#define EAX_PRESET_UNDERWATER      EAX_ENVIRONMENT_UNDERWATER,1.0F,1.499F,0.0F
#define EAX_PRESET_DRUGGED         EAX_ENVIRONMENT_DRUGGED,0.875F,8.392F,1.388F
#define EAX_PRESET_DIZZY           EAX_ENVIRONMENT_DIZZY,0.139F,17.234F,0.666F
#define EAX_PRESET_PSYCHOTIC       EAX_ENVIRONMENT_PSYCHOTIC,0.486F,7.563F,0.806F
// Add your own presets here. Don't forget to add also an EAX_ENVIRONMENT item to the
// enumlist above.


////////////////////////////////////////////////////////////////////////
//                          
//						TYPEDEFS
//
////////////////////////////////////////////////////////////////////////

// Purpose: Sample info structure & flags. Use this struct to pass data to DemoGL
// for playback a sample element using the parameters in the struct. 
typedef struct 
{
	DWORD		freq;		// default playback rate
	DWORD		volume;		// default volume (0-100)
	int			pan;		// default pan (-100=left, 0=middle, 100=right)
	DWORD		flags;		// BASS_SAMPLE_xxx flags
	DWORD		length;		// length (in samples, not bytes)
	DWORD		max;		// maximum simultaneous playbacks

	// The following are the sample's default 3D attributes (if the sample
	// is 3D, BASS_SAMPLE_3D is in flags) see BASS_ChannelSet3DAttributes
	DWORD		mode3d;		// BASS_3DMODE_xxx mode
	float		mindist;	// minimum distance
	float		maxdist;	// maximum distance
	DWORD		iangle;		// angle of inside projection cone
	DWORD		oangle;		// angle of outside projection cone
	DWORD		outvol;		// delta-volume outside the projection cone

	// The following are the defaults used if the sample uses the DirectX 7
	// voice allocation/management features.
	DWORD		vam;		// voice allocation/management flags (BASS_VAM_xxx)
	DWORD		priority;	// priority (0=lowest, 0xffffffff=highest)
} BASS_SAMPLE;


// Purpose: 3D vector (for 3D positions/velocities/orientations)
typedef struct 
{
	float		x;			// +=right, -=left
	float		y;			// +=up, -=down
	float		z;			// +=front, -=behind
} BASS_3DVECTOR;


#endif // _DEMOGL_BASS_H