/***********************************************************

Copyright (c) 1991  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.


Copyright 1991 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $XConsortium: ws.h,v 1.4 94/04/17 20:29:56 keith Exp $ */

#define NOMAPYET        (ColormapPtr) 1

#define  ARG_DPIX	(1 << 0)
#define  ARG_DPIY	(1 << 1)
#define  ARG_DPI	(1 << 2)
#define  ARG_BLACKVALUE	(1 << 3)
#define  ARG_WHITEVALUE	(1 << 4)
#define	 ARG_CLASS	(1 << 5)
#define	 ARG_EDGE_L	(1 << 6)
#define	 ARG_EDGE_R	(1 << 7)
#define	 ARG_EDGE_T	(1 << 8)
#define	 ARG_EDGE_B	(1 << 9)
#define  ARG_MONITOR	(1 << 10)
#define	 ARG_DEPTH	(1 << 11)

typedef struct  {
	int flags;
	int dpix;
	int dpiy;
	int dpi;
	int class;
	char  *blackValue;
	char  *whiteValue;
	int edge_left;
	int edge_right;
	int edge_top;
	int edge_bottom;
	ws_monitor monitor;
	int depth;
} ScreenArgsRec;

typedef struct {
	unsigned int		currentmask; 	/* saved plane mask */
	BoxPtr			cursorConstraint;
	ws_screen_descriptor	*screenDesc;
	ColormapPtr		pInstalledMap;
	ScreenArgsRec 		*args;
	Bool			(*CloseScreen)();
} wsScreenPrivate;

typedef struct {
	char *moduleID;	/* graphic module ID */
	Bool (*createProc)();	/* create procedure for this hardware type */
} wsAcceleratorTypes;

extern void wsStoreColors();
extern void wsInstallColormap();
extern void wsUninstallColormap();
extern int wsListInstalledColormaps();
extern int wsScreenPrivateIndex;
extern Bool wsSaveScreen();
extern int dpix, dpiy, dpi;

extern ScreenArgsRec screenArgs[];

extern ScreenPtr wsScreens[];
extern int class;
extern int forceDepth;
extern int fdPM;   /* this is the file descriptor for screen so
		    can do IOCTL to colormap */
extern int ws_cpu;

