#if 0
static char *rcsid = "@(#) $RCSfile: ossaudio.c,v $ $Revision: 1.4 $ (SHARK) $Date: 1997/10/06 16:36:45 $";
#endif

/*
 * Copyright 1997
 * Digital Equipment Corporation. All rights reserved.
 *
 * This software is furnished under license and may be used and
 * copied only in accordance with the following terms and conditions.
 * Subject to these conditions, you may download, copy, install,
 * use, modify and distribute this software in source and/or binary
 * form. No title or ownership is transferred hereby.
 *
 * 1) Any source code used, modified or distributed must reproduce
 *    and retain this copyright notice and list of conditions as
 *    they appear in the source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of
 *    Digital Equipment Corporation. Neither the "Digital Equipment
 *    Corporation" name nor any trademark or logo of Digital Equipment
 *    Corporation may be used to endorse or promote products derived
 *    from this software without the prior written permission of
 *    Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied
 *    warranties, including but not limited to, any implied warranties
 *    of merchantability, fitness for a particular purpose, or
 *    non-infringement are disclaimed. In no event shall DIGITAL be
 *    liable for any damages whatsoever, and in particular, DIGITAL
 *    shall not be liable for special, indirect, consequential, or
 *    incidental damages or damages for lost profits, loss of
 *    revenue or loss of use, whether such damages arise in contract,
 *    negligence, tort, under statute, in equity, at law or otherwise,
 *    even if advised of the possibility of such damage.
 */

/*
**++
**
**  ossaudio.c
**
**  FACILITY:
**
**	DIGITAL Network Appliance Reference Design (DNARD)
**
**  MODULE DESCRIPTION:
**
**      This module contains an audio driver that is compatible with
**      the interface provided by Open Sound System (OSS).  This
**      driver simply maps calls made to it to the appropriate calls
**      on the standard audio driver.
**
**  AUTHORS:
**
**	Blair Fidler	Software Engineering Australia
**			Gold Coast, Australia.
**
**	Code for mapping OSS ioctls to NetBSD ioctls based on code by
**	Lennart Augustsson in compat/osssaudio/ossaudio.c.
**
**  CREATION DATE:  
**
**	June 2, 1997.
**
**  MODIFICATION HISTORY:
**
**--
*/
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/vnode.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/systm.h>
#include <sys/syslog.h>
#include <sys/kernel.h>
#include <sys/signalvar.h>
#include <sys/conf.h>
#include <sys/audioio.h>
#include <sys/soundcard.h>

#include <dev/audio_if.h>
#include <dev/audiovar.h>


#define REALAUDIODEV(x) (makedev(36, minor(x)))
/* REVISIT: should this come from soundcard.h? */
#define OSS_GET_DEV(cmd) ((cmd) & 0xff)


int
ossaudioopen(dev_t dev,
	  int flags,
	  int ifmt,
	  struct proc *p)
{
	return audioopen(REALAUDIODEV(dev), flags, ifmt, p);
}
 
int
ossaudioclose(dev_t dev,
	   int flags,
	   int ifmt,
	   struct proc *p)
{
	return audioclose(REALAUDIODEV(dev), flags, ifmt, p);
}
 
int
ossaudioread(dev_t dev,
	     struct uio *uio,
	     int ioflag)
{
	return audioread(REALAUDIODEV(dev), uio, ioflag);
}
 
int
ossaudiowrite(dev_t dev,
	   struct uio *uio,
	   int ioflag)
{
	return audiowrite(REALAUDIODEV(dev), uio, ioflag);
}

int
ossaudioioctl(dev_t dev,
	      u_long cmd,
	      caddr_t addr,
	      int flag,
	      struct proc *p)
{
	/*
	 * REVISIT: This assumes NetBSD mappings of minor device
	 *          numbers.  Are these valid for OSS-compatible
	 *          interface?
	 */
	switch (AUDIODEV(dev)) {
	case SOUND_DEVICE:
	case AUDIO_DEVICE:
		return (oss_ioctl_audio(dev, cmd, addr, flag, p));
	case MIXER_DEVICE:
		return (oss_ioctl_mixer(dev, cmd, addr, flag, p));
	default:
		return (ENXIO);
	}

}

int
ossaudiopoll(dev_t dev,
	     int events,
	     struct proc *p)
{
	return audiopoll(REALAUDIODEV(dev), events, p);
}

int
ossaudiommap(dev_t dev,
	     int off,
	     int prot)
{
	return audiommap(REALAUDIODEV(dev), off, prot);
}


int
oss_ioctl_audio(dev_t dev,
		u_long cmd,
		caddr_t addr,
		int flag,
		struct proc *p)
{
	struct audio_info tmpinfo;
	int idat;
	int error;
	dev_t adev;


