/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/minix/mnx_video.c,v 3.4 1996/12/23 06:50:20 dawes Exp $ */
/*
 * Copyright 1993 by Vrije Universiteit, The Netherlands
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of The Vrije Universiteit and David 
 * Wexelblat not be used in advertising or publicity pertaining to 
 * distribution of the software without specific, written prior permission.
 * The Vrije Universiteit and David Wexelblat make no representations about 
 * the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THE VRIJE UNIVERSITEIT AND DAVID WEXELBLAT DISCLAIM ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE VRIJE UNIVERSITEIT OR
 * DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR 
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: mnx_video.c /main/5 1996/02/21 17:52:19 kaleb $ */

#include "X.h"
#include "input.h"
#include "scrnintstr.h"

#include "local.h"
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"

/***************************************************************************/
/* Video Memory Mapping section                                            */
/***************************************************************************/

/* ARGSUSED */
pointer xf86MapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
	ErrorF("xf86MapVidMem(ScreenNum= %d, Base= %p, Size= 0x%x\n",
		ScreenNum, Base, Size);
	return((pointer)xf86VideoBase + ((unsigned)Base-0xA0000));
}

/* ARGSUSED */
void xf86UnMapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
	ErrorF("(warning) xf86UnmapVidMem is not implemented\n");
	return;
}

Bool xf86LinearVidMem()
{
	return(FALSE);
}

/***************************************************************************/
/* Interrupt Handling section                                              */
/***************************************************************************/

Bool xf86DisableInterrupts()
{
#ifdef __GNUC__
	__asm__ __volatile__("cli");
#else
	extern void intr_disable();

	intr_disable();
#endif /* __GNUC__ */

	return(TRUE);
}

void xf86EnableInterrupts()
{
#ifdef __GNUC__
	__asm__ __volatile__("sti");
#else
	extern void intr_enable();

	intr_enable();
#endif /* __GNUC__ */

	return;
}
