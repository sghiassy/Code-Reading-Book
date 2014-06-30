/*
 * $XConsortium: ibmKeybd.h /main/3 1995/12/05 15:44:36 matt $
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

#ifndef IBMKEYBD_H
#define IBMKEYBD_H

#define	IBM_LED_NUMLOCK		1
#define IBM_LED_CAPSLOCK	2
#define IBM_LED_SCROLLOCK	4

extern	DevicePtr	ibmKeybd;
extern	int 		ibmUsePCKeys;
extern	int		ibmBellPitch;
extern	int		ibmBellDuration;
#ifdef _IBM_LFT
extern	int		ibmBellVolume;
#endif
extern	int		ibmLEDState;
extern	int		ibmKeyClick;
extern	int		ibmKeyRepeat;
extern	int		ibmLockState;
extern	int		ibmCurLockKey;
extern	int		ibmLockEnabled;

/* defined in OS specific directories */
extern	int		osKeybdProc();
extern	KeySym		ibmmap[];

#endif /* IBMKBD_H */