	adev = REALAUDIODEV(dev);

	switch (cmd) {
	case SNDCTL_DSP_RESET:
		error = audio_ioctl(adev, AUDIO_FLUSH, addr, flag, p);
		if (error)
			return error;
		break;
	case SNDCTL_DSP_SYNC:
	case SNDCTL_DSP_POST:
		error = audio_ioctl(adev, AUDIO_DRAIN, addr, flag, p);
		if (error)
			return error;
		break;
	case SNDCTL_DSP_SPEED:
		AUDIO_INITINFO(&tmpinfo);
		tmpinfo.play.sample_rate =
		tmpinfo.record.sample_rate = *(int *)addr;
		(void) audio_ioctl(adev, AUDIO_SETINFO, &tmpinfo, flag, p);
		/* fall into ... */
	case SOUND_PCM_READ_RATE:
		error = audio_ioctl(adev, AUDIO_GETINFO, &tmpinfo, flag, p);
		if (error)
			return error;
		*(int *)addr = tmpinfo.play.sample_rate;
		break;
	case SNDCTL_DSP_STEREO:
		AUDIO_INITINFO(&tmpinfo);
		tmpinfo.play.channels =
		tmpinfo.record.channels = (*(int *)addr) ? 2 : 1;
		(void) audio_ioctl(adev, AUDIO_SETINFO, &tmpinfo, flag, p);
		error = audio_ioctl(adev, AUDIO_GETINFO, &tmpinfo, flag, p);
		if (error)
			return error;
		*(int *)addr = tmpinfo.play.channels - 1;
		break;
	case SNDCTL_DSP_GETBLKSIZE:
		error = audio_ioctl(adev, AUDIO_GETINFO, &tmpinfo, flag, p);
		if (error)
			return error;
		*(int *)addr = tmpinfo.blocksize;
		break;
	case SNDCTL_DSP_SETFMT:
		AUDIO_INITINFO(&tmpinfo);
		switch (*(int *)addr) {
		case AFMT_MU_LAW:
			tmpinfo.play.precision =
			tmpinfo.record.precision = 8;
			tmpinfo.play.encoding =
			tmpinfo.record.encoding = AUDIO_ENCODING_ULAW;
			break;
		case AFMT_A_LAW:
			tmpinfo.play.precision =
			tmpinfo.record.precision = 8;
			tmpinfo.play.encoding =
			tmpinfo.record.encoding = AUDIO_ENCODING_ALAW;
			break;
		case AFMT_U8:
			tmpinfo.play.precision =
			tmpinfo.record.precision = 8;
			tmpinfo.play.encoding =
			tmpinfo.record.encoding = AUDIO_ENCODING_ULINEAR;
			break;
		case AFMT_S8:
			tmpinfo.play.precision =
			tmpinfo.record.precision = 8;
			tmpinfo.play.encoding =
			tmpinfo.record.encoding = AUDIO_ENCODING_LINEAR;
			break;
		case AFMT_S16_LE:
			tmpinfo.play.precision =
			tmpinfo.record.precision = 16;
			tmpinfo.play.encoding =
			tmpinfo.record.encoding = AUDIO_ENCODING_LINEAR_LE;
			break;
		case AFMT_S16_BE:
			tmpinfo.play.precision =
			tmpinfo.record.precision = 16;
			tmpinfo.play.encoding =
			tmpinfo.record.encoding = AUDIO_ENCODING_LINEAR_BE;
			break;
		case AFMT_U16_LE:
			tmpinfo.play.precision =
			tmpinfo.record.precision = 16;
			tmpinfo.play.encoding =
			tmpinfo.record.encoding = AUDIO_ENCODING_ULINEAR_LE;
			break;
		case AFMT_U16_BE:
			tmpinfo.play.precision =
			tmpinfo.record.precision = 16;
			tmpinfo.play.encoding =
			tmpinfo.record.encoding = AUDIO_ENCODING_ULINEAR_BE;
			break;
		default:
			return EINVAL;
		}
		(void) audio_ioctl(adev, AUDIO_SETINFO, &tmpinfo, flag, p);
		/* fall into ... */
	case SOUND_PCM_READ_BITS:
		error = audio_ioctl(adev, AUDIO_GETINFO, &tmpinfo, flag, p);
		if (error)
			return error;
		switch (tmpinfo.play.encoding) {
		case AUDIO_ENCODING_ULAW:
			idat = AFMT_MU_LAW;
			break;
		case AUDIO_ENCODING_ALAW:
			idat = AFMT_A_LAW;
			break;
		case AUDIO_ENCODING_LINEAR_LE:
			if (tmpinfo.play.precision == 16)
				idat = AFMT_S16_LE;
			else
				idat = AFMT_S8;
			break;
		case AUDIO_ENCODING_LINEAR_BE:
			if (tmpinfo.play.precision == 16)
				idat = AFMT_S16_BE;
			else
				idat = AFMT_S8;
			break;
		case AUDIO_ENCODING_ULINEAR_LE:
			if (tmpinfo.play.precision == 16)
				idat = AFMT_U16_LE;
			else
				idat = AFMT_U8;
			break;
		case AUDIO_ENCODING_ULINEAR_BE:
			if (tmpinfo.play.precision == 16)
				idat = AFMT_U16_BE;
			else
				idat = AFMT_U8;
			break;
		case AUDIO_ENCODING_ADPCM:
			idat = AFMT_IMA_ADPCM;
			break;
		}
		*(int *)addr = idat;
		break;
	case SOUND_PCM_WRITE_CHANNELS:
		AUDIO_INITINFO(&tmpinfo);
		tmpinfo.play.channels =
		tmpinfo.record.channels = *(int *)addr;
		(void) audio_ioctl(adev, AUDIO_SETINFO, &tmpinfo, flag, p);
		/* fall into ... */
	case SOUND_PCM_READ_CHANNELS:
		error = audio_ioctl(adev, AUDIO_GETINFO, &tmpinfo, flag, p);
		if (error)
			return error;
		*(int *)addr = tmpinfo.play.channels;
		break;
	case SOUND_PCM_WRITE_FILTER:
	case SOUND_PCM_READ_FILTER:
		return EINVAL; /* XXX unimplemented */
	case SNDCTL_DSP_SUBDIVIDE:
		idat = *(int *)addr;
		error = audio_ioctl(adev, AUDIO_GETINFO, &tmpinfo, flag, p);
		if (error)
			return error;
		if (idat == 0)
			idat = tmpinfo.buffersize / tmpinfo.blocksize;
		idat = (tmpinfo.buffersize / idat) & -4;
		AUDIO_INITINFO(&tmpinfo);
		tmpinfo.blocksize = idat;
		error = audio_ioctl(adev, AUDIO_SETINFO, &tmpinfo, flag, p);
		if (error)
			return error;
		idat = tmpinfo.buffersize / tmpinfo.blocksize;
		*(int *)addr = idat;
		break;
	case SNDCTL_DSP_SETFRAGMENT:
		AUDIO_INITINFO(&tmpinfo);
		idat = *(int *)addr;
		if ((idat & 0xffff) < 4 || (idat & 0xffff) > 17)
			return EINVAL;
		tmpinfo.blocksize = 1 << (idat & 0xffff);
		if ((idat >> 16) & 0xffff)
		{
			tmpinfo.hiwat = (idat >> 16) & 0xffff;
			tmpinfo.lowat = tmpinfo.hiwat * 3 / 4;
		}
		(void) audio_ioctl(adev, AUDIO_SETINFO, &tmpinfo, flag, p);
		error = audio_ioctl(adev, AUDIO_GETINFO, &tmpinfo, flag, p);
		if (error)
			return error;
		*(int *)addr = tmpinfo.blocksize;
		break;
	case SNDCTL_DSP_GETFMTS:
		*(int *)addr = AFMT_MU_LAW | AFMT_U8 | AFMT_S16_LE;
		break;
	case SNDCTL_DSP_GETOSPACE:
		error = audio_ioctl(adev, AUDIO_GETINFO, &tmpinfo, flag, p);
		if (error)
			return error;
		((audio_buf_info *)addr)->fragments = tmpinfo.play.nblk;
		((audio_buf_info *)addr)->fragsize = tmpinfo.blocksize;
		((audio_buf_info *)addr)->bytes = tmpinfo.play.nblk * tmpinfo.blocksize;
		break;
	case SNDCTL_DSP_GETISPACE:
	case SNDCTL_DSP_NONBLOCK:
		return EINVAL; /* XXX unimplemented */
	default:
		return EINVAL;
	}

