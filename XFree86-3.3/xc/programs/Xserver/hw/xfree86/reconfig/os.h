/* $XFree86: xc/programs/Xserver/hw/xfree86/reconfig/os.h,v 3.6 1996/12/23 06:51:43 dawes Exp $ */





/* $XConsortium: os.h /main/5 1996/10/19 18:08:25 kaleb $ */

/* from <X11/Xosdefs.h> */
#ifdef NOSTDHDRS
#define X_NOT_STDC_ENV
#endif
#if defined(USL) && defined(SYSV)
#define X_NOT_STDC_ENV
#endif
#if defined(SYSV) && defined(i386)
#ifndef SCO
#define X_NOT_STDC_ENV
#endif
#endif

/* from <X11/Xlibint.h> */
#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#include <string.h>
#else
char *malloc(), *realloc(), *calloc();
#ifdef SYSV
#include <string.h>
#else
#include <strings.h>
#endif
#endif

#if defined(MACH) || defined(__bsdi__)
#include <ctype.h>
#endif
