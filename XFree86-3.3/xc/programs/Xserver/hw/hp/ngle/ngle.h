/* $XConsortium: ngle.h,v 1.3 95/01/24 01:55:45 dpw Exp $ */
/*************************************************************************
 * 
 * (c)Copyright 1992 Hewlett-Packard Co.,  All Rights Reserved.
 * 
Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of Hewlett Packard not be used in
advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.
 *
 *************************************************************************/

#ifndef NGLE_H
#define NGLE_H

/* Rummor has it that NDEBUG is defined by default in X build environment.
 * It is used to activate assert statements (see /usr/include/assert.h).
 *	Compiling with the preprocessor option -DNDEBUG (see cpp(1)), or with
 *	the preprocessor control statement #define NDEBUG ahead of the
 *	#include <assert.h> statement, stops assertions from being compiled
 *	into the program.
 */

#include <unistd.h>         /* keys off _[POSIX|HPUX|XOPEN]_SOURCE */
#include <stdlib.h>         /* For prototype of getenv() */
#include <stdio.h>
#include <errno.h>
#include <assert.h>         /* For assert() statements */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/framebuf.h>
#include <fcntl.h>

#include "X.h"
#include "Xproto.h"

#include "scrnintstr.h"
#include "cursorstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "colormapst.h"
#include "resource.h"
#include "opaque.h"

/* included for the mi private structure */
#include "mibstorest.h"

#ifndef Card32
#define Card32 unsigned int
#endif
#ifndef Int32
#define Int32 int 
#endif

#ifndef Card16
#define Card16 unsigned short
#endif
#ifndef Int16
#define Int16 short 
#endif

#ifndef Card8
#define Card8 unsigned char
#endif
#ifndef Int8
#define Int8 char 
#endif

#include "ngleextern.h"
#include "dregs.h"
#include "ngledevrom.h"
#include "nglehdw.h"
#include "nglecursor.h"
#include "nglecopy.h"
#include "nglecolormap.h"
#include "nglenoop.h"
#include "hppriv.h"
#include "nglescreen.h"


#   ifndef S9000_ID_TIMBER		/* Bushmaster (710) Graphics */
#       define S9000_ID_TIMBER		0x27F12392
#   endif

#   ifndef S9000_ID_TOMCAT		/* 2-headed ELK */
#       define S9000_ID_TOMCAT          0x27FCCB6D
#   endif

#   ifndef CRX24_OVERLAY_PLANES		/* Overlay planes on CRX24 */
#       define CRX24_OVERLAY_PLANES	0x920825AA
#   endif

#   ifndef S9000_ID_ARTIST		/* Artist (Gecko/712 & 715) Graphics */
#       define S9000_ID_ARTIST          0x2B4DED6D
#   endif

#   ifndef S9000_ID_HCRX		/* hyperdrive Graphics */
#       define S9000_ID_HCRX		0x2BCB015A
#   endif

#define hpGivingUp  (dispatchException & DE_TERMINATE)

#define MAX_BITS_PER_RGB 8 /* The maximum number of bits that any
			      of the DACs have on our hardware. */

#endif /* NGLE_H */