	return 0;
}

/* If the NetBSD mixer device should have more than 32 devices
 * some will not be available to Linux */
#define NETBSD_MAXDEVS 32
struct audiodevinfo {
	int done;
	dev_t dev;
	int16_t devmap[SOUND_MIXER_NRDEVICES], 
	        rdevmap[NETBSD_MAXDEVS];
        u_long devmask, recmask, stereomask;
	u_long caps, source;
};

/* 
 * Collect the audio device information to allow faster
 * emulation of the OSS mixer ioctls.  Cache the information
 * to eliminate the overhead of repeating all the ioctls needed
 * to collect the information.
 */
static struct audiodevinfo *
getdevinfo(dev_t adev,
	   int flag,
	   struct proc *p)
{
	mixer_devinfo_t mi;
	int i;
	static struct {
		char *name;
		int code;
	} *dp, devs[] = {
		{ AudioNmicrophone,	SOUND_MIXER_MIC },
		{ AudioNline,		SOUND_MIXER_LINE },
		{ AudioNcd,		SOUND_MIXER_CD },
		{ AudioNdac,		SOUND_MIXER_PCM },
		{ AudioNrecord,		SOUND_MIXER_IMIX },
		{ AudioNvolume,		SOUND_MIXER_VOLUME },
		{ AudioNtreble,		SOUND_MIXER_TREBLE },
		{ AudioNbass,		SOUND_MIXER_BASS },
		{ AudioNspeaker,	SOUND_MIXER_SPEAKER },
/*		{ AudioNheadphone,	?? },*/
		{ AudioNoutput,		SOUND_MIXER_OGAIN },
		{ AudioNinput,		SOUND_MIXER_IGAIN },
		{ AudioNmaster,		SOUND_MIXER_SPEAKER },
/*		{ AudioNstereo,		?? },*/
/*		{ AudioNmono,		?? },*/
		{ AudioNfmsynth,	SOUND_MIXER_SYNTH },
/*		{ AudioNwave,		SOUND_MIXER_PCM },*/
		{ AudioNmidi,		SOUND_MIXER_SYNTH },
/*		{ AudioNmixerout,	?? },*/
		{ 0, -1 }
	};
	static struct audiodevinfo devcache = { 0 };
	struct audiodevinfo *di = &devcache;

