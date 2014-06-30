/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/xf86_ClkPr.c,v 3.8 1996/12/23 06:44:24 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL, KEVIN E. MARTIN, RICKARD E. FAITH, SCOTT LAIRD,
 * AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL THOMAS ROELL, KEVIN E. MARTIN, RICKARD E. FAITH, SCOTT LAIRD, OR
 * TIAGO GONS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 *
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Further modifications by Scott Laird (lair@kimbark.uchicago.edu)
 * and Tiago Gons (tiago@comosjn.hobby.nl)
 *
 */
/* $XConsortium: xf86_ClkPr.c /main/6 1996/06/11 06:56:22 kaleb $ */

#include "X.h"
#include "input.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"

#if defined(CSRG_BASED) || defined(MACH386)
#include <sys/resource.h>
#endif

/*
 * xf86GetClocks -- generalized version of (old) vgaGetClocks
 *      get the dot-clocks via a BIG BAD hack ...
 */
void
xf86GetClocks(int num,
#if NeedNestedPrototypes
		Bool (*ClockFunc)(int),
		void (*ProtectRegs)(Bool),
#else
		Bool (*ClockFunc)(),
		void (*ProtectRegs)(),
#endif
		SaveScreenProcPtr SaveScreen,
		int vertsyncreg,
		int maskval, 
		int knownclkindex,
		int knownclkvalue,
		ScrnInfoRec *InfoRec)
{
    register int status = vertsyncreg;
    unsigned long i, cnt, rcnt, sync;
    int saved_nice;

    /* First save registers that get written on */
    (*ClockFunc)(CLK_REG_SAVE);

#if defined(CSRG_BASED) || defined(MACH386)
    saved_nice = getpriority(PRIO_PROCESS, 0);
    setpriority(PRIO_PROCESS, 0, -20);
#endif
#if defined(SYSV) || defined(SVR4) || defined(linux)
    saved_nice = nice(0);
    nice(-20 - saved_nice);
#endif


    for (i = 0; i < num; i++) 
    {
	(*ProtectRegs)(TRUE);
	if (!(*ClockFunc)(i))
	{
	    InfoRec->clock[i] = -1;
	    continue;
	}
	(*ProtectRegs)(FALSE);
	(*SaveScreen)(NULL, FALSE);
	    
    	usleep(50000);     /* let VCO stabilise */

    	cnt  = 0;
    	sync = 200000;

    	if (!xf86DisableInterrupts())
    	{
	    (*ClockFunc)(CLK_REG_RESTORE);
	    ErrorF("Failed to disable interrupts during clock probe.  If\n");
	    ErrorF("your OS does not support disabling interrupts, then you\n");
	    FatalError("must specify a Clocks line in the XF86Config file.\n");
	}
	while ((inb(status) & maskval) == 0x00) 
	    if (sync-- == 0) goto finish;
	/* Something appears to be happening, so reset sync count */
	sync = 200000;
	while ((inb(status) & maskval) == maskval) 
	    if (sync-- == 0) goto finish;
	/* Something appears to be happening, so reset sync count */
	sync = 200000;
	while ((inb(status) & maskval) == 0x00) 
	    if (sync-- == 0) goto finish;
    
	for (rcnt = 0; rcnt < 5; rcnt++) 
	{
	    while (!(inb(status) & maskval)) 
		cnt++;
	    while ((inb(status) & maskval)) 
		cnt++;
	}
    
finish:
	xf86EnableInterrupts();

	InfoRec->clock[i] = cnt ? cnt : -1;
        (*SaveScreen)(NULL, TRUE);
    }

#if defined(CSRG_BASED) || defined(MACH386)
    setpriority(PRIO_PROCESS, 0, saved_nice);
#endif
#if defined(SYSV) || defined(SVR4) || defined(linux)
    nice(20 + saved_nice);
#endif

    for (i = 0; i < num; i++) 
    {
	if (i != knownclkindex)
	{
	    if (InfoRec->clock[i] == -1)
	    {
		InfoRec->clock[i] = 0;
	    }
	    else 
	    {
		InfoRec->clock[i] = (int)(0.5 +
                    (((float)knownclkvalue) * InfoRec->clock[knownclkindex]) / 
	            (InfoRec->clock[i]));
		/* Round to nearest 10KHz */
		InfoRec->clock[i] += 5;
		InfoRec->clock[i] /= 10;
		InfoRec->clock[i] *= 10;
	    }
	}
    }

    InfoRec->clock[knownclkindex] = knownclkvalue;
    InfoRec->clocks = num; 

    /* Restore registers that were written on */
    (*ClockFunc)(CLK_REG_RESTORE);
}
