/*
 * $XConsortium: hftUtils.h /main/3 1995/12/05 15:44:17 matt $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
#ifndef _IBM_LFT

#ifndef HFTUTILS_H
#define HFTUTILS_H

	/*
	 * void		hftSetLEDS(unsigned which,unsigned how)
	 * void		hftSetLocatorThresholds(unsigned horz,unsigned vert)
	 * void		hftSetTableDeadZone(unsigned horz,unsigned vert)
	 * void		hftSetLPFK(unsigned keys,unsigned flags)
	 * void		hftSetDialGranularity(unsigned dials,unsigned *settings)
	 * void 	hftSound(unsigned duration,unsigned frequency)
	 * void 	hftSetTypematicDelay(unsigned delay)
	 * void 	hftSetTypematicRate(unsigned rate)
	 * void 	hftSetKeyClick(unsigned on)
	 * void 	hftSetLocatorSampleRate(unsigned rate)
	 * void 	hftSetVolume(unsigned vol)
	 * int 		hftQueryDeviceIds(hftDeviceID **ppDevices)
	 * unsigned	hftQueryDefaultDisplay()
	 * int 		hftHasAnAttached(unsigned devId,char *pName)
	 * int	        hftFindAScreen()
	 */

/***====================================================================***/

#define	HFT_MAXDEVICES	8

#define HF_INT(f)	( (unsigned int)((f)[0]<<24|(f)[1]<<16 | \
					 (f)[2]<<8 |(f)[3]) )

typedef struct hftdid  {
	unsigned	hftDevID;
	unsigned	hftDevClass;
} hftDeviceID;

#define	HFT_DEVID_MASK		0xffff0000
#define	HFT_DEVBUSTED_MASK	0x0000ff00
#define	HFT_DEVNUM_MASK		0x000000ff

#define	HFT_ILLEGAL_KEYBOARD	-1

#define	HFT_ILLEGAL_ID	0x00000000

#define	HFT_SKYWAY_ID	0x04210000
#define	HFT_SKYMONO_ID	0x04220000
#define	HFT_SABINE_ID	0x04250000
#define	HFT_GEMINI_ID	0x04270000
#define	HFT_PEDER_ID	0x04290000

#define	HFT_APA8_ID	0x04020000
#define	HFT_EGA_ID	0x04040000
#define	HFT_APA16_ID	0x04050000
#define	HFT_APA8C_ID	0x04060000
#define HFT_MEGAPEL_ID	0x04080000

#define	HFT_VGA_8503_ID		0x04110000
#define	HFT_VGA_8512_ID		0x04120000
#define	HFT_VGA_8513_ID		0x04130000
#define	HFT_VGA_8514_ID		0x04140000
#define	HFT_VGA_8507_ID		0x04150000
#define	HFT_VGA_8604_ID		0x04160000

#define	HFT_8514A_8503_ID	0x04180000
#define	HFT_8514A_8512_ID	0x04190000
#define	HFT_8514A_8513_ID	0x041A0000
#define	HFT_8514A_8514_ID	0x041B0000
#define	HFT_8514A_8507_ID	0x041C0000
#define	HFT_8514A_8604_ID	0x041D0000

/***====================================================================***/

extern	void		 hftSetLEDS();
extern	void		 hftSetLocatorThresholds();
extern	void		 hftSetTableDeadZone();
extern	void		 hftSetLPFK();
extern	void		 hftSetDialGranularity();
extern	void		 hftSound();
extern	void		 hftSetTypematicDelay();
extern	void		 hftSetTypematicRate();
extern	void		 hftSetKeyClick();
extern	void		 hftSetLocatorSampleRate();
extern	void		 hftSetVolume();
extern	int		 hftQueryDeviceIDs();
extern	unsigned	 hftQueryDefaultDisplay();
extern	int		 hftHasAnAttached();
extern	int		 hftFindAScreen();
extern	void		 hftQueryHardwareConfig();

#endif /* HFTUTILS_H */

#endif /* !_IBM_LFT */