	/*
	 * Just return the cached data if it is valid.
	 */
	if (di->done && di->dev == adev)
		return di;

	di->done = 1;
	di->dev = adev;
	di->devmask = 0;
	di->recmask = 0;
	di->stereomask = 0;
	di->source = -1;
	di->caps = 0;
	for(i = 0; i < SOUND_MIXER_NRDEVICES; i++)
		di->devmap[i] = -1;
	for(i = 0; i < NETBSD_MAXDEVS; i++)
		di->rdevmap[i] = -1;
	for(i = 0; i < NETBSD_MAXDEVS; i++) {
		mi.index = i;
		if (mixer_ioctl(adev, AUDIO_MIXER_DEVINFO, &mi, flag, p) < 0)
			break;
		switch(mi.type) {
		case AUDIO_MIXER_VALUE:
			for(dp = devs; dp->name; dp++)
		    		if (strcmp(dp->name, mi.label.name) == 0)
					break;
			if (dp->code >= 0) {
				di->devmap[dp->code] = i;
				di->rdevmap[i] = dp->code;
				di->devmask |= 1 << dp->code;
				if (mi.un.v.num_channels == 2)
					di->stereomask |= 1 << dp->code;
			}
			break;
		case AUDIO_MIXER_ENUM:
			if (strcmp(mi.label.name, AudioNsource) == 0) {
				int j;
				di->source = i;
				for(j = 0; j < mi.un.e.num_mem; j++)
					di->recmask |= 1 << di->rdevmap[mi.un.e.member[j].ord];
				di->caps = SOUND_CAP_EXCL_INPUT;
			}
			break;
		case AUDIO_MIXER_SET:
			if (strcmp(mi.label.name, AudioNsource) == 0) {
				int j;
				di->source = i;
				for(j = 0; j < mi.un.s.num_mem; j++) {
					int k, mask = mi.un.s.member[j].mask;
					if (mask) {
						for(k = 0; !(mask & 1); mask >>= 1, k++)
							;
						di->recmask |= 1 << di->rdevmap[k];
					}
				}
			}
			break;
		}
	}
	return di;
}

int
oss_ioctl_mixer(dev_t dev,
		u_long cmd,
		caddr_t addr,
		int flag,
		struct proc *p)
{	       
	struct audiodevinfo *di;
	mixer_ctrl_t mc;
	int idat;
	int i;
	int error;
	int l, r, n;
	dev_t adev;

	adev = REALAUDIODEV(dev);

	di = getdevinfo(adev, flag, p);
	if (di == 0)
		return EINVAL;

