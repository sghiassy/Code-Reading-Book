/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/shared/IO_utils.c,v 3.1 1996/12/23 06:50:55 dawes Exp $ */
/*
 * Copyright 1993 by David Dawes <dawes@XFree86.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Dawes 
 * not be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.
 * David Dawes makes no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * DAVID DAWES DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL DAVID DAWES BE LIABLE FOR 
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: IO_utils.c /main/3 1996/02/21 17:53:21 kaleb $ */

#include "X.h"
#include "input.h"
#include "scrnintstr.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"


void xf86InitPortLists(EnabledPorts, NumEnabledPorts, ScreenEnabled,
		       ExtendedPorts, NumScr)
unsigned **EnabledPorts;
int *NumEnabledPorts;
Bool *ScreenEnabled;
Bool *ExtendedPorts;
int NumScr;
{
	int i;

	for (i = 0; i < NumScr; i++)
	{
		EnabledPorts[i] = NULL;
		NumEnabledPorts[i] = 0;
		ScreenEnabled[i] = FALSE;
		ExtendedPorts[i] = FALSE;
	}
}


/*
 * Check if Port is enabled in any of the currently enabled screens.
 * If it is enabled, then it should be neither re-enabled or disabled.
 * The screen being changed should be marked as disabled when calling this.
 */

Bool xf86CheckPorts(Port, EnabledPorts, NumEnabledPorts, ScreenEnabled, NumScr)
unsigned Port;
unsigned **EnabledPorts;
int *NumEnabledPorts;
Bool *ScreenEnabled;
int NumScr;
{
	int i, j;

	for (i = 0; i < NumScr; i++)
	{
		if (ScreenEnabled[i])
		{
			for (j = 0; j < NumEnabledPorts[i]; j++)
			{
				if (Port == EnabledPorts[i][j])
					return(FALSE);
			}
		}
	}
	return(TRUE);
}
