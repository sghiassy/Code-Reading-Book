/* $XConsortium: xfsconf.h /main/2 1996/10/19 19:06:59 kaleb $ */





/* $XFree86: xc/programs/Xserver/hw/xfree86/XF86Setup/xfsconf.h,v 3.2 1996/12/27 06:54:27 dawes Exp $ */

/* Fake some stuff to get xf86Config.c to compile here */

#include "tcl.h"
#define NEED_RETURN_VALUE
#define RET_OKAY	TCL_OK
#define RET_ERROR	TCL_ERROR

extern char *XtMalloc(
#if NeedFunctionPrototypes
    unsigned int	/* size */
#endif
);

extern char *XtCalloc(
#if NeedFunctionPrototypes
    unsigned int	/* num */,
    unsigned int	/* size */
#endif
);

extern char *XtRealloc(
#if NeedFunctionPrototypes
    char*		/* ptr */,
    unsigned int	/* num */
#endif
);

extern void XtFree(
#if NeedFunctionPrototypes
    char*		/* ptr */
#endif
);

#define Xcalloc(size)	XtCalloc(1,size)
#define Xfree		XtFree
#define Xalloc		XtMalloc
#define Xrealloc	XtRealloc

#ifndef XF86SETUP_NO_FUNC_RENAME
#define FatalError	return XF86SetupFatalError
#define xf86ConfigError	return XF86SetupXF86ConfigError
#endif