	switch (cmd) {
	case SOUND_MIXER_READ_RECSRC:
		if (di->source == -1)
			return EINVAL;
		mc.dev = di->source;
		if (di->caps & SOUND_CAP_EXCL_INPUT) {
			mc.type = AUDIO_MIXER_ENUM;
			error = mixer_ioctl(adev, AUDIO_MIXER_READ, &mc, flag, p);
			if (error)
				return error;
			idat = 1 << di->rdevmap[mc.un.ord];
		} else {
			int k;
			unsigned int mask;
			mc.type = AUDIO_MIXER_SET;
			error = mixer_ioctl(adev, AUDIO_MIXER_READ, &mc, flag, p);
			if (error)
				return error;
			idat = 0;
			for(mask = mc.un.mask, k = 0; mask; mask >>= 1, k++)
				if (mask & 1)
					idat |= 1 << di->rdevmap[k];
		}
		break;
	case SOUND_MIXER_READ_DEVMASK:
		idat = di->devmask;
		break;
	case SOUND_MIXER_READ_RECMASK:
		idat = di->recmask;
		break;
	case SOUND_MIXER_READ_STEREODEVS:
		idat = di->stereomask;
		break;
	case SOUND_MIXER_READ_CAPS:
		idat = di->caps;
		break;
	case SOUND_MIXER_WRITE_RECSRC:
		if (di->source == -1)
			return EINVAL;
		mc.dev = di->source;
		idat = *(int *)addr;
		if (di->caps & SOUND_CAP_EXCL_INPUT) {
			mc.type = AUDIO_MIXER_ENUM;
			for(i = 0; i < SOUND_MIXER_NRDEVICES; i++)
				if (idat & (1 << i))
					break;
			if (i >= SOUND_MIXER_NRDEVICES ||
			    di->devmap[i] == -1)
				return EINVAL;
			mc.un.ord = di->devmap[i];
		} else {
			mc.type = AUDIO_MIXER_SET;
			mc.un.mask = 0;
			for(i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
				if (idat & (1 << i)) {
					if (di->devmap[i] == -1)
						return EINVAL;
					mc.un.mask |= 1 << di->devmap[i];
				}
			}
		}
		return mixer_ioctl(adev, AUDIO_MIXER_WRITE, &mc, flag, p);
	default:
		/* REVISIT: Need to define SOUND_MIXER_FIRST */
		if (MIXER_READ(0/*SOUND_MIXER_FIRST*/) <= cmd &&
		    cmd < MIXER_READ(SOUND_MIXER_NRDEVICES)) {
			n = OSS_GET_DEV(cmd);
			if (di->devmap[n] == -1)
				return EINVAL;
			mc.dev = di->devmap[n];
			mc.type = AUDIO_MIXER_VALUE;
		    doread:
			mc.un.value.num_channels = di->stereomask & (1<<n) ? 2 : 1;
			error = mixer_ioctl(adev, AUDIO_MIXER_READ, &mc, flag, p);
			if (error)
				return error;
			if (mc.type != AUDIO_MIXER_VALUE)
				return EINVAL;
			if (mc.un.value.num_channels != 2) {
				l = r = mc.un.value.level[AUDIO_MIXER_LEVEL_MONO];
			} else {
				l = mc.un.value.level[AUDIO_MIXER_LEVEL_LEFT];
				r = mc.un.value.level[AUDIO_MIXER_LEVEL_RIGHT];
			}
			l = l * 100 / AUDIO_MAX_GAIN;
			r = r * 100 / AUDIO_MAX_GAIN;
			idat = l | (r << 8);
			break;
		/* REVISIT: Need to define SOUND_MIXER_FIRST */
		} else if (MIXER_WRITE(0/*SOUND_MIXER_FIRST*/) <= cmd &&
			   cmd < MIXER_WRITE(SOUND_MIXER_NRDEVICES)) {
			n = OSS_GET_DEV(cmd);
			if (di->devmap[n] == -1)
				return EINVAL;
			idat = *(int *)addr;
			l = (idat & 0xff) * AUDIO_MAX_GAIN / 100;
			r = (idat >> 8) * AUDIO_MAX_GAIN / 100;
			mc.dev = di->devmap[n];
			mc.type = AUDIO_MIXER_VALUE;
			if (di->stereomask & (1<<n)) {
				mc.un.value.num_channels = 2;
				mc.un.value.level[AUDIO_MIXER_LEVEL_LEFT] = l;
				mc.un.value.level[AUDIO_MIXER_LEVEL_RIGHT] = r;
			} else {
				mc.un.value.num_channels = 1;
				mc.un.value.level[AUDIO_MIXER_LEVEL_MONO] = (l+r)/2;
			}
			error = mixer_ioctl(adev, AUDIO_MIXER_WRITE, &mc, flag, p);
			if (error)
				return error;
			goto doread;
		} else
			return EINVAL;
	}
	*(int *)addr = idat;

	return 0;
}
