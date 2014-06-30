/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/amoeba/am_init.c,v 3.5 1996/12/23 06:49:11 dawes Exp $ */
/*
 * Copyright 1993 by Vrije Universiteit, The Netherlands
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of the Vrije Universiteit and David 
 * Wexelblat not be used in advertising or publicity pertaining to 
 * distribution of the software without specific, written prior permission.
 * The Vrije Universiteit and David Wexelblat make no representations
 * about the suitability of this software for any purpose.  It is provided
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
/* $XConsortium: am_init.c /main/5 1996/02/21 17:50:35 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"

static capability vgaMemCap;

void xf86OpenConsole()
{
    char *findhole();
    segid seg;
    int nbytes;
    errstat err;

    if (serverGeneration == 1)
    {
	/* 
	 * Get segment capability for video memory, and map it in.
	 * We do it this early since a side effect is that the I/O
	 * space is also mapped in.
	 */
	if ((err = iop_map_mem(&iopcap, &vgaMemCap)) != STD_OK)
	{
	    FatalError("xf86OpenConsole: iop_map_mem failed (%s)\n", 
		       err_why(err));
	}

	/* Map in aligned screen memory */
	nbytes = 0x10000; /* 64 Kb */
	xf86Info.screenPtr = 
		(pointer)(((unsigned) findhole(2 * nbytes) & ~0xFFF) + 0x1000);
	if ((seg = seg_map(&vgaMemCap,xf86Info.screenPtr, nbytes,
			   MAP_TYPEDATA|MAP_READWRITE|MAP_INPLACE)) < 0)
	{
	    FatalError("xf86OpenConsole: Map segment failed: %s\n", 
		       err_why(ERR_CONVERT(seg)));
	}
 
	xf86Config(FALSE); /* Read XF86Config */
    }
    return;
}

void xf86CloseConsole()
{
    static am_port_t nullport;
    errstat err;

    if (!NULLPORT(&vgaMemCap.cap_port)) {
	/* Unmap video's memory segment */
	if ((err = iop_unmap_mem(&iopcap, &vgaMemCap)) != STD_OK)
	{
	    xf86FatalError("xf86CloseConsole: iop_map_mem failed (%s)\n", 
			   err_why(err));
	}
	vgaMemCap.cap_port = nullport;
    }
    return;
}

/* ARGSUSED */
int xf86ProcessArgument (argc, argv, i)
int argc;
char *argv[];
int i;
{
	return(0);
}

void xf86UseMsg()
{
	return;
}
