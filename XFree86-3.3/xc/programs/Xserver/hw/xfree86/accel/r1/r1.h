#ifndef _R1_H_
#define _R1_H_

#define R1_PATCHLEVEL "0"

#ifndef LINKKIT

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "pixmap.h"
#include "region.h"
#include "gc.h"
#include "gcstruct.h"
#include "colormap.h"
#include "colormapst.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "mipointer.h"
#include "cursorstr.h"
#include "windowstr.h"
#include "compiler.h"
#include "misc.h"
#include "xf86.h"
#include "regionstr.h"
#include "xf86_OSlib.h"
#include "xf86bcache.h"
#include "xf86fcache.h"
#include "xf86Procs.h"

#include <X11/Xfuncproto.h>

#else /* !LINKKIT */
#include "X.h"
#include "input.h"
#include "misc.h"
#include "xf86.h"
#include "xf86_OSlib.h"
#endif /* !LINKKIT */

#if !defined(__GNUC__) || defined(NO_INLINE)
#define __inline__ /**/
#endif

extern ScrnInfoRec r1InfoRec;

#ifndef LINKKIT
_XFUNCPROTOBEGIN

extern int r1ValidTokens[];

/* Function Prototypes */

/* r1.c */
void r1PrintIdent(
#if NeedFunctionPrototypes
    void
#endif
);
Bool r1Probe(
#if NeedFunctionPrototypes
    void
#endif
);
Bool r1Initialize(
#if NeedFunctionPrototypes
    int,
    ScreenPtr,
    int,
    char **
#endif
);
void r1EnterLeaveVT(
#if NeedFunctionPrototypes
    Bool,
    int 
#endif
);
Bool r1CloseScreen(
#if NeedFunctionPrototypes
    int,
    ScreenPtr
#endif
);
Bool r1SaveScreen(
#if NeedFunctionPrototypes
    ScreenPtr,
    Bool 
#endif
);
Bool r1SwitchMode(
#if NeedFunctionPrototypes
    DisplayModePtr 
#endif
);
void r1DPMSSet(
#if NeedFunctionPrototypes
    int PowerManagementMode
#endif
);
void r1AdjustFrame(
#if NeedFunctionPrototypes
    int,
    int 
#endif
);
/* r1cmap.c */
int r1ListInstalledColormaps(
#if NeedFunctionPrototypes
    ScreenPtr,
    Colormap *
#endif
);
void r1StoreColors(
#if NeedFunctionPrototypes
    ColormapPtr,
    int,
    xColorItem *
#endif
);
void r1InstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr 
#endif
);
void r1UninstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr 
#endif
);

_XFUNCPROTOEND

#endif /* !LINKKIT */
#endif /* _R1_H_ */
