/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86Procs.h,v 3.10 1996/12/23 06:43:38 dawes Exp $ */
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
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: xf86Procs.h /main/9 1996/10/19 17:59:14 kaleb $ */

#ifndef _XF86PROCS_H
#define _XF86PROCS_H

#include <X11/Xfuncproto.h>
#include "xf86.h"
#include "xf86Priv.h"

_XFUNCPROTOBEGIN

/* xf86Config.c */
extern void xf86CheckBeta(
#if NeedFunctionPrototypes
	int			/* extraDays */,
	char *			/* key */
#endif 
);

/* xf86Config.c */

#ifndef CONFIG_RETURN_TYPE
#ifdef XF86SETUP
#define CONFIG_RETURN_TYPE int
#else
#define CONFIG_RETURN_TYPE void
#endif
#endif

extern CONFIG_RETURN_TYPE xf86Config(
#if NeedFunctionPrototypes
    int vtopen
#endif
);

extern Bool xf86LookupMode(
#if NeedFunctionPrototypes
	DisplayModePtr,		/* target */
	ScrnInfoPtr,		/* driver */
	int			/* flags */
#endif 
);

extern void xf86VerifyOptions(
#if NeedFunctionPrototypes
	OFlagSet *,		/* allowedOptions */
	ScrnInfoPtr		/* driver */
#endif
);

extern void xf86DeleteMode(
#if NeedFunctionPrototypes
	ScrnInfoPtr,		/* infoptr */
	DisplayModePtr		/* dispmp */
#endif
);

extern int xf86GetToken(
#if NeedFunctionPrototypes
	SymTabPtr		/* table */
#endif
);

extern char *xf86TokenToString(
#if NeedFunctionPrototypes
	SymTabPtr,		/* table */
	int			/* token */
#endif
);

extern int xf86StringToToken(
#if NeedFunctionPrototypes
	SymTabPtr,		/* table */
	char *			/* string */
#endif
);

extern void xf86ConfigError(
#if NeedFunctionPrototypes
	char *			/* msg */
#endif
)
#if __GNUC__
__attribute__((noreturn))
#endif
;

/* xf86Cursor.c */

extern void xf86InitViewport(
#if NeedFunctionPrototypes
	ScrnInfoPtr		/* pScr */
#endif 
);

extern void xf86SetViewport(
#if NeedFunctionPrototypes
	ScreenPtr,		/* pScreen */
	int,			/* x */
	int			/* y */
#endif 
);

extern void xf86ZoomViewport(
#if NeedFunctionPrototypes
	ScreenPtr,		/* pScreen */
	int			/* zoom */
#endif 
);


/* xf86Events.c */

extern void ProcessInputEvents(
#if NeedFunctionPrototypes
	void
#endif
);

extern void xf86PostKbdEvent(
#if NeedFunctionPrototypes
	unsigned		/* key */
#endif 
);

extern void xf86PostMseEvent(
#if NeedFunctionPrototypes
        DeviceIntPtr,		/* device */
	int,			/* buttons */
	int,			/* dx */
	int			/* dy */
#endif
);

extern void xf86Block(
#if NeedFunctionPrototypes
	pointer,		/* blockData */
	OSTimePtr,		/* pTimeout */
	pointer			/* pReadmask */
#endif
);

extern void xf86Wakeup(
#if NeedFunctionPrototypes
	pointer,		/* blockData */
	int,			/* err */
	pointer			/* pReadmask */
#endif
);

extern void xf86VTRequest(
#if NeedFunctionPrototypes
	int			/* signo */
#endif
);

extern void xf86SigHandler(
#if NeedFunctionPrototypes
	int		       /* signo */
#endif
);

/* xf86Io.c */

extern void xf86KbdLeds(
#if NeedFunctionPrototypes
	void
#endif
);

extern void xf86InitKBD(
#if NeedFunctionPrototypes
	Bool			/* init */
#endif
);

#ifdef NEED_EVENTS
extern void xf86KbdBell(
#if NeedFunctionPrototypes
	int,			/* loudness */
	DeviceIntPtr, 		/* pKeyboard */
	pointer, 		/* ctrl */
	int
#endif
);
#endif

extern void xf86KbdCtrl(
#if NeedFunctionPrototypes
	DevicePtr,		/* pKeyboard */
	KeybdCtrl *		/* ctrl */
#endif
);

extern int  xf86KbdProc(
#if NeedFunctionPrototypes
	DeviceIntPtr,		/* pKeyboard */
	int			/* what */
#endif
);

extern void xf86KbdEvents(
#if NeedFunctionPrototypes
	void
#endif
);

extern void xf86MseCtrl(
#if NeedFunctionPrototypes
	DevicePtr,		/* pPointer */
	PtrCtrl*		/* ctrl */
#endif
);

extern int  xf86MseProc(
#if NeedFunctionPrototypes
	DeviceIntPtr,		/* pPointer */
	int			/* what */
#endif
);

extern int  
xf86MseProcAux(
#if NeedFunctionPrototypes
       DeviceIntPtr,		/* pPointer */
       int,			/* what */
       MouseDevPtr,		/* mouse */
       int*,                    /* fd */
       PtrCtrlProcPtr		/* ctrl */
#endif
	       );

extern void xf86MseEvents(
#if NeedFunctionPrototypes
        MouseDevPtr		/* mouse */
#endif
);

/* xf86_Mouse.c */

extern Bool xf86MouseSupported(
#if NeedFunctionPrototypes
	int			/* mousetype */
#endif
);

extern void xf86SetupMouse(
#if NeedFunctionPrototypes
	MouseDevPtr
#endif
);

extern void xf86MouseProtocol(
#if NeedFunctionPrototypes
        DeviceIntPtr,		/* device */
	unsigned char *,	/* rBuf */
	int			/* nBytes */
#endif
);


/* xf86Kbd.c */

extern void xf86KbdGetMapping(
#if NeedFunctionPrototypes
	KeySymsRec *,		/* pKeySyms */
	CARD8 *			/* pModMap */
#endif
);

/* xf86XKB.c */
extern void xf86InitXkb(
#if NeedFunctionPrototypes
	void
#endif
);

_XFUNCPROTOEND

#endif /* _XF86PROCS_H */


