/*
 * $XConsortium: ibmKeybd.c /main/5 1996/07/31 10:32:01 kaleb $
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

#include "X.h"
#include "input.h"

#include "OSio.h"

#include "ibmTrace.h"

extern void ibmInfoMsg() ;

DevicePtr	ibmKeybd;
int		ibmBellPitch;
int		ibmBellDuration;
int		ibmLEDState;
int		ibmKeyClick;
#ifdef _IBM_LFT
int		ibmKeyRepeat = TRUE;
int		ibmBellVolume;
#else
int		ibmKeyRepeat;
#endif

int ibmUsePCKeys =	0 ;
int ibmLockState =	0 ;
int ibmCurLockKey =	0 ;
int ibmLockEnabled =	TRUE ;

void
ibmChangeKeyboardControl(pDevice,pCtrl)
	DevicePtr	*pDevice;
	KeybdCtrl	*pCtrl;
{
    TRACE(("ibmChangeKeyboardControl(0x%x,0x%x)\n",pDevice,pCtrl));
    ibmKeyClick=	pCtrl->click;
    ibmKeyRepeat=	pCtrl->autoRepeat;
    ibmBellPitch=	pCtrl->bell_pitch;
    ibmBellDuration=	pCtrl->bell_duration;
    ibmLEDState=	pCtrl->leds;
#ifdef _IBM_LFT
    ibmBellVolume= 	pCtrl->bell;
#endif
    return;
}
