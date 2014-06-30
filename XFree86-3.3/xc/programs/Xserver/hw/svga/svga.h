/* $XConsortium: svga.h,v 1.2 93/09/18 18:18:14 rws Exp $ */
/*
 * Copyright 1990,91,92,93 by Thomas Roell, Germany.
 * Copyright 1991,92,93    by SGCS (Snitily Graphics Consulting Services), USA.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this  permission notice appear
 * in supporting documentation, and that the name of Thomas Roell nor
 * SGCS be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Thomas Roell nor SGCS makes no representations about the suitability
 * of this software for any purpose. It is provided "as is" without
 * express or implied warranty.
 *
 * THOMAS ROELL AND SGCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THOMAS ROELL OR SGCS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _SVGA_H
#define _SVGA_H

#undef _POSIX_SOURCE
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#define _POSIX_SOURCE
#include <signal.h>
#include <sys/xque.h>
#include "compiler.h"

#include "X.h"
#include "Xproto.h"
#include "input.h"
#include "scrnintstr.h"

extern int svgaConsoleFd;
extern Bool svgaInputPending;
extern Bool svgaVTRequestsPending;
extern Bool svgaVTActive;
extern xqEventQueue *XqueQaddr;


/* svgaBank.c */

extern Bool
svgaBankScreenInit(
    ScreenPtr  pScreen,
    ulong      winAAttributes,
    ulong      winBAttributes,
    int        winGranularity,
    int        winSize,
    ulong      winABase,
    ulong      winBBase,
    ulong      bytesPerScanLine,
    short      width,
    short      height,
    void       (*SetWindow)()
);


/* svgaFb.c */

extern Bool
svgaSVGABlankScreen(
    ScreenPtr pScreen,
    int       on
);

extern Bool
svgaSVGAScreenInit(
    int        index,
    ScreenPtr  pScreen,
    int        argc,
    char       **argv
);


/* svgaIo.c */

extern void
XqueRequest(
    int       signo
);

extern int
XquePointerProc(
    DeviceIntPtr pPointer,
    int       what
);

extern int
XqueKeyboardProc(
    DeviceIntPtr pKeyboard,
    int       what
);


/* svgaKbd.c */

extern void
svgaKbdGetMapping(
    KeySymsPtr pKeySyms,
    CARD8      *pModMap
);


/* svgaSVPMI.c */

extern Bool
svgaSVPMIOpen(
    ScreenPtr pScreen,
    int        argc,
    char       **argv,
    ulong      *winAAttributes,		/* RETURN */
    ulong      *winBAttributes,		/* RETURN */
    int        *winGranularity,		/* RETURN */
    int        *winSize,		/* RETURN */
    ulong      *winABase,		/* RETURN */
    ulong      *winBBase,		/* RETURN */
    int        *bytesPerScanLine,	/* RETURN */
    int        *XResolution,		/* RETURN */
    int        *YResolution,		/* RETURN */
    int        *bitsRGB			/* RETURN */
);

extern void
svgaSVPMISetText(
    ScreenPtr pScreen
);

extern void
svgaSVPMISetGraphics(
    ScreenPtr pScreen
);

extern void
svgaSVPMISetWindow(
    ScreenPtr pScreen,
    int       position,
    int       windowNo
);

extern void
svgaSVPMISetColor(
    ScreenPtr pScreen,
    int       index,
    ushort    red,
    ushort    green,
    ushort    blue
);
    

/* svgaVT.c */

extern void
svgaVTRequest(
    int signo
);

extern void
svgaVTSwitch(
    void
);

#endif /* _SVGA_H */
