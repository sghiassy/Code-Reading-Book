/*
 * $XConsortium: ibmScreen.c,v 1.3 91/09/09 13:23:20 rws Exp $
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
#include "misc.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "ibmScreen.h"

ibmPerScreenInfo *ibmScreens[MAXSCREENS];
int              ibmNumScreens=         0;
int              ibmXWrapScreen=        FALSE;
int              ibmYWrapScreen=        FALSE;
int              ibmCurrentScreen=      0;
int              ibmUseHardware=        1;

#ifdef IBM_DEV
extern ibmPerScreenInfo dev1ScreenInfoStruct ;
extern ibmPerScreenInfo dev2ScreenInfoStruct ;
extern ibmPerScreenInfo dev3ScreenInfoStruct ;
#endif
#ifdef IBM_GENERIC
extern ibmPerScreenInfo genericScreenInfoStruct ;
#endif
#ifdef IBM_ADAM
extern ibmPerScreenInfo adamScreenInfoStruct ;
#endif
#ifdef IBM_SKYWAY
extern ibmPerScreenInfo skywayScreenInfoStruct ;
#endif
#ifdef IBM_SKYMONO
extern ibmPerScreenInfo skyMonoScreenInfoStruct ;
#endif
#ifdef IBM_SABINE
extern ibmPerScreenInfo sabineScreenInfoStruct ;
#endif
#ifdef IBM_GEMINI
extern ibmPerScreenInfo geminiScreenInfoStruct ;
#endif
#ifdef IBM_PED
extern ibmPerScreenInfo pedScreenInfoStruct ;
#endif
#ifdef IBM_VGA
extern ibmPerScreenInfo vgaScreenInfoStruct ;
#endif
#ifdef IBM_8514
extern ibmPerScreenInfo ibm8514ScreenInfoStruct ;
#endif
#ifdef IBM_CFBLANNO
extern ibmPerScreenInfo cfblannoScreenInfoStruct ;
#endif
#ifdef IBM_MFBLANNO
extern ibmPerScreenInfo mfblannoScreenInfoStruct ;
#endif

ibmPerScreenInfo *ibmPossibleScreens[] = {
#ifdef IBM_ADAM
	&adamScreenInfoStruct,
#endif
#ifdef IBM_SKYWAY
	&skywayScreenInfoStruct,
#endif
#ifdef IBM_SKYMONO
	&skyMonoScreenInfoStruct,
#endif
#ifdef IBM_SABINE
	&sabineScreenInfoStruct,
#endif
#ifdef IBM_GEMINI
	&geminiScreenInfoStruct,
#endif
#ifdef IBM_GENERIC
	&genericScreenInfoStruct,
#endif
#ifdef IBM_PED
	&pedScreenInfoStruct,
#endif
#ifdef IBM_VGA
	&vgaScreenInfoStruct,
#endif
#ifdef IBM_8514
	&ibm8514ScreenInfoStruct,
#endif
#ifdef IBM_CFBLANNO
	&cfblannoScreenInfoStruct,
#endif
#ifdef IBM_MFBLANNO
	&mfblannoScreenInfoStruct,
#endif
#ifdef IBM_DEV
	&dev1ScreenInfoStruct,
	&dev2ScreenInfoStruct,
	&dev3ScreenInfoStruct,
#endif
	0
} ;
